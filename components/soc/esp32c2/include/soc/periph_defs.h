/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

#include "soc/interrupts.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PERIPH_TIMG0_MODULE,
    PERIPH_UHCI0_MODULE,
    PERIPH_RNG_MODULE,
    PERIPH_WIFI_MODULE,
    PERIPH_BT_MODULE,
    PERIPH_WIFI_BT_COMMON_MODULE,
    PERIPH_SYSTIMER_MODULE,
    PERIPH_PHY_CALIBRATION_MODULE,
    PERIPH_COEX_MODULE,
    PERIPH_MODULE_MAX
} shared_periph_module_t;

#ifdef __cplusplus
}
#endif
