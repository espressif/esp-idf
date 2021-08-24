/* cmd_mcpwm_motor.h

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include "rotary_encoder.h"
#include "motor_ctrl_timer.h"
#include "pid_ctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_PWM0A_OUT 15   //Set GPIO 15 as PWM0A
#define GPIO_PWM0B_OUT 16   //Set GPIO 16 as PWM0B
#define GPIO_PCNT_PINA 18   //Set GPIO 18 as phaseA/C1
#define GPIO_PCNT_PINB 19   //Set GPIO 19 as phaseB/C2

typedef enum {
    MOTOR_CTRL_MODE_FIXED = 0,
    MOTOR_CTRL_MODE_TRIANGLE,
    MOTOR_CTRL_MODE_RECTANGLE
} expect_mode_t;

typedef struct {
    /* Handles */
    rotary_encoder_t *encoder;               // PCNT rotary encoder handler
    motor_ctrl_timer_info_t *timer_info;     // Timer infomation handler
    pid_ctrl_block_handle_t pid;             // PID algoritm handler
    pid_ctrl_parameter_t pid_param;          // PID parameters
    QueueHandle_t timer_evt_que;             // Timer event queue handler

    /* Control visualization */
    int pulse_in_one_period;                 // PCNT pulse in one control period
    float error;                             // The error between the expectation(expt) and actual value (pulse_in_one_period)
    float expt;                              // The expectation
    float pid_output;                        // PID algorithm output

    /* Status */
    unsigned int sec_cnt;                    // Seconds count
    bool start_flag;                         // Motor start flag

    /* Configurations */
    struct {
        /* PID configuration */
        bool pid_enable;                     // PID enable flag

        /* Expectation configuration */
        float expt_init;                     // Initial expectation
        float expt_max;                      // Max expectation in dynamic mode
        float expt_min;                      // Min expectation in dynamic mode
        float expt_pace;                     // The expection pace. It can change expectation wave period
        expect_mode_t expt_mode;             // Expectation wave mode (MOTOR_CTRL_EXPT_FIXED/MOTOR_CTRL_EXPT_TRIANGLE/MOTOR_CTRL_EXPT_RECTANGLE)

        /* Other configurations */
        unsigned int ctrl_period;            // Control period
        unsigned int pwm_freq;               // MCPWM output frequency
        unsigned int running_sec;            // Motor running seconds
    } cfg;                                   // Configurations that should be initialized for this example
} mcpwm_motor_control_t;


/**
 * @brief Set pwm duty to drive the motor
 *
 * @param duty_cycle PWM duty cycle (100~-100), the motor will go backward if the duty is set to a negative value
 */
void brushed_motor_set_duty(float duty_cycle);

/**
 * @brief start motor
 *
 * @param mc mcpwm_motor_control_t pointer
 */
void brushed_motor_start(mcpwm_motor_control_t *mc);

/**
 * @brief stop motor
 *
 * @param mc mcpwm_motor_control_t pointer
 */
void brushed_motor_stop(mcpwm_motor_control_t *mc);

/**
 * @brief Initialize the motor control console
 *
 * @param motor_ctrl The top infomation struct of this example
 */
extern void cmd_mcpwm_motor_init(mcpwm_motor_control_t *motor_ctrl);

#ifdef __cplusplus
}
#endif
