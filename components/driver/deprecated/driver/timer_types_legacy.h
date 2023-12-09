/*
 * SPDX-FileCopyrightText: 2010-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
#include "hal/timer_types.h"
#include "esp_intr_alloc.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Timer-Group ID
 */
typedef enum {
    TIMER_GROUP_0 = 0, /*!< Hw timer group 0 */
#if SOC_TIMER_GROUPS > 1
    TIMER_GROUP_1 = 1, /*!< Hw timer group 1 */
#endif
    TIMER_GROUP_MAX    /*!< Maximum number of Hw timer groups */
} timer_group_t;

/**
 * @brief Timer ID
 */
typedef enum {
    TIMER_0 = 0, /*!< Select timer0 of GROUPx*/
#if SOC_TIMER_GROUP_TIMERS_PER_GROUP > 1
    TIMER_1 = 1, /*!< Select timer1 of GROUPx*/
#endif
    TIMER_MAX,
} timer_idx_t;

/**
 * @brief Interrupt types of the timer.
 */
typedef enum {
    TIMER_INTR_T0 = 1 << 0,  /*!< interrupt of timer 0 */
#if SOC_TIMER_GROUP_TIMERS_PER_GROUP > 1
    TIMER_INTR_T1 = 1 << 1,  /*!< interrupt of timer 1 */
    TIMER_INTR_WDT = 1 << 2, /*!< interrupt of watchdog */
#else
    TIMER_INTR_WDT = 1 << 1, /*!< interrupt of watchdog */
#endif
    TIMER_INTR_NONE = 0
} timer_intr_t;

/**
 * @brief Timer count direction
 */
typedef enum {
    TIMER_COUNT_DOWN = GPTIMER_COUNT_DOWN, /*!< Descending Count from cnt.high|cnt.low*/
    TIMER_COUNT_UP = GPTIMER_COUNT_UP,     /*!< Ascending Count from Zero*/
    TIMER_COUNT_MAX                        /*!< Maximum number of timer count directions */
} timer_count_dir_t;

/**
 * @brief Timer start/stop command
 */
typedef enum {
    TIMER_PAUSE, /*!< Pause timer counter*/
    TIMER_START, /*!< Start timer counter*/
} timer_start_t;

/**
 * @brief Timer alarm command
 */
typedef enum {
    TIMER_ALARM_DIS = 0, /*!< Disable timer alarm*/
    TIMER_ALARM_EN = 1,  /*!< Enable timer alarm*/
    TIMER_ALARM_MAX
} timer_alarm_t;

/**
 * @brief Timer interrupt type
 */
typedef enum {
    TIMER_INTR_LEVEL = 0, /*!< Interrupt mode: level mode*/
    TIMER_INTR_MAX
} timer_intr_mode_t;

/**
 * @brief Timer autoreload command
 */
typedef enum {
    TIMER_AUTORELOAD_DIS = 0, /*!< Disable auto-reload: hardware will not load counter value after an alarm event*/
    TIMER_AUTORELOAD_EN = 1,  /*!< Enable auto-reload: hardware will load counter value after an alarm event*/
    TIMER_AUTORELOAD_MAX,
} timer_autoreload_t;

#if SOC_GPTIMER_SUPPORTED
/**
 * @brief Timer group clock source
 */
typedef soc_periph_tg_clk_src_legacy_t timer_src_clk_t;
#else
/**
 *  @brief Default type
 */
typedef int                            timer_src_clk_t;
#endif

/**
 * @brief Interrupt handler callback function
 *
 * @return
 *     - True Do task yield at the end of ISR
 *     - False Not do task yield at the end of ISR
 *
 * @note If you called FreeRTOS functions in callback, you need to return true or false based on
 *       the retrun value of argument `pxHigherPriorityTaskWoken`.
 *       For example, `xQueueSendFromISR` is called in callback, if the return value `pxHigherPriorityTaskWoken`
 *       of any FreeRTOS calls is pdTRUE, return true; otherwise return false.
 */
typedef bool (*timer_isr_t)(void *);

/**
 * @brief Interrupt handle, used in order to free the isr after use.
 */
typedef intr_handle_t timer_isr_handle_t;

/**
 * @brief Timer configurations
 */
typedef struct {
    timer_alarm_t alarm_en;         /*!< Timer alarm enable */
    timer_start_t counter_en;       /*!< Counter enable */
    timer_intr_mode_t intr_type;    /*!< Interrupt mode */
    timer_count_dir_t counter_dir;  /*!< Counter direction  */
    timer_autoreload_t auto_reload; /*!< Timer auto-reload */
    timer_src_clk_t clk_src;        /*!< Selects source clock. */
    uint32_t divider;               /*!< Counter clock divider */
} timer_config_t;

#ifdef __cplusplus
}
#endif
