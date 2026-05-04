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
	/* 上电初始化先配置为普通输出并拉低，避免电机驱动输入悬空误动作。 */
	gpio_mode_set(PORT_IN1_A,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,PIN_IN1_A);
	/* 配置为推挽输出 50MHZ */
	gpio_output_options_set(PORT_IN1_A,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,PIN_IN1_A);
	L_FRONT=0;
	gpio_af_set(PORT_IN1_A,TIMER1_CH0,PIN_IN1_A);//配置GPIO的复用
	
	/* 使能时钟 */
	rcu_periph_clock_enable(RCU_IN2_A);
	/* 上电初始化先配置为普通输出并拉低，避免电机驱动输入悬空误动作。 */
	gpio_mode_set(PORT_IN2_A,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,PIN_IN2_A);
	/* 配置为推挽输出 50MHZ */
	gpio_output_options_set(PORT_IN2_A,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,PIN_IN2_A);
	L_BACK=0;
	gpio_af_set(PORT_IN2_A,TIMER1_CH1,PIN_IN2_A);//配置GPIO的复用
	
	/* 使能时钟 */
	rcu_periph_clock_enable(RCU_IN1_B);
	/* 上电初始化先配置为普通输出并拉低，避免电机驱动输入悬空误动作。 */
	gpio_mode_set(PORT_IN1_B,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,PIN_IN1_B);
	/* 配置为推挽输出 50MHZ */
	gpio_output_options_set(PORT_IN1_B,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,PIN_IN1_B);
	R_FRONT=0;
	gpio_af_set(PORT_IN1_B,TIMER1_CH2,PIN_IN1_B);//配置GPIO的复用
	
	/* 使能时钟 */
	rcu_periph_clock_enable(RCU_IN2_B);
	/* 上电初始化先配置为普通输出并拉低，避免电机驱动输入悬空误动作。 */
	gpio_mode_set(PORT_IN2_B,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,PIN_IN2_B);
	/* 配置为推挽输出 50MHZ */
	gpio_output_options_set(PORT_IN2_B,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,PIN_IN2_B);
	R_BACK=0;
	gpio_af_set(PORT_IN2_B,TIMER1_CH3,PIN_IN2_B);//配置GPIO的复用
	
}
/************************************************
函数名称 ： motor_L_front
功    能 ： 左轮正转
参    数 ： speed
返 回 值 ： 无
*************************************************/
void motor_L_front(uint16_t speed)
{
	timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_0,0);
	gpio_mode_set(PORT_IN1_A,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,PIN_IN1_A);//通用推挽输出
	L_FRONT=0;
	
	gpio_mode_set(PORT_IN2_A,GPIO_MODE_AF,GPIO_PUPD_NONE,PIN_IN2_A);	//复用推挽输出
	timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_1,speed); // 配置定时器通道输出脉冲值
}

/************************************************
函数名称 ： motor_L_back
功    能 ： 左轮反转
参    数 ： speed
返 回 值 ： 无
*************************************************/
void motor_L_back(uint16_t speed)
{
	timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_1,0);
	gpio_mode_set(PORT_IN2_A,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,PIN_IN2_A);//通用推挽输出
	L_BACK=0;
	
	gpio_mode_set(PORT_IN1_A,GPIO_MODE_AF,GPIO_PUPD_NONE,PIN_IN1_A);	//复用推挽输出
	timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_0,speed); // 配置定时器通道输出脉冲值
}

/************************************************
函数名称 ： motor_L_stop
功    能 ： 左轮停止
参    数 ： stopMode=0：熄火停止；stopMode=1：刹车停止
返 回 值 ： 无
*************************************************/
void motor_L_stop(uint16_t stopMode)
{
	motor_L_pwm_off();
	if(stopMode)
	{
		gpio_mode_set(PORT_IN2_A,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,PIN_IN2_A);//通用推挽输出
		L_BACK=1;
		gpio_mode_set(PORT_IN1_A,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,PIN_IN1_A);//通用推挽输出
		L_FRONT=1;
	}
	else
	{
		gpio_mode_set(PORT_IN2_A,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,PIN_IN2_A);//通用推挽输出
		L_BACK=0;
		gpio_mode_set(PORT_IN1_A,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,PIN_IN1_A);//通用推挽输出
		L_FRONT=0;	
	}
}
/************************************************
函数名称 ： motor_R_front
功    能 ： 右轮正转
参    数 ： speed
返 回 值 ： 无
*************************************************/
void motor_R_front(uint16_t speed)
{
	timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_2,0);
	gpio_mode_set(PORT_IN1_B,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,PIN_IN1_B);//通用推挽输出
	R_FRONT=0;
	
	gpio_mode_set(PORT_IN2_B,GPIO_MODE_AF,GPIO_PUPD_NONE,PIN_IN2_B);	//复用推挽输出
	timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_3,speed); // 配置定时器通道输出脉冲值
}

/************************************************
函数名称 ： motor_R_back
功    能 ： 右轮反转
参    数 ： speed
返 回 值 ： 无
*************************************************/
void motor_R_back(uint16_t speed)
{
	timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_3,0);
	gpio_mode_set(PORT_IN2_B,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,PIN_IN2_B);//通用推挽输出
	R_BACK=0;
	
	gpio_mode_set(PORT_IN1_B,GPIO_MODE_AF,GPIO_PUPD_NONE,PIN_IN1_B);	//复用推挽输出
	timer_channel_output_pulse_value_config(BSP_PWM1_TIMER,BSP_PWM1_CHANNEL_2,speed); // 配置定时器通道输出脉冲值
}

/************************************************
函数名称 ： motor_R_stop
功    能 ： 右轮停止
参    数 ： stopMode=0：熄火停止；stopMode=1：刹车停止
返 回 值 ： 无
*************************************************/
void motor_R_stop(uint16_t stopMode)
{
	motor_R_pwm_off();
	if(stopMode)
	{
		gpio_mode_set(PORT_IN2_B,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,PIN_IN2_B);//通用推挽输出
		R_BACK=1;
		gpio_mode_set(PORT_IN1_B,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,PIN_IN1_B);//通用推挽输出
		R_FRONT=1;
	}
	else
	{
		gpio_mode_set(PORT_IN2_B,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,PIN_IN2_B);//通用推挽输出
		R_BACK=0;
		gpio_mode_set(PORT_IN1_B,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,PIN_IN1_B);//通用推挽输出
		R_FRONT=0;	
	}
}

/************************************************
函数名称 ： motor_forward
功    能 ： 电机前进
参    数 ： speed 速度
返 回 值 ： 无
*************************************************/
void motor_forward(uint16_t speed)	// 电机前进
{
		motor_L_front(speed);
		motor_R_front(speed);
}

/************************************************
函数名称 ： motor_backward
功    能 ： 电机后退
参    数 ： speed 速度
返 回 值 ： 无
*************************************************/
void motor_backward(uint16_t speed)	// 电机后退
{
		motor_L_back(speed);
		motor_R_back(speed);
}

/************************************************
函数名称 ： motor_rightward
功    能 ： 电机右转
参    数 ： speed 速度
返 回 值 ： 无
*************************************************/
void motor_rightward(uint16_t speed)	// 电机右转
{
		motor_L_back(speed);
		motor_R_front(speed);
}

/************************************************
函数名称 ： motor_leftward
功    能 ： 电机左转
参    数 ： speed 速度
返 回 值 ： 无
*************************************************/
void motor_leftward(uint16_t speed)// 电机左转
{
		motor_L_front(speed);
		motor_R_back(speed);
}

/************************************************
函数名称 ： motor_stop
功    能 ： 电机停止
参    数 ： stopMode=0：熄火停止；stopMode=1：刹车停止
返 回 值 ： 无
*************************************************/
void motor_stop(uint16_t stopMode)//电机停止
{
	motor_pwm_off();
	if(stopMode)
	{
		motor_L_stop(1);
		motor_R_stop(1);
	}
	else
	{
		motor_L_stop(0);
		motor_R_stop(0);
	}
}
