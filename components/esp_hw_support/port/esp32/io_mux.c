/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/io_mux.h"

esp_err_t io_mux_set_clock_source(soc_module_clk_t clk_src)
{
    // IO MUX clock source is not selectable
    return ESP_OK;
}
