#ifndef _BSP_TIMER_H
#define _BSP_TIMER_H

#include "gd32f4xx.h"
#include "systick.h"
/* 基本定时器5 */
#define BSP_TIMER_RCU5  				RCU_TIMER5            	// 定时器时钟
#define BSP_TIMER5      				TIMER5                	// 定时器
#define BSP_TIMER_IRQ5  				TIMER5_DAC_IRQn       	// 定时器中断
#define BSP_TIMER5_IRQHANDLER   TIMER5_DAC_IRQHandler 	// 定时器中断服务函数
/* 通用定时器2 */
#define BSP_TIMER_RCU2  				RCU_TIMER2					  	// 定时器时钟
#define BSP_TIMER2      				TIMER2									// 定时器
#define BSP_TIMER_IRQ2  			  TIMER2_IRQn							// 定时器中断
#define BSP_TIMER2_IRQHANDLER   TIMER2_IRQHandler				// 定时器中断服务函数


void basic_timer5_config(uint16_t prescaler,uint16_t period);			// 基本定时器配置
void universal_timer2_config(uint16_t prescaler,uint16_t period); // 通用定时器配置

#endif  /* BSP_TIMER_H */
