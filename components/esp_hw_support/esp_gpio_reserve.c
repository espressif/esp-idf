/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_types.h"
#include "esp_bit_defs.h"
#include "soc/soc_caps.h"

static uint64_t s_reserve_status = 0;

void esp_gpio_reserve_pins(uint64_t mask)
{
#if SOC_GPIO_PIN_COUNT < 64
    mask &= BIT64(SOC_GPIO_PIN_COUNT) - 1;
#endif
    s_reserve_status |= mask;
}

bool esp_gpio_is_pin_reserved(uint32_t gpio_num)
{
    if (gpio_num >= SOC_GPIO_PIN_COUNT) {
        return false;
    }
    return !!(s_reserve_status & BIT64(gpio_num));
}

// TODO: IDF-6968 reserve the pins that not fanned out regarding the SiP version
