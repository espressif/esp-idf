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
    SLEEP_RETENTION_MODULE_CLOCK_MODEM  = 2,
    /* digital peripheral module, which includes Interrupt Matrix, HP_SYSTEM,
     * TEE, APM, UART, IOMUX, SPIMEM, SysTimer, etc.. */
    SLEEP_RETENTION_MODULE_SYS_PERIPH   = 3,
    /* Timer Group by target*/
    SLEEP_RETENTION_MODULE_TG0_WDT      = 4,
    SLEEP_RETENTION_MODULE_TG1_WDT      = 5,
    SLEEP_RETENTION_MODULE_TG0_TIMER0   = 6,
    SLEEP_RETENTION_MODULE_TG1_TIMER0   = 7,
    /* GDMA by channel */
    SLEEP_RETENTION_MODULE_GDMA_CH0     = 8,
    SLEEP_RETENTION_MODULE_GDMA_CH1     = 9,
    SLEEP_RETENTION_MODULE_GDMA_CH2     = 10,
    /* MISC Peripherals */
    SLEEP_RETENTION_MODULE_ADC          = 11,
    SLEEP_RETENTION_MODULE_I2C0         = 12,
    SLEEP_RETENTION_MODULE_I2C1         = 13,
    SLEEP_RETENTION_MODULE_RMT0         = 14,
    SLEEP_RETENTION_MODULE_UART0        = 15,
    SLEEP_RETENTION_MODULE_UART1        = 16,
    SLEEP_RETENTION_MODULE_I2S0         = 17,
    SLEEP_RETENTION_MODULE_ETM0         = 18,
    SLEEP_RETENTION_MODULE_TEMP_SENSOR  = 19,
    SLEEP_RETENTION_MODULE_TWAI0        = 20,
    SLEEP_RETENTION_MODULE_PARLIO0      = 21,
    SLEEP_RETENTION_MODULE_GPSPI2       = 22,
    SLEEP_RETENTION_MODULE_LEDC         = 23,
    SLEEP_RETENTION_MODULE_PCNT0        = 24,
    SLEEP_RETENTION_MODULE_MCPWM0       = 25,

    /* Modem module, which includes BLE and 802.15.4 */
    SLEEP_RETENTION_MODULE_BLE_MAC      = 28,
    SLEEP_RETENTION_MODULE_BT_BB        = 29,
    SLEEP_RETENTION_MODULE_802154_MAC   = 30,

    SLEEP_RETENTION_MODULE_MAX          = SOC_PM_RETENTION_MODULE_NUM - 1
} periph_retention_module_t;

#define is_top_domain_module(m)                           \
    ( ((m) == SLEEP_RETENTION_MODULE_NULL)         ? true \
    : ((m) == SLEEP_RETENTION_MODULE_CLOCK_SYSTEM) ? true \
    : ((m) == SLEEP_RETENTION_MODULE_SYS_PERIPH)   ? true \
    : ((m) == SLEEP_RETENTION_MODULE_TG0_WDT)      ? true \
    : ((m) == SLEEP_RETENTION_MODULE_TG1_WDT)      ? true \
    : ((m) == SLEEP_RETENTION_MODULE_TG0_TIMER0)   ? true \
    : ((m) == SLEEP_RETENTION_MODULE_TG1_TIMER0)   ? true \
    : ((m) == SLEEP_RETENTION_MODULE_GDMA_CH0)     ? true \
    : ((m) == SLEEP_RETENTION_MODULE_GDMA_CH1)     ? true \
    : ((m) == SLEEP_RETENTION_MODULE_GDMA_CH2)     ? true \
    : ((m) == SLEEP_RETENTION_MODULE_ADC)          ? true \
    : ((m) == SLEEP_RETENTION_MODULE_I2C0)         ? true \
    : ((m) == SLEEP_RETENTION_MODULE_I2C1)         ? true \
    : ((m) == SLEEP_RETENTION_MODULE_RMT0)         ? true \
    : ((m) == SLEEP_RETENTION_MODULE_UART0)        ? true \
    : ((m) == SLEEP_RETENTION_MODULE_UART1)        ? true \
    : ((m) == SLEEP_RETENTION_MODULE_I2S0)         ? true \
    : ((m) == SLEEP_RETENTION_MODULE_ETM0)         ? true \
    : ((m) == SLEEP_RETENTION_MODULE_TEMP_SENSOR)  ? true \
    : ((m) == SLEEP_RETENTION_MODULE_TWAI0)        ? true \
    : ((m) == SLEEP_RETENTION_MODULE_PARLIO0)      ? true \
    : ((m) == SLEEP_RETENTION_MODULE_GPSPI2)       ? true \
    : ((m) == SLEEP_RETENTION_MODULE_LEDC)         ? true \
    : ((m) == SLEEP_RETENTION_MODULE_PCNT0)        ? true \
    : ((m) == SLEEP_RETENTION_MODULE_MCPWM0)       ? true \
    : false)

#ifdef __cplusplus
}
#endif
