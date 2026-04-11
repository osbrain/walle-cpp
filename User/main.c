/********************************************************************************
  * 文 件 名: main.c
  * 版 本 号: 初版
  * 修改作者: 少不入川
  * 修改日期: 2023年02月28日
  * 功能介绍: 综合      
  ******************************************************************************
  * 注意事项:
*********************************************************************************/

#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "sys.h"
#include "string.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"
#include "bsp_timer.h"
#include "bsp_pwm.h"
#include "bsp_dma.h"
#include "bsp_buzzer.h"
#include "bsp_eye.h"
#include "bsp_adc.h"
#include "bsp_lcd.h"
#include "bsp_motor.h"
#include "bsp_bluetooth.h"
#include "bmp.h"
#include "bsp_pca9685.h"
#include "bsp_ps2.h"

/***********************************************
函数名称 ： main
功    能 ： 主函数
参    数 ： 无
返 回 值 ： 无
作    者 ： 少不入川
*************************************************/
//下载调试
extern uint8_t g_recv_buff[USART_RECEIVE_LENGTH];           // 接收缓冲区
extern uint16_t g_recv_length;                              // 接收数据长度
extern uint8_t g_recv_complete_flag;                        // 接收数据完成标志位

//VC-02
extern uint8_t g_recv_buff_usart2[USART_RECEIVE_LENGTH];    // 接收缓冲区
extern uint16_t g_recv_length_usart2;                       // 接收数据长度
extern uint8_t g_recv_complete_flag_usart2;                 // 接收数据完成标志位

//蓝牙连接
extern uint8_t g_recv_buff_uart6[USART_RECEIVE_LENGTH];     // 接收缓冲区
extern uint16_t g_recv_length_uart6;                        // 接收数据长度
extern uint8_t g_recv_complete_flag_uart6;                  // 接收数据完成标志位

uint16_t speed = 65;

/************************************************
函数名称 ： main
功    能 ： 主函数
参    数 ： 无
返 回 值 ： 无
*************************************************/
int main(void)
{
    uint8_t i = 0;

    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    systick_config();                        // 滴答定时器初始化

    usart_gpio_config(9600U);                // 下载器串口初始化
    usart2_gpio_config(115200U);             // 语音串口初始化
    uart6_gpio_config(9600U);                // 蓝牙串口初始化
    pwm1_config(200, 100);                   // PWM1初始化
    led_gpio_config();                       // LED初始化
    lcd_gpio_config();                       // LCD初始化
    buzzer_gpio_config();                    // 蜂鸣器初始化
    eye_gpio_config();                       // 眼灯初始化
    motor_gpio_config();                     // 电机初始化
    PCA9685_Init(50, 0);                     // PCA9685--16路舵机初始化  频率50Hz -- 0度

    // PS2手柄初始化
    PS2_SetInit();
    printf("start\r\n");
    delay_1ms(1000);

    LCD_Clear(WHITE);
    BACK_COLOR = WHITE;                      // LCD背景
    BUZZER_ON;
    delay_1ms(100);
    BUZZER_OFF;                              // 下载提示
    LCD_ShowPicture(0, 0, 240, 240, WALL_E); // 默认显示

    delay_1ms(1000);

#if 0
    printf("-------This is bluetooth AT test-------\r\n");
    HC05_Bluetooth2_0_config(); // 直接使用串口调试助手发送AT指令也行，需要借助USB转TTL模块
#endif

    while (1) {
        if (PS2_RedLight() == 0) { // 如果当前是红灯模式
            // 输出左右两个遥感的XY值
            printf(" %5d %5d %5d %5d\r\n",
                   PS2_AnologData(PSS_LX),
                   PS2_AnologData(PSS_LY),
                   PS2_AnologData(PSS_RX),
                   PS2_AnologData(PSS_RY));
        }

        delay_1ms(50); // 防止发送太快，接收器反应不过来

        /* 蓝牙，等待数据传输完成 */
        if (g_recv_complete_flag_uart6) {               // 数据接收完成
            g_recv_complete_flag_uart6 = 0;             // 等待下次接收
            printf("g_recv_length:%d ", g_recv_length_uart6); // 打印接收的数据长度
            printf("g_recv_buff:%s\r\n", g_recv_buff_uart6); // 打印接收的数据

            switch (g_recv_buff_uart6[0]) {
                case 1:
                    motor_forward(speed);
                    LCD_Clear(WHITE);
                    LCD_ShowPicture(53, 0, 186, 240, shang);
                    break;
                case 2:
                    motor_backward(speed);
                    LCD_Clear(WHITE);
                    LCD_ShowPicture(53, 0, 187, 240, xia);
                    break;
                case 3:
                    motor_rightward(speed);
                    LCD_Clear(WHITE);
                    LCD_ShowPicture(0, 53, 240, 187, you);
                    break;
                case 4:
                    motor_leftward(speed);
                    LCD_Clear(WHITE);
                    LCD_ShowPicture(0, 53, 240, 187, zuo);
                    break;
                case 5:
                    motor_stop(0);
                    LCD_Clear(WHITE);
                    LCD_ShowPicture(0, 0, 240, 240, heart);
                    break;
                case 6:
                    motor_stop(1);
                    LCD_Clear(WHITE);
                    LCD_ShowPicture(0, 0, 240, 240, WALL_E);
                    break;
                case 7:
                    switch_buzzer_status();
                    break;
                case 8:
                    switch_eye_left_status();
                    break;
                case 9:
                    switch_eye_right_status();
                    break;
                case 10:
                    BUZZER_OFF;
                    break;
                case 11:
                    EYE_L_OFF;
                    break;
                case 12:
                    EYE_R_OFF;
                    break;
                case 21:
                    speed = 65;
                    break;
                case 22:
                    speed = 80;
                    break;
                case 23:
                    speed = 100;
                    break;
                case 24:
                    setAngle(0, i);
                    i = (i + 1) % 180;      // 舵机测试
                    break;
                case 25:
                    setAngle(1, i);
                    i = (i + 1) % 180;      // 舵机测试
                    break;
                default:
                    break;
            }

            memset(g_recv_buff_uart6, 0, g_recv_length_uart6); // 清空数组
            g_recv_length_uart6 = 0;                           // 清空长度
        }

        /* VC-02，等待数据传输完成 */
        if (g_recv_complete_flag_usart2) {              // 数据接收完成
            g_recv_complete_flag_usart2 = 0;            // 等待下次接收
            printf("g_recv_length:%d ", g_recv_length_usart2); // 打印接收的数据长度
            printf("g_recv_buff:%s\r\n", g_recv_buff_usart2); // 打印接收的数据

            switch (g_recv_buff_usart2[0]) {
                case 1:
                    motor_forward(speed);
                    LCD_Clear(WHITE);
                    LCD_ShowPicture(53, 0, 186, 240, shang);
                    break;
                case 2:
                    motor_backward(speed);
                    LCD_Clear(WHITE);
                    LCD_ShowPicture(53, 0, 187, 240, xia);
                    break;
                case 3:
                    motor_rightward(speed);
                    LCD_Clear(WHITE);
                    LCD_ShowPicture(0, 53, 240, 187, you);
                    break;
                case 4:
                    motor_leftward(speed);
                    LCD_Clear(WHITE);
                    LCD_ShowPicture(0, 53, 240, 187, zuo);
                    break;
                case 5:
                    motor_stop(0);
                    LCD_Clear(WHITE);
                    LCD_ShowPicture(0, 0, 240, 240, heart);
                    break;
                case 6:
                    motor_stop(1);
                    LCD_Clear(WHITE);
                    LCD_ShowPicture(0, 0, 240, 240, WALL_E);
                    break;
                case 7:
                    BUZZER_ON;
                    break;
                case 8:
                    EYE_L_ON;
                    break;
                case 9:
                    EYE_R_ON;
                    break;
                case 10:
                    BUZZER_OFF;
                    break;
                case 11:
                    EYE_L_OFF;
                    break;
                case 12:
                    EYE_R_OFF;
                    break;
                case 13:
                    speed = 65;
                    break;
                case 14:
                    speed = 80;
                    break;
                case 15:
                    speed = 100;
                    break;
                default:
                    break;
            }

            memset(g_recv_buff_usart2, 0, g_recv_length_usart2); // 清空数组
            g_recv_length_usart2 = 0;                            // 清空长度
        }
    }
}
