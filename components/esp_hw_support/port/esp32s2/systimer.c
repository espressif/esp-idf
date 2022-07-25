/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/systimer.h"

/**
 * @brief systimer's clock source can be switch between XTAL (40MHz) and APB (80MHz).
 *        But the resolution of the systimer is configured to fix to 80MHz.
 */

uint64_t systimer_ticks_to_us(uint64_t ticks)
{
    return ticks / 80;
}

uint64_t systimer_us_to_ticks(uint64_t us)
{
    return us * 80;
}
