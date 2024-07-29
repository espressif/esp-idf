/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "hal/mipi_dsi_hal.h"
#include "hal/mipi_dsi_ll.h"
#include "esp_heap_caps.h"
#include "esp_private/periph_ctrl.h"
#include "esp_pm.h"

#if SOC_PERIPH_CLK_CTRL_SHARED
#define DSI_CLOCK_SRC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define DSI_CLOCK_SRC_ATOMIC()
#endif

#if !SOC_RCC_IS_INDEPENDENT
#define DSI_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define DSI_RCC_ATOMIC()
#endif

#if CONFIG_LCD_DSI_ISR_IRAM_SAFE
#define DSI_MEM_ALLOC_CAPS      (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define DSI_MEM_ALLOC_CAPS      MALLOC_CAP_DEFAULT
#endif

#define DPI_PANEL_MAX_FB_NUM             3 // maximum number of frame buffers that can be maintained by the driver
#define DPI_PANEL_MIN_DMA_NODES_PER_LINK 1 // NOTE: we assume 1 DMA link item can carry the WHOLE image

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esp_lcd_dsi_bus_t {
    int bus_id;
    mipi_dsi_hal_context_t hal;
    esp_pm_lock_handle_t pm_lock;
} esp_lcd_dsi_bus_t;

#ifdef __cplusplus
}
#endif
