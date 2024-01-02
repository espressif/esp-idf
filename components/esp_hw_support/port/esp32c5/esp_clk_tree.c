/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_clk_tree.h"
#include "esp_err.h"
#include "esp_check.h"
#include "soc/rtc.h"
#include "hal/clk_tree_hal.h"
#include "hal/clk_tree_ll.h"
#include "esp_private/esp_clk_tree_common.h"

static const char *TAG = "esp_clk_tree";

esp_err_t esp_clk_tree_src_get_freq_hz(soc_module_clk_t clk_src, esp_clk_tree_src_freq_precision_t precision,
uint32_t *freq_value)
{
    // TODO: [ESP32C5] IDF-8642
    ESP_LOGW(TAG, "esp_clk_tree_src_get_freq_hz() has not implemented yet");
    *freq_value = 0;
    return ESP_OK;
}
