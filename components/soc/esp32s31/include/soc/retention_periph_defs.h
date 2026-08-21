/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum periph_retention_module {
    SLEEP_RETENTION_MODULE_MIN          = 0,
    SLEEP_RETENTION_MODULE_NULL         = 1, /* This module is for all peripherals that can't survive from PD_TOP to call init only. Shouldn't have any dependency. */
    /* PLL source (I2C master clock + SYSPLL self-calibration) */
    SLEEP_RETENTION_MODULE_PLL_SOURCE   = 2,
    /* clock module, which includes system and modem */
    SLEEP_RETENTION_MODULE_CLOCK_SYSTEM = 3,
    /* digital peripheral module, which includes Interrupt Matrix, HP_SYSTEM,
     * TEE, APM, UART, IOMUX, SPIMEM, SysTimer, etc.. */
    SLEEP_RETENTION_MODULE_SYS_PERIPH   = 4,
    /* Timer Group by target*/
    SLEEP_RETENTION_MODULE_TG0_WDT      = 5,
    SLEEP_RETENTION_MODULE_TG1_WDT      = 6,
    SLEEP_RETENTION_MODULE_TG0_TIMER0   = 7,
    SLEEP_RETENTION_MODULE_TG0_TIMER1   = 8,
    SLEEP_RETENTION_MODULE_TG1_TIMER0   = 9,
    SLEEP_RETENTION_MODULE_TG1_TIMER1   = 10,

    /* AHB_DMA by channel */
    SLEEP_RETENTION_MODULE_AHB_DMA_CH0  = 11,
    SLEEP_RETENTION_MODULE_AHB_DMA_CH1  = 12,
    SLEEP_RETENTION_MODULE_AHB_DMA_CH2  = 13,
    SLEEP_RETENTION_MODULE_AHB_DMA_CH3  = 14,
    SLEEP_RETENTION_MODULE_AHB_DMA_CH4  = 15,
    /* AXI_DMA by channel */
    SLEEP_RETENTION_MODULE_AXI_DMA_CH0  = 16,
    SLEEP_RETENTION_MODULE_AXI_DMA_CH1  = 17,
    SLEEP_RETENTION_MODULE_AXI_DMA_CH2  = 18,
    /* LP_AHB_DMA by channel */
    SLEEP_RETENTION_MODULE_LP_AHB_DMA_CH0 = 19,
    SLEEP_RETENTION_MODULE_LP_AHB_DMA_CH1 = 20,
    /* MISC Peripherals */
    SLEEP_RETENTION_MODULE_UART0        = 21,
    SLEEP_RETENTION_MODULE_UART1        = 22,
    SLEEP_RETENTION_MODULE_UART2        = 23,
    SLEEP_RETENTION_MODULE_UART3        = 24,
    SLEEP_RETENTION_MODULE_RMT0         = 25,
    SLEEP_RETENTION_MODULE_I2S0         = 26,
    SLEEP_RETENTION_MODULE_I2S1         = 27,
    SLEEP_RETENTION_MODULE_I2C0         = 28,
    SLEEP_RETENTION_MODULE_I2C1         = 29,
    SLEEP_RETENTION_MODULE_ETM0         = 30,
    SLEEP_RETENTION_MODULE_TWAI0        = 31,
    SLEEP_RETENTION_MODULE_TWAI1        = 32,
    SLEEP_RETENTION_MODULE_PARLIO0      = 33,
    SLEEP_RETENTION_MODULE_GPSPI2       = 34,
    SLEEP_RETENTION_MODULE_GPSPI3       = 35,
    SLEEP_RETENTION_MODULE_LEDC0        = 36,
    SLEEP_RETENTION_MODULE_LEDC1        = 37,
    SLEEP_RETENTION_MODULE_MCPWM0       = 38,
    SLEEP_RETENTION_MODULE_MCPWM1       = 39,
    SLEEP_RETENTION_MODULE_MCPWM2       = 40,
    SLEEP_RETENTION_MODULE_MCPWM3       = 41,
    SLEEP_RETENTION_MODULE_SDM0         = 42,
    SLEEP_RETENTION_MODULE_LCDCAM       = 43,
    SLEEP_RETENTION_MODULE_JPEG         = 44,
    SLEEP_RETENTION_MODULE_DMA2D        = 45,
    SLEEP_RETENTION_MODULE_PPA          = 46,

    /* Modem module, which includes WiFi, BLE and 802.15.4 */
    SLEEP_RETENTION_MODULE_WIFI_MAC     = 47,
    SLEEP_RETENTION_MODULE_WIFI_BB      = 48,
    SLEEP_RETENTION_MODULE_BLE_MAC      = 49,
    SLEEP_RETENTION_MODULE_BT_BB        = 50,
    SLEEP_RETENTION_MODULE_802154_MAC   = 51,
    SLEEP_RETENTION_MODULE_CLOCK_MODEM  = 52,
    SLEEP_RETENTION_MODULE_MODEM_PHY    = 53,
    SLEEP_RETENTION_MODULE_PHY_FE       = 54,

    SLEEP_RETENTION_MODULE_ASRC         = 55,

    SLEEP_RETENTION_MODULE_MAX,
} periph_retention_module_t;

#define is_top_domain_module(m)     ((m >= SLEEP_RETENTION_MODULE_PLL_SOURCE) && ((m <= SLEEP_RETENTION_MODULE_PPA)))

#ifdef __cplusplus
}
#endif
