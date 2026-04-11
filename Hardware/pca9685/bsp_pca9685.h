#ifndef _BSP_PCA9685_H_
#define _BSP_PCA9685_H_

#include "gd32f4xx.h"


//端口移植
#define RCU_SDA  RCU_GPIOB
#define PORT_SDA GPIOB
#define GPIO_SDA GPIO_PIN_9

#define RCU_SCL  RCU_GPIOB
#define PORT_SCL GPIOB
#define GPIO_SCL GPIO_PIN_8

//设置SDA输出模式
#define SDA_OUT()	gpio_mode_set(PORT_SDA,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_SDA)
//设置SDA输入模式
#define SDA_IN()	gpio_mode_set(PORT_SDA,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,GPIO_SDA)
//获取SDA引脚的电平变化
#define SDA_GET()	gpio_input_bit_get(PORT_SDA,GPIO_SDA)
//SDA与SCL输出
#define SDA(x)          gpio_bit_write(PORT_SDA,GPIO_SDA, (x?SET:RESET))
#define SCL(x)          gpio_bit_write(PORT_SCL,GPIO_SCL, (x?SET:RESET))

#define PCA_Addr 		0x80	//IIC地址
#define PCA_Model 	    0x00	
#define LED0_ON_L 	    0x06
#define LED0_ON_H 	    0x07
#define LED0_OFF_L 	    0x08
#define LED0_OFF_H 	    0x09
#define PCA_Pre 		0xFE	//配置频率地址

void PCA9685_Init(float hz,uint8_t angle);
void setAngle(uint8_t num,uint8_t angle);
void setAngle2(uint8_t num1,uint8_t num2,uint8_t angle);
void PCA9685_setFreq(float freq);
void PCA9685_setPWM(uint8_t num,uint32_t on,uint32_t off);
void PCA9685_setPWM2(uint8_t num1,uint8_t num2,uint32_t on,uint32_t off);

#endif
