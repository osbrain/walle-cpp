#include "bsp_ps2.h"
#include "bsp_usart.h"
#include "stdio.h"

uint8_t Comd[2]={0x01,0x42};	//开始命令和请求数据命令
uint8_t Data[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //数据存储数组

//按键值
uint16_t MASK[]={
    PSB_SELECT,     //SELECT键       
    PSB_L3,         //左遥杆被按下       
    PSB_R3 ,        //右遥杆被按下
    PSB_START,      //START键
    PSB_PAD_UP,     //上键
    PSB_PAD_RIGHT,  //右键
    PSB_PAD_DOWN,   //下键
    PSB_PAD_LEFT,   //左键
    PSB_L2,         //左边顶部2键    
    PSB_R2,         //右边顶部2键
    PSB_L1,         //左边顶部1键
    PSB_R1 ,        //右边顶部1键
    PSB_GREEN,      //绿色的三角形图案键
    PSB_RED,        //红色的圆形图案键
    PSB_BLUE,       //蓝色的X形图案键
    PSB_PINK        //粉色的矩形图案键
	};	
    
/**********************************************************
 * 函 数 名 称：PS2_GPIO_Init
 * 函 数 功 能：初始化PS2手柄接收端引脚
 * 传 入 参 数：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
**********************************************************/
void PS2_GPIO_Init(void)
{
  /* 使能时钟 */
	rcu_periph_clock_enable(RCU_CLK);
	rcu_periph_clock_enable(RCU_DAT);
	rcu_periph_clock_enable(RCU_CMD);
	rcu_periph_clock_enable(RCU_CS);
	
	/* 配置CLK */
	gpio_mode_set(PORT_CLK,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_CLK);
	gpio_output_options_set(PORT_CLK,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_CLK);
	CLK_OUT(1);
	
	/* 配置DI */
	gpio_mode_set(PORT_DAT,GPIO_MODE_INPUT,GPIO_PUPD_PULLDOWN,GPIO_DAT);
	
	/* 配置DO */
	gpio_mode_set(PORT_CMD,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_CMD);
	gpio_output_options_set(PORT_CMD,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_CMD);

	/* 配置CS */
	gpio_mode_set(PORT_CS,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_CS);
	gpio_output_options_set(PORT_CS,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_CS);
	CS_OUT(1);
}



/******************************************************************
 * 函 数 名 称：PS2_Cmd
 * 函 数 说 明：向手柄发送命令
 * 函 数 形 参：CMD要发送的命令
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void PS2_Cmd(uint8_t CMD)
{
	volatile uint16_t ref=0x01;
	Data[1] = 0;
	
	for(ref=0x01;ref<0x0100;ref<<=1)
	{
        CMD_OUT( (ref&CMD) );
		CLK_OUT(1); //时钟拉高
		delay_1us(50);
		CLK_OUT(0);
		delay_1us(50);
        CLK_OUT(1);
		if( DAT_GET() )
		{
			Data[1] = ref|Data[1];
		}				
	}
}
/******************************************************************
 * 函 数 名 称：PS2_ReadData
 * 函 数 说 明：读取手柄数据
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void PS2_ReadData(void)
{
	volatile uint8_t byte=0;
	volatile uint16_t ref=0x01;
    uint16_t i = 0;
	CS_OUT(0);
    
	PS2_Cmd(Comd[0]);  //开始命令
	PS2_Cmd(Comd[1]);  //请求数据

	for(byte=2;byte<9;byte++)          //开始接受数据
	{
		for(ref=0x01;ref<0x100;ref<<=1)
		{
			CLK_OUT(1);
      delay_1us(10);
			CLK_OUT(0);
			delay_1us(10);
			CLK_OUT(1);
            
		    if( DAT_GET() )
			{ 
				Data[byte] = ref|Data[byte];
			}
		}
        delay_1us(16);
	}	
	CS_OUT(1);
}

/******************************************************************
 * 函 数 名 称：PS2_RedLight
 * 函 数 说 明：判断是否为红灯模式
 * 函 数 形 参：无
 * 函 数 返 回：0：红灯模式  其他：其他模式
 * 作       者：LC
 * 备       注：0X73=红灯亮的模式  0x41红灯灭的模式
******************************************************************/
uint8_t PS2_RedLight(void)
{
	CS_OUT(0);
	PS2_Cmd(Comd[0]);  //开始命令
	PS2_Cmd(Comd[1]);  //请求数据
	CS_OUT(1);
	if( Data[1] == 0X73)   return 0 ;
	else return 1;
}


/******************************************************************
 * 函 数 名 称：PS2_ClearData
 * 函 数 说 明：清除数据缓冲区
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void PS2_ClearData(void)
{
	uint8_t a;
	for(a=0;a<9;a++)
		Data[a]=0x00;
}


//对读出来的PS2的数据进行处理      
//只处理了按键部分,默认数据是红灯模式,只有一个按键按下时           
//按下为0， 未按下为1


/******************************************************************
 * 函 数 名 称：PS2_DataKey
 * 函 数 说 明：对读出来的PS2的数据进行处理，处理了按键部分
 * 函 数 形 参：无
 * 函 数 返 回：0=无按键按下   其他=对应键值，返回的值见数组MASK里的按键
 * 作       者：LC
 * 备       注：当有按键按下，对应位为“0”，其他位为“1”，
 *              例如当键“SELECT”被按下时，Data[3]=11111110B。
 *                                                 低为在前         									  
******************************************************************/
uint8_t PS2_DataKey(void)
{
	uint8_t index=0;
	uint16_t Handkey=0;
	
	PS2_ClearData();//清除接收缓存
	PS2_ReadData();//读取PS2发送过来的数据

	//这是16个按键  按下为0， 未按下为1
	Handkey=(Data[4]<<8)|Data[3];     
	
	for(index=0;index<16;index++)
	{	    
		if( (Handkey & ( 1 << (MASK[index]-1) ) ) ==0 )
		{
			return index+1;//返回键值
		}
	}
	return 0;          //没有任何按键按下 
}

/******************************************************************
 * 函 数 名 称：PS2_AnologData
 * 函 数 说 明：得到一个摇杆的模拟量
 * 函 数 形 参：button=摇杆的哪个方向的模拟量，取值范围有：
 *              PSS_LX  左摇杆左右方向的模拟量
 *              PSS_LY  左摇杆上下方向的模拟量
 *              PSS_RX  右摇杆左右方向的模拟量
 *              PSS_RY  右摇杆上下方向的模拟量
 * 函 数 返 回：摇杆的模拟量
 * 作       者：LC
 * 备       注：范围0~255
******************************************************************/
uint8_t PS2_AnologData(uint8_t button)
{
	return Data[button];
}


/******************************************************************
 * 函 数 名 称：PS2_shortPoll
 * 函 数 说 明：手柄配置初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void PS2_shortPoll(void)
{
    CS_OUT(0);
    delay_1us(16);
    PS2_Cmd(0x01);
    PS2_Cmd(0x42);
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    CS_OUT(1);
    delay_1us(16);
}

/******************************************************************
 * 函 数 名 称：PS2_EnterConfing
 * 函 数 说 明：PS2进入配置模式
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void PS2_EnterConfing(void)
{
    CS_OUT(0);
    delay_1us(16);
    PS2_Cmd(0x01);
    PS2_Cmd(0x43);
    PS2_Cmd(0X00);
    PS2_Cmd(0x01);
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    CS_OUT(1);
    delay_1us(16);
    
}

/******************************************************************
 * 函 数 名 称：PS2_TurnOnAnalogMode
 * 函 数 说 明：发送模式设置
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void PS2_TurnOnAnalogMode(void)
{
    CS_OUT(0);
    PS2_Cmd(0x01);
    PS2_Cmd(0x44);
    PS2_Cmd(0X00);
    PS2_Cmd(0x01); //analog=Ox01;digital=Ox00软件设置发送模式    

    PS2_Cmd(0xEE);//Ox03锁存设置，即不可通过按键“MODE”设置模式。
//OxEE不锁存软件设置，可通过按键“MODE”设置模式。

    PS2_Cmd(0X00);
    PS2_Cmd(0X00);
    PS2_Cmd(0X00);
    PS2_Cmd(0x00);
    CS_OUT(1);
    delay_1us(16);
}

/******************************************************************
 * 函 数 名 称：PS2_VibrationMode
 * 函 数 说 明：振动设置
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void PS2_VibrationMode(void)    
{
    CS_OUT(0);
    delay_1us(16);
    PS2_Cmd(0x01);
    PS2_Cmd(0x4D);
    PS2_Cmd(0X00);
    PS2_Cmd(0x00);
    PS2_Cmd(0X01);
    CS_OUT(1);
    delay_1us(16);
}

/******************************************************************
 * 函 数 名 称：PS2_ExitConfing
 * 函 数 说 明：完成并保存配置
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void PS2_ExitConfing(void)
{
    CS_OUT(0);
    delay_1us(16);
    PS2_Cmd(0x01);
    PS2_Cmd(0x43);
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    PS2_Cmd(0x5A);
    PS2_Cmd(0x5A);
    PS2_Cmd(0x5A);
    PS2_Cmd(0x5A);
    PS2_Cmd(0x5A);
    CS_OUT(1);
    delay_1us(16);
}

/******************************************************************
 * 函 数 名 称：PS2_SetInit
 * 函 数 说 明：手柄配置初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void PS2_SetInit(void)
{
    PS2_GPIO_Init();//引脚初始化
    PS2_shortPoll();
    PS2_shortPoll();
    PS2_shortPoll();
    PS2_EnterConfing();//进入配置模式
    PS2_TurnOnAnalogMode();//“红绿灯”配置模式，并选择是否保存
    PS2_VibrationMode();
    PS2_ExitConfing();    //完成并保存配置
}