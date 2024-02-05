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
    SLEEP_RETENTION_MODULE_REGDMA_SYSTEM = 2,

    /* digital peripheral module, which includes Interrupt Matrix, HP_SYSTEM,
     * TEE, APM, UART, Timer Group, IOMUX, SPIMEM, SysTimer, etc.. */
    SLEEP_RETENTION_MODULE_INTR_MATRIX  = 10,
    SLEEP_RETENTION_MODULE_HP_SYSTEM    = 11,
    SLEEP_RETENTION_MODULE_TEE_APM      = 12,
    SLEEP_RETENTION_MODULE_UART0        = 13,
    SLEEP_RETENTION_MODULE_TG0          = 14,
    SLEEP_RETENTION_MODULE_IOMUX        = 15,
    SLEEP_RETENTION_MODULE_SPIMEM       = 16,
    SLEEP_RETENTION_MODULE_SYSTIMER     = 17,
    SLEEP_RETENTION_MODULE_L2_CACHE     = 18,

    SLEEP_RETENTION_MODULE_MAX          = 31
} periph_retention_module_t;

typedef enum periph_retention_module_bitmap {
    /* clock module, which includes system and modem */
    SLEEP_RETENTION_MODULE_BM_CLOCK_SYSTEM = BIT(SLEEP_RETENTION_MODULE_CLOCK_SYSTEM),
    SLEEP_RETENTION_MODULE_BM_REGDMA_SYSTEM = BIT(SLEEP_RETENTION_MODULE_REGDMA_SYSTEM),

    /* digital peripheral module, which includes Interrupt Matrix, HP_SYSTEM,
     * TEE, APM, UART, Timer Group, IOMUX, SPIMEM, SysTimer, etc.. */
    SLEEP_RETENTION_MODULE_BM_INTR_MATRIX  = BIT(SLEEP_RETENTION_MODULE_INTR_MATRIX),
    SLEEP_RETENTION_MODULE_BM_HP_SYSTEM    = BIT(SLEEP_RETENTION_MODULE_HP_SYSTEM),
    SLEEP_RETENTION_MODULE_BM_TEE_APM      = BIT(SLEEP_RETENTION_MODULE_TEE_APM),
    SLEEP_RETENTION_MODULE_BM_UART0        = BIT(SLEEP_RETENTION_MODULE_UART0),
    SLEEP_RETENTION_MODULE_BM_TG0          = BIT(SLEEP_RETENTION_MODULE_TG0),
    SLEEP_RETENTION_MODULE_BM_IOMUX        = BIT(SLEEP_RETENTION_MODULE_IOMUX),
    SLEEP_RETENTION_MODULE_BM_SPIMEM       = BIT(SLEEP_RETENTION_MODULE_SPIMEM),
    SLEEP_RETENTION_MODULE_BM_SYSTIMER     = BIT(SLEEP_RETENTION_MODULE_SYSTIMER),
    SLEEP_RETENTION_MODULE_BM_L2_CACHE     = BIT(SLEEP_RETENTION_MODULE_L2_CACHE),

    SLEEP_RETENTION_MODULE_BM_ALL          = (uint32_t)-1
} periph_retention_module_bitmap_t;

#ifdef __cplusplus
}
#endif
