/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum periph_retention_module {
    SLEEP_RETENTION_MODULE_MIN          = 0,
    SLEEP_RETENTION_MODULE_NULL         = SLEEP_RETENTION_MODULE_MIN, /* This module is for all peripherals that can't survive from PD_TOP to call init only. Shouldn't have any dependency. */
    /* clock module, which includes system and modem */
    SLEEP_RETENTION_MODULE_CLOCK_SYSTEM = 1,
    /* digital peripheral module, which includes Interrupt Matrix, HP_SYSTEM,
     * TEE, APM, UART, IOMUX, SPIMEM, SysTimer, etc.. */
    SLEEP_RETENTION_MODULE_SYS_PERIPH   = 2,
    /* Timer Group by target*/
    SLEEP_RETENTION_MODULE_TG0_WDT      = 3,
    SLEEP_RETENTION_MODULE_TG1_WDT      = 4,
    SLEEP_RETENTION_MODULE_TG0_TIMER    = 5,
    SLEEP_RETENTION_MODULE_TG1_TIMER    = 6,
    /* AHB_DMA by channel */
    SLEEP_RETENTION_MODULE_AHB_DMA_CH0  = 7,
    SLEEP_RETENTION_MODULE_AHB_DMA_CH1  = 8,
    SLEEP_RETENTION_MODULE_AHB_DMA_CH2  = 9,
    /* AXI_DMA by channel */
    SLEEP_RETENTION_MODULE_AXI_DMA_CH0  = 10,
    SLEEP_RETENTION_MODULE_AXI_DMA_CH1  = 11,
    SLEEP_RETENTION_MODULE_AXI_DMA_CH2  = 12,
    /* MISC Peripherals */
    SLEEP_RETENTION_MODULE_UART0        = 13,
    SLEEP_RETENTION_MODULE_UART1        = 14,
    SLEEP_RETENTION_MODULE_UART2        = 15,
    SLEEP_RETENTION_MODULE_UART3        = 16,
    SLEEP_RETENTION_MODULE_UART4        = 17,
    SLEEP_RETENTION_MODULE_RMT0         = 18,
    SLEEP_RETENTION_MODULE_I2S0         = 19,
    SLEEP_RETENTION_MODULE_I2S1         = 20,
    SLEEP_RETENTION_MODULE_I2S2         = 21,
    SLEEP_RETENTION_MODULE_I2C0         = 22,
    SLEEP_RETENTION_MODULE_I2C1         = 23,
    SLEEP_RETENTION_MODULE_ETM0         = 24,
    SLEEP_RETENTION_MODULE_TWAI0        = 25,
    SLEEP_RETENTION_MODULE_TWAI1        = 26,
    SLEEP_RETENTION_MODULE_TWAI2        = 27,
    SLEEP_RETENTION_MODULE_PARLIO0      = 28,
    SLEEP_RETENTION_MODULE_GPSPI2       = 29,
    SLEEP_RETENTION_MODULE_GPSPI3       = 30,
    SLEEP_RETENTION_MODULE_LEDC         = 31,
    SLEEP_RETENTION_MODULE_MCPWM0       = 32,
    SLEEP_RETENTION_MODULE_MCPWM1       = 33,
    SLEEP_RETENTION_MODULE_PCNT0        = 34,

    SLEEP_RETENTION_MODULE_MAX          = SOC_PM_RETENTION_MODULE_NUM - 1
} periph_retention_module_t;

#define is_top_domain_module(m)                           \
    ( ((m) == SLEEP_RETENTION_MODULE_NULL)         ? true \
    : ((m) == SLEEP_RETENTION_MODULE_CLOCK_SYSTEM) ? true \
    : ((m) == SLEEP_RETENTION_MODULE_SYS_PERIPH)   ? true \
    : ((m) == SLEEP_RETENTION_MODULE_TG0_WDT)      ? true \
    : ((m) == SLEEP_RETENTION_MODULE_TG1_WDT)      ? true \
    : ((m) == SLEEP_RETENTION_MODULE_TG0_TIMER)    ? true \
    : ((m) == SLEEP_RETENTION_MODULE_TG1_TIMER)    ? true \
    : ((m) == SLEEP_RETENTION_MODULE_AHB_DMA_CH0)  ? true \
    : ((m) == SLEEP_RETENTION_MODULE_AHB_DMA_CH1)  ? true \
    : ((m) == SLEEP_RETENTION_MODULE_AHB_DMA_CH2)  ? true \
    : ((m) == SLEEP_RETENTION_MODULE_AXI_DMA_CH0)  ? true \
    : ((m) == SLEEP_RETENTION_MODULE_AXI_DMA_CH1)  ? true \
    : ((m) == SLEEP_RETENTION_MODULE_AXI_DMA_CH2)  ? true \
    : ((m) == SLEEP_RETENTION_MODULE_UART0)        ? true \
    : ((m) == SLEEP_RETENTION_MODULE_UART1)        ? true \
    : ((m) == SLEEP_RETENTION_MODULE_UART2)        ? true \
    : ((m) == SLEEP_RETENTION_MODULE_UART3)        ? true \
    : ((m) == SLEEP_RETENTION_MODULE_UART4)        ? true \
    : ((m) == SLEEP_RETENTION_MODULE_RMT0)         ? true \
    : ((m) == SLEEP_RETENTION_MODULE_I2S0)         ? true \
    : ((m) == SLEEP_RETENTION_MODULE_I2S1)         ? true \
    : ((m) == SLEEP_RETENTION_MODULE_I2S2)         ? true \
    : ((m) == SLEEP_RETENTION_MODULE_I2C0)         ? true \
    : ((m) == SLEEP_RETENTION_MODULE_I2C1)         ? true \
    : ((m) == SLEEP_RETENTION_MODULE_ETM0)         ? true \
    : ((m) == SLEEP_RETENTION_MODULE_TWAI0)        ? true \
    : ((m) == SLEEP_RETENTION_MODULE_TWAI1)        ? true \
    : ((m) == SLEEP_RETENTION_MODULE_TWAI2)        ? true \
    : ((m) == SLEEP_RETENTION_MODULE_PARLIO0)      ? true \
    : ((m) == SLEEP_RETENTION_MODULE_GPSPI2)       ? true \
    : ((m) == SLEEP_RETENTION_MODULE_GPSPI3)       ? true \
    : ((m) == SLEEP_RETENTION_MODULE_LEDC)         ? true \
    : ((m) == SLEEP_RETENTION_MODULE_MCPWM0)       ? true \
    : ((m) == SLEEP_RETENTION_MODULE_MCPWM1)       ? true \
    : ((m) == SLEEP_RETENTION_MODULE_PCNT0)        ? true \
    : false)

#ifdef __cplusplus
}
#endif
