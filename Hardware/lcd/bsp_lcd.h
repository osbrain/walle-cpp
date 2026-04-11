#ifndef _BSP_LCD_H
#define _BSP_LCD_H


#include "sys.h"
#include "stdlib.h"


#define USE_HORIZONTAL 0  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 240
#define LCD_H 240

#else
#define LCD_W 240
#define LCD_H 240
#endif

#define	u8 unsigned char
#define	u16 unsigned int
#define	u32 unsigned long


//-----------------LCD端口定义---------------- 
#define LCD_SCL_RCU    RCU_GPIOB
#define LCD_SCL_PORT   GPIOB
#define LCD_SCL_PIN    GPIO_PIN_3

#define LCD_RES_RCU    RCU_GPIOB
#define LCD_RES_PORT   GPIOB
#define LCD_RES_PIN    GPIO_PIN_4

#define LCD_SDA_RCU    RCU_GPIOB
#define LCD_SDA_PORT   GPIOB
#define LCD_SDA_PIN    GPIO_PIN_5

#define LCD_CS_RCU     RCU_GPIOB
#define LCD_CS_PORT    GPIOB
#define LCD_CS_PIN     GPIO_PIN_6

#define LCD_SDC_RCU    RCU_GPIOB
#define LCD_SDC_PORT   GPIOB
#define LCD_SDC_PIN    GPIO_PIN_7

#define LCD_BLK_RCU    RCU_GPIOB
#define LCD_BLK_PORT   GPIOB
#define LCD_BLK_PIN    GPIO_PIN_12
   						  
//-----------------测试LED端口定义---------------- 

#define LED_ON 		gpio_bit_write(GPIOB,GPIO_PIN_11,RESET)
#define LED_OFF 	gpio_bit_write(GPIOB,GPIO_PIN_11,SET)

//-----------------LCD端口设置---------------- 

#define OLED_SCLK_Clr() 	gpio_bit_write(GPIOB,GPIO_PIN_3,RESET)//SCL     
#define OLED_SCLK_Set() 	gpio_bit_write(GPIOB,GPIO_PIN_3,SET)	

#define OLED_RST_Clr() 		gpio_bit_write(GPIOB,GPIO_PIN_4,RESET)//RES
#define OLED_RST_Set() 		gpio_bit_write(GPIOB,GPIO_PIN_4,SET)

#define OLED_SDIN_Clr() 	gpio_bit_write(GPIOB,GPIO_PIN_5,RESET)//SDA
#define OLED_SDIN_Set() 	gpio_bit_write(GPIOB,GPIO_PIN_5,SET)

#define OLED_CS_Clr()  		gpio_bit_write(GPIOB,GPIO_PIN_6,RESET)//CS
#define OLED_CS_Set()  		gpio_bit_write(GPIOB,GPIO_PIN_6,SET)

#define OLED_DC_Clr() 		gpio_bit_write(GPIOB,GPIO_PIN_7,RESET)//DC
#define OLED_DC_Set() 		gpio_bit_write(GPIOB,GPIO_PIN_7,SET)	 
 		     
#define OLED_BLK_Clr()  	gpio_bit_write(GPIOB,GPIO_PIN_12,RESET)//BLK
#define OLED_BLK_Set()  	gpio_bit_write(GPIOB,GPIO_PIN_12,SET)

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

extern  u16 BACK_COLOR;   //背景色

void LCD_Writ_Bus(u8 dat);//LCD串行数据写入函数
void LCD_WR_DATA8(u8 dat);//LCD写入数据
void LCD_WR_DATA(u16 dat);//LCD写入数据
void LCD_WR_REG(u8 dat);//LCD写入命令
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//设置起始和结束地址
void lcd_gpio_config(void);//LCD初始化函数 
void LCD_Clear(u16 Color);//LCD清屏函数
void LCD_ShowChinese(u16 x,u16 y,u8 index,u8 size,u16 color);//LCD显示汉字
void LCD_DrawPoint(u16 x,u16 y,u16 color);//LCD画点
void LCD_DrawPoint_big(u16 x,u16 y,u16 colory);//LCD画一个大的点
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);//在指定区域填充颜色
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);//画线
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);//画矩形
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color);//画圆
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 mode,u16 color);//显示字符
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 color);//显示字符串
u32 mypow(u8 m,u8 n);//数字
void LCD_ShowNum(u16 x,u16 y,u16 num,u8 len,u16 color);//显示数字
void LCD_ShowNum1(u16 x,u16 y,float num,u8 len,u16 color);//显示小数
void LCD_ShowPicture4040(u16 x1,u16 y1,u16 x2,u16 y2,const u8 pic[]);//显示40x40图片
void LCD_ShowPicture200200(u16 x1,u16 y1,u16 x2,u16 y2,const u8 pic[]);//显示200x200图片
void LCD_ShowPicture(u16 x,u16 y,u16 length,u16 width,const u8 pic[]);//显示图片

//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40 //棕色
#define BRRED 			     0XFC07 //棕红色
#define GRAY  			     0X8430 //灰色
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 

#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY 			     0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)

#endif /*_BSP_LCD_H*/