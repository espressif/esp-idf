/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

#include <stdint.h>
#include "soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum periph_retention_module {
    SLEEP_RETENTION_MODULE_MIN          = 0,
    SLEEP_RETENTION_MODULE_NULL         = 1, /* This module is for all peripherals that can't survive from PD_TOP to call init only. Shouldn't have any dependency. */
    /* clock module, which includes system and modem */
    SLEEP_RETENTION_MODULE_CLOCK_SYSTEM = 2,
    /* digital peripheral module, which includes Interrupt Matrix, HP_SYSTEM,
     * TEE, APM, UART, IOMUX, SPIMEM, SysTimer, etc.. */
    SLEEP_RETENTION_MODULE_SYS_PERIPH   = 3,
    /* Timer Group by target*/
    SLEEP_RETENTION_MODULE_TG0_WDT      = 4,
    SLEEP_RETENTION_MODULE_TG1_WDT      = 5,
    SLEEP_RETENTION_MODULE_TG0_TIMER0   = 6,
    SLEEP_RETENTION_MODULE_TG0_TIMER1   = 7,
    SLEEP_RETENTION_MODULE_TG1_TIMER0   = 8,
    SLEEP_RETENTION_MODULE_TG1_TIMER1   = 9,

    /* AHB_DMA by channel */
    SLEEP_RETENTION_MODULE_AHB_DMA_CH0  = 10,
    SLEEP_RETENTION_MODULE_AHB_DMA_CH1  = 11,
    SLEEP_RETENTION_MODULE_AHB_DMA_CH2  = 12,
    SLEEP_RETENTION_MODULE_AHB_DMA_CH3  = 13,
    SLEEP_RETENTION_MODULE_AHB_DMA_CH4  = 14,
    /* AXI_DMA by channel */
    SLEEP_RETENTION_MODULE_AXI_DMA_CH0  = 15,
    SLEEP_RETENTION_MODULE_AXI_DMA_CH1  = 16,
    SLEEP_RETENTION_MODULE_AXI_DMA_CH2  = 17,
    /* MISC Peripherals */
    SLEEP_RETENTION_MODULE_UART0        = 18,
    SLEEP_RETENTION_MODULE_UART1        = 19,
    SLEEP_RETENTION_MODULE_UART2        = 20,
    SLEEP_RETENTION_MODULE_UART3        = 21,
    SLEEP_RETENTION_MODULE_RMT0         = 22,
    SLEEP_RETENTION_MODULE_I2S0         = 23,
    SLEEP_RETENTION_MODULE_I2S1         = 24,
    SLEEP_RETENTION_MODULE_I2C0         = 25,
    SLEEP_RETENTION_MODULE_I2C1         = 26,
    SLEEP_RETENTION_MODULE_ETM0         = 27,
    SLEEP_RETENTION_MODULE_TWAI0        = 28,
    SLEEP_RETENTION_MODULE_TWAI1        = 29,
    SLEEP_RETENTION_MODULE_PARLIO0      = 30,
    SLEEP_RETENTION_MODULE_GPSPI2       = 31,
    SLEEP_RETENTION_MODULE_GPSPI3       = 32,
    SLEEP_RETENTION_MODULE_LEDC0        = 33,
    SLEEP_RETENTION_MODULE_LEDC1        = 34,
    SLEEP_RETENTION_MODULE_MCPWM0       = 35,
    SLEEP_RETENTION_MODULE_MCPWM1       = 36,
    SLEEP_RETENTION_MODULE_MCPWM2       = 37,
    SLEEP_RETENTION_MODULE_MCPWM3       = 38,
    SLEEP_RETENTION_MODULE_PCNT0        = 39,
    SLEEP_RETENTION_MODULE_PCNT1        = 40,
    SLEEP_RETENTION_MODULE_SDM0         = 41,

    /* Modem module, which includes WiFi, BLE and 802.15.4 */
    SLEEP_RETENTION_MODULE_WIFI_MAC     = 42,
    SLEEP_RETENTION_MODULE_WIFI_BB      = 43,
    SLEEP_RETENTION_MODULE_BLE_MAC      = 44,
    SLEEP_RETENTION_MODULE_BT_BB        = 45,
    SLEEP_RETENTION_MODULE_802154_MAC   = 46,
    SLEEP_RETENTION_MODULE_CLOCK_MODEM  = 47,
    SLEEP_RETENTION_MODULE_MODEM_PHY    = 48,

    SLEEP_RETENTION_MODULE_MAX          = SOC_PM_RETENTION_MODULE_NUM - 1
} periph_retention_module_t;

#define is_top_domain_module(m)     ((m >= SLEEP_RETENTION_MODULE_CLOCK_SYSTEM) && ((m <= SLEEP_RETENTION_MODULE_SDM0)))

#ifdef __cplusplus
}
#endif
