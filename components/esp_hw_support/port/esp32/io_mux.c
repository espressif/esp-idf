/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/io_mux.h"

bool io_mux_is_lp_io_in_use(gpio_num_t gpio_num)
{
    // ESP32 does not have SOC_LP_IO_CLOCK_IS_INDEPENDENT
    // which means we don't need LP_IO clock management
    // So we just always return true here
    return true;
}
