// Copyright 2010-2019 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

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

typedef void (*timer_isr_t)(void *);

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
esp_err_t timer_get_counter_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t *timer_val);

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
esp_err_t timer_get_counter_time_sec(timer_group_t group_num, timer_idx_t timer_num, double *time);

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
esp_err_t timer_get_alarm_value(timer_group_t group_num, timer_idx_t timer_num, uint64_t *alarm_value);

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
 * @brief Add ISR handle callback for the corresponding timer.
 *
 * @param group_num Timer group number
 * @param timer_num Timer index of timer group
 * @param isr_handler Interrupt handler function, it is a callback function.
 * @param arg Parameter for handler function
 * @param intr_alloc_flags Flags used to allocate the interrupt. One or multiple (ORred)
 *        ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info.
 *
 * @note This ISR handler will be called from an ISR.
 *       This ISR handler do not need to handle interrupt status, and should be kept short.
 *       If you want to realize some specific applications or write the whole ISR, you can
 *       call timer_isr_register(...) to register ISR.
 *
 *       If the intr_alloc_flags value ESP_INTR_FLAG_IRAM is set,
 *       the handler function must be declared with IRAM_ATTR attribute
 *       and can only call functions in IRAM or ROM. It cannot call other timer APIs.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_isr_callback_add(timer_group_t group_num, timer_idx_t timer_num, timer_isr_t isr_handler, void *arg, int intr_alloc_flags);

/**
 * @brief Remove ISR handle callback for the corresponding timer.
 *
 * @param group_num Timer group number
 * @param timer_num Timer index of timer group
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_isr_callback_remove(timer_group_t group_num, timer_idx_t timer_num);

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
 * @note If use this function to reigster ISR, you need to write the whole ISR.
 *       In the interrupt handler, you need to call timer_spinlock_take(..) before
 *       your handling, and call timer_spinlock_give(...) after your handling.
 *
 *       If the intr_alloc_flags value ESP_INTR_FLAG_IRAM is set,
 *       the handler function must be declared with IRAM_ATTR attribute
 *       and can only call functions in IRAM or ROM. It cannot call other timer APIs.
 *       Use direct register access to configure timers from inside the ISR in this case.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_isr_register(timer_group_t group_num, timer_idx_t timer_num, void (*fn)(void *), void *arg, int intr_alloc_flags, timer_isr_handle_t *handle);

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
esp_err_t timer_init(timer_group_t group_num, timer_idx_t timer_num, const timer_config_t *config);

/** @brief Deinitializes the timer.
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index, 0 for hw_timer[0] & 1 for hw_timer[1]
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_deinit(timer_group_t group_num, timer_idx_t timer_num);

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

/** @brief Clear timer interrupt status, just used in ISR
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index.
 *
 */
void timer_group_intr_clr_in_isr(timer_group_t group_num, timer_idx_t timer_num) __attribute__((deprecated));

/** @brief Clear timer interrupt status, just used in ISR
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index.
 *
 */
void timer_group_clr_intr_status_in_isr(timer_group_t group_num, timer_idx_t timer_num);

/** @brief Enable alarm interrupt, just used in ISR
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index.
 *
 */
void timer_group_enable_alarm_in_isr(timer_group_t group_num, timer_idx_t timer_num);

/** @brief Get the current counter value, just used in ISR
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index.
 *
 * @return
 *     - Counter value
 */
uint64_t timer_group_get_counter_value_in_isr(timer_group_t group_num, timer_idx_t timer_num);

/** @brief Set the alarm threshold for the timer, just used in ISR
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index.
 * @param alarm_val Alarm threshold.
 *
 */
void timer_group_set_alarm_value_in_isr(timer_group_t group_num, timer_idx_t timer_num, uint64_t alarm_val);

/** @brief Enable/disable a counter, just used in ISR
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index.
 * @param counter_en Enable/disable.
 *
 */
void timer_group_set_counter_enable_in_isr(timer_group_t group_num, timer_idx_t timer_num, timer_start_t counter_en);

/** @brief Get the masked interrupt status, just used in ISR
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 *
 * @return
 *     - Interrupt status
 */
timer_intr_t timer_group_intr_get_in_isr(timer_group_t group_num) __attribute__((deprecated));

/** @brief Get interrupt status, just used in ISR
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 *
 * @return
 *     - Interrupt status
 */
uint32_t timer_group_get_intr_status_in_isr(timer_group_t group_num);

/** @brief Clear the masked interrupt status, just used in ISR
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param intr_mask Masked interrupt.
 *
 */
void timer_group_clr_intr_sta_in_isr(timer_group_t group_num, timer_intr_t intr_mask) __attribute__((deprecated));

/** @brief Get auto reload enable status, just used in ISR
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 * @param timer_num Timer index
 *
 * @return
 *     - True Auto reload enabled
 *     - False Auto reload disabled
 */
bool timer_group_get_auto_reload_in_isr(timer_group_t group_num, timer_idx_t timer_num);

/** @brief Take timer spinlock to enter critical protect
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_spinlock_take(timer_group_t group_num);

/** @brief Give timer spinlock to exit critical protect
 *
 * @param group_num Timer group number, 0 for TIMERG0 or 1 for TIMERG1
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t timer_spinlock_give(timer_group_t group_num);

#ifdef __cplusplus
}
#endif
