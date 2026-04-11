#include "bsp_buzzer.h"

int BUZZER_STATUS = 0;

void buzzer_gpio_config(void)
{
		  /* 使能时钟 */
			rcu_periph_clock_enable(RCU_BUZZER);
			/* 配置为输出模式 浮空模式 */
			gpio_mode_set(PORT_BUZZER,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,BUZZER_PIN);
			/* 配置为推挽输出 50MHZ */
			gpio_output_options_set(PORT_BUZZER,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,BUZZER_PIN);
}

void switch_buzzer_status(){
	if(BUZZER_STATUS==0){
		BUZZER_ON;
		BUZZER_STATUS=1;
	}else{
		BUZZER_OFF;
		BUZZER_STATUS=0;
	}
}