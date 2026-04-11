#ifndef _ADC_H
#define _ADC_H

#include "gd32f4xx.h"
#include "systick.h"
#include "bsp_usart.h"

/* PC5 ADC0_1__IN15*/
#define ADC_RCU    RCU_GPIOC
#define ADC_PORT   GPIOC
#define ADC_PIN    GPIO_PIN_5

extern uint16_t 	adcValue;

void adc_config(void);
uint16_t adc_channel_sample(uint8_t channel);
void adc_get_val(void);

#endif /* ADC_H */

