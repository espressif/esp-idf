/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_private/systimer.h"
#include "hal/clk_tree_ll.h"

/**
 * @brief systimer's clock source is fixed to XTAL, the fixed fractional divider is changed according to
 * EFUSE_XTAL_48M_SEL. No matter 48MHz or 40MHz XTAL, the resolution of the systimer is always 16MHz.
 */

uint64_t systimer_ticks_to_us(uint64_t ticks)
{
    return ticks / 16;
}

uint64_t systimer_us_to_ticks(uint64_t us)
{
    return us * 16;
}
