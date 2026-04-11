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

#define ROBOT_DEFAULT_SPEED       65U
#define ROBOT_MEDIUM_SPEED        80U
#define ROBOT_HIGH_SPEED          100U
#define ROBOT_LOOP_DELAY_MS       50U
#define ROBOT_STARTUP_DELAY_MS    1000U
#define ROBOT_BUZZER_DELAY_MS     100U

#define SERVO_STEP_ANGLE          3U
#define SERVO_MAX_ANGLE           180U
#define SERVO_MIN_ANGLE           0U
#define SERVO_FORWARD_DIRECTION   1
#define SERVO_REVERSE_DIRECTION  -1

#define SERVO_LEFT_EYE_CH         0U
#define SERVO_RIGHT_EYE_CH        1U
#define SERVO_HEAD_CH             2U
#define SERVO_LEFT_ARM_CH         3U
#define SERVO_RIGHT_ARM_CH        4U

enum {
    CMD_FORWARD = 1,
    CMD_BACKWARD = 2,
    CMD_RIGHT = 3,
    CMD_LEFT = 4,
    CMD_STOP_HEART = 5,
    CMD_STOP_WALL_E = 6,
    CMD_BUZZER_TOGGLE = 7,
    CMD_LEFT_EYE_TOGGLE = 8,
    CMD_RIGHT_EYE_TOGGLE = 9,
    CMD_BUZZER_OFF = 10,
    CMD_LEFT_EYE_OFF = 11,
    CMD_RIGHT_EYE_OFF = 12,
    CMD_VOICE_SPEED_LOW = 13,
    CMD_VOICE_SPEED_MEDIUM = 14,
    CMD_VOICE_SPEED_HIGH = 15,
    CMD_BT_SPEED_LOW = 21,
    CMD_BT_SPEED_MEDIUM = 22,
    CMD_BT_SPEED_HIGH = 23,
    CMD_BT_LEFT = 31,
    CMD_BT_RIGHT = 32,
    CMD_BT_FORWARD = 33,
    CMD_BT_BACKWARD = 34,
    CMD_SERVO_LEFT_EYE = 24,
    CMD_SERVO_RIGHT_EYE = 25,
    CMD_SERVO_HEAD = 26,
    CMD_SERVO_BOTH_ARMS = 27,
    CMD_SERVO_LEFT_ARM = 28,
    CMD_SERVO_RIGHT_ARM = 29
};

typedef struct {
    uint8_t angle;
    int8_t direction;
} servo_state_t;

typedef struct {
    uint16_t speed;
    servo_state_t left_eye;
    servo_state_t right_eye;
    servo_state_t head;
    servo_state_t left_arm;
    servo_state_t right_arm;
    servo_state_t arm_pair;
} robot_state_t;

static void init_servo_state(servo_state_t *state)
{
    if (state == NULL) {
        return;
    }

    state->angle = SERVO_MIN_ANGLE;
    state->direction = SERVO_FORWARD_DIRECTION;
}

static void init_robot_state(robot_state_t *state)
{
    if (state == NULL) {
        return;
    }

    state->speed = ROBOT_DEFAULT_SPEED;
    init_servo_state(&state->left_eye);
    init_servo_state(&state->right_eye);
    init_servo_state(&state->head);
    init_servo_state(&state->left_arm);
    init_servo_state(&state->right_arm);
    init_servo_state(&state->arm_pair);
}

static void lcd_show_robot_picture(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const unsigned char *picture)
{
    LCD_Clear(WHITE);
    LCD_ShowPicture(x, y, width, height, picture);
}

static void show_default_picture(void)
{
    lcd_show_robot_picture(0, 0, 240, 240, WALL_E);
}

static void show_forward_picture(void)
{
    lcd_show_robot_picture(53, 0, 186, 240, shang);
}

static void show_backward_picture(void)
{
    lcd_show_robot_picture(53, 0, 187, 240, xia);
}

