/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "ble_cte_antenna.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_rom_gpio.h"
#include "soc/gpio_sig_map.h"

static const char *tag = "CTE_ANTENNA";

#ifndef CTE_ANT0_IDX
#define CTE_ANT0_IDX ANT_SEL0_IDX
#define CTE_ANT1_IDX ANT_SEL1_IDX
#define CTE_ANT2_IDX ANT_SEL2_IDX
#define CTE_ANT3_IDX ANT_SEL3_IDX
#endif

static const uint8_t antenna_signal_index[] = {
    CTE_ANT0_IDX, CTE_ANT1_IDX, CTE_ANT2_IDX, CTE_ANT3_IDX,
};

esp_err_t ble_cte_antenna_init(const uint8_t *gpios, uint8_t count)
{
    uint64_t gpio_pin_mask = 0;

    if (!gpios || count > 4 || count == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    for (int i = 0; i < count; i++) {
        gpio_pin_mask |= (1ULL << gpios[i]);
    }
    gpio_config_t gpio_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = gpio_pin_mask,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    esp_err_t ret = gpio_config(&gpio_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Antenna GPIO configuration failed: %d", ret);
        return ret;
    }
    for (int i = 0; i < count; i++) {
        ESP_LOGI(tag, "Bind antenna GPIO %d to signal %d", gpios[i], antenna_signal_index[i]);
        esp_rom_gpio_connect_out_signal(gpios[i], antenna_signal_index[i], 0, 0);
    }
    return ESP_OK;
}
