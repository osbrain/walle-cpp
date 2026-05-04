#include "bsp_eye.h"


int EYE_L_STATUS = 0;

int EYE_R_STATUS = 0;

void eye_gpio_config(void)
{
		  /* 使能时钟 */
			rcu_periph_clock_enable(RCU_EYE);
			/* 配置为输出模式 浮空模式 */
			gpio_mode_set(PORT_EYE,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,EYE_L_PIN | EYE_R_PIN);
			/* 配置为推挽输出 50MHZ */
			gpio_output_options_set(PORT_EYE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,EYE_L_PIN | EYE_R_PIN); 
}

void eye_left_on(void)
{
		EYE_L_ON;
		EYE_L_STATUS=1;
}

void eye_left_off(void)
{
		EYE_L_OFF;
		EYE_L_STATUS=0;
}

void eye_right_on(void)
{
		EYE_R_ON;
		EYE_R_STATUS=1;
}

void eye_right_off(void)
{
		EYE_R_OFF;
		EYE_R_STATUS=0;
}

void switch_eye_left_status(){
		if(EYE_L_STATUS==0){
			eye_left_on();
		}else{
			eye_left_off();
		}
}

void switch_eye_right_status(){
		if(EYE_R_STATUS==0){
			eye_right_on();
		}else{
			eye_right_off();
		}
}