static void show_right_picture(void)
{
    lcd_show_robot_picture(0, 53, 240, 187, you);
}

static void show_left_picture(void)
{
    lcd_show_robot_picture(0, 53, 240, 187, zuo);
}

static void show_heart_picture(void)
{
    lcd_show_robot_picture(0, 0, 240, 240, heart);
}

static uint8_t parse_uart_command(const uint8_t *buf, uint16_t len, uint8_t *cmd)
{
    uint16_t index = 0;
    uint16_t value = 0;
    uint8_t has_digit = 0;

    if ((buf == NULL) || (cmd == NULL) || (len == 0)) {
        return 0;
    }

    while ((index < len) &&
           ((buf[index] == ' ') || (buf[index] == '\r') ||
            (buf[index] == '\n'))) {
        index++;
    }

    if (index >= len) {
        return 0;
    }

    if ((buf[index] < '0') || (buf[index] > '9')) {
        *cmd = buf[index];
        return 1;
    }

    while ((index < len) && (buf[index] >= '0') && (buf[index] <= '9')) {
        value = (uint16_t)(value * 10 + (buf[index] - '0'));
        has_digit = 1;
        index++;
    }

    if ((has_digit == 0) || (value > 255)) {
        return 0;
    }

    while (index < len) {
        if ((buf[index] != ' ') && (buf[index] != '\r') &&
            (buf[index] != '\n')) {
            return 0;
        }
        index++;
    }

    *cmd = (uint8_t)value;
    return 1;
}

static uint8_t servo_next_angle(servo_state_t *state)
{
    int16_t next_angle = 0;

    if (state == NULL) {
        return 0;
    }

    next_angle = (int16_t)state->angle + (int16_t)(state->direction * SERVO_STEP_ANGLE);

    if (next_angle >= SERVO_MAX_ANGLE) {
        next_angle = SERVO_MAX_ANGLE;
        state->direction = SERVO_REVERSE_DIRECTION;
    } else if (next_angle <= SERVO_MIN_ANGLE) {
        next_angle = SERVO_MIN_ANGLE;
        state->direction = SERVO_FORWARD_DIRECTION;
    }

    state->angle = (uint8_t)next_angle;
    return state->angle;
}

static void handle_drive_command(uint8_t cmd, uint16_t speed)
{
    switch (cmd) {
        case CMD_FORWARD:
            motor_forward(speed);
            show_forward_picture();
            break;
        case CMD_BACKWARD:
            motor_backward(speed);
            show_backward_picture();
            break;
        case CMD_RIGHT:
            motor_rightward(speed);
            show_right_picture();
            break;
        case CMD_LEFT:
            motor_leftward(speed);
            show_left_picture();
            break;
        case CMD_STOP_HEART:
            motor_stop(0);
            show_heart_picture();
            break;
        case CMD_STOP_WALL_E:
            motor_stop(1);
            show_default_picture();
            break;
        default:
            break;
    }
}

static void handle_servo_command(uint8_t cmd, robot_state_t *state)
{
    uint8_t servo_angle = 0;

    if (state == NULL) {
        return;
    }

    switch (cmd) {
        case CMD_SERVO_LEFT_EYE:
            servo_angle = servo_next_angle(&state->left_eye);
            setAngle(SERVO_LEFT_EYE_CH, servo_angle);
            break;
        case CMD_SERVO_RIGHT_EYE:
            servo_angle = servo_next_angle(&state->right_eye);
            setAngle(SERVO_RIGHT_EYE_CH, servo_angle);
            break;
        case CMD_SERVO_HEAD:
            servo_angle = servo_next_angle(&state->head);
            setAngle(SERVO_HEAD_CH, servo_angle);
            break;
        case CMD_SERVO_BOTH_ARMS:
            servo_angle = servo_next_angle(&state->arm_pair);
            state->left_arm.angle = servo_angle;
            state->left_arm.direction = state->arm_pair.direction;
            state->right_arm.angle = servo_angle;
            state->right_arm.direction = state->arm_pair.direction;
            setAngle2(SERVO_LEFT_ARM_CH, SERVO_RIGHT_ARM_CH, servo_angle);
            break;
        case CMD_SERVO_LEFT_ARM:
            servo_angle = servo_next_angle(&state->left_arm);
            state->arm_pair.angle = state->left_arm.angle;
            state->arm_pair.direction = state->left_arm.direction;
            setAngle(SERVO_LEFT_ARM_CH, servo_angle);
            break;
        case CMD_SERVO_RIGHT_ARM:
            servo_angle = servo_next_angle(&state->right_arm);
            state->arm_pair.angle = state->right_arm.angle;
            state->arm_pair.direction = state->right_arm.direction;
            setAngle(SERVO_RIGHT_ARM_CH, servo_angle);
            break;
        default:
            break;
    }
}

