#ifndef _BSP_USART_H
#define _BSP_USART_H

#include "gd32f4xx.h"
#include "systick.h"


//下载调试接口
#define BSP_USART_TX_RCU  		RCU_GPIOA   			// 串口0TX的端口时钟
#define BSP_USART_RX_RCU  		RCU_GPIOA   			// 串口0RX的端口时钟
#define BSP_USART_RCU     		RCU_USART0  			// 串口0的时钟

#define BSP_USART_TX_PORT 		GPIOA							// 串口0TX的端口
#define BSP_USART_RX_PORT 		GPIOA							// 串口0RX的端口
#define BSP_USART_AF 					GPIO_AF_7   			// 串口0的复用功能
#define BSP_USART_TX_PIN  		GPIO_PIN_9  			// 串口0TX的引脚
#define BSP_USART_RX_PIN  		GPIO_PIN_10 			// 串口0RX的引脚

#define BSP_USART 						USART0      			// 串口0
#define BSP_USART_IRQ     		USART0_IRQn 			// 串口0中断
#define BSP_USART_IRQHandler  USART0_IRQHandler	// 串口0中断服务函数

//VC-02连接接口
#define BSP_USART2_TX_RCU  		RCU_GPIOC   			// 串口2TX的端口时钟
#define BSP_USART2_RX_RCU 		RCU_GPIOC     		// 串口2RX的端口时钟
#define BSP_USART2_RCU     		RCU_USART2   			// 串口2的时钟

#define BSP_USART2_TX_PORT 		GPIOC		    			// 串口2TX的端口
#define BSP_USART2_RX_PORT 		GPIOC			 				// 串口2RX的端口
#define BSP_USART2_AF 				GPIO_AF_7  		 		// 串口2的复用功能
#define BSP_USART2_TX_PIN  		GPIO_PIN_10  			// 串口2TX的引脚
#define BSP_USART2_RX_PIN  		GPIO_PIN_11 			// 串口2RX的引脚

#define BSP_USART2 						USART2      	 	 	// 串口2
#define BSP_USART2_IRQ     		USART2_IRQn 		 	// 串口2中断
#define BSP_USART2_IRQHandler USART2_IRQHandler // 串口2中断服务函数

//蓝牙连接接口
#define BSP_UART6_TX_RCU  		RCU_GPIOF   			// 串口6TX的端口时钟
#define BSP_UART6_RX_RCU 			RCU_GPIOF     		// 串口6RX的端口时钟
#define BSP_UART6_RCU     		RCU_UART6   			// 串口6的时钟

#define BSP_UART6_TX_PORT 		GPIOF		    			// 串口6TX的端口
#define BSP_UART6_RX_PORT 		GPIOF			 				// 串口6RX的端口
#define BSP_UART6_AF 					GPIO_AF_8  		 		// 串口6的复用功能
#define BSP_UART6_TX_PIN  		GPIO_PIN_7  			// 串口6TX的引脚
#define BSP_UART6_RX_PIN  		GPIO_PIN_6 				// 串口6RX的引脚

#define BSP_UART6 						UART6      	 	 		// 串口6
#define BSP_UART6_IRQ     		UART6_IRQn 		 		// 串口6中断
#define BSP_UART6_IRQHandler  UART6_IRQHandler  // 串口6中断服务函数



/* 串口缓冲区的数据长度 */
#define USART_RECEIVE_LENGTH  4096


/* 0 : 中断接收  1 ：DMA接收 */
#define USB_USART_DMA 1

//下载调试
extern uint8_t 	  g_recv_buff[USART_RECEIVE_LENGTH]; 					// 接收缓冲区
extern uint16_t		g_recv_length;										 					// 接收数据长度
extern uint8_t  	g_recv_complete_flag; 						 					// 接收完成标志位
//VC-02
extern uint8_t 		g_recv_buff_usart2[USART_RECEIVE_LENGTH];   // 接收缓冲区
extern uint16_t 	g_recv_length_usart2;									  		// 接收数据长度
extern uint8_t		g_recv_complete_flag_usart2; 					  		// 接收数据完成标志位
//蓝牙连接
extern uint8_t 		g_recv_buff_uart6[USART_RECEIVE_LENGTH];    // 接收缓冲区
extern uint16_t 	g_recv_length_uart6;									  		// 接收数据长度
extern uint8_t		g_recv_complete_flag_uart6; 					  		// 接收数据完成标志位


extern char *str1;extern char *str2; 
extern char *str3;extern char *str4;
extern char *str5;extern char *str6; 
extern char *str7;extern char *str8;


void usart_gpio_config(uint32_t band_rate);  			 // 配置串口0
void usart_send_data(uint8_t ucch);          			 // 发送一个字符
void usart_send_string(uint8_t *ucstr);      			 // 发送一个字符串
void usart2_gpio_config(uint32_t band_rate);	 		 // 配置串口2
void uart6_gpio_config(uint32_t band_rate);	 			 // 配置串口6


#endif