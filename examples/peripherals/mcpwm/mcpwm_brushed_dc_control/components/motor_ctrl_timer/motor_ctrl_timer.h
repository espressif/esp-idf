/* To set the control period for DC motor Timer

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/timer.h"

typedef struct {
    int (*get_pulse_callback)(void *);
    void *callback_args;
    int pulse_cnt;
} pulse_info_t;


typedef struct {
    timer_group_t timer_group;      /* Timer Group number */
    timer_idx_t timer_idx;          /* Timer ID */
    unsigned int ctrl_period_ms;    /* Motor control period, unit in ms */
    QueueHandle_t timer_evt_que;    /* The queue of timer events */
    pulse_info_t pulse_info;
} motor_ctrl_timer_info_t;

/**
 * @brief Initialize the motor control timer
 *
 * @param timer_info the secondary pointer of motor_ctrl_timer_info_t
 * @param evt_que timer event queue
 * @param ctrl_period_ms motor control period
 * @param pulse_info quadrature encoder pulse information
 *  @return
 *      - ESP_OK: Motor control timer initialized successfully
 *      - ESP_FAIL: motor control timer failed to initialize because of other errors
 */
esp_err_t motor_ctrl_new_timer(motor_ctrl_timer_info_t **timer_info,
                               QueueHandle_t evt_que,
                               unsigned int ctrl_period_ms,
                               pulse_info_t pulse_info);

/**
 * @brief Set timer alarm period
 *
 * @param period Timer alarm period
 */
void motor_ctrl_timer_set_period(unsigned int period);

/**
 * @brief Start the timer
 */
void motor_ctrl_timer_start(void);


/**
 * @brief Pause the timer and clear the counting value
 */
void motor_ctrl_timer_stop(void);

/**
 * @brief Deinitialize the timer
 *
 * @param timer_info the secondary pointer of motor_ctrl_timer_info_t, the memory will be freed
 */
void motor_ctrl_timer_deinit(motor_ctrl_timer_info_t **timer_info);

#ifdef __cplusplus
}
#endif
