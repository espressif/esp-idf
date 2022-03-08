/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __ULP_COMMON_H__
#define __ULP_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "esp_err.h"
#include "ulp_common_defs.h"

/**
 * @brief Set one of ULP wakeup period values
 *
 * ULP coprocessor starts running the program when the wakeup timer counts up
 * to a given value (called period). There are 5 period values which can be
 * programmed into SENS_ULP_CP_SLEEP_CYCx_REG registers, x = 0..4 for ESP32, and
 * one period value which can be programmed into RTC_CNTL_ULP_CP_TIMER_1_REG register for ESP32-S2/S3.
 * By default, for ESP32, wakeup timer will use the period set into SENS_ULP_CP_SLEEP_CYC0_REG,
 * i.e. period number 0. ULP program code can use SLEEP instruction to select
 * which of the SENS_ULP_CP_SLEEP_CYCx_REG should be used for subsequent wakeups.
 *
 * However, please note that SLEEP instruction issued (from ULP program) while the system
 * is in deep sleep mode does not have effect, and sleep cycle count 0 is used.
 *
 * For ESP32-S2/S3 the SLEEP instruction not exist. Instead a WAKE instruction will be used.
 *
 * @param period_index wakeup period setting number (0 - 4)
 * @param period_us wakeup period, us
 * @note  The ULP FSM requires two clock cycles to wakeup before being able to run the program.
 *        Then additional 16 cycles are reserved after wakeup waiting until the 8M clock is stable.
 *        The FSM also requires two more clock cycles to go to sleep after the program execution is halted.
 *        The minimum wakeup period that may be set up for the ULP
 *        is equal to the total number of cycles spent on the above internal tasks.
 *        For a default configuration of the ULP running at 150kHz it makes about 133us.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if period_index is out of range
 */
esp_err_t ulp_set_wakeup_period(size_t period_index, uint32_t period_us);

/**
 * @brief Stop the ULP timer
 *
 * @note This will stop the ULP from waking up if halted, but will not abort any program
 *       currently executing on the ULP.
 */
void ulp_timer_stop(void);

/**
 * @brief Resume the ULP timer
 *
 * @note This will resume an already configured timer, but does no other configuration
 *
 */
void ulp_timer_resume(void);

#ifdef __cplusplus
}
#endif

#endif // __ULP_COMMON_H__
