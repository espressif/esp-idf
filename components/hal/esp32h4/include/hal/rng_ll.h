/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc.h"
#include "soc/wdev_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t rng_ll_read_data(void)
{
    // return REG_READ(WDEV_RND_REG); // TODO: IDF-12265
    return 0;
}

#ifdef __cplusplus
}
#endif
