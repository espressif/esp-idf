
/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "esp_private/periph_ctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_PERIPH_CLK_CTRL_SHARED
#define HP_UART_SRC_CLK_ATOMIC()       PERIPH_RCC_ATOMIC()
#else
#define HP_UART_SRC_CLK_ATOMIC()
#endif

#if SOC_RCC_IS_INDEPENDENT
#define HP_UART_BUS_CLK_ATOMIC()
#else
#define HP_UART_BUS_CLK_ATOMIC()       PERIPH_RCC_ATOMIC()
#endif

#if (SOC_UART_LP_NUM >= 1)
#define LP_UART_SRC_CLK_ATOMIC()    PERIPH_RCC_ATOMIC()
#define LP_UART_BUS_CLK_ATOMIC()    PERIPH_RCC_ATOMIC()
#endif

#ifdef __cplusplus
}
#endif
