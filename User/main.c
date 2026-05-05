/********************************************************************************
  * 文 件 名: main.c
  * 版 本 号: 初版
  * 功能介绍: 瓦力机器人主控程序，负责串口命令解析、运动控制、LCD状态显示、
  *           蜂鸣器/眼灯控制以及多路舵机动作控制。
  * 控制说明: 蓝牙控制支持前进、后退、左转、右转、停止/刹车、调速、
  *           蜂鸣器、左右眼灯，以及左眼、右眼、头部、双臂、左右手舵机。
  *           语音控制支持常用运动、停止/刹车、调速、蜂鸣器和左右眼灯。
  *           蓝牙临时诊断指令41~44用于单独测试PA0~PA3电机PWM，45用于停止。
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
// 下载调试串口，用于打印日志和基础调试。
extern uint8_t g_recv_buff[USART_RECEIVE_LENGTH];           // 接收缓冲区
extern uint16_t g_recv_length;                              // 接收数据长度
extern uint8_t g_recv_complete_flag;                        // 接收数据完成标志位

// VC-02语音模块串口，接收语音识别后的控制码。
extern uint8_t g_recv_buff_usart2[USART_RECEIVE_LENGTH];    // 接收缓冲区
extern uint16_t g_recv_length_usart2;                       // 接收数据长度
extern uint8_t g_recv_complete_flag_usart2;                 // 接收数据完成标志位

// 蓝牙模块串口，接收手机/上位机发送的控制码。
extern uint8_t g_recv_buff_uart6[USART_RECEIVE_LENGTH];     // 接收缓冲区
extern uint16_t g_recv_length_uart6;                        // 接收数据长度
extern uint8_t g_recv_complete_flag_uart6;                  // 接收数据完成标志位

// 机器人运动速度和主循环节拍配置。
#define ROBOT_LOW_SPEED           80U
#define ROBOT_DEFAULT_SPEED       100U
#define ROBOT_MEDIUM_SPEED        90U
#define ROBOT_HIGH_SPEED          100U
#define ROBOT_MIN_SPEED           80U
#define ROBOT_MAX_SPEED           100U
#define ROBOT_LOOP_DELAY_MS       50U
#define ROBOT_STARTUP_DELAY_MS    1000U
#define ROBOT_BUZZER_DELAY_MS     100U
#define ROBOT_MOTION_TIMEOUT_MS   30000U
#define ROBOT_TURN_GUARD_DELAY_MS 100U

// 舵机控制恢复为两天前的慢速往复逻辑：上电0度，每次命令推进3度，0~180度到边界后反向。
#define SERVO_HOME_ANGLE          0U
#define SERVO_SAFE_MIN_ANGLE      0U
#define SERVO_SAFE_MAX_ANGLE      180U
#define SERVO_EYE_STEP_ANGLE      3U
#define SERVO_HEAD_STEP_ANGLE     3U
#define SERVO_ARM_STEP_ANGLE      3U
#define SERVO_FORWARD_DIRECTION   1
#define SERVO_REVERSE_DIRECTION  -1

// PCA9685舵机通道分配。
#define SERVO_LEFT_EYE_CH         0U
#define SERVO_RIGHT_EYE_CH        1U
#define SERVO_HEAD_CH             2U
#define SERVO_LEFT_ARM_CH         3U
#define SERVO_RIGHT_ARM_CH        4U

enum {
    CMD_FORWARD = 1,              // 前进。
    CMD_BACKWARD = 2,             // 后退。
    CMD_RIGHT = 3,                // 右转。
    CMD_LEFT = 4,                 // 左转。
    CMD_STOP_HEART = 5,           // 停止并显示爱心图。
    CMD_STOP_WALL_E = 6,          // 停止并显示默认瓦力图。
    CMD_BUZZER_TOGGLE = 7,        // 切换蜂鸣器状态。
    CMD_LEFT_EYE_ON = 8,          // 打开左眼灯。
    CMD_RIGHT_EYE_ON = 9,         // 打开右眼灯。
    CMD_SPEED_RESET = 10,         // 速度复位。
    CMD_LEFT_EYE_OFF = 11,        // 关闭左眼灯。
    CMD_RIGHT_EYE_OFF = 12,       // 关闭右眼灯。
    CMD_SPEED_PLUS_5 = 13,        // 速度增加5。
    CMD_SPEED_PLUS_10 = 14,       // 速度增加10。
    CMD_BRAKE_STOP = 15,          // 停止指令，当前按熄火停止处理。
    CMD_SPEED_MINUS_10 = 16,      // 速度降低10。
    CMD_SPEED_MINUS_5 = 17,       // 速度降低5。
    CMD_BT_SPEED_LOW = 21,        // 蓝牙低速。
    CMD_BT_SPEED_MEDIUM = 22,     // 蓝牙中速。
    CMD_BT_SPEED_HIGH = 23,       // 蓝牙高速。
    CMD_BT_LEFT = 31,             // 蓝牙左转。
    CMD_BT_RIGHT = 32,            // 蓝牙右转。
    CMD_BT_FORWARD = 33,          // 蓝牙前进。
    CMD_BT_BACKWARD = 34,         // 蓝牙后退。
    CMD_SERVO_LEFT_EYE = 24,      // 左眼舵机摆动。
    CMD_SERVO_RIGHT_EYE = 25,     // 右眼舵机摆动。
    CMD_SERVO_HEAD = 26,          // 头部舵机摆动。
    CMD_SERVO_BOTH_ARMS = 27,     // 双臂舵机同步摆动。
    CMD_SERVO_LEFT_ARM = 28,      // 左臂舵机摆动。
    CMD_SERVO_RIGHT_ARM = 29,     // 右臂舵机摆动。
    CMD_MOTOR_TEST_CH0 = 41,      // 调试：单独输出PA0/TIMER1_CH0。
    CMD_MOTOR_TEST_CH1 = 42,      // 调试：单独输出PA1/TIMER1_CH1。
    CMD_MOTOR_TEST_CH2 = 43,      // 调试：单独输出PA2/TIMER1_CH2。
    CMD_MOTOR_TEST_CH3 = 44,      // 调试：单独输出PA3/TIMER1_CH3。
    CMD_MOTOR_TEST_STOP = 45,     // 调试：停止所有电机通道。
    CMD_MOTOR_TEST_PA2_PA3_LOW = 46,  // 调试：PA2输出PWM，PA3拉低。
    CMD_MOTOR_TEST_PA2_PA3_HIGH = 47  // 调试：PA2输出PWM，PA3拉高。
};

typedef enum {
    MOTION_STOP = 0,
    MOTION_FORWARD,
    MOTION_BACKWARD,
    MOTION_RIGHT,
    MOTION_LEFT,
    MOTION_DEBUG
} motion_state_t;

typedef struct {
    uint8_t angle;        // 当前舵机角度。
    uint8_t min_angle;    // 允许的最小角度。
    uint8_t max_angle;    // 允许的最大角度。
    uint8_t step_angle;   // 每次命令推进的角度。
    int8_t direction;     // 当前摆动方向，达到边界后自动反向。
} servo_state_t;

typedef struct {
    uint16_t speed;             // 当前运动速度，由蓝牙速度命令动态调整。
    motion_state_t motion;      // 当前底盘运动状态，用于去重、平滑切换和超时停车。
    uint8_t display_cmd;        // 当前LCD显示状态，避免重复清屏刷新。
    uint16_t motion_idle_ms;    // 距离最近一次运动命令的时间，用于通信失联自动停车。
    servo_state_t left_eye;     // 左眼舵机状态。
    servo_state_t right_eye;    // 右眼舵机状态。
    servo_state_t head;         // 头部舵机状态。
    servo_state_t left_arm;     // 左臂舵机状态。
    servo_state_t right_arm;    // 右臂舵机状态。
    servo_state_t arm_pair;     // 双臂同步动作时使用的共享状态。
} robot_state_t;

// 初始化单个舵机状态，默认从中位开始向正方向摆动。
static void init_servo_state(servo_state_t *state,
                             uint8_t min_angle,
                             uint8_t max_angle,
                             uint8_t step_angle)
{
    if (state == NULL) {
        return;
    }

    state->angle = SERVO_HOME_ANGLE;
    state->min_angle = min_angle;
    state->max_angle = max_angle;
    state->step_angle = step_angle;
    state->direction = SERVO_FORWARD_DIRECTION;
}

// 初始化机器人运行状态，包括默认速度和所有舵机的初始状态。
static void init_robot_state(robot_state_t *state)
{
    if (state == NULL) {
        return;
    }

    state->speed = ROBOT_DEFAULT_SPEED;
    state->motion = MOTION_STOP;
    state->display_cmd = 0U;
    state->motion_idle_ms = 0U;
    init_servo_state(&state->left_eye, SERVO_SAFE_MIN_ANGLE, SERVO_SAFE_MAX_ANGLE, SERVO_EYE_STEP_ANGLE);
    init_servo_state(&state->right_eye, SERVO_SAFE_MIN_ANGLE, SERVO_SAFE_MAX_ANGLE, SERVO_EYE_STEP_ANGLE);
    init_servo_state(&state->head, SERVO_SAFE_MIN_ANGLE, SERVO_SAFE_MAX_ANGLE, SERVO_HEAD_STEP_ANGLE);
    init_servo_state(&state->left_arm, SERVO_SAFE_MIN_ANGLE, SERVO_SAFE_MAX_ANGLE, SERVO_ARM_STEP_ANGLE);
    init_servo_state(&state->right_arm, SERVO_SAFE_MIN_ANGLE, SERVO_SAFE_MAX_ANGLE, SERVO_ARM_STEP_ANGLE);
    init_servo_state(&state->arm_pair, SERVO_SAFE_MIN_ANGLE, SERVO_SAFE_MAX_ANGLE, SERVO_ARM_STEP_ANGLE);
}

// 清屏后显示指定图片，供不同运动状态复用。
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

    if (len == 1U) {
        if ((buf[0] >= '0') && (buf[0] <= '9')) {
            *cmd = (uint8_t)(buf[0] - '0');
        } else {
            *cmd = buf[0];
        }
        return 1;
    }

    // 跳过命令前的空白字符，兼容串口调试助手附带的回车换行。
    while ((index < len) &&
           ((buf[index] == ' ') || (buf[index] == '\r') ||
            (buf[index] == '\n'))) {
        index++;
    }

    if (index >= len) {
        return 0;
    }

    // 非数字命令按单字节原样返回，保留对旧协议的兼容。
    if ((buf[index] < '0') || (buf[index] > '9')) {
        *cmd = buf[index];
        return 1;
    }

    // 数字命令支持ASCII形式，例如蓝牙发送"33"表示前进。
    while ((index < len) && (buf[index] >= '0') && (buf[index] <= '9')) {
        value = (uint16_t)(value * 10 + (buf[index] - '0'));
        has_digit = 1;
        index++;
    }

    if ((has_digit == 0) || (value > 255)) {
        return 0;
    }

    // 数字命令后只允许空白字符，避免"33abc"被误识别为有效命令。
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

    // 每触发一次命令就前进一个小角度，形成安全范围内的往复摆动效果。
    next_angle = (int16_t)state->angle + (int16_t)(state->direction * state->step_angle);

    if (next_angle >= state->max_angle) {
        next_angle = state->max_angle;
        state->direction = SERVO_REVERSE_DIRECTION;
    } else if (next_angle <= state->min_angle) {
        next_angle = state->min_angle;
        state->direction = SERVO_FORWARD_DIRECTION;
    }

    state->angle = (uint8_t)next_angle;
    return state->angle;
}

static uint16_t clamp_speed(uint16_t speed)
{
    if (speed > ROBOT_MAX_SPEED) {
        return ROBOT_MAX_SPEED;
    }

    return speed;
}

static void show_drive_picture(robot_state_t *state, uint8_t display_cmd)
{
    if ((state == NULL) || (state->display_cmd == display_cmd)) {
        return;
    }

    switch (display_cmd) {
        case CMD_FORWARD:
            show_forward_picture();
            break;
        case CMD_BACKWARD:
            show_backward_picture();
            break;
        case CMD_RIGHT:
            show_right_picture();
            break;
        case CMD_LEFT:
            show_left_picture();
            break;
        case CMD_STOP_HEART:
            show_heart_picture();
            break;
        case CMD_STOP_WALL_E:
            show_default_picture();
            break;
        default:
            return;
    }

    state->display_cmd = display_cmd;
}

static void apply_motor_motion(motion_state_t motion, uint16_t speed)
{
    switch (motion) {
        case MOTION_FORWARD:
            motor_forward(speed);
            break;
        case MOTION_BACKWARD:
            motor_backward(speed);
            break;
        case MOTION_RIGHT:
            motor_rightward(speed);
            break;
        case MOTION_LEFT:
            motor_leftward(speed);
            break;
        default:
            motor_stop(0);
            break;
    }
}

static uint8_t command_to_motion(uint8_t cmd, motion_state_t *motion, uint8_t *display_cmd)
{
    if ((motion == NULL) || (display_cmd == NULL)) {
        return 0;
    }

    switch (cmd) {
        case CMD_FORWARD:
        case CMD_BT_FORWARD:
            *motion = MOTION_FORWARD;
            *display_cmd = CMD_FORWARD;
            return 1;
        case CMD_BACKWARD:
        case CMD_BT_BACKWARD:
            *motion = MOTION_BACKWARD;
            *display_cmd = CMD_BACKWARD;
            return 1;
        case CMD_RIGHT:
        case CMD_BT_RIGHT:
            *motion = MOTION_RIGHT;
            *display_cmd = CMD_RIGHT;
            return 1;
        case CMD_LEFT:
        case CMD_BT_LEFT:
            *motion = MOTION_LEFT;
            *display_cmd = CMD_LEFT;
            return 1;
        default:
            return 0;
    }
}

static void stop_robot(robot_state_t *state, uint8_t stop_mode, uint8_t display_cmd)
{
    if (state == NULL) {
        return;
    }

    motor_stop(stop_mode);
    state->motion = MOTION_STOP;
    state->motion_idle_ms = 0U;
    show_drive_picture(state, display_cmd);
}

static void handle_drive_command(uint8_t cmd, robot_state_t *state)
{
    motion_state_t next_motion = MOTION_STOP;
    uint8_t display_cmd = 0U;

    if (state == NULL) {
        return;
    }

    if (cmd == CMD_STOP_HEART) {
        stop_robot(state, 0U, CMD_STOP_HEART);
        return;
    }

    if ((cmd == CMD_STOP_WALL_E) || (cmd == CMD_BRAKE_STOP)) {
        stop_robot(state, 0U, CMD_STOP_WALL_E);
        return;
    }

    if (!command_to_motion(cmd, &next_motion, &display_cmd)) {
        return;
    }

    state->motion_idle_ms = 0U;
    if ((state->motion != MOTION_STOP) && (state->motion != next_motion)) {
        motor_stop(0);
        delay_1ms(ROBOT_TURN_GUARD_DELAY_MS);
    }

    if (state->motion != next_motion) {
        state->motion = next_motion;
        apply_motor_motion(state->motion, state->speed);
        show_drive_picture(state, display_cmd);
        return;
    }

    apply_motor_motion(state->motion, state->speed);
}

static void handle_servo_command(uint8_t cmd, robot_state_t *state)
{
    uint8_t servo_angle = 0;

    if (state == NULL) {
        return;
    }

    // 舵机命令只改变对应关节；双臂命令会同步左右臂状态。
    switch (cmd) {
        case CMD_SERVO_LEFT_EYE:       // 指令24：左眼舵机按步进角度摆动。
            servo_angle = servo_next_angle(&state->left_eye);
            setAngle(SERVO_LEFT_EYE_CH, servo_angle);
            break;
        case CMD_SERVO_RIGHT_EYE:      // 指令25：右眼舵机按步进角度摆动。
            servo_angle = servo_next_angle(&state->right_eye);
            setAngle(SERVO_RIGHT_EYE_CH, servo_angle);
            break;
        case CMD_SERVO_HEAD:           // 指令26：头部舵机按步进角度摆动。
            servo_angle = servo_next_angle(&state->head);
            setAngle(SERVO_HEAD_CH, servo_angle);
            break;
        case CMD_SERVO_BOTH_ARMS:      // 指令27：左右手臂舵机同步摆动。
            servo_angle = servo_next_angle(&state->arm_pair);
            // 同步刷新左右臂状态，避免后续单臂动作从旧角度突跳。
            state->left_arm.angle = servo_angle;
            state->left_arm.direction = state->arm_pair.direction;
            state->right_arm.angle = servo_angle;
            state->right_arm.direction = state->arm_pair.direction;
            setAngle2(SERVO_LEFT_ARM_CH, SERVO_RIGHT_ARM_CH, servo_angle);
            break;
        case CMD_SERVO_LEFT_ARM:       // 指令28：左臂舵机单独摆动。
            servo_angle = servo_next_angle(&state->left_arm);
            // 单臂动作后回写双臂共享状态，下一次双臂动作从当前位置继续。
            state->arm_pair.angle = state->left_arm.angle;
            state->arm_pair.direction = state->left_arm.direction;
            setAngle(SERVO_LEFT_ARM_CH, servo_angle);
            break;
        case CMD_SERVO_RIGHT_ARM:      // 指令29：右臂舵机单独摆动。
            servo_angle = servo_next_angle(&state->right_arm);
            // 单臂动作后回写双臂共享状态，下一次双臂动作从当前位置继续。
            state->arm_pair.angle = state->right_arm.angle;
            state->arm_pair.direction = state->right_arm.direction;
            setAngle(SERVO_RIGHT_ARM_CH, servo_angle);
            break;
        default:
            break;
    }
}

static void handle_motor_test_command(uint8_t cmd, robot_state_t *state)
{
    uint8_t channel = 0U;

    if (state == NULL) {
        return;
    }

    state->motion_idle_ms = 0U;

    if (cmd == CMD_MOTOR_TEST_STOP) {
        stop_robot(state, 0U, CMD_STOP_WALL_E);
        return;
    }

    if (cmd == CMD_MOTOR_TEST_PA2_PA3_LOW) {
        state->motion = MOTION_DEBUG;
        motor_debug_right_pa2_pwm_pa3_level(0U, ROBOT_DEFAULT_SPEED);
        printf("motor test PA2 PWM, PA3 low\r\n");
        return;
    }

    if (cmd == CMD_MOTOR_TEST_PA2_PA3_HIGH) {
        state->motion = MOTION_DEBUG;
        motor_debug_right_pa2_pwm_pa3_level(1U, ROBOT_DEFAULT_SPEED);
        printf("motor test PA2 PWM, PA3 high\r\n");
        return;
    }

    switch (cmd) {
        case CMD_MOTOR_TEST_CH0:
            channel = 0U;
            break;
        case CMD_MOTOR_TEST_CH1:
            channel = 1U;
            break;
        case CMD_MOTOR_TEST_CH2:
            channel = 2U;
            break;
        case CMD_MOTOR_TEST_CH3:
            channel = 3U;
            break;
        default:
            return;
    }

    // 诊断命令不走前进/后退语义，只单独输出一路PWM，用来核对IO和接线。
    state->motion = MOTION_DEBUG;
    motor_debug_channel(channel, ROBOT_DEFAULT_SPEED);
    printf("motor test channel:%d\r\n", channel);
}

static void reapply_current_motion(robot_state_t *state)
{
    if ((state == NULL) || (state->motion == MOTION_STOP)) {
        return;
    }

    apply_motor_motion(state->motion, state->speed);
}

static void set_robot_speed(robot_state_t *state, uint16_t speed)
{
    if (state == NULL) {
        return;
    }

    state->speed = clamp_speed(speed);
    reapply_current_motion(state);
}

static void adjust_robot_speed(robot_state_t *state, int16_t delta)
{
    int16_t next_speed = 0;

    if (state == NULL) {
        return;
    }

    next_speed = (int16_t)state->speed + delta;
    if (next_speed < (int16_t)ROBOT_MIN_SPEED) {
        next_speed = (int16_t)ROBOT_MIN_SPEED;
    } else if (next_speed > (int16_t)ROBOT_MAX_SPEED) {
        next_speed = (int16_t)ROBOT_MAX_SPEED;
    }

    set_robot_speed(state, (uint16_t)next_speed);
}

static void handle_uart6_command(uint8_t cmd, robot_state_t *state)
{
    if (state == NULL) {
        return;
    }

    // UART6用于蓝牙控制，支持运动、速度、蜂鸣器、眼灯和舵机命令。
    switch (cmd) {
        case CMD_BT_FORWARD:
        case CMD_BT_BACKWARD:
        case CMD_BT_RIGHT:
        case CMD_BT_LEFT:
        case CMD_FORWARD:
        case CMD_BACKWARD:
        case CMD_RIGHT:
        case CMD_LEFT:
        case CMD_STOP_HEART:
        case CMD_STOP_WALL_E:
        case CMD_BRAKE_STOP:
            handle_drive_command(cmd, state);
            break;
        case CMD_BUZZER_TOGGLE:
            switch_buzzer_status();
            break;
        case CMD_LEFT_EYE_ON:
            eye_left_on();
            break;
        case CMD_RIGHT_EYE_ON:
            eye_right_on();
            break;
        case CMD_SPEED_RESET:
            set_robot_speed(state, ROBOT_DEFAULT_SPEED);
            break;
        case CMD_LEFT_EYE_OFF:
            eye_left_off();
            break;
        case CMD_RIGHT_EYE_OFF:
            eye_right_off();
            break;
        case CMD_SPEED_MINUS_10:
            adjust_robot_speed(state, -10);
            break;
        case CMD_SPEED_MINUS_5:
            adjust_robot_speed(state, -5);
            break;
        case CMD_SPEED_PLUS_5:
            adjust_robot_speed(state, 5);
            break;
        case CMD_SPEED_PLUS_10:
            adjust_robot_speed(state, 10);
            break;
        case CMD_BT_SPEED_LOW:
            // 档位调速会立即刷新当前运动PWM。
            set_robot_speed(state, ROBOT_LOW_SPEED);
            break;
        case CMD_BT_SPEED_MEDIUM:
            set_robot_speed(state, ROBOT_MEDIUM_SPEED);
            break;
        case CMD_BT_SPEED_HIGH:
            set_robot_speed(state, ROBOT_HIGH_SPEED);
            break;
        case CMD_SERVO_LEFT_EYE:
        case CMD_SERVO_RIGHT_EYE:
        case CMD_SERVO_HEAD:
        case CMD_SERVO_BOTH_ARMS:
        case CMD_SERVO_LEFT_ARM:
        case CMD_SERVO_RIGHT_ARM:
            handle_servo_command(cmd, state);
            break;
        case CMD_MOTOR_TEST_CH0:
        case CMD_MOTOR_TEST_CH1:
        case CMD_MOTOR_TEST_CH2:
        case CMD_MOTOR_TEST_CH3:
        case CMD_MOTOR_TEST_STOP:
        case CMD_MOTOR_TEST_PA2_PA3_LOW:
        case CMD_MOTOR_TEST_PA2_PA3_HIGH:
            handle_motor_test_command(cmd, state);
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

    // USART2用于VC-02语音模块，语音侧只开放常用运动和眼灯控制。
    switch (cmd) {
        case CMD_FORWARD:              // 语音指令1：前进。
        case CMD_BACKWARD:             // 语音指令2：后退。
        case CMD_RIGHT:                // 语音指令3：右转。
        case CMD_LEFT:                 // 语音指令4：左转。
            handle_drive_command(cmd, state);
            break;
        case CMD_STOP_HEART:           // 语音指令5：停止并显示爱心图。
        case CMD_STOP_WALL_E:          // 语音指令6：停止并显示默认瓦力图。
        case CMD_BRAKE_STOP:           // 语音指令15：停止并显示默认瓦力图。
            handle_drive_command(cmd, state);
            break;
        case CMD_BUZZER_TOGGLE:
            switch_buzzer_status();
            break;
        case CMD_LEFT_EYE_ON:          // 语音指令8：打开左眼灯。
            eye_left_on();
            break;
        case CMD_RIGHT_EYE_ON:         // 语音指令9：打开右眼灯。
            eye_right_on();
            break;
        case CMD_SPEED_RESET:
            set_robot_speed(state, ROBOT_DEFAULT_SPEED);
            break;
        case CMD_LEFT_EYE_OFF:
            eye_left_off();
            break;
        case CMD_RIGHT_EYE_OFF:
            eye_right_off();
            break;
        case CMD_SPEED_MINUS_10:
            adjust_robot_speed(state, -10);
            break;
        case CMD_SPEED_MINUS_5:
            adjust_robot_speed(state, -5);
            break;
        case CMD_SPEED_PLUS_5:
            adjust_robot_speed(state, 5);
            break;
        case CMD_SPEED_PLUS_10:
            adjust_robot_speed(state, 10);
            break;
        case CMD_BT_SPEED_LOW:
            set_robot_speed(state, ROBOT_LOW_SPEED);
            break;
        case CMD_BT_SPEED_MEDIUM:
            set_robot_speed(state, ROBOT_MEDIUM_SPEED);
            break;
        case CMD_BT_SPEED_HIGH:
            set_robot_speed(state, ROBOT_HIGH_SPEED);
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

    // 清掉已处理数据，避免下一帧串口数据混入残留内容。
    memset(buf, 0, *len);
    *len = 0;
}

static void process_uart6_data(robot_state_t *state)
{
    uint8_t uart6_cmd = 0;

    if (!g_recv_complete_flag_uart6) {
        return;
    }

    // 串口中断置位完成标志，主循环中统一解析和执行命令。
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
    uint8_t usart2_cmd = 0;

    if (!g_recv_complete_flag_usart2) {
        return;
    }

    // 兼容语音模块发送的单字节命令和ASCII数字命令。
    g_recv_complete_flag_usart2 = 0;
    printf("g_recv_length:%d ", g_recv_length_usart2);
    printf("g_recv_buff:%s\r\n", g_recv_buff_usart2);

    if (parse_uart_command(g_recv_buff_usart2, g_recv_length_usart2, &usart2_cmd)) {
        handle_usart2_command(usart2_cmd, state);
    }

    clear_received_data(g_recv_buff_usart2, &g_recv_length_usart2);
}

static void update_motion_timeout(robot_state_t *state)
{
    if ((state == NULL) || (state->motion == MOTION_STOP)) {
        return;
    }

    if (state->motion_idle_ms < ROBOT_MOTION_TIMEOUT_MS) {
        state->motion_idle_ms = (uint16_t)(state->motion_idle_ms + ROBOT_LOOP_DELAY_MS);
    }

    if (state->motion_idle_ms >= ROBOT_MOTION_TIMEOUT_MS) {
        stop_robot(state, 0U, CMD_STOP_WALL_E);
    }
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
    // 按依赖顺序完成系统时钟、串口、外设和初始显示配置。
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
    motor_stop(0);                            // 上电默认熄火停止，避免驱动板高高刹车逻辑误动作
    PCA9685_Init(50, SERVO_HOME_ANGLE);       // PCA9685--16路舵机初始化  频率50Hz -- 0度

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

    // 先准备软件状态，再初始化硬件，避免外设启动后状态未定义。
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
        update_motion_timeout(&robot_state);  // 长时间没有运动命令时自动停车
    }
}
