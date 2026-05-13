/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "hal/config.h"
#include "soc/soc.h"
#include "soc/lp_system_reg.h"
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
    #include "soc/trng_reg.h"
#else
    #include "soc/lp_system_reg.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t rng_ll_read_data(void)
{
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
    return REG_READ(RNG_DATA_REG);
#else
    return REG_READ(LP_SYSTEM_REG_RNG_DATA_REG);
#endif
}

#ifdef __cplusplus
}
#endif
