/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-C6 LP_CLKRST & LP PERI register operations

#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include "soc/soc.h"
#include "soc/lp_clkrst_struct.h"
#include "soc/lpperi_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((always_inline))
static inline void _lp_clkrst_ll_enable_rng_clock(bool en)
{
    LPPERI.clk_en.rng_ck_en = en;
}

/// LPPERI.clk_en is a shared register, so this function must be used in an atomic way
#define lp_clkrst_ll_enable_rng_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _lp_clkrst_ll_enable_rng_clock(__VA_ARGS__); \
    } while(0)

#ifdef __cplusplus
}
#endif
