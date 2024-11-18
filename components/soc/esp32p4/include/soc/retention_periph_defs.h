/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_bit_defs.h"

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

    SLEEP_RETENTION_MODULE_MAX          = 31
} periph_retention_module_t;

typedef enum periph_retention_module_bitmap {
    SLEEP_RETENTION_MODULE_BM_NULL = BIT(SLEEP_RETENTION_MODULE_NULL),

    /* clock module, which includes system and modem */
    SLEEP_RETENTION_MODULE_BM_CLOCK_SYSTEM = BIT(SLEEP_RETENTION_MODULE_CLOCK_SYSTEM),
    /* digital peripheral module, which includes Interrupt Matrix, HP_SYSTEM,
     * TEE, APM, UART, Timer Group, IOMUX, SPIMEM, SysTimer, etc.. */
    SLEEP_RETENTION_MODULE_BM_SYS_PERIPH   = BIT(SLEEP_RETENTION_MODULE_SYS_PERIPH),
    /* Timer Group by target*/
    SLEEP_RETENTION_MODULE_BM_TG0_WDT      = BIT(SLEEP_RETENTION_MODULE_TG0_WDT),
    SLEEP_RETENTION_MODULE_BM_TG1_WDT      = BIT(SLEEP_RETENTION_MODULE_TG1_WDT),
    SLEEP_RETENTION_MODULE_BM_TG0_TIMER    = BIT(SLEEP_RETENTION_MODULE_TG0_TIMER),
    SLEEP_RETENTION_MODULE_BM_TG1_TIMER    = BIT(SLEEP_RETENTION_MODULE_TG1_TIMER),
    /* AHB_DMA by channel */
    SLEEP_RETENTION_MODULE_BM_AHB_DMA_CH0  = BIT(SLEEP_RETENTION_MODULE_AHB_DMA_CH0),
    SLEEP_RETENTION_MODULE_BM_AHB_DMA_CH1  = BIT(SLEEP_RETENTION_MODULE_AHB_DMA_CH1),
    SLEEP_RETENTION_MODULE_BM_AHB_DMA_CH2  = BIT(SLEEP_RETENTION_MODULE_AHB_DMA_CH2),
    /* AXI_DMA by channel */
    SLEEP_RETENTION_MODULE_BM_AXI_DMA_CH0  = BIT(SLEEP_RETENTION_MODULE_AXI_DMA_CH0),
    SLEEP_RETENTION_MODULE_BM_AXI_DMA_CH1  = BIT(SLEEP_RETENTION_MODULE_AXI_DMA_CH1),
    SLEEP_RETENTION_MODULE_BM_AXI_DMA_CH2  = BIT(SLEEP_RETENTION_MODULE_AXI_DMA_CH2),
    /* MISC Peripherals */
    SLEEP_RETENTION_MODULE_BM_UART0        = BIT(SLEEP_RETENTION_MODULE_UART0),
    SLEEP_RETENTION_MODULE_BM_UART1        = BIT(SLEEP_RETENTION_MODULE_UART1),
    SLEEP_RETENTION_MODULE_BM_UART2        = BIT(SLEEP_RETENTION_MODULE_UART2),
    SLEEP_RETENTION_MODULE_BM_UART3        = BIT(SLEEP_RETENTION_MODULE_UART3),
    SLEEP_RETENTION_MODULE_BM_UART4        = BIT(SLEEP_RETENTION_MODULE_UART4),
    SLEEP_RETENTION_MODULE_BM_RMT0         = BIT(SLEEP_RETENTION_MODULE_RMT0),
    SLEEP_RETENTION_MODULE_BM_I2S0         = BIT(SLEEP_RETENTION_MODULE_I2S0),
    SLEEP_RETENTION_MODULE_BM_I2S1         = BIT(SLEEP_RETENTION_MODULE_I2S1),
    SLEEP_RETENTION_MODULE_BM_I2S2         = BIT(SLEEP_RETENTION_MODULE_I2S2),
    SLEEP_RETENTION_MODULE_BM_ETM0         = BIT(SLEEP_RETENTION_MODULE_ETM0),
    SLEEP_RETENTION_MODULE_BM_I2C0         = BIT(SLEEP_RETENTION_MODULE_I2C0),
    SLEEP_RETENTION_MODULE_BM_I2C1         = BIT(SLEEP_RETENTION_MODULE_I2C1),
    SLEEP_RETENTION_MODULE_BM_TWAI0        = BIT(SLEEP_RETENTION_MODULE_TWAI0),
    SLEEP_RETENTION_MODULE_BM_TWAI1        = BIT(SLEEP_RETENTION_MODULE_TWAI1),
    SLEEP_RETENTION_MODULE_BM_TWAI2        = BIT(SLEEP_RETENTION_MODULE_TWAI2),
    SLEEP_RETENTION_MODULE_BM_PARLIO0      = BIT(SLEEP_RETENTION_MODULE_PARLIO0),
    SLEEP_RETENTION_MODULE_BM_GPSPI2       = BIT(SLEEP_RETENTION_MODULE_GPSPI2),
    SLEEP_RETENTION_MODULE_BM_GPSPI3       = BIT(SLEEP_RETENTION_MODULE_GPSPI3),
    SLEEP_RETENTION_MODULE_BM_LEDC         = BIT(SLEEP_RETENTION_MODULE_LEDC),

    SLEEP_RETENTION_MODULE_BM_ALL = (uint32_t)-1
} periph_retention_module_bitmap_t;

