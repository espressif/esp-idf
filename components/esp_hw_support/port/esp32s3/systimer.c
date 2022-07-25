/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/systimer.h"

/**
 * @brief systimer's clock source is fixed to XTAL (40MHz), and has a fixed fractional divider (2.5).
 *        So the resolution of the systimer is 40MHz/2.5 = 16MHz.
 */

uint64_t systimer_ticks_to_us(uint64_t ticks)
{
    return ticks / 16;
}

uint64_t systimer_us_to_ticks(uint64_t us)
{
    return us * 16;
}
