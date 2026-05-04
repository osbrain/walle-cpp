#include "bsp_motor.h"

static void motor_L_pwm_off(void)
{
	timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_0,0);
	timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_1,0);
}

static void motor_R_pwm_off(void)
{
	timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_2,0);
	timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_3,0);
}

static void motor_pwm_off(void)
{
	motor_L_pwm_off();
	motor_R_pwm_off();
}

static uint16_t motor_turn_inner_speed(uint16_t speed)
{
	uint16_t inner_speed = (uint16_t)(speed / 2U);

	if ((speed > 0U) && (inner_speed == 0U)) {
		inner_speed = 1U;
	}

	return inner_speed;
}

static void motor_left_set(int16_t speed)
{
	if (speed > 0) {
		timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_1,0);
		timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_0,(uint16_t)speed);
	} else if (speed < 0) {
		timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_0,0);
		timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_1,(uint16_t)(-speed));
	} else {
		motor_L_pwm_off();
	}
}

static void motor_right_set(int16_t speed)
{
	if (speed > 0) {
		timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_3,0);
		timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_2,(uint16_t)speed);
	} else if (speed < 0) {
		timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_2,0);
		timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_3,(uint16_t)(-speed));
	} else {
		motor_R_pwm_off();
	}
}

static void motor_drive(int16_t left_speed, int16_t right_speed)
{
	motor_left_set(left_speed);
	motor_right_set(right_speed);
}


/************************************************
函数名称 ： motor_gpio_config
功    能 ： 电机驱动gpio引脚配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
void motor_gpio_config(void)
{
	motor_pwm_off();

	/* 使能时钟 */
	rcu_periph_clock_enable(RCU_IN1_A);
	/* 四个电机输入脚常驻PWM复用模式，停止时通过占空比0保证输出低电平。 */
	gpio_mode_set(PORT_IN1_A,GPIO_MODE_AF,GPIO_PUPD_PULLDOWN,PIN_IN1_A);
	/* 配置为推挽输出 50MHZ */
	gpio_output_options_set(PORT_IN1_A,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,PIN_IN1_A);
	gpio_af_set(PORT_IN1_A,TIMER1_CH0,PIN_IN1_A);//配置GPIO的复用
	
	/* 使能时钟 */
	rcu_periph_clock_enable(RCU_IN2_A);
	/* 四个电机输入脚常驻PWM复用模式，停止时通过占空比0保证输出低电平。 */
	gpio_mode_set(PORT_IN2_A,GPIO_MODE_AF,GPIO_PUPD_PULLDOWN,PIN_IN2_A);
	/* 配置为推挽输出 50MHZ */
	gpio_output_options_set(PORT_IN2_A,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,PIN_IN2_A);
	gpio_af_set(PORT_IN2_A,TIMER1_CH1,PIN_IN2_A);//配置GPIO的复用
	
	/* 使能时钟 */
	rcu_periph_clock_enable(RCU_IN1_B);
	/* 四个电机输入脚常驻PWM复用模式，停止时通过占空比0保证输出低电平。 */
	gpio_mode_set(PORT_IN1_B,GPIO_MODE_AF,GPIO_PUPD_PULLDOWN,PIN_IN1_B);
	/* 配置为推挽输出 50MHZ */
	gpio_output_options_set(PORT_IN1_B,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,PIN_IN1_B);
	gpio_af_set(PORT_IN1_B,TIMER1_CH2,PIN_IN1_B);//配置GPIO的复用
	
	/* 使能时钟 */
	rcu_periph_clock_enable(RCU_IN2_B);
	/* 四个电机输入脚常驻PWM复用模式，停止时通过占空比0保证输出低电平。 */
	gpio_mode_set(PORT_IN2_B,GPIO_MODE_AF,GPIO_PUPD_PULLDOWN,PIN_IN2_B);
	/* 配置为推挽输出 50MHZ */
	gpio_output_options_set(PORT_IN2_B,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,PIN_IN2_B);
	gpio_af_set(PORT_IN2_B,TIMER1_CH3,PIN_IN2_B);//配置GPIO的复用

	motor_pwm_off();
	
}
/************************************************
函数名称 ： motor_L_front
功    能 ： 左轮按前进方向转动
参    数 ： speed
返 回 值 ： 无
*************************************************/
void motor_L_front(uint16_t speed)
{
	motor_left_set((int16_t)speed);
}

/************************************************
函数名称 ： motor_L_back
功    能 ： 左轮按后退方向转动
参    数 ： speed
返 回 值 ： 无
*************************************************/
void motor_L_back(uint16_t speed)
{
	motor_left_set(-(int16_t)speed);
}

/************************************************
函数名称 ： motor_L_stop
功    能 ： 左轮停止
参    数 ： stopMode=0：熄火停止；stopMode=1：刹车停止
返 回 值 ： 无
*************************************************/
void motor_L_stop(uint16_t stopMode)
{
	(void)stopMode;
	motor_L_pwm_off();
}
/************************************************
函数名称 ： motor_R_front
功    能 ： 右轮按前进方向转动
参    数 ： speed
返 回 值 ： 无
*************************************************/
void motor_R_front(uint16_t speed)
{
	motor_right_set((int16_t)speed);
}

/************************************************
函数名称 ： motor_R_back
功    能 ： 右轮按后退方向转动
参    数 ： speed
返 回 值 ： 无
*************************************************/
void motor_R_back(uint16_t speed)
{
	motor_right_set(-(int16_t)speed);
}

/************************************************
函数名称 ： motor_R_stop
功    能 ： 右轮停止
参    数 ： stopMode=0：熄火停止；stopMode=1：刹车停止
返 回 值 ： 无
*************************************************/
void motor_R_stop(uint16_t stopMode)
{
	(void)stopMode;
	motor_R_pwm_off();
}

/************************************************
函数名称 ： motor_forward
功    能 ： 两个电机按前进方向同速转动
参    数 ： speed 速度
返 回 值 ： 无
*************************************************/
void motor_forward(uint16_t speed)	// 电机前进
{
	motor_drive((int16_t)speed, (int16_t)speed);
}

/************************************************
函数名称 ： motor_backward
功    能 ： 两个电机按后退方向同速转动
参    数 ： speed 速度
返 回 值 ： 无
*************************************************/
void motor_backward(uint16_t speed)	// 电机后退
{
	motor_drive(-(int16_t)speed, -(int16_t)speed);
}

/************************************************
函数名称 ： motor_rightward
功    能 ： 电机右转
参    数 ： speed 速度
返 回 值 ： 无
*************************************************/
void motor_rightward(uint16_t speed)	// 电机右转
{
	motor_drive((int16_t)speed, (int16_t)motor_turn_inner_speed(speed));
}

/************************************************
函数名称 ： motor_leftward
功    能 ： 电机左转
参    数 ： speed 速度
返 回 值 ： 无
*************************************************/
void motor_leftward(uint16_t speed)// 电机左转
{
	motor_drive((int16_t)motor_turn_inner_speed(speed), (int16_t)speed);
}

/************************************************
函数名称 ： motor_stop
功    能 ： 电机停止
参    数 ： stopMode=0：熄火停止；stopMode=1：刹车停止
返 回 值 ： 无
*************************************************/
void motor_stop(uint16_t stopMode)//电机停止
{
	(void)stopMode;
	motor_pwm_off();
}
