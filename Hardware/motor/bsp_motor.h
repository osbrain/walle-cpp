#ifndef _BSP_MOTOR_H
#define _BSP_MOTOR_H


#include "gd32f4xx.h"
#include "systick.h"
#include "sys.h"
#include "bsp_pwm.h"

/* 左电机方向A输入：IN1_A --> PA0 / TIMER1_CH0 */
#define RCU_IN1_A  		RCU_GPIOA   	// GPIOA的时钟
#define PORT_IN1_A 		GPIOA			// GPIOA的端口
#define PIN_IN1_A 		GPIO_PIN_0  	// GPIOA的引脚
#define TIMER1_CH0  	GPIO_AF_1		// PWM引脚复用
#define L_FRONT 		PAout(0)
/* 左电机方向B输入：IN2_A --> PA1 / TIMER1_CH1 */
#define RCU_IN2_A  		RCU_GPIOA   	// GPIOA的时钟
#define PORT_IN2_A 		GPIOA			// GPIOA的端口
#define PIN_IN2_A 		GPIO_PIN_1  	// GPIOA的引脚
#define TIMER1_CH1  	GPIO_AF_1		// PWM引脚复用
#define L_BACK 			PAout(1)
/* 右电机方向A输入：IN1_B --> PA2 / TIMER1_CH2 */
#define RCU_IN1_B  		RCU_GPIOA   	// GPIOA的时钟
#define PORT_IN1_B 		GPIOA			// GPIOA的端口
#define PIN_IN1_B 		GPIO_PIN_2  	// GPIOA的引脚
#define TIMER1_CH2  	GPIO_AF_1		// PWM引脚复用
#define R_FRONT 		PAout(2)
/* 右电机方向B输入：IN2_B --> PA3 / TIMER1_CH3 */
#define RCU_IN2_B  		RCU_GPIOA   	// GPIOA的时钟
#define PORT_IN2_B 		GPIOA			// GPIOA的端口
#define PIN_IN2_B 		GPIO_PIN_3  	// GPIOA的引脚
#define TIMER1_CH3  	GPIO_AF_1		// PWM引脚复用
#define R_BACK			PAout(3)


#define Flameout_Stop 	0
#define Brake_Stop 		1


void motor_gpio_config(void);        // 电机驱动gpio引脚配置

void motor_L_front(uint16_t speed);  // 左轮按前进方向转动
void motor_L_back(uint16_t speed);   // 左轮按后退方向转动
void motor_L_stop(uint16_t stopMode);// 左轮停止
void motor_R_front(uint16_t speed);  // 右轮按前进方向转动
void motor_R_back(uint16_t speed);   // 右轮按后退方向转动
void motor_R_stop(uint16_t stopMode);// 右轮停止

void motor_forward(uint16_t speed);	 // 两个电机按前进方向同速转动
void motor_backward(uint16_t speed); // 两个电机按后退方向同速转动
void motor_leftward(uint16_t speed); // 左慢右快差速左转
void motor_rightward(uint16_t speed);// 左快右慢差速右转
void motor_stop(uint16_t stopMode);	 // 四路PWM清零停止

#endif  /* _BSP_MOTOR_H */
