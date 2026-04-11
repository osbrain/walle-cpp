#ifndef _BSP_LED_H
#define _BSP_LED_H

#include "gd32f4xx.h"
#include "systick.h"
/*LED1 --> PE3*/
#define RCU_LED1  	RCU_GPIOE   // GPIOE的时钟
#define PORT_LED1 	GPIOE				// GPIOE的端口
#define PIN_LED1 		GPIO_PIN_3  // GPIOE的引脚
/*LED2 --> PD7*/
#define RCU_LED2  	RCU_GPIOD   // GPIOD的时钟
#define PORT_LED2 	GPIOD				// GPIOD的端口
#define PIN_LED2 		GPIO_PIN_7  // GPIOD的引脚
/*LED3 --> PG3*/
#define RCU_LED3  	RCU_GPIOG   // GPIOG的时钟
#define PORT_LED3 	GPIOG				// GPIOG的端口
#define PIN_LED3 		GPIO_PIN_3  // GPIOG的引脚
/*LED4 --> PA5*/
#define RCU_LED4  	RCU_GPIOA   // GPIOA的时钟
#define PORT_LED4 	GPIOA				// GPIOA的端口
#define PIN_LED4 		GPIO_PIN_5  // GPIOA的引脚

void led_gpio_config(void); 	  // led gpio引脚配置

#endif /* BSP_LED_H */