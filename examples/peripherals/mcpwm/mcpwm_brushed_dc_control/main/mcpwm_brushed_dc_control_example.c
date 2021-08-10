/* brushed dc motor control example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
 * This example will show you how to use MCPWM module to control brushed dc motor.
 * This code is tested with L298 motor driver.
 * User may need to make changes according to the motor driver they use.
*/

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_attr.h"

#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"
#include "driver/pcnt.h"

#include "mcpwm_brushed_dc_control_example.h"

#define MOTOR_CTRL_MCPWM_UNIT   MCPWM_UNIT_0
#define MOTOR_CTRL_MCPWM_TIMER  MCPWM_TIMER_0

/* The global infomation structure */
static mcpwm_motor_control_t motor_ctrl;

SemaphoreHandle_t g_motor_mux;

/**
 * @brief Initialize the gpio as mcpwm output
 */
static void mcpwm_example_gpio_initialize(void)
{
    printf("initializing mcpwm gpio...\n");
    mcpwm_gpio_init(MOTOR_CTRL_MCPWM_UNIT, MCPWM0A, GPIO_PWM0A_OUT);
    mcpwm_gpio_init(MOTOR_CTRL_MCPWM_UNIT, MCPWM0B, GPIO_PWM0B_OUT);
}

/**
 * @brief set motor moves speed and direction with duty cycle = duty %
 */
void brushed_motor_set_duty(float duty_cycle)
{
    /* motor moves in forward direction, with duty cycle = duty % */
    if (duty_cycle > 0) {
        mcpwm_set_signal_low(MOTOR_CTRL_MCPWM_UNIT, MOTOR_CTRL_MCPWM_TIMER, MCPWM_OPR_A);
        mcpwm_set_duty(MOTOR_CTRL_MCPWM_UNIT, MOTOR_CTRL_MCPWM_TIMER, MCPWM_OPR_B, duty_cycle);
        mcpwm_set_duty_type(MOTOR_CTRL_MCPWM_UNIT, MOTOR_CTRL_MCPWM_TIMER, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);  //call this each time, if operator was previously in low/high state
    }
    /* motor moves in backward direction, with duty cycle = -duty % */
    else {
        mcpwm_set_signal_low(MOTOR_CTRL_MCPWM_UNIT, MOTOR_CTRL_MCPWM_TIMER, MCPWM_OPR_B);
        mcpwm_set_duty(MOTOR_CTRL_MCPWM_UNIT, MOTOR_CTRL_MCPWM_TIMER, MCPWM_OPR_A, -duty_cycle);
        mcpwm_set_duty_type(MOTOR_CTRL_MCPWM_UNIT, MOTOR_CTRL_MCPWM_TIMER, MCPWM_OPR_A, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
    }
}

/**
 * @brief start motor
 *
 * @param mc mcpwm_motor_control_t pointer
 */
void brushed_motor_start(mcpwm_motor_control_t *mc)
{
    motor_ctrl_timer_start();
    mc->sec_cnt = 0;
    mc->start_flag = true;
}

/**
 * @brief stop motor
 *
 * @param mc mcpwm_motor_control_t pointer
 */
void brushed_motor_stop(mcpwm_motor_control_t *mc)
{
    mc->expt = 0;
    mc->sec_cnt = 0;
    mc->start_flag = false;
    motor_ctrl_timer_stop();
    brushed_motor_set_duty(0);
}

/**
 * @brief The callback function of timer interrupt
 * @note This callback is called by timer interrupt callback. It need to offer the PCNT pulse in one control period for PID calculation
 * @param args the rotary_encoder_t pointer, it is given by timer interrupt callback
 * @return
 *      - int: the PCNT pulse in one control period
 */
static int pcnt_get_pulse_callback(void *args)
{
    /* Record the last count value */
    static unsigned int last_pulse = 0;
    /* Get the encoder from args */
    rotary_encoder_t *encoder = (rotary_encoder_t *)args;
    /* Get the value current count value */
    unsigned int temp = encoder->get_counter_value(encoder);
    /* Calculate the pulse count in one control period */
    unsigned int ret = temp - last_pulse;
    /* Update last count value */
    last_pulse = temp;

    return (int)ret;
}

/**
 * @brief Initialize the PCNT rotaty encoder
 */
static void motor_ctrl_default_init(void)
{
    motor_ctrl.cfg.pid_enable = true;
    motor_ctrl.pid_param.kp = 0.8;
    motor_ctrl.pid_param.ki = 0.0;
    motor_ctrl.pid_param.kd = 0.1;
    motor_ctrl.pid_param.cal_type = PID_CAL_TYPE_INCREMENTAL;
    motor_ctrl.pid_param.max_output   = 100;
    motor_ctrl.pid_param.min_output   = -100;
    motor_ctrl.pid_param.max_integral = 1000;
    motor_ctrl.pid_param.min_integral = -1000;
    motor_ctrl.cfg.expt_init = 30;
    motor_ctrl.cfg.expt_mode = MOTOR_CTRL_MODE_TRIANGLE;
    motor_ctrl.cfg.expt_max = 50;
    motor_ctrl.cfg.expt_min = -50;
    motor_ctrl.cfg.expt_pace = 1.0;
    motor_ctrl.cfg.pwm_freq = 1000;
    motor_ctrl.cfg.running_sec = 10;
    motor_ctrl.cfg.ctrl_period = 10;
}

/**
 * @brief Initialize the PCNT rotaty encoder
 */
static void motor_ctrl_pcnt_rotary_encoder_init(void)
{
    /* Rotary encoder underlying device is represented by a PCNT unit in this example */
    uint32_t pcnt_unit = 0;
    /* Create rotary encoder instance */
    rotary_encoder_config_t config = ROTARY_ENCODER_DEFAULT_CONFIG(
                                         (rotary_encoder_dev_t)pcnt_unit,
                                         GPIO_PCNT_PINA, GPIO_PCNT_PINB);
    ESP_ERROR_CHECK(rotary_encoder_new_ec11(&config, &motor_ctrl.encoder));
    /* Filter out glitch (1us) */
    ESP_ERROR_CHECK(motor_ctrl.encoder->set_glitch_filter(motor_ctrl.encoder, 1));
    /* Start encoder */
    ESP_ERROR_CHECK(motor_ctrl.encoder->start(motor_ctrl.encoder));
    pcnt_counter_clear((pcnt_unit_t)pcnt_unit);
}

/**
 * @brief Initialize the MCPWM
 */
static void motor_ctrl_mcpwm_init(void)
{
    /* mcpwm gpio initialization */
    mcpwm_example_gpio_initialize();
    /* initial mcpwm configuration */
    printf("Configuring Initial Parameters of mcpwm...\n");
    mcpwm_config_t pwm_config;
    pwm_config.frequency = motor_ctrl.cfg.pwm_freq;     //frequency = 1kHz,
    pwm_config.cmpr_a = 0;                              //initial duty cycle of PWMxA = 0
    pwm_config.cmpr_b = 0;                              //initial duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;         //up counting mode
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MOTOR_CTRL_MCPWM_UNIT, MOTOR_CTRL_MCPWM_TIMER, &pwm_config);    //Configure PWM0A & PWM0B with above settings
}

