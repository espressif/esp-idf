/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

#include "soc/interrupts.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    /* HP peripherals */
    PERIPH_TIMG0_MODULE,
    PERIPH_TIMG1_MODULE,
    PERIPH_UHCI0_MODULE,
    PERIPH_SYSTIMER_MODULE,
    /* Peripherals clock managed by the modem_clock driver must be listed last in the enumeration */
    PERIPH_WIFI_MODULE,
    PERIPH_BT_MODULE,
    PERIPH_IEEE802154_MODULE,
    PERIPH_COEX_MODULE,
    PERIPH_PHY_MODULE,
    PERIPH_ANA_I2C_MASTER_MODULE,
    PERIPH_MODEM_ETM_MODULE,
    PERIPH_MODEM_ADC_COMMON_FE_MODULE,
    PERIPH_PHY_CALIBRATION_MODULE,
    PERIPH_MODULE_MAX
    /*  !!! Don't append soc modules here !!! */
} shared_periph_module_t;

#define PERIPH_MODEM_MODULE_MIN PERIPH_WIFI_MODULE
#define PERIPH_MODEM_MODULE_MAX PERIPH_PHY_CALIBRATION_MODULE
#define PERIPH_MODEM_MODULE_NUM (PERIPH_MODEM_MODULE_MAX - PERIPH_MODEM_MODULE_MIN + 1)
#define IS_MODEM_MODULE(periph)  ((periph>=PERIPH_MODEM_MODULE_MIN) && (periph<=PERIPH_MODEM_MODULE_MAX))

#ifdef __cplusplus
}
#endif
