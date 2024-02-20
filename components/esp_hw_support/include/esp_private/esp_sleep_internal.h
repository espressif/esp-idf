/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdbool.h>
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif


#if CONFIG_ESP_SLEEP_DEBUG
typedef struct {
    uint32_t lightsleep_cnt;
    uint64_t sleep_in_rtc_time_stamp;
    uint64_t sleep_out_rtc_time_stamp;
    uint32_t wakeup_triggers;
    uint32_t sleep_flags;
    esp_err_t sleep_request_result;
} esp_sleep_context_t;

/**
 * @brief Set the context pointer of last sleep request
 * @param sleep_ctx Structure where the context of the sleep information needs to be recorded in
 */
void esp_sleep_set_sleep_context(esp_sleep_context_t *sleep_ctx);
#endif

/**
 * @brief Enables the use of ADC and temperature sensor in monitor (ULP) mode
 *
 * @note  This state is kept in RTC memory and will keep its value after a deep sleep wakeup
 *
 */
void esp_sleep_enable_adc_tsens_monitor(bool enable);

#if !SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP
/**
 * @brief Isolate all digital IOs except those that are held during deep sleep
 *
 * Reduce digital IOs current leakage during deep sleep.
 */
void esp_sleep_isolate_digital_gpio(void);
#endif

#ifdef __cplusplus
}
#endif
