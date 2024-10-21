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
    /* digital peripheral module, which includes Interrupt Matrix, HP_SYSTEM,
     * TEE, APM, IOMUX, SPIMEM, SysTimer, etc.. */
    SLEEP_RETENTION_MODULE_SYS_PERIPH   = 3,
    /* Timer Group by target*/
    SLEEP_RETENTION_MODULE_TG0_WDT      = 4,
    SLEEP_RETENTION_MODULE_TG1_WDT      = 5,
    SLEEP_RETENTION_MODULE_TG0_TIMER    = 6,
    SLEEP_RETENTION_MODULE_TG1_TIMER    = 7,
    /* GDMA by channel */
    SLEEP_RETENTION_MODULE_GDMA_CH0     = 8,
    SLEEP_RETENTION_MODULE_GDMA_CH1     = 9,
    SLEEP_RETENTION_MODULE_GDMA_CH2     = 10,
    /* MISC Peripherals */
    SLEEP_RETENTION_MODULE_ADC          = 11,
    SLEEP_RETENTION_MODULE_I2C0         = 12,
    SLEEP_RETENTION_MODULE_RMT0         = 13,
    SLEEP_RETENTION_MODULE_UART0        = 14,
    SLEEP_RETENTION_MODULE_UART1        = 15,
    SLEEP_RETENTION_MODULE_I2S0         = 16,
    SLEEP_RETENTION_MODULE_ETM0         = 17,
    SLEEP_RETENTION_MODULE_TEMP_SENSOR  = 18,
    SLEEP_RETENTION_MODULE_TWAI0        = 19,
    SLEEP_RETENTION_MODULE_TWAI1        = 20,

    /* Modem module, which includes WiFi, BLE and 802.15.4 */
    SLEEP_RETENTION_MODULE_WIFI_MAC     = 26,
    SLEEP_RETENTION_MODULE_WIFI_BB      = 27,
    SLEEP_RETENTION_MODULE_BLE_MAC      = 28,
    SLEEP_RETENTION_MODULE_BT_BB        = 29,
    SLEEP_RETENTION_MODULE_802154_MAC   = 30,
    SLEEP_RETENTION_MODULE_MAX          = 31
} periph_retention_module_t;

