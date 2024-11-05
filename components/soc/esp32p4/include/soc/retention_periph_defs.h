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
    /* clock module, which includes system and modem */
    SLEEP_RETENTION_MODULE_CLOCK_SYSTEM = 1,
    /* digital peripheral module, which includes Interrupt Matrix, HP_SYSTEM,
     * TEE, APM, UART, IOMUX, SPIMEM, SysTimer, etc.. */
    SLEEP_RETENTION_MODULE_SYS_PERIPH   = 2,
    /* Timer Group by target*/
    SLEEP_RETENTION_MODULE_TG0_WDT      = 3,
    SLEEP_RETENTION_MODULE_TG1_WDT      = 4,
    SLEEP_RETENTION_MODULE_TG0_TIMER0   = 5,
    SLEEP_RETENTION_MODULE_TG0_TIMER1   = 6,
    SLEEP_RETENTION_MODULE_TG1_TIMER0   = 7,
    SLEEP_RETENTION_MODULE_TG1_TIMER1   = 8,
    /* MISC Peripherals */
    SLEEP_RETENTION_MODULE_UART0        = 9,
    SLEEP_RETENTION_MODULE_UART1        = 10,
    SLEEP_RETENTION_MODULE_UART2        = 11,
    SLEEP_RETENTION_MODULE_UART3        = 12,
    SLEEP_RETENTION_MODULE_UART4        = 13,

    SLEEP_RETENTION_MODULE_MAX          = SOC_PM_RETENTION_MODULE_NUM - 1
} periph_retention_module_t;

#define is_top_domain_module(m)                           \
    ( ((m) == SLEEP_RETENTION_MODULE_CLOCK_SYSTEM) ? true \
    : ((m) == SLEEP_RETENTION_MODULE_SYS_PERIPH)   ? true \
    : ((m) == SLEEP_RETENTION_MODULE_TG0_WDT)      ? true \
    : ((m) == SLEEP_RETENTION_MODULE_TG1_WDT)      ? true \
    : ((m) == SLEEP_RETENTION_MODULE_TG0_TIMER0)   ? true \
    : ((m) == SLEEP_RETENTION_MODULE_TG0_TIMER1)   ? true \
    : ((m) == SLEEP_RETENTION_MODULE_TG1_TIMER0)   ? true \
    : ((m) == SLEEP_RETENTION_MODULE_TG1_TIMER1)   ? true \
    : ((m) == SLEEP_RETENTION_MODULE_UART0)        ? true \
    : ((m) == SLEEP_RETENTION_MODULE_UART1)        ? true \
    : ((m) == SLEEP_RETENTION_MODULE_UART2)        ? true \
    : ((m) == SLEEP_RETENTION_MODULE_UART3)        ? true \
    : ((m) == SLEEP_RETENTION_MODULE_UART4)        ? true \
    : false)

#ifdef __cplusplus
}
#endif
