/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for GPIO (common part)

#include "soc/soc.h"
#include "esp_attr.h"
#include "hal/gpio_hal.h"
#include "esp_rom_gpio.h"
#include "hal/config.h"

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

void gpio_hal_iomux_in(gpio_hal_context_t *hal, uint32_t gpio_num, int func, uint32_t signal_idx)
{
    gpio_ll_set_input_signal_from(hal->dev, signal_idx, false);
    gpio_ll_input_enable(hal->dev, gpio_num);
    gpio_ll_func_sel(hal->dev, gpio_num, func);
}

void gpio_hal_iomux_out(gpio_hal_context_t *hal, uint32_t gpio_num, int func)
{
    gpio_ll_func_sel(hal->dev, gpio_num, func);
    // as long as the func sel is not GPIO, the oe can only be controlled by the peripheral
}

void gpio_hal_matrix_in(gpio_hal_context_t *hal, uint32_t gpio_num, uint32_t signal_idx, bool in_inv)
{
    if (gpio_num < GPIO_NUM_MAX) {  // skip const_0/1 io num from enabling input
        gpio_ll_input_enable(hal->dev, gpio_num);
    }
#if HAL_CONFIG(GPIO_USE_ROM_API)
    esp_rom_gpio_connect_in_signal(gpio_num, signal_idx, in_inv);
#else
    gpio_ll_set_input_signal_matrix_source(hal->dev, signal_idx, gpio_num, in_inv);
#endif
}

void gpio_hal_matrix_out(gpio_hal_context_t *hal, uint32_t gpio_num, uint32_t signal_idx, bool out_inv, bool oen_inv)
{
    gpio_ll_func_sel(hal->dev, gpio_num, PIN_FUNC_GPIO);
#if HAL_CONFIG(GPIO_USE_ROM_API)
    esp_rom_gpio_connect_out_signal(gpio_num, signal_idx, out_inv, oen_inv);
#else
    gpio_ll_set_output_signal_matrix_source(hal->dev, gpio_num, signal_idx, out_inv);
    gpio_ll_set_output_enable_ctrl(hal->dev, gpio_num, true, oen_inv); // output is enabled at the end to avoid undesired level change
#endif
}

#if SOC_GPIO_SUPPORT_PIN_HYS_FILTER
void gpio_hal_hysteresis_soft_enable(gpio_hal_context_t *hal, uint32_t gpio_num, bool enable)
{
#if SOC_GPIO_SUPPORT_PIN_HYS_CTRL_BY_EFUSE
    gpio_ll_pin_input_hysteresis_ctrl_sel_soft(hal->dev, gpio_num);
#endif

    if (enable) {
        gpio_ll_pin_input_hysteresis_enable(hal->dev, gpio_num);
    } else {
        gpio_ll_pin_input_hysteresis_disable(hal->dev, gpio_num);
    }
}
#endif //SOC_GPIO_SUPPORT_PIN_HYS_FILTER

void gpio_hal_isolate_in_sleep(gpio_hal_context_t *hal, uint32_t gpio_num)
{
    gpio_ll_sleep_input_disable(hal->dev, gpio_num);
    gpio_ll_sleep_output_disable(hal->dev, gpio_num);
    gpio_ll_sleep_pullup_dis(hal->dev, gpio_num);
    gpio_ll_sleep_pulldown_dis(hal->dev, gpio_num);
    gpio_ll_sleep_sel_en(hal->dev, gpio_num);
}
