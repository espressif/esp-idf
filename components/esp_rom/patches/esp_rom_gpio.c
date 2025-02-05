/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_rom_gpio.h"
#include "soc/gpio_reg.h"

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
// On such targets, the ROM code for this function enabled output for the pad first, and then connected the signal
// This could result in an undesired glitch at the pad
IRAM_ATTR void esp_rom_gpio_connect_out_signal(uint32_t gpio_num, uint32_t signal_idx, bool out_inv, bool oen_inv)
{
    uint32_t value = signal_idx << GPIO_FUNC0_OUT_SEL_S;
    if (out_inv) {
        value |= GPIO_FUNC0_OUT_INV_SEL_M;
    }
    if (oen_inv) {
        value |= GPIO_FUNC0_OEN_INV_SEL_M;
    }
    REG_WRITE(GPIO_FUNC0_OUT_SEL_CFG_REG + (gpio_num * 4), value);

    if (gpio_num < 32) {
        REG_WRITE(GPIO_ENABLE_W1TS_REG, (1 << gpio_num));
    } else {
        REG_WRITE(GPIO_ENABLE1_W1TS_REG, (1 << (gpio_num - 32)));
    }
}
#endif