typedef enum periph_retention_module_bitmap {
    /* clock module, which includes system and modem */
    SLEEP_RETENTION_MODULE_BM_CLOCK_SYSTEM = BIT(SLEEP_RETENTION_MODULE_CLOCK_SYSTEM),
    SLEEP_RETENTION_MODULE_BM_CLOCK_MODEM  = BIT(SLEEP_RETENTION_MODULE_CLOCK_MODEM),
    /* digital peripheral module, which includes Interrupt Matrix, HP_SYSTEM,
     * TEE, APM, IOMUX, SPIMEM, SysTimer, etc.. */
    SLEEP_RETENTION_MODULE_BM_SYS_PERIPH   = BIT(SLEEP_RETENTION_MODULE_SYS_PERIPH),
    /* Timer Group by target*/
    SLEEP_RETENTION_MODULE_BM_TASK_WDT     = BIT(SLEEP_RETENTION_MODULE_TG0_WDT),
    SLEEP_RETENTION_MODULE_BM_INT_WDT      = BIT(SLEEP_RETENTION_MODULE_TG1_WDT),
    SLEEP_RETENTION_MODULE_BM_TG0_TIMER    = BIT(SLEEP_RETENTION_MODULE_TG0_TIMER),
    SLEEP_RETENTION_MODULE_BM_TG1_TIMER    = BIT(SLEEP_RETENTION_MODULE_TG1_TIMER),
    /* GDMA by channel */
    SLEEP_RETENTION_MODULE_BM_GDMA_CH0     = BIT(SLEEP_RETENTION_MODULE_GDMA_CH0),
    SLEEP_RETENTION_MODULE_BM_GDMA_CH1     = BIT(SLEEP_RETENTION_MODULE_GDMA_CH1),
    SLEEP_RETENTION_MODULE_BM_GDMA_CH2     = BIT(SLEEP_RETENTION_MODULE_GDMA_CH2),
    /* MISC Peripherals */
    SLEEP_RETENTION_MODULE_BM_ADC          = BIT(SLEEP_RETENTION_MODULE_ADC),
    SLEEP_RETENTION_MODULE_BM_I2C0         = BIT(SLEEP_RETENTION_MODULE_I2C0),
    SLEEP_RETENTION_MODULE_BM_RMT0         = BIT(SLEEP_RETENTION_MODULE_RMT0),
    SLEEP_RETENTION_MODULE_BM_UART0        = BIT(SLEEP_RETENTION_MODULE_UART0),
    SLEEP_RETENTION_MODULE_BM_UART1        = BIT(SLEEP_RETENTION_MODULE_UART1),
    SLEEP_RETENTION_MODULE_BM_I2S0         = BIT(SLEEP_RETENTION_MODULE_I2S0),
    SLEEP_RETENTION_MODULE_BM_ETM0         = BIT(SLEEP_RETENTION_MODULE_ETM0),
    SLEEP_RETENTION_MODULE_BM_TEMP_SENSOR  = BIT(SLEEP_RETENTION_MODULE_TEMP_SENSOR),
    SLEEP_RETENTION_MODULE_BM_TWAI0        = BIT(SLEEP_RETENTION_MODULE_TWAI0),
    SLEEP_RETENTION_MODULE_BM_TWAI1        = BIT(SLEEP_RETENTION_MODULE_TWAI1),
    /* modem module, which includes WiFi, BLE and 802.15.4 */
    SLEEP_RETENTION_MODULE_BM_WIFI_MAC     = BIT(SLEEP_RETENTION_MODULE_WIFI_MAC),
    SLEEP_RETENTION_MODULE_BM_WIFI_BB      = BIT(SLEEP_RETENTION_MODULE_WIFI_BB),
    SLEEP_RETENTION_MODULE_BM_BLE_MAC      = BIT(SLEEP_RETENTION_MODULE_BLE_MAC),
    SLEEP_RETENTION_MODULE_BM_BT_BB        = BIT(SLEEP_RETENTION_MODULE_BT_BB),
    SLEEP_RETENTION_MODULE_BM_802154_MAC   = BIT(SLEEP_RETENTION_MODULE_802154_MAC),
    SLEEP_RETENTION_MODULE_BM_ALL          = (uint32_t)-1
} periph_retention_module_bitmap_t;

#define TOP_DOMAIN_PERIPHERALS_BM ( SLEEP_RETENTION_MODULE_BM_SYS_PERIPH  \
                                  | SLEEP_RETENTION_MODULE_BM_TASK_WDT    \
                                  | SLEEP_RETENTION_MODULE_BM_INT_WDT     \
                                  | SLEEP_RETENTION_MODULE_BM_TG0_TIMER   \
                                  | SLEEP_RETENTION_MODULE_BM_TG1_TIMER   \
                                  | SLEEP_RETENTION_MODULE_BM_GDMA_CH0    \
                                  | SLEEP_RETENTION_MODULE_BM_GDMA_CH1    \
                                  | SLEEP_RETENTION_MODULE_BM_GDMA_CH2    \
                                  | SLEEP_RETENTION_MODULE_BM_ADC         \
                                  | SLEEP_RETENTION_MODULE_BM_I2C0        \
                                  | SLEEP_RETENTION_MODULE_BM_RMT0        \
                                  | SLEEP_RETENTION_MODULE_BM_UART0       \
                                  | SLEEP_RETENTION_MODULE_BM_UART1       \
                                  | SLEEP_RETENTION_MODULE_BM_I2S0        \
                                  | SLEEP_RETENTION_MODULE_BM_ETM0        \
                                  | SLEEP_RETENTION_MODULE_BM_TEMP_SENSOR \
                                  | SLEEP_RETENTION_MODULE_BM_TWAI0       \
                                  | SLEEP_RETENTION_MODULE_BM_TWAI1       \
                                  )

#ifdef __cplusplus
}
#endif