/**
 * @brief Initialize the timer
 */
static void motor_ctrl_timer_init(void)
{
    /* Initialize timer alarm event queue */
    motor_ctrl.timer_evt_que = xQueueCreate(10, sizeof(motor_ctrl_timer_info_t));
    /* Set PCNT rotary encoder handler and pulse getting callback function */
    pulse_info_t pulse_info = {.callback_args = motor_ctrl.encoder,
                               .get_pulse_callback = pcnt_get_pulse_callback
                              };
    motor_ctrl_new_timer(&motor_ctrl.timer_info, motor_ctrl.timer_evt_que, motor_ctrl.cfg.ctrl_period, pulse_info);
}

/**
 * @brief the top initialization function in this example
 */
static void motor_ctrl_init_all(void)
{
    /* 1. Set default configurations */
    motor_ctrl_default_init();
    /* 2.rotary encoder initialization */
    motor_ctrl_pcnt_rotary_encoder_init();
    /* 3.MCPWM initialization */
    motor_ctrl_mcpwm_init();
    /* 4.pid_ctrl initialization */
    pid_ctrl_config_t pid_config = {
        .init_param = motor_ctrl.pid_param,
    };
    pid_new_control_block(&pid_config, &motor_ctrl.pid);
    /* 5.Timer initialization */
    motor_ctrl_timer_init();
}

/**
 * @brief Motor control thread
 *
 * @param arg Information pointer transmitted by task creating function
 */
static void mcpwm_brushed_motor_ctrl_thread(void *arg)
{
    motor_ctrl_timer_info_t recv_info;
    while (1) {
        /* Wait for recieving information of timer interrupt from timer event queque */
        xQueueReceive(motor_ctrl.timer_evt_que, &recv_info, portMAX_DELAY);
        /* Get the pcnt pulse during one control period */
        motor_ctrl.pulse_in_one_period = recv_info.pulse_info.pulse_cnt;
        if (motor_ctrl.cfg.pid_enable) {
            /* Calculate the output by PID algorithm according to the pulse. Pid_output here is the duty of MCPWM */
            motor_ctrl.error = motor_ctrl.expt - motor_ctrl.pulse_in_one_period;
            pid_compute(motor_ctrl.pid, motor_ctrl.error, &motor_ctrl.pid_output);
        } else {
            motor_ctrl.pid_output = motor_ctrl.expt;
        }

        /* Set the MCPWM duty */
        brushed_motor_set_duty(motor_ctrl.pid_output);
    }
}

/**
 * @brief Motor control thread
 *
 * @param arg Information pointer transmitted by task creating function
 */
static void mcpwm_brushed_motor_expt_thread(void *arg)
{
    float cnt = 0;
    while (1) {
        xSemaphoreTake(g_motor_mux, portMAX_DELAY);
        switch (motor_ctrl.cfg.expt_mode) {
        /* Static expectation */
        case MOTOR_CTRL_MODE_FIXED:
            motor_ctrl.expt = motor_ctrl.cfg.expt_init;
            break;
        /* Dynamic expectation: triangle wave */
        case MOTOR_CTRL_MODE_TRIANGLE:
            motor_ctrl.expt += motor_ctrl.cfg.expt_pace;
            motor_ctrl.cfg.expt_pace = (motor_ctrl.expt > motor_ctrl.cfg.expt_max - 0.0001 ||
                                        motor_ctrl.expt < motor_ctrl.cfg.expt_min + 0.0001) ?
                                       - motor_ctrl.cfg.expt_pace : motor_ctrl.cfg.expt_pace;
            break;
        /* Dynamic expectation: rectangle wave */
        case MOTOR_CTRL_MODE_RECTANGLE:
            cnt += motor_ctrl.cfg.expt_pace;
            if (cnt > motor_ctrl.cfg.expt_max - 0.0001) {
                motor_ctrl.cfg.expt_pace = -motor_ctrl.cfg.expt_pace;
                motor_ctrl.expt = motor_ctrl.cfg.expt_min;
            }
            if (cnt < motor_ctrl.cfg.expt_min - 0.0001) {
                motor_ctrl.cfg.expt_pace = -motor_ctrl.cfg.expt_pace;
                motor_ctrl.expt = motor_ctrl.cfg.expt_max;
            }
            break;
        default:
            motor_ctrl.expt = motor_ctrl.cfg.expt_init;
            break;
        }
        xSemaphoreGive(g_motor_mux);
        /* Motor automatic stop judgement */
        if (motor_ctrl.start_flag) {
            motor_ctrl.sec_cnt++;
            /* Show the seconds count */
            if ((motor_ctrl.sec_cnt + 1) % 20 == 0) {
                printf("%d\n", (motor_ctrl.sec_cnt + 1) / 20);
            }
            /* Stop motor if time up */
            if (motor_ctrl.sec_cnt > 20 * motor_ctrl.cfg.running_sec && motor_ctrl.cfg.running_sec != 0) {
                brushed_motor_stop(&motor_ctrl);
                printf("\nTime up: motor stoped\n");
            }
        }

        /* Delay 50ms */
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief The main entry of this example
 */
void app_main(void)
{
    printf("Testing brushed motor with PID...\n");
    /* Create semaphore */
    g_motor_mux = xSemaphoreCreateMutex();
    /* Initialize peripherals and modules */
    motor_ctrl_init_all();
    /* Initialize the console */
    cmd_mcpwm_motor_init(&motor_ctrl);
    /* Motor control thread */
    xTaskCreate(mcpwm_brushed_motor_ctrl_thread, "mcpwm_brushed_motor_ctrl_thread", 4096, NULL, 3, NULL);
    /* Motor expectation wave generate thread */
    xTaskCreate(mcpwm_brushed_motor_expt_thread, "mcpwm_brushed_motor_expt_thread", 4096, NULL, 5, NULL);
}
