/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/lock.h>
#include <stdatomic.h>
#include "sdkconfig.h"
#if CONFIG_TWAI_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for rmt driver
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/idf_additions.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_err.h"
#include "soc/soc_caps.h"
#include "soc/twai_periph.h"
#include "hal/twai_hal.h"
#include "esp_intr_alloc.h"
#include "esp_heap_caps.h"
#include "esp_clk_tree.h"
#include "esp_memory_utils.h"
#include "esp_pm.h"
#include "esp_attr.h"
#include "esp_private/esp_gpio_reserve.h"
#include "esp_private/gpio.h"
#include "esp_private/sleep_retention.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_clk_tree_common.h"

#ifdef __cplusplus
extern "C" {
#endif

///!< Logging settings
#define TAG "esp_twai"

#ifdef CONFIG_TWAI_OBJ_CACHE_SAFE
#define TWAI_MALLOC_CAPS    (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define TWAI_MALLOC_CAPS    MALLOC_CAP_DEFAULT
#endif  //CONFIG_TWAI_OBJ_CACHE_SAFE

#if CONFIG_TWAI_ISR_CACHE_SAFE
#define TWAI_INTR_ALLOC_FLAGS    (ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_INTRDISABLED)
#else
#define TWAI_INTR_ALLOC_FLAGS    ESP_INTR_FLAG_INTRDISABLED
#endif

#ifdef __cplusplus
}
#endif
