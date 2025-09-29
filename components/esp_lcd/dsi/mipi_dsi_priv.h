/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "sdkconfig.h"
#if CONFIG_LCD_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for gptimer driver
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#endif
#include "soc/mipi_dsi_periph.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_clk_tree_common.h"
#include "esp_pm.h"
#include "hal/mipi_dsi_hal.h"
#include "hal/mipi_dsi_ll.h"

#if CONFIG_LCD_DSI_OBJ_FORCE_INTERNAL
#define DSI_MEM_ALLOC_CAPS      (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define DSI_MEM_ALLOC_CAPS      MALLOC_CAP_DEFAULT
#endif

#define DPI_PANEL_MAX_FB_NUM             3 // maximum number of frame buffers that can be maintained by the driver
#define DPI_PANEL_MIN_DMA_NODES_PER_LINK 1 // NOTE: we assume 1 DMA link item can carry the WHOLE image

///!< Logging settings
#define TAG "lcd.dsi"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esp_lcd_dsi_bus_t {
    int bus_id;
    mipi_dsi_hal_context_t hal;
#if CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif
} esp_lcd_dsi_bus_t;

#ifdef __cplusplus
}
#endif
