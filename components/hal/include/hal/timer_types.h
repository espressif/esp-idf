// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <esp_bit_defs.h>
#include "esp_attr.h"
#include "soc/soc_caps.h"

/**
 * @brief Selects a Timer-Group out of 2 available groups
 */
typedef enum {
    TIMER_GROUP_0 = 0, /*!<Hw timer group 0*/
#if SOC_TIMER_GROUPS > 1
    TIMER_GROUP_1 = 1, /*!<Hw timer group 1*/
#endif
    TIMER_GROUP_MAX,
} timer_group_t;

/**
 * @brief Select a hardware timer from timer groups
 */
typedef enum {
    TIMER_0 = 0, /*!<Select timer0 of GROUPx*/
#if SOC_TIMER_GROUP_TIMERS_PER_GROUP > 1
    TIMER_1 = 1, /*!<Select timer1 of GROUPx*/
#endif
    TIMER_MAX,
} timer_idx_t;

/**
 * @brief Decides the direction of counter
 */
typedef enum {
    TIMER_COUNT_DOWN = 0, /*!< Descending Count from cnt.high|cnt.low*/
    TIMER_COUNT_UP = 1,   /*!< Ascending Count from Zero*/
    TIMER_COUNT_MAX
} timer_count_dir_t;

/**
 * @brief Decides whether timer is on or paused
 */
typedef enum {
    TIMER_PAUSE = 0, /*!<Pause timer counter*/
    TIMER_START = 1, /*!<Start timer counter*/
} timer_start_t;

/**
 * @brief Interrupt types of the timer.
 */
//this is compatible with the value of esp32.
typedef enum {
    TIMER_INTR_T0 = BIT(0),  /*!< interrupt of timer 0 */
#if SOC_TIMER_GROUP_TIMERS_PER_GROUP > 1
    TIMER_INTR_T1 = BIT(1),  /*!< interrupt of timer 1 */
    TIMER_INTR_WDT = BIT(2), /*!< interrupt of watchdog */
#else
    TIMER_INTR_WDT = BIT(1), /*!< interrupt of watchdog */
#endif
    TIMER_INTR_NONE = 0
} timer_intr_t;
FLAG_ATTR(timer_intr_t)

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

#if SOC_TIMER_GROUP_SUPPORT_XTAL
/**
 * @brief Select timer source clock.
 */
typedef enum {
    TIMER_SRC_CLK_APB = 0,  /*!< Select APB as the source clock*/
    TIMER_SRC_CLK_XTAL = 1, /*!< Select XTAL as the source clock*/
} timer_src_clk_t;
#endif

/**
 * @brief Data structure with timer's configuration settings
 */
typedef struct {
    timer_alarm_t alarm_en;      /*!< Timer alarm enable */
    timer_start_t counter_en;    /*!< Counter enable */
    timer_intr_mode_t intr_type; /*!< Interrupt mode */
    timer_count_dir_t counter_dir; /*!< Counter direction  */
    timer_autoreload_t auto_reload;   /*!< Timer auto-reload */
    uint32_t divider;   /*!< Counter clock divider. The divider's range is from from 2 to 65536. */
#if SOC_TIMER_GROUP_SUPPORT_XTAL
    timer_src_clk_t clk_src;  /*!< Use XTAL as source clock. */
#endif
} timer_config_t;

#ifdef __cplusplus
}
#endif
