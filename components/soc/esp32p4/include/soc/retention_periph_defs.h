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
    SLEEP_RETENTION_MODULE_REGDMA_SYSTEM = BIT(2),

    /* digital peripheral module, which includes Interrupt Matrix, HP_SYSTEM,
     * TEE, APM, UART, Timer Group, IOMUX, SPIMEM, SysTimer, etc.. */
    SLEEP_RETENTION_MODULE_INTR_MATRIX  = BIT(10),
    SLEEP_RETENTION_MODULE_HP_SYSTEM    = BIT(11),
    SLEEP_RETENTION_MODULE_TEE_APM      = BIT(12),
    SLEEP_RETENTION_MODULE_UART0        = BIT(13),
    SLEEP_RETENTION_MODULE_TG0          = BIT(14),
    SLEEP_RETENTION_MODULE_IOMUX        = BIT(15),
    SLEEP_RETENTION_MODULE_SPIMEM       = BIT(16),
    SLEEP_RETENTION_MODULE_SYSTIMER     = BIT(17),
    SLEEP_RETENTION_MODULE_L2_CACHE     = BIT(18),

    SLEEP_RETENTION_MODULE_ALL          = (uint32_t)-1
} periph_retention_module_bitmap_t;

#ifdef __cplusplus
}
#endif