#define TOP_DOMAIN_PERIPHERALS_BM ( SLEEP_RETENTION_MODULE_BM_SYS_PERIPH \
                                  | SLEEP_RETENTION_MODULE_BM_TG0_WDT    \
                                  | SLEEP_RETENTION_MODULE_BM_TG1_WDT    \
                                  | SLEEP_RETENTION_MODULE_BM_TG0_TIMER  \
                                  | SLEEP_RETENTION_MODULE_BM_TG1_TIMER  \
                                  | SLEEP_RETENTION_MODULE_BM_AHB_DMA_CH0 \
                                  | SLEEP_RETENTION_MODULE_BM_AHB_DMA_CH1 \
                                  | SLEEP_RETENTION_MODULE_BM_AHB_DMA_CH2 \
                                  | SLEEP_RETENTION_MODULE_BM_AXI_DMA_CH0 \
                                  | SLEEP_RETENTION_MODULE_BM_AXI_DMA_CH1 \
                                  | SLEEP_RETENTION_MODULE_BM_AXI_DMA_CH2 \
                                  | SLEEP_RETENTION_MODULE_BM_UART0      \
                                  | SLEEP_RETENTION_MODULE_BM_UART1      \
                                  | SLEEP_RETENTION_MODULE_BM_UART2      \
                                  | SLEEP_RETENTION_MODULE_BM_UART3      \
                                  | SLEEP_RETENTION_MODULE_BM_UART4      \
                                  | SLEEP_RETENTION_MODULE_BM_RMT0       \
                                  | SLEEP_RETENTION_MODULE_BM_I2S0       \
                                  | SLEEP_RETENTION_MODULE_BM_I2S1       \
                                  | SLEEP_RETENTION_MODULE_BM_I2S2       \
                                  | SLEEP_RETENTION_MODULE_BM_ETM0       \
                                  | SLEEP_RETENTION_MODULE_BM_I2C0       \
                                  | SLEEP_RETENTION_MODULE_BM_I2C1       \
                                  | SLEEP_RETENTION_MODULE_BM_TWAI0      \
                                  | SLEEP_RETENTION_MODULE_BM_TWAI1      \
                                  | SLEEP_RETENTION_MODULE_BM_TWAI2      \
                                  | SLEEP_RETENTION_MODULE_BM_PARLIO0    \
                                  | SLEEP_RETENTION_MODULE_BM_GPSPI2     \
                                  | SLEEP_RETENTION_MODULE_BM_GPSPI3     \
                                  | SLEEP_RETENTION_MODULE_BM_LEDC       \
                                  | SLEEP_RETENTION_MODULE_BM_NULL       \
                                  )

#ifdef __cplusplus
}
#endif
