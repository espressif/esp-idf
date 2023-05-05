/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_system.h"

/**
 * @brief  Internal function to restart PRO and APP CPUs.
 *
 * @note This function should not be called from FreeRTOS applications.
 *       Use esp_restart instead.
 *
 * This is an internal function called by esp_restart. It is called directly
 * by the panic handler and brownout detector interrupt.
 */
void esp_restart_noos(void) __attribute__ ((noreturn));

/**
 * @brief Similar to esp_restart_noos, but resets all the digital peripherals.
 */
void esp_restart_noos_dig(void) __attribute__ ((noreturn));

/**
 * @brief  Internal function to set reset reason hint
 *
 * The hint is used do distinguish different reset reasons when software reset
 * is performed.
 *
 * The hint is stored in RTC store register, RTC_RESET_CAUSE_REG.
 *
 * @param hint  Desired esp_reset_reason_t value for the real reset reason
 */
void esp_reset_reason_set_hint(esp_reset_reason_t hint);

/**
 * @brief  Internal function to get the reset hint value
 * @return  - Reset hint value previously stored into RTC_RESET_CAUSE_REG using
 *          esp_reset_reason_set_hint function
 *          - ESP_RST_UNKNOWN if the value in RTC_RESET_CAUSE_REG is invalid
 */
esp_reset_reason_t esp_reset_reason_get_hint(void);

/**
 * @brief Get the time in microseconds since startup
 *
 * @returns time since g_startup_time; definition should be fixed by system time provider
 * no matter the underlying timer used.
 */
int64_t esp_system_get_time(void);

/**
 * @brief Get the resolution of the time returned by `esp_system_get_time`.
 *
 * @returns the resolution in nanoseconds
 */
uint32_t esp_system_get_time_resolution(void);

/**
 * @brief Before the system exit (e.g. panic, brownout, restart, etc.), this function is to be called to reset all necessary peripherals.
 */
void esp_system_reset_modules_on_exit(void);

#ifdef __cplusplus
}
#endif
