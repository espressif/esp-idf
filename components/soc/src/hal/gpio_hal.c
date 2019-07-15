// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// The HAL layer for GPIO (common part)

#include "soc/soc.h"
#include "soc/gpio_periph.h"
#include "hal/gpio_hal.h"
#include "esp32/rom/gpio.h"

void gpio_hal_intr_enable_on_core(gpio_hal_context_t *hal, gpio_num_t gpio_num, uint32_t core_id)
{
    if (gpio_num < 32) {
        gpio_ll_clear_intr_status(hal->dev, BIT(gpio_num));
    } else {
        gpio_ll_clear_intr_status_high(hal->dev, BIT(gpio_num - 32));
    }
    gpio_ll_intr_enable_on_core(hal->dev, core_id, gpio_num);
}

void gpio_hal_intr_disable(gpio_hal_context_t *hal, gpio_num_t gpio_num)
{
    gpio_ll_intr_disable(hal->dev, gpio_num);
    if (gpio_num < 32) {
        gpio_ll_clear_intr_status(hal->dev, BIT(gpio_num));
    } else {
        gpio_ll_clear_intr_status_high(hal->dev, BIT(gpio_num - 32));
    }
}