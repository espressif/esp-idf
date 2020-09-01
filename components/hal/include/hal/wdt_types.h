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

typedef enum {
    WDT_RWDT = 0,   /*!< RTC Watchdog Timer (RWDT) */
    WDT_MWDT0,      /*!< Main System Watchdog Timer (MWDT) of Timer Group 0 */
    WDT_MWDT1,      /*!< Main System Watchdog Timer (MWDT) of Timer Group 1 */
} wdt_inst_t;

/**
 * @brief Stages of a Watchdog Timer. A WDT has 4 stages.
 */
typedef enum {
    WDT_STAGE0 = 0,     /*!< Stage 0 */
    WDT_STAGE1 = 1,     /*!< Stage 1 */
    WDT_STAGE2 = 2,     /*!< Stage 2 */
    WDT_STAGE3 = 3      /*!< Stage 3 */
} wdt_stage_t;

/**
 * @brief Behavior of the WDT stage if it times out
 *
 * @note These enum values should be compatible with the corresponding register
 *       field values.
 */
typedef enum {
    WDT_STAGE_ACTION_OFF = 0,           /*!< Disabled. This stage will have no effects on the system. */
    WDT_STAGE_ACTION_INT = 1,           /*!< Trigger an interrupt when the stage expires. */
    WDT_STAGE_ACTION_RESET_CPU = 2,     /*!< Reset a CPU core when the stage expires. */
    WDT_STAGE_ACTION_RESET_SYSTEM = 3,  /*!< Reset the main system when the stage expires. This includes the CPU and all peripherals. The RTC is an exception and will not be reset. */
    WDT_STAGE_ACTION_RESET_RTC = 4,     /*!< Reset the main system and the RTC when the stage expires. ONLY AVAILABLE FOR RWDT */
} wdt_stage_action_t;

/**
 * @brief Length of CPU or System Reset signals
 *
 * @note These enum values should be compatible with the corresponding register
 *       field values.
 */
typedef enum {
    WDT_RESET_SIG_LENGTH_100ns = 0,    /*!< 100 ns */
    WDT_RESET_SIG_LENGTH_200ns = 1,    /*!< 200 ns */
    WDT_RESET_SIG_LENGTH_300ns = 2,    /*!< 300 ns */
    WDT_RESET_SIG_LENGTH_400ns = 3,    /*!< 400 ns */
    WDT_RESET_SIG_LENGTH_500ns = 4,    /*!< 500 ns */
    WDT_RESET_SIG_LENGTH_800ns = 5,    /*!< 800 ns */
    WDT_RESET_SIG_LENGTH_1_6us = 6,    /*!< 1.6 us */
    WDT_RESET_SIG_LENGTH_3_2us = 7     /*!< 3.2 us */
} wdt_reset_sig_length_t;


#ifdef __cplusplus
}
#endif
