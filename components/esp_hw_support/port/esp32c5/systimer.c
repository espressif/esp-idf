/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_private/systimer.h"

#if CONFIG_XTAL_FREQ_40
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
#elif CONFIG_XTAL_FREQ_48
uint64_t systimer_ticks_to_us(uint64_t ticks)
{
    return ticks * 5 / 96;
}

uint64_t systimer_us_to_ticks(uint64_t us)
{
    return us * 96 / 5;
}
#else
#error "Unsupported XTAL frequency by systimer"
#endif // CONFIG_XTAL_FREQ_xx