static void handle_uart6_command(uint8_t cmd, robot_state_t *state)
{
    if (state == NULL) {
        return;
    }

    switch (cmd) {
        case CMD_BT_FORWARD:
            handle_drive_command(CMD_FORWARD, state->speed);
            break;
        case CMD_BT_BACKWARD:
            handle_drive_command(CMD_BACKWARD, state->speed);
            break;
        case CMD_BT_RIGHT:
            handle_drive_command(CMD_RIGHT, state->speed);
            break;
        case CMD_BT_LEFT:
            handle_drive_command(CMD_LEFT, state->speed);
            break;
        case CMD_FORWARD:
        case CMD_BACKWARD:
        case CMD_RIGHT:
        case CMD_LEFT:
        case CMD_STOP_HEART:
        case CMD_STOP_WALL_E:
            handle_drive_command(cmd, state->speed);
            break;
        case CMD_BUZZER_TOGGLE:
            switch_buzzer_status();
            break;
        case CMD_LEFT_EYE_TOGGLE:
            switch_eye_left_status();
            break;
        case CMD_RIGHT_EYE_TOGGLE:
            switch_eye_right_status();
            break;
        case CMD_BUZZER_OFF:
            BUZZER_OFF;
            break;
        case CMD_LEFT_EYE_OFF:
            EYE_L_OFF;
            break;
        case CMD_RIGHT_EYE_OFF:
            EYE_R_OFF;
            break;
        case CMD_BT_SPEED_LOW:
            state->speed = ROBOT_DEFAULT_SPEED;
            break;
        case CMD_BT_SPEED_MEDIUM:
            state->speed = ROBOT_MEDIUM_SPEED;
            break;
        case CMD_BT_SPEED_HIGH:
            state->speed = ROBOT_HIGH_SPEED;
            break;
        case CMD_SERVO_LEFT_EYE:
        case CMD_SERVO_RIGHT_EYE:
        case CMD_SERVO_HEAD:
        case CMD_SERVO_BOTH_ARMS:
        case CMD_SERVO_LEFT_ARM:
        case CMD_SERVO_RIGHT_ARM:
            handle_servo_command(cmd, state);
            break;
        default:
            break;
    }
}

static void handle_usart2_command(uint8_t cmd, robot_state_t *state)
{
    if (state == NULL) {
        return;
    }

    switch (cmd) {
        case CMD_FORWARD:
        case CMD_BACKWARD:
        case CMD_RIGHT:
        case CMD_LEFT:
            handle_drive_command(cmd, state->speed);
            break;
        case CMD_STOP_HEART:
        case CMD_STOP_WALL_E:
            handle_drive_command(CMD_STOP_WALL_E, state->speed);
            break;
        case CMD_LEFT_EYE_TOGGLE:
        case CMD_RIGHT_EYE_TOGGLE:
            EYE_L_ON;
            EYE_R_ON;
            break;
        case CMD_LEFT_EYE_OFF:
        case CMD_RIGHT_EYE_OFF:
            EYE_L_OFF;
            EYE_R_OFF;
            break;
        default:
            break;
    }
}

