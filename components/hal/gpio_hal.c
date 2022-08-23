/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for GPIO (common part)

#include "soc/soc.h"
#include "soc/gpio_periph.h"
#include "hal/gpio_hal.h"

void gpio_hal_intr_enable_on_core(gpio_hal_context_t *hal, uint32_t gpio_num, uint32_t core_id)
{
    if (gpio_num < 32) {
        gpio_ll_clear_intr_status(hal->dev, BIT(gpio_num));
    } else {
        gpio_ll_clear_intr_status_high(hal->dev, BIT(gpio_num - 32));
    }
    gpio_ll_intr_enable_on_core(hal->dev, core_id, gpio_num);
}

void gpio_hal_intr_disable(gpio_hal_context_t *hal, uint32_t gpio_num)
{
    gpio_ll_intr_disable(hal->dev, gpio_num);
    if (gpio_num < 32) {
        gpio_ll_clear_intr_status(hal->dev, BIT(gpio_num));
    } else {
        gpio_ll_clear_intr_status_high(hal->dev, BIT(gpio_num - 32));
    }
}
