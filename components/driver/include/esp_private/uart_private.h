
/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "esp_private/periph_ctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (SOC_UART_LP_NUM >= 1)
#define LP_UART_CLK_ATOMIC()    PERIPH_RCC_ATOMIC()
#endif

#if CONFIG_IDF_TARGET_ESP32P4
#define UART_SCLK_ATOMIC()      PERIPH_RCC_ATOMIC()
#else
#define UART_SCLK_ATOMIC()
#endif

#if SOC_RCC_IS_INDEPENDENT
#define UART_CLK_ATOMIC()
#else
#define UART_CLK_ATOMIC()       PERIPH_RCC_ATOMIC()
#endif

#ifdef __cplusplus
}
#endif
