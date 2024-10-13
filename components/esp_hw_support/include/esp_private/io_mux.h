/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "soc/clk_tree_defs.h"
#include "soc/gpio_num.h"
#include "soc/soc_caps.h"
#include "soc/io_mux_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the clock source for IO MUX
 *
 * @note IO MUX clock is shared by submodules like SDM, Glitch Filter.
 *       The submodule drivers should call this function to detect if the user set the clock differently.
 *
 * @param clk_src The clock source for IO MUX
 * @return
 *      - ESP_OK: Success
 *      - ESP_ERR_INVALID_STATE: The IO MUX has been set to another clock source
 */
esp_err_t io_mux_set_clock_source(soc_module_clk_t clk_src);

#if SOC_LP_IO_CLOCK_IS_INDEPENDENT
typedef struct {
    uint8_t rtc_io_enabled_cnt[MAX_RTC_GPIO_NUM + 1];
    uint32_t rtc_io_using_mask;
} rtc_io_status_t;

/**
 * @brief Enable/Disable LP_IO peripheral clock
 *
 * @param gpio_num GPIO number
 * @param enable   true to enable the clock / false to disable the clock
 */
void io_mux_enable_lp_io_clock(gpio_num_t gpio_num, bool enable);
#endif

#ifdef __cplusplus
}
#endif
