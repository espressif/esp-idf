/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc.h"
#include "soc/lpperi_reg.h"
#include "hal/lp_clkrst_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t rng_ll_read_data(void)
{
    return REG_READ(LPPERI_RNG_DATA_REG);
}

static inline void rng_ll_enable(void)
{
    _lp_clkrst_ll_enable_rng_clock(true);
}

static inline void rng_ll_disable(void)
{
    _lp_clkrst_ll_enable_rng_clock(false);
}

#ifdef __cplusplus
}
#endif
