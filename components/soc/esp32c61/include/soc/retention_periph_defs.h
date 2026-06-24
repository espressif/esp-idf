/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
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
    SLEEP_RETENTION_MODULE_NULL         = SLEEP_RETENTION_MODULE_MIN, /* This module is for all peripherals that can't survive from PD_TOP to call init only. Shouldn't have any dependency. */
    /* clock module */
    SLEEP_RETENTION_MODULE_CLOCK_SYSTEM = 1,
    /* digital peripheral module, which includes Interrupt Matrix, HP_SYSTEM,
     * TEE, APM, IOMUX, SPIMEM, SysTimer, etc.. */
    SLEEP_RETENTION_MODULE_SYS_PERIPH   = 2,
    /* Timer Group by target */
    SLEEP_RETENTION_MODULE_TG0_WDT      = 3,
    SLEEP_RETENTION_MODULE_TG1_WDT      = 4,
    SLEEP_RETENTION_MODULE_TG0_TIMER0   = 5,
    SLEEP_RETENTION_MODULE_TG1_TIMER0   = 6,
    /* GDMA by channel */
    SLEEP_RETENTION_MODULE_GDMA_CH0     = 7,
    SLEEP_RETENTION_MODULE_GDMA_CH1     = 8,
    /* MISC Peripherals */
    SLEEP_RETENTION_MODULE_I2C0         = 9,
    SLEEP_RETENTION_MODULE_UART0        = 10,
    SLEEP_RETENTION_MODULE_UART1        = 11,
    SLEEP_RETENTION_MODULE_UART2        = 12,
    SLEEP_RETENTION_MODULE_ETM0         = 13,
    SLEEP_RETENTION_MODULE_GPSPI2       = 14,
    SLEEP_RETENTION_MODULE_LEDC         = 15,
    SLEEP_RETENTION_MODULE_I2S0         = 16,
    SLEEP_RETENTION_MODULE_TEMP_SENSOR  = 17,

    /* Modem module, which includes WiFi, BLE and 802.15.4 */
    SLEEP_RETENTION_MODULE_CLOCK_MODEM  = 25,
    SLEEP_RETENTION_MODULE_WIFI_MAC     = 26,
    SLEEP_RETENTION_MODULE_WIFI_BB      = 27,
    SLEEP_RETENTION_MODULE_BLE_MAC      = 28,
    SLEEP_RETENTION_MODULE_BT_BB        = 29,
    SLEEP_RETENTION_MODULE_802154_MAC   = 30,

    SLEEP_RETENTION_MODULE_MAX          = SOC_PM_RETENTION_MODULE_NUM - 1
} periph_retention_module_t;

#define is_top_domain_module(m)     (m <= SLEEP_RETENTION_MODULE_TEMP_SENSOR)

#ifdef __cplusplus
}
#endif
