/* To set the control period for DC motor Timer

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include "motor_ctrl_timer.h"
#include "esp_check.h"

#define MOTOR_CTRL_TIMER_DIVIDER         (16)  //  Hardware timer clock divider
#define MOTOR_CTRL_TIMER_SCALE           (TIMER_BASE_CLK / MOTOR_CTRL_TIMER_DIVIDER)  // convert counter value to seconds

#define MOTOR_CONTROL_TIMER_GROUP        TIMER_GROUP_0
#define MOTOR_CONTROL_TIMER_ID           TIMER_0

static const char *TAG = "motor_ctrl_timer";

/**
 * @brief Callback function of timer intterupt
 *
 * @param args The parameter transmited to callback function from timer_isr_callback_add. Args here is for timer_info.
 * @return
 *     - True Do task yield at the end of ISR
 *     - False Not do task yield at the end of ISR
*/
static bool IRAM_ATTR motor_ctrl_timer_isr_callback(void *args)
{
    BaseType_t high_task_awoken = pdFALSE;
    motor_ctrl_timer_info_t *info = (motor_ctrl_timer_info_t *) args;
    info->pulse_info.pulse_cnt = info->pulse_info.get_pulse_callback(info->pulse_info.callback_args);

    /* Now just send the event data back to the main program task */
    xQueueSendFromISR(info->timer_evt_que, info, &high_task_awoken);

    return high_task_awoken == pdTRUE; // return whether we need to yield at the end of ISR
}

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
                               pulse_info_t pulse_info)
{
    esp_err_t ret = ESP_FAIL;
    /* Select and initialize basic parameters of the timer */
    timer_config_t config = {
        .divider = MOTOR_CTRL_TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = true,
    }; // default clock source is APB
    ret = timer_init(MOTOR_CONTROL_TIMER_GROUP, MOTOR_CONTROL_TIMER_ID, &config);
    ESP_RETURN_ON_ERROR(ret, TAG, "timer init failed\n");

    /* Timer's counter will initially start from value below.
       Since auto_reload is set, this value will be automatically reload on alarm */
    timer_set_counter_value(MOTOR_CONTROL_TIMER_GROUP, MOTOR_CONTROL_TIMER_ID, 0);

    /* Configure the alarm value and the interrupt on alarm. */
    timer_set_alarm_value(MOTOR_CONTROL_TIMER_GROUP, MOTOR_CONTROL_TIMER_ID, ctrl_period_ms * MOTOR_CTRL_TIMER_SCALE / 1000);
    timer_enable_intr(MOTOR_CONTROL_TIMER_GROUP, MOTOR_CONTROL_TIMER_ID);

    /* Check the pointers */
    ESP_GOTO_ON_FALSE(evt_que, ESP_ERR_INVALID_ARG, err, TAG, "timer event queue handler is NULL\n");
    ESP_GOTO_ON_FALSE(timer_info, ESP_ERR_INVALID_ARG, err, TAG, "timer info structure pointer is NULL\n");
    /* Alloc and config the infomation structure for this file */
    *timer_info = calloc(1, sizeof(motor_ctrl_timer_info_t));
    ESP_GOTO_ON_FALSE(*timer_info, ESP_ERR_NO_MEM, err, TAG, "timer_info calloc failed\n");
    (*timer_info)->timer_group = MOTOR_CONTROL_TIMER_GROUP;
    (*timer_info)->timer_idx = MOTOR_CONTROL_TIMER_ID;
    (*timer_info)->timer_evt_que = evt_que;
    (*timer_info)->ctrl_period_ms = ctrl_period_ms;
    (*timer_info)->pulse_info = pulse_info;
    timer_isr_callback_add(MOTOR_CONTROL_TIMER_GROUP, MOTOR_CONTROL_TIMER_ID, motor_ctrl_timer_isr_callback, *timer_info, 0);

    return ret;
err:
    timer_deinit(MOTOR_CONTROL_TIMER_GROUP, MOTOR_CONTROL_TIMER_ID);
    return ret;
}

/**
 * @brief Set timer alarm period
 *
 * @param period Timer alarm period
 * @return
 *      - void
 */
void motor_ctrl_timer_set_period(unsigned int period)
{
    timer_set_alarm_value(MOTOR_CONTROL_TIMER_GROUP, MOTOR_CONTROL_TIMER_ID, period * MOTOR_CTRL_TIMER_SCALE / 1000);
}

/**
 * @brief Start the timer
 */
void motor_ctrl_timer_start(void)
{
    /* start the timer */
    timer_start(MOTOR_CONTROL_TIMER_GROUP, MOTOR_CONTROL_TIMER_ID);
}


/**
 * @brief Pause the timer and clear the counting value
 */
void motor_ctrl_timer_stop(void)
{
    /* stop the timer */
    timer_pause(MOTOR_CONTROL_TIMER_GROUP, MOTOR_CONTROL_TIMER_ID);
    timer_set_counter_value(MOTOR_CONTROL_TIMER_GROUP, MOTOR_CONTROL_TIMER_ID, 0);
}

/**
 * @brief Deinitialize the timer
 *
 * @param timer_info the secondary pointer of motor_ctrl_timer_info_t, the memory will be freed
 */
void motor_ctrl_timer_deinit(motor_ctrl_timer_info_t **timer_info)
{
    if (*timer_info != NULL) {
        timer_deinit(MOTOR_CONTROL_TIMER_GROUP, MOTOR_CONTROL_TIMER_ID);
        free(*timer_info);
        *timer_info = NULL;
    }
}
