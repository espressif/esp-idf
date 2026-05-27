/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc.h"
#include "soc/syscon_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t rng_ll_read_data(void)
{
    return REG_READ(RNG_DATA_REG);
}

/* For compatibility. */
static inline void rng_ll_enable(void)
{
}

static inline void rng_ll_disable(void)
{
}

#ifdef __cplusplus
}
#endif
