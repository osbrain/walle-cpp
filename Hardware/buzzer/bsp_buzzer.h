#ifndef _BSP_BUZZER_H
#define _BSP_BUZZER_H

#include "gd32f4xx.h"
#include "systick.h"


#define RCU_BUZZER    RCU_GPIOF
#define PORT_BUZZER   GPIOF
#define BUZZER_PIN    GPIO_PIN_8

#define BUZZER_ON     gpio_bit_write(GPIOF,GPIO_PIN_8,RESET)
#define BUZZER_OFF    gpio_bit_write(GPIOF,GPIO_PIN_8,SET)

void buzzer_gpio_config(void);

void switch_buzzer_status();

#endif /*_BSP_BUZZER_H*/