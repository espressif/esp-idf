/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_log.h"
#include "esp_sleep.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTC_STR(str) (__extension__({static const RTC_RODATA_ATTR char _fmt[] = (str); (const char *)&_fmt;}))
#define RTC_LOG_FORMAT(letter, format)  LOG_COLOR_ ## letter format LOG_RESET_COLOR "\n"

#define ESP_RTC_LOG( level, format, ... )  if (LOG_LOCAL_LEVEL >= level) { esp_rom_printf(RTC_STR(format), ##__VA_ARGS__); \
                                                                            esp_wake_stub_uart_tx_wait_idle(0); }

#define ESP_RTC_LOGE( format, ... )  ESP_RTC_LOG(ESP_LOG_ERROR, RTC_LOG_FORMAT(E, format), ##__VA_ARGS__)
#define ESP_RTC_LOGW( format, ... )  ESP_RTC_LOG(ESP_LOG_WARN, RTC_LOG_FORMAT(W, format), ##__VA_ARGS__)
#define ESP_RTC_LOGI( format, ... )  ESP_RTC_LOG(ESP_LOG_INFO, RTC_LOG_FORMAT(I, format), ##__VA_ARGS__)
#define ESP_RTC_LOGD( format, ... )  ESP_RTC_LOG(ESP_LOG_DEBUG, RTC_LOG_FORMAT(D, format), ##__VA_ARGS__)
#define ESP_RTC_LOGV( format, ... )  ESP_RTC_LOG(ESP_LOG_VERBOSE, RTC_LOG_FORMAT(V, format), ##__VA_ARGS__)

/**
 * @brief Enter deep-sleep mode from deep sleep wake stub code
 *
 * This should be called from the wake stub code.
 *
 * @param new_stub  new wake stub function will be set
 */
void esp_wake_stub_sleep(esp_deep_sleep_wake_stub_fn_t new_stub);

/**
 * @brief Wait while uart transmission is in progress
 *
 * This function is waiting while uart transmission is not completed,
 * and this function should be called from the wake stub code.
 *
 * @param uart_no  UART port to wait idle
 */
void esp_wake_stub_uart_tx_wait_idle(uint8_t uart_no);

/**
 * @brief Set wakeup time from deep sleep stub.
 *
 * This should be called from the wake stub code.
 *
 * @param time_in_us  wakeup time in us
 */
void esp_wake_stub_set_wakeup_time(uint64_t time_in_us);

/**
 * @brief Get wakeup cause from deep sleep stub.
 *
 * This should be called from the wake stub code.
 *
 * @return wakeup casue value
 */
uint32_t esp_wake_stub_get_wakeup_cause(void);

#ifdef __cplusplus
}
#endif
