#include "bsp_timer.h"
#include "bsp_led.h"
#include "stdio.h"

/************************************************
函数名称 ： basic_timer5_config
功    能 ： 基本定时器配置
参    数 ： prescaler：时钟预分频值
					  period：周期 
返 回 值 ： 无
作    者 ： 少不入川
*************************************************/
void basic_timer5_config(uint16_t prescaler,uint16_t period)
{
	/* 一个周期的时间T = 1/f, 定时时间time = T * 周期
    设预分频值位pre,周期位per
    time = (pre + 1) * (per + 1) / psc_clk
	*/
	timer_parameter_struct timer_initpara; 							 // 定义定时器结构体
//01	/* 开启时钟 */
	rcu_periph_clock_enable(BSP_TIMER_RCU5); 						 // 开启定时器时钟
	/* CK_TIMERx = 4 x CK_APB1  = 4x50M = 200MHZ */
	rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);// 配置定时器时钟
																			/*四分频*/

	timer_deinit(BSP_TIMER5);														 // 复位定时器
//02	/* 配置定时器参数 */
	timer_initpara.prescaler = prescaler-1;              // 时钟预分频值 0-65535   计算器时钟psc_clk = CK_TIMER / pre
	timer_initpara.alignedmode = TIMER_COUNTER_EDGE;     // 边缘对齐                  
	timer_initpara.counterdirection = TIMER_COUNTER_UP;  // 向上计数    
	timer_initpara.period = period-1;                    // 周期  
	/* 在输入捕获的时候使用  数字滤波器使用的采样频率之间的分频比例 */
	timer_initpara.clockdivision = TIMER_CKDIV_DIV1;     // 分频因子         
  /* 只有高级定时器才有 配置为x，就重复x+1次进入中断 */    
	timer_initpara.repetitioncounter = 0;							   // 重复计数器 0-255  
	
	timer_init(BSP_TIMER5,&timer_initpara);							 // 初始化定时器
	
	/****************************************/
	/* 定时时间 = prescaler/CK_TIMER*period *///《=======/* 1s = 20000/200M*10000 */
	/****************************************/
	
//03	/* 配置中断优先级 */
	nvic_irq_enable(BSP_TIMER_IRQ5,3,2); 								 // 设置中断优先级为 3,2
	
//04	/* 使能中断、定时器 */
	/* 使能中断 */
	timer_interrupt_enable(BSP_TIMER5,TIMER_INT_UP);     // 使能更新事件中断 
	/* 使能中断、定时器 */
	timer_enable(BSP_TIMER5);
}
/************************************************
函数名称 ： universal_timer2_config
功    能 ： 通用定时器配置
参    数 ： pre：时钟预分频值
					  per：周期 
返 回 值 ： 无
作    者 ： 少不入川
*************************************************/
void universal_timer2_config(uint16_t prescaler,uint16_t period)
{
	timer_parameter_struct timer_initpara; 								// 定义定时器结构体
//01	/* 开启时钟 */
	rcu_periph_clock_enable(BSP_TIMER_RCU2); 							// 开启定时器时钟
	rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4); // 配置定时器时钟
	
	timer_deinit(BSP_TIMER2);														  // 复位定时器
//02	/* 配置定时器参数 */	
	/****************************************/
	/* 定时时间 = prescaler/CK_TIMER*period *///《=======/* 1s = 20000/200M*10000 */
	/****************************************/
  timer_initpara.prescaler         = prescaler-1; 
  timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
  timer_initpara.counterdirection  = TIMER_COUNTER_UP;
  timer_initpara.period            = period-1;
  timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
  timer_initpara.repetitioncounter = 0;
	
	timer_init(BSP_TIMER2,&timer_initpara);								// 初始化定时器
//03	/* 配置中断优先级 */
	nvic_irq_enable(BSP_TIMER_IRQ2,2,1); 									// 设置中断优先级
//04	/* 使能中断、定时器 */
	timer_interrupt_enable(BSP_TIMER2,TIMER_INT_UP);      // 使能更新事件中断 
	
	timer_enable(BSP_TIMER2);
}
/************************************************
函数名称 ： BSP_TIMER5_IRQHandler
功    能 ： 基本定时器5中断服务函数 
参    数 ： 无
返 回 值 ： 无
作    者 ： 少不入川
*************************************************/
void BSP_TIMER5_IRQHANDLER(void)
{
	/* 这里是定时器中断 */
  if(timer_interrupt_flag_get(BSP_TIMER5,TIMER_INT_FLAG_UP) == SET)
	{
		timer_interrupt_flag_clear(BSP_TIMER5,TIMER_INT_FLAG_UP);  // 清除中断标志位 
		/* 执行功能 */
		gpio_bit_toggle(PORT_LED1,PIN_LED1);                       // 翻转led
		printf("BSP_TIMER5_IRQHANDLER!\r\n");          						 // 串口打印
	}
}
/************************************************
函数名称 ： BSP_TIMER2_IRQHandler
功    能 ： 通用定时器2中断服务函数 
参    数 ： 无
返 回 值 ： 无
作    者 ： 少不入川
*************************************************/
void BSP_TIMER2_IRQHANDLER(void)
{
	/* 这里是定时器中断 */
  if(timer_interrupt_flag_get(BSP_TIMER2,TIMER_INT_FLAG_UP) == SET)
	{
		timer_interrupt_flag_clear(BSP_TIMER2,TIMER_INT_FLAG_UP);  // 清除中断标志位 
		/* 执行功能 */
		gpio_bit_toggle(PORT_LED2,PIN_LED2);                       // 翻转led
		printf("BSP_TIMER2_IRQHANDLER!\r\n");          						 // 串口打印
	}
}