// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _DRIVER_TIMER_H_
#define _DRIVER_TIMER_H_
#include "esp_err.h"
#include "esp_attr.h"
#include "soc/soc.h"
#include "soc/timer_periph.h"
#include "esp_intr_alloc.h"
#include "hal/timer_types.h"

#ifdef __cplusplus
extern "C" {
#endif


#define TIMER_BASE_CLK   (APB_CLK_FREQ)  /*!< Frequency of the clock on the input of the timer groups */

/**
 * @brief Selects a Timer-Group out of 2 available groups
 */
typedef enum {
    TIMER_GROUP_0 = 0, /*!<Hw timer group 0*/
    TIMER_GROUP_1 = 1, /*!<Hw timer group 1*/
    TIMER_GROUP_MAX,
} timer_group_t;

/**
 * @brief Decides the direction of counter
 */
typedef enum {
    TIMER_COUNT_DOWN = 0, /*!< Descending Count from cnt.high|cnt.low*/
    TIMER_COUNT_UP = 1,   /*!< Ascending Count from Zero*/
    TIMER_COUNT_MAX
} timer_count_dir_t;

/**
 * @brief Decides whether to enable alarm mode
 */
typedef enum {
    TIMER_ALARM_DIS = 0,  /*!< Disable timer alarm*/
    TIMER_ALARM_EN = 1,   /*!< Enable timer alarm*/
    TIMER_ALARM_MAX
} timer_alarm_t;

/**
 * @brief Select interrupt type if running in alarm mode.
 */
typedef enum {
    TIMER_INTR_LEVEL = 0,  /*!< Interrupt mode: level mode*/
    //TIMER_INTR_EDGE = 1, /*!< Interrupt mode: edge mode, Not supported Now*/
    TIMER_INTR_MAX
} timer_intr_mode_t;

/**
 * @brief Select if Alarm needs to be loaded by software or automatically reload by hardware.
 */
typedef enum {
    TIMER_AUTORELOAD_DIS = 0,  /*!< Disable auto-reload: hardware will not load counter value after an alarm event*/
    TIMER_AUTORELOAD_EN = 1,   /*!< Enable auto-reload: hardware will load counter value after an alarm event*/
    TIMER_AUTORELOAD_MAX,
} timer_autoreload_t;

/**
 * @brief Data structure with timer's configuration settings
 */
typedef struct {
    bool alarm_en;      /*!< Timer alarm enable */
    bool counter_en;    /*!< Counter enable */
    timer_intr_mode_t intr_type; /*!< Interrupt mode */
    timer_count_dir_t counter_dir; /*!< Counter direction  */
    bool auto_reload;   /*!< Timer auto-reload */
    uint32_t divider;   /*!< Counter clock divider. The divider's range is from from 2 to 65536. */
} timer_config_t;


/**
 * @brief Interrupt handle, used in order to free the isr after use.
 * Aliases to an int handle for now.
 */
typedef intr_handle_t timer_isr_handle_t;

/**
 * @brief Read the counter value of hardware timer.
 *
 * @param group_num Timer group, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index, 0 for hw_timer[0] & 1 for hw_timer[1]
 * @param timer_val Pointer to accept timer counter value.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_get_counter_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t* timer_val);

/**
 * @brief Read the counter value of hardware timer, in unit of a given scale.
 *
 * @param group_num Timer group, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index, 0 for hw_timer[0] & 1 for hw_timer[1]
 * @param time Pointer, type of double*, to accept timer counter value, in seconds.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_get_counter_time_sec(timer_group_t group_num, timer_idx_t timer_num, double* time);

/**
 * @brief Set counter value to hardware timer.
 *
 * @param group_num Timer group, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index, 0 for hw_timer[0] & 1 for hw_timer[1]
 * @param load_val Counter value to write to the hardware timer.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_set_counter_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t load_val);

/**
 * @brief Start the counter of hardware timer.
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index, 0 for hw_timer[0] & 1 for hw_timer[1]
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_start(timer_group_t group_num, timer_idx_t timer_num);

/**
 * @brief Pause the counter of hardware timer.
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index, 0 for hw_timer[0] & 1 for hw_timer[1]
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_pause(timer_group_t group_num, timer_idx_t timer_num);

/**
 * @brief Set counting mode for hardware timer.
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index, 0 for hw_timer[0] & 1 for hw_timer[1]
 * @param counter_dir Counting direction of timer, count-up or count-down
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_set_counter_mode(timer_group_t group_num, timer_idx_t timer_num, timer_count_dir_t counter_dir);

/**
 * @brief Enable or disable counter reload function when alarm event occurs.
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index, 0 for hw_timer[0] & 1 for hw_timer[1]
 * @param reload Counter reload mode.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_set_auto_reload(timer_group_t group_num, timer_idx_t timer_num, timer_autoreload_t reload);

/**
 * @brief Set hardware timer source clock divider. Timer groups clock are divider from APB clock.
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index, 0 for hw_timer[0] & 1 for hw_timer[1]
 * @param divider Timer clock divider value. The divider's range is from from 2 to 65536.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_set_divider(timer_group_t group_num, timer_idx_t timer_num, uint32_t divider);

/**
 * @brief Set timer alarm value.
 *
 * @param group_num Timer group, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index, 0 for hw_timer[0] & 1 for hw_timer[1]
 * @param alarm_value A 64-bit value to set the alarm value.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_set_alarm_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t alarm_value);

/**
 * @brief Get timer alarm value.
 *
 * @param group_num Timer group, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index, 0 for hw_timer[0] & 1 for hw_timer[1]
 * @param alarm_value Pointer of A 64-bit value to accept the alarm value.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_get_alarm_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t* alarm_value);

/**
 * @brief Enable or disable generation of timer alarm events.
 *
 * @param group_num Timer group, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index, 0 for hw_timer[0] & 1 for hw_timer[1]
 * @param alarm_en To enable or disable timer alarm function.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_set_alarm(timer_group_t group_num, timer_idx_t timer_num, timer_alarm_t alarm_en);

/**
 * @brief Register Timer interrupt handler, the handler is an ISR.
 *        The handler will be attached to the same CPU core that this function is running on.
 *
 * @param group_num Timer group number
 * @param timer_num Timer index of timer group
 * @param fn Interrupt handler function.
 * @param arg Parameter for handler function
 * @param intr_alloc_flags Flags used to allocate the interrupt. One or multiple (ORred)
 *        ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info.
 * @param handle Pointer to return handle. If non-NULL, a handle for the interrupt will
 *        be returned here.
 *
 * @note If the intr_alloc_flags value ESP_INTR_FLAG_IRAM is set,
 *       the handler function must be declared with IRAM_ATTR attribute
 *       and can only call functions in IRAM or ROM. It cannot call other timer APIs.
 *       Use direct register access to configure timers from inside the ISR in this case.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_isr_register(timer_group_t group_num, timer_idx_t timer_num, void (*fn)(void*), void * arg, int intr_alloc_flags, timer_isr_handle_t *handle);

/** @brief Initializes and configure the timer.
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index, 0 for hw_timer[0] & 1 for hw_timer[1]
 * @param config Pointer to timer initialization parameters.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_init(timer_group_t group_num, timer_idx_t timer_num, const timer_config_t* config);

/** @brief Get timer configure value.
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index, 0 for hw_timer[0] & 1 for hw_timer[1]
 * @param config Pointer of struct to accept timer parameters.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_get_config(timer_group_t group_num, timer_idx_t timer_num, timer_config_t *config);

/** @brief Enable timer group interrupt, by enable mask
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param intr_mask Timer interrupt enable mask.
 *          - TIMER_INTR_T0: t0 interrupt
 *          - TIMER_INTR_T1: t1 interrupt
 *          - TIMER_INTR_WDT: watchdog interrupt
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_group_intr_enable(timer_group_t group_num, timer_intr_t intr_mask);

/** @brief Disable timer group interrupt, by disable mask
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param intr_mask Timer interrupt disable mask.
 *          - TIMER_INTR_T0: t0 interrupt
 *          - TIMER_INTR_T1: t1 interrupt
 *          - TIMER_INTR_WDT: watchdog interrupt
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_group_intr_disable(timer_group_t group_num, timer_intr_t intr_mask);

/** @brief Enable timer interrupt
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_enable_intr(timer_group_t group_num, timer_idx_t timer_num);

/** @brief Disable timer interrupt
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_disable_intr(timer_group_t group_num, timer_idx_t timer_num);

/** @cond */
/* Utilities functions that can be used in the ISR */
/* Preview, don't treat them as stable API. */

/**
 * Clear interrupt status bit.
 */
void timer_group_intr_clr_in_isr(timer_group_t group_num, timer_idx_t timer_num);

/**
 * Enable alarm.
 */
void timer_group_enable_alarm_in_isr(timer_group_t group_num, timer_idx_t timer_num);

/**
 * Get the current counter value.
 */
uint64_t timer_group_get_counter_value_in_isr(timer_group_t group_num, timer_idx_t timer_num);

/**
 * Set the alarm threshold for the timer.
 */
void timer_group_set_alarm_value_in_isr(timer_group_t group_num, timer_idx_t timer_num, uint64_t alarm_val);

/**
 * Enable/disable a counter.
 */
void timer_group_set_counter_enable_in_isr(timer_group_t group_num, timer_idx_t timer_num, timer_start_t counter_en);

/**
 * Get the masked interrupt status.
 */
timer_intr_t timer_group_intr_get_in_isr(timer_group_t group_num);

/**
 * Clear interrupt.
 */
void timer_group_clr_intr_sta_in_isr(timer_group_t group_num, timer_intr_t intr_mask);

/**
 * Get auto reload enable status.
 */
bool timer_group_get_auto_reload_in_isr(timer_group_t group_num, timer_idx_t timer_num);

/** @endcond */

#ifdef __cplusplus
}
#endif

#endif /* _TIMER_H_ */
