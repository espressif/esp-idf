/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "soc/soc_caps.h"

typedef struct {
    uint8_t threshold;
    bool enabled;
    bool reset_enabled;
    bool flash_power_down;
    bool rf_power_down;
} brownout_hal_config_t;

/**
 * @brief Config brown out hal.
 *
 * @param cfg Pointer of brown out configuration structure.
 */
void brownout_hal_config(const brownout_hal_config_t *cfg);


#ifdef __cplusplus
}
#endif
