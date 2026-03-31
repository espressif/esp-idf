/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "sdkconfig.h"
#include "ulp_sum.h"
#include "ulp_clamp.h"

int ulp_sum_func(int a, int b)
{
    /* Uses ulp_clamp() from the ulp_clamp managed component.
     * CONFIG_ULP_CLAMP_MAX_VALUE comes from ulp_clamp's Kconfig. */
    return ulp_clamp(a + b, 0, CONFIG_ULP_CLAMP_MAX_VALUE);
}
