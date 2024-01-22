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

#define DSI_MEM_ALLOC_CAPS MALLOC_CAP_DEFAULT

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esp_lcd_dsi_bus_t {
    int bus_id;
    mipi_dsi_hal_context_t hal;
} esp_lcd_dsi_bus_t;

#ifdef __cplusplus
}
#endif
