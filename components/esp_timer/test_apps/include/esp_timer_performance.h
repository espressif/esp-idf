/* SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#if __has_include("esp_timer_performance_esp32c2.h")
#include "esp_timer_performance_esp32c2.h"
#elif __has_include("esp_timer_performance_esp32c5.h")
#include "esp_timer_performance_esp32c5.h"
#elif __has_include("esp_timer_performance_esp32c6.h")
#include "esp_timer_performance_esp32c6.h"
#elif __has_include("esp_timer_performance_esp32c61.h")
#include "esp_timer_performance_esp32c61.h"
#elif __has_include("esp_timer_performance_esp32h2.h")
#include "esp_timer_performance_esp32h2.h"
#elif __has_include("esp_timer_performance_esp32h21.h")
#include "esp_timer_performance_esp32h21.h"
#endif

#ifndef IDF_PERFORMANCE_MAX_ESP_TIMER_GET_TIME_PER_CALL
#define IDF_PERFORMANCE_MAX_ESP_TIMER_GET_TIME_PER_CALL                         1000
#endif
