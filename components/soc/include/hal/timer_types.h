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


/**
 * @brief Select a hardware timer from timer groups
 */
typedef enum {
    TIMER_0 = 0, /*!<Select timer0 of GROUPx*/
    TIMER_1 = 1, /*!<Select timer1 of GROUPx*/
    TIMER_MAX,
} timer_idx_t;

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
    TIMER_INTR_T0 = BIT(0), /*!< interrupt of timer 0 */
    TIMER_INTR_T1 = BIT(1), /*!< interrupt of timer 1 */
    TIMER_INTR_WDT = BIT(2), /*!< interrupt of watchdog */
} timer_intr_t;
FLAG_ATTR(timer_intr_t)

/**
 * @brief Behavior of the watchdog if a stage times out.
 */
//this is compatible with the value of esp32.
typedef enum {
    TIMER_WDT_OFF = 0,          ///< The stage is turned off
    TIMER_WDT_INT = 1,          ///< The stage will trigger an interrupt
    TIMER_WDT_RESET_CPU = 2,    ///< The stage will reset the CPU
    TIMER_WDT_RESET_SYSTEM = 3, ///< The stage will reset the whole system
} timer_wdt_behavior_t;

#ifdef __cplusplus
}
#endif