static void clear_received_data(uint8_t *buf, uint16_t *len)
{
    if ((buf == NULL) || (len == NULL)) {
        return;
    }

    memset(buf, 0, *len);
    *len = 0;
}

static void process_uart6_data(robot_state_t *state)
{
    uint8_t uart6_cmd = 0;

    if (!g_recv_complete_flag_uart6) {
        return;
    }

    g_recv_complete_flag_uart6 = 0;
    printf("g_recv_length:%d ", g_recv_length_uart6);
    printf("g_recv_buff:%s\r\n", g_recv_buff_uart6);

    if (parse_uart_command(g_recv_buff_uart6, g_recv_length_uart6, &uart6_cmd)) {
        handle_uart6_command(uart6_cmd, state);
    }

    clear_received_data(g_recv_buff_uart6, &g_recv_length_uart6);
}

static void process_usart2_data(robot_state_t *state)
{
    if (!g_recv_complete_flag_usart2) {
        return;
    }

    g_recv_complete_flag_usart2 = 0;
    printf("g_recv_length:%d ", g_recv_length_usart2);
    printf("g_recv_buff:%s\r\n", g_recv_buff_usart2);

    handle_usart2_command(g_recv_buff_usart2[0], state);
    clear_received_data(g_recv_buff_usart2, &g_recv_length_usart2);
}

static void report_ps2_status(void)
{
    if (PS2_RedLight() == 0) { // 如果当前是红灯模式
        printf(" %5d %5d %5d %5d\r\n",
               PS2_AnologData(PSS_LX),
               PS2_AnologData(PSS_LY),
               PS2_AnologData(PSS_RX),
               PS2_AnologData(PSS_RY));
    }
}

static void init_board(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    systick_config();                         // 滴答定时器初始化

    usart_gpio_config(9600U);                 // 下载器串口初始化
    usart2_gpio_config(115200U);              // 语音串口初始化
    uart6_gpio_config(9600U);                 // 蓝牙串口初始化
    pwm1_config(200, 100);                    // PWM1初始化
    led_gpio_config();                        // LED初始化
    lcd_gpio_config();                        // LCD初始化
    buzzer_gpio_config();                     // 蜂鸣器初始化
    eye_gpio_config();                        // 眼灯初始化
    motor_gpio_config();                      // 电机初始化
    motor_stop(1);                            // 上电默认停止
    PCA9685_Init(50, SERVO_MIN_ANGLE);        // PCA9685--16路舵机初始化  频率50Hz -- 0度

    PS2_SetInit();                            // PS2手柄初始化
    printf("start\r\n");
    delay_1ms(ROBOT_STARTUP_DELAY_MS);

    LCD_Clear(WHITE);
    BACK_COLOR = WHITE;                       // LCD背景
    BUZZER_ON;
    delay_1ms(ROBOT_BUZZER_DELAY_MS);
    BUZZER_OFF;                               // 下载提示
    show_default_picture();                   // 默认显示

    delay_1ms(ROBOT_STARTUP_DELAY_MS);
}

/************************************************
函数名称 ： main
功    能 ： 主函数
参    数 ： 无
返 回 值 ： 无
*************************************************/
int main(void)
{
    robot_state_t robot_state;

    init_robot_state(&robot_state);
    init_board();

#if 0
    printf("-------This is bluetooth AT test-------\r\n");
    HC05_Bluetooth2_0_config(); // 直接使用串口调试助手发送AT指令也行，需要借助USB转TTL模块
#endif

    while (1) {
        report_ps2_status();
        delay_1ms(ROBOT_LOOP_DELAY_MS);       // 防止发送太快，接收器反应不过来
        process_uart6_data(&robot_state);     // 蓝牙命令处理
        process_usart2_data(&robot_state);    // 语音命令处理
    }
}
