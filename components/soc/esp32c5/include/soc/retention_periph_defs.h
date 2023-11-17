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

typedef enum periph_retention_module_bitmap {
    /* clock module, which includes system and modem */
    SLEEP_RETENTION_MODULE_CLOCK_SYSTEM = BIT(1),
    SLEEP_RETENTION_MODULE_CLOCK_MODEM  = BIT(2),

    /* modem module, which includes WiFi, BLE and 802.15.4 */
    SLEEP_RETENTION_MODULE_WIFI_MAC     = BIT(10),
    SLEEP_RETENTION_MODULE_WIFI_BB      = BIT(11),
    SLEEP_RETENTION_MODULE_BLE_MAC      = BIT(12),
    SLEEP_RETENTION_MODULE_BT_BB        = BIT(13),
    SLEEP_RETENTION_MODULE_802154_MAC   = BIT(14),

    /* digital peripheral module, which includes Interrupt Matrix, HP_SYSTEM,
     * TEE, APM, UART, Timer Group, IOMUX, SPIMEM, SysTimer, etc.. */
    SLEEP_RETENTION_MODULE_INTR_MATRIX  = BIT(16),
    SLEEP_RETENTION_MODULE_HP_SYSTEM    = BIT(17),
    SLEEP_RETENTION_MODULE_TEE_APM      = BIT(18),
    SLEEP_RETENTION_MODULE_UART0        = BIT(19),
    SLEEP_RETENTION_MODULE_TG0          = BIT(20),
    SLEEP_RETENTION_MODULE_IOMUX        = BIT(21),
    SLEEP_RETENTION_MODULE_SPIMEM       = BIT(22),
    SLEEP_RETENTION_MODULE_SYSTIMER     = BIT(23),
    SLEEP_RETENTION_MODULE_GDMA_CH0     = BIT(24),
    SLEEP_RETENTION_MODULE_GDMA_CH1     = BIT(25),
    SLEEP_RETENTION_MODULE_GDMA_CH2     = BIT(26),
    SLEEP_RETENTION_MODULE_ALL          = (uint32_t)-1
} periph_retention_module_bitmap_t;

#ifdef __cplusplus
}
#endif
