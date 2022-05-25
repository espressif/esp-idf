/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
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
 * @brief get rated frequency in MHz
 */
uint32_t efuse_hal_get_rated_freq_mhz(void);

#ifdef __cplusplus
}
#endif
