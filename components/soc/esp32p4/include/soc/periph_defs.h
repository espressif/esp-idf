/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

#include "soc/interrupts.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PERIPH_LCD_CAM_MODULE,
    PERIPH_TIMG0_MODULE,
    PERIPH_TIMG1_MODULE,
    PERIPH_SYSTIMER_MODULE,
    /* MISC */
    PERIPH_ANA_I2C_MASTER_MODULE,
    PERIPH_MODULE_MAX
} shared_periph_module_t;

#ifdef __cplusplus
}
#endif
