/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "hal/efuse_ll.h"
#include_next "hal/efuse_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief set eFuse timings
 *
 * @param apb_freq_hz APB frequency in Hz
 */
void efuse_hal_set_timing(uint32_t apb_freq_hz);

#ifdef __cplusplus
}
#endif
