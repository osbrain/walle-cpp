#include "bsp_pwm.h"
#include "sys.h"



/************************************************
函数名称 ： pwm1_config
功    能 ： 定时器1的PWM模式配置 
参    数 ： 无
返 回 值 ： 无
作    者 ： LC
*************************************************/
void pwm1_config(uint16_t pre,uint16_t per)
{
	timer_parameter_struct timere_initpara;               // 定义定时器结构体
	timer_oc_parameter_struct timer_ocintpara;					  // 定时器比较输出结构体
	
	rcu_periph_clock_enable(BSP_PWM1_TIMER_RCU);					// 开启定时器时钟
	rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);	// 配置定时器时钟
	
	/* 配置定时器参数 */
	timer_deinit(BSP_PWM1_TIMER);													 // 复位定时器
	timere_initpara.prescaler = pre-1;                   	 // 时钟预分频值   PSC_CLK= 200MHZ / 200 = 1MHZ       
	timere_initpara.alignedmode = TIMER_COUNTER_EDGE;      // 边缘对齐                
	timere_initpara.counterdirection = TIMER_COUNTER_UP;   // 向上计数    	                    
	timere_initpara.period = per-1;      						    	 // 周期    T = 10000 * 1MHZ = 10ms  f = 100HZ   
	/* 在输入捕获的时候使用  数字滤波器使用的采样频率之间的分频比例 */	
	timere_initpara.clockdivision = TIMER_CKDIV_DIV1;    	 // 分频因子
	/* 只有高级定时器才有 配置为x，就重复x+1次进入中断 */
	timere_initpara.repetitioncounter = 0;                 // 重复计数器 0-255  
	timer_init(BSP_PWM1_TIMER,&timere_initpara);			   	 // 初始化定时器
	
	/* 配置输出结构体 */
	timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;    // 有效电平的极性
	timer_ocintpara.outputstate = TIMER_CCX_ENABLE;					// 配置比较输出模式状态 也就是使能PWM输出到端口
	/* 配置定时器输出功能 */
	timer_channel_output_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_0,&timer_ocintpara);			//使能的定时器通道0
	timer_channel_output_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_1,&timer_ocintpara);			//使能的定时器通道1
	timer_channel_output_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_2,&timer_ocintpara);			//使能的定时器通道2
	timer_channel_output_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_3,&timer_ocintpara);			//使能的定时器通道3

	/* 配置占空比 */
	timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_0,0);									// 配置定时器通道0输出脉冲值
	timer_channel_output_mode_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_0,TIMER_OC_MODE_PWM0);				// 配置定时器通道0输出比较模式
	timer_channel_output_shadow_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_0,TIMER_OC_SHADOW_DISABLE);// 配置定时器通道0输出影子寄存器
	
	timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_1,0);									// 配置定时器通道1输出脉冲值
	timer_channel_output_mode_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_1,TIMER_OC_MODE_PWM0);				// 配置定时器通道1输出比较模式
	timer_channel_output_shadow_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_1,TIMER_OC_SHADOW_DISABLE);// 配置定时器通道1输出影子寄存器
	
	timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_2,0);									// 配置定时器通道2输出脉冲值
	timer_channel_output_mode_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_2,TIMER_OC_MODE_PWM0);				// 配置定时器通道2输出比较模式
	timer_channel_output_shadow_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_2,TIMER_OC_SHADOW_DISABLE);// 配置定时器通道2输出影子寄存器
	
	timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_3,0);									// 配置定时器通道3输出脉冲值
	timer_channel_output_mode_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_3,TIMER_OC_MODE_PWM0);				// 配置定时器通道3输出比较模式
	timer_channel_output_shadow_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_3,TIMER_OC_SHADOW_DISABLE);// 配置定时器通道3输出影子寄存器
	
	timer_auto_reload_shadow_enable(BSP_PWM1_TIMER);
	 /* 使能定时器 */
	timer_enable(BSP_PWM1_TIMER);
}

