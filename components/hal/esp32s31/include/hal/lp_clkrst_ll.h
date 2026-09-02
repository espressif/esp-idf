/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "hal/rng_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ESP32-S31 has no LP_CLKRST RNG clock gate. Reuse this hook so
 * hw_random.c can enable the full RNG module without chip-specific ifdefs.
 */
static inline void _lp_clkrst_ll_enable_rng_clock(bool en)
{
    if (en) {
        rng_ll_enable();
    } else {
        rng_ll_disable();
    }
}

#ifdef __cplusplus
}
#endif
