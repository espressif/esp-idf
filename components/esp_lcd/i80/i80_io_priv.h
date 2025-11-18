/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/queue.h>
#include "sdkconfig.h"
#if CONFIG_LCD_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for gptimer driver
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#endif
#include "soc/soc_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "esp_intr_alloc.h"
#include "esp_clk_tree.h"
#include "esp_pm.h"
#include "esp_cache.h"
#include "esp_memory_utils.h"
#include "esp_lcd_panel_io_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_common.h"
#include "driver/gpio.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_clk_tree_common.h"
#include "esp_private/gdma_link.h"
#include "esp_private/esp_dma_utils.h"
#include "esp_private/gpio.h"
#include "hal/lcd_periph.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_sig_map.h"

///!< Logging settings
#define TAG "lcd.i80"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
