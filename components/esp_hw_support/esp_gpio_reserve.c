/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdatomic.h>
#include "soc/soc_caps.h"
#include "esp_types.h"
#include "esp_bit_defs.h"
#include "esp_private/esp_gpio_reserve.h"

static _Atomic uint64_t s_reserved_pin_mask = ATOMIC_VAR_INIT(~(SOC_GPIO_VALID_OUTPUT_GPIO_MASK));

uint64_t esp_gpio_reserve(uint64_t gpio_mask)
{
    return atomic_fetch_or(&s_reserved_pin_mask, gpio_mask);
}

uint64_t esp_gpio_revoke(uint64_t gpio_mask)
{
    return atomic_fetch_and(&s_reserved_pin_mask, ~gpio_mask);
}

bool esp_gpio_is_reserved(uint64_t gpio_mask)
{
    return atomic_load(&s_reserved_pin_mask) & gpio_mask;
}

// TODO: IDF-6968 reserve the pins that not fanned out regarding the SiP version
