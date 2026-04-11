#ifndef _BSP_EYE_H
#define _BSP_EYE_H

#include "gd32f4xx.h"
#include "systick.h"


#define RCU_EYE      RCU_GPIOB
#define PORT_EYE     GPIOB
#define EYE_L_PIN    GPIO_PIN_0
#define EYE_R_PIN    GPIO_PIN_1

#define EYE_L_ON     gpio_bit_write(GPIOB,GPIO_PIN_0,SET)
#define EYE_L_OFF    gpio_bit_write(GPIOB,GPIO_PIN_0,RESET)
#define EYE_R_ON     gpio_bit_write(GPIOB,GPIO_PIN_1,SET)
#define EYE_R_OFF    gpio_bit_write(GPIOB,GPIO_PIN_1,RESET)

void eye_gpio_config(void);

void switch_eye_left_status();

void switch_eye_right_status();

#endif /*_BSP_EYE_H*/