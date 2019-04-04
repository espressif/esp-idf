// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

/* Recommendation of using API RTC_WDT.
1) Setting and enabling rtc_wdt:
@code
    rtc_wdt_protect_off();
    rtc_wdt_disable();
    rtc_wdt_set_length_of_reset_signal(RTC_WDT_SYS_RESET_SIG, RTC_WDT_LENGTH_3_2us);
    rtc_wdt_set_stage(RTC_WDT_STAGE0, RTC_WDT_STAGE_ACTION_RESET_SYSTEM); //RTC_WDT_STAGE_ACTION_RESET_SYSTEM or RTC_WDT_STAGE_ACTION_RESET_RTC
    rtc_wdt_set_time(RTC_WDT_STAGE0, 7000);     // timeout rtd_wdt 7000ms.
    rtc_wdt_enable();
    rtc_wdt_protect_on();
 @endcode

* If you use this option RTC_WDT_STAGE_ACTION_RESET_SYSTEM then after reset you can see these messages.
They can help to understand where the CPUs were when the WDT was triggered.
    W (30) boot: PRO CPU has been reset by WDT.
	W (30) boot: WDT reset info: PRO CPU PC=0x400xxxxx
	... function where it happened

	W (31) boot: WDT reset info: APP CPU PC=0x400xxxxx
	... function where it happened

* If you use this option RTC_WDT_STAGE_ACTION_RESET_RTC then you will see message (rst:0x10 (RTCWDT_RTC_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)) 
without description where were CPUs when it happened.

2) Reset counter of rtc_wdt:
@code
    rtc_wdt_feed();
@endcode

3) Disable rtc_wdt:
@code
    rtc_wdt_disable();
@endcode
 */

#ifndef _SOC_RTC_WDT_H
#define _SOC_RTC_WDT_H

#include <stdint.h>
#include <stdbool.h>
#include "soc/rtc_cntl_reg.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

/// List of stage of rtc watchdog. WDT has 4 stage.
typedef enum {
    RTC_WDT_STAGE0 = 0,     /*!< Stage 0 */
    RTC_WDT_STAGE1 = 1,     /*!< Stage 1 */
    RTC_WDT_STAGE2 = 2,     /*!< Stage 2 */
    RTC_WDT_STAGE3 = 3      /*!< Stage 3 */
} rtc_wdt_stage_t;

/// List of action. When the time of stage expires this action will be triggered.
typedef enum {
    RTC_WDT_STAGE_ACTION_OFF            = RTC_WDT_STG_SEL_OFF,          /*!< Disabled. This stage will have no effects on the system. */
    RTC_WDT_STAGE_ACTION_INTERRUPT      = RTC_WDT_STG_SEL_INT,          /*!< Trigger an interrupt. When the stage expires an interrupt is triggered. */
    RTC_WDT_STAGE_ACTION_RESET_CPU      = RTC_WDT_STG_SEL_RESET_CPU,    /*!< Reset a CPU core. */
    RTC_WDT_STAGE_ACTION_RESET_SYSTEM   = RTC_WDT_STG_SEL_RESET_SYSTEM, /*!< Reset the main system includes the CPU and all peripherals. The RTC is an exception to this, and it will not be reset. */
    RTC_WDT_STAGE_ACTION_RESET_RTC      = RTC_WDT_STG_SEL_RESET_RTC     /*!< Reset the main system and the RTC. */
} rtc_wdt_stage_action_t;

/// Type of reset signal
typedef enum {
    RTC_WDT_SYS_RESET_SIG = 0,     /*!< System reset signal length selection */
    RTC_WDT_CPU_RESET_SIG = 1      /*!< CPU reset signal length selection */
} rtc_wdt_reset_sig_t;

/// Length of reset signal
typedef enum {
    RTC_WDT_LENGTH_100ns = 0,     /*!< 100 ns */
    RTC_WDT_LENGTH_200ns = 1,     /*!< 200 ns */
    RTC_WDT_LENGTH_300ns = 2,     /*!< 300 ns */
    RTC_WDT_LENGTH_400ns = 3,     /*!< 400 ns */
    RTC_WDT_LENGTH_500ns = 4,     /*!< 500 ns */
    RTC_WDT_LENGTH_800ns = 5,     /*!< 800 ns */
    RTC_WDT_LENGTH_1_6us = 6,     /*!< 1.6 us */
    RTC_WDT_LENGTH_3_2us = 7      /*!< 3.2 us */
} rtc_wdt_length_sig_t;

/**
 * @brief Get status of protect of rtc_wdt.
 *
 * @return
 *         - True if the protect of RTC_WDT is set
 */
bool rtc_wdt_get_protect_status();

/**
 * @brief Set protect of rtc_wdt.
 */
void rtc_wdt_protect_on();

/**
 * @brief Reset protect of rtc_wdt.
 */
void rtc_wdt_protect_off();

/**
 * @brief Enable rtc_wdt.
 */
void rtc_wdt_enable();

/**
 * @brief Enable the flash boot protection procedure for WDT.
 *
 * Do not recommend to use it in the app.
 * This function was added to be compatibility with the old bootloaders.
 * This mode is disabled in bootloader or using rtc_wdt_disable() function.
 */
void rtc_wdt_flashboot_mode_enable();

/**
 * @brief Disable rtc_wdt.
 */
void rtc_wdt_disable();

/**
 * @brief Reset counter rtc_wdt.
 *
 * It returns to stage 0 and its expiry counter restarts from 0.
 */
void rtc_wdt_feed();

/**
 * @brief Set time for required stage.
 *
 * @param[in] stage Stage of rtc_wdt.
 * @param[in] timeout_ms Timeout for this stage.
 *
 * @return
 *         - ESP_OK In case of success
 *         - ESP_ERR_INVALID_ARG If stage has invalid value
 */
esp_err_t rtc_wdt_set_time(rtc_wdt_stage_t stage, unsigned int timeout_ms);

/**
 * @brief Get the timeout set for the required stage.
 *
 * @param[in]  stage Stage of rtc_wdt.
 * @param[out] timeout_ms Timeout set for this stage. (not elapsed time).
 *
 * @return
 *         - ESP_OK In case of success
 *         - ESP_ERR_INVALID_ARG If stage has invalid value
 */
esp_err_t rtc_wdt_get_timeout(rtc_wdt_stage_t stage, unsigned int* timeout_ms);

/**
 * @brief Set an action for required stage.
 *
 * @param[in] stage Stage of rtc_wdt.
 * @param[in] stage_sel Action for this stage. When the time of stage expires this action will be triggered.
 *
 * @return
 *         - ESP_OK In case of success
 *         - ESP_ERR_INVALID_ARG If stage or stage_sel have invalid value
 */
esp_err_t rtc_wdt_set_stage(rtc_wdt_stage_t stage, rtc_wdt_stage_action_t stage_sel);

/**
 * @brief Set a length of reset signal.
 *
 * @param[in] reset_src Type of reset signal.
 * @param[in] reset_signal_length A length of reset signal.
 *
 * @return
 *         - ESP_OK In case of success
 *         - ESP_ERR_INVALID_ARG If reset_src  or reset_signal_length have invalid value
 */
esp_err_t rtc_wdt_set_length_of_reset_signal(rtc_wdt_reset_sig_t reset_src, rtc_wdt_length_sig_t reset_signal_length);

/**
 * @brief Return true if rtc_wdt is enabled.
 *
 * @return
 *         - True rtc_wdt is enabled
 */
bool rtc_wdt_is_on();

#ifdef __cplusplus
}
#endif

#endif // _SOC_RTC_WDT_H
