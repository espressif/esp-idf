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
    /* clock module, which includes system and modem */
    SLEEP_RETENTION_MODULE_CLOCK_SYSTEM = 1,
    SLEEP_RETENTION_MODULE_CLOCK_MODEM  = 2,

    /* modem module, which includes WiFi, BLE and 802.15.4 */
    SLEEP_RETENTION_MODULE_WIFI_MAC     = 10,
    SLEEP_RETENTION_MODULE_WIFI_BB      = 11,
    SLEEP_RETENTION_MODULE_BLE_MAC      = 12,
    SLEEP_RETENTION_MODULE_BT_BB        = 13,
    SLEEP_RETENTION_MODULE_802154_MAC   = 14,

    /* digital peripheral module, which includes Interrupt Matrix, HP_SYSTEM,
     * TEE, APM, UART, Timer Group, IOMUX, SPIMEM, SysTimer, etc.. */
    SLEEP_RETENTION_MODULE_SYS_PERIPH   = 16,

    SLEEP_RETENTION_MODULE_ADC          = 17,

    SLEEP_RETENTION_MODULE_GDMA_CH0     = 24,
    SLEEP_RETENTION_MODULE_GDMA_CH1     = 25,
    SLEEP_RETENTION_MODULE_GDMA_CH2     = 26,
    SLEEP_RETENTION_MODULE_I2C0         = 27,
    SLEEP_RETENTION_MODULE_RMT0         = 28,

    SLEEP_RETENTION_MODULE_MAX          = 31
} periph_retention_module_t;

typedef enum periph_retention_module_bitmap {
    /* clock module, which includes system and modem */
    SLEEP_RETENTION_MODULE_BM_CLOCK_SYSTEM = BIT(SLEEP_RETENTION_MODULE_CLOCK_SYSTEM),
    SLEEP_RETENTION_MODULE_BM_CLOCK_MODEM  = BIT(SLEEP_RETENTION_MODULE_CLOCK_MODEM),

    /* modem module, which includes WiFi, BLE and 802.15.4 */
    SLEEP_RETENTION_MODULE_BM_WIFI_MAC     = BIT(SLEEP_RETENTION_MODULE_WIFI_MAC),
    SLEEP_RETENTION_MODULE_BM_WIFI_BB      = BIT(SLEEP_RETENTION_MODULE_WIFI_BB),
    SLEEP_RETENTION_MODULE_BM_BLE_MAC      = BIT(SLEEP_RETENTION_MODULE_BLE_MAC),
    SLEEP_RETENTION_MODULE_BM_BT_BB        = BIT(SLEEP_RETENTION_MODULE_BT_BB),
    SLEEP_RETENTION_MODULE_BM_802154_MAC   = BIT(SLEEP_RETENTION_MODULE_802154_MAC),

    /* digital peripheral module, which includes Interrupt Matrix, HP_SYSTEM,
     * TEE, APM, UART, Timer Group, IOMUX, SPIMEM, SysTimer, etc.. */
    SLEEP_RETENTION_MODULE_BM_SYS_PERIPH   = BIT(SLEEP_RETENTION_MODULE_SYS_PERIPH),

    SLEEP_RETENTION_MODULE_BM_ADC          = BIT(SLEEP_RETENTION_MODULE_ADC),

    SLEEP_RETENTION_MODULE_BM_GDMA_CH0     = BIT(SLEEP_RETENTION_MODULE_GDMA_CH0),
    SLEEP_RETENTION_MODULE_BM_GDMA_CH1     = BIT(SLEEP_RETENTION_MODULE_GDMA_CH1),
    SLEEP_RETENTION_MODULE_BM_GDMA_CH2     = BIT(SLEEP_RETENTION_MODULE_GDMA_CH2),
    SLEEP_RETENTION_MODULE_BM_I2C0         = BIT(SLEEP_RETENTION_MODULE_I2C0),
    SLEEP_RETENTION_MODULE_BM_RMT0         = BIT(SLEEP_RETENTION_MODULE_RMT0),
    SLEEP_RETENTION_MODULE_BM_ALL          = (uint32_t)-1
} periph_retention_module_bitmap_t;

#ifdef __cplusplus
}
#endif
