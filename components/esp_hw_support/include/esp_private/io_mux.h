/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "soc/clk_tree_defs.h"
#include "soc/gpio_num.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Acquire the clock source for IO MUX
 *
 * @note IO MUX clock is shared by submodules like SDM, Glitch Filter.
 *       Each consumer should call this function when starting to use a specific clock source,
 *       and call @ref io_mux_release_clock_source when done.
 *
 * @param clk_src The clock source for IO MUX
 * @return
 *      - ESP_OK: Success
 *      - ESP_ERR_INVALID_STATE: The IO MUX has been acquired with another clock source
 */
esp_err_t io_mux_acquire_clock_source(soc_module_clk_t clk_src);

/**
 * @brief Release the clock source for IO MUX
 *
 * When the last consumer releases, the clock source will be switched back to SOC_MOD_CLK_XTAL.
 *
 * @param clk_src The clock source to release (must match the one used in acquire)
 * @return
 *      - ESP_OK: Success
 *      - ESP_ERR_INVALID_STATE: Reference count is zero or clock source mismatch
 */
esp_err_t io_mux_release_clock_source(soc_module_clk_t clk_src);

#if SOC_LP_IO_CLOCK_IS_INDEPENDENT
typedef struct {
    uint8_t rtc_io_enabled_cnt[SOC_RTCIO_PIN_COUNT];
    uint32_t rtc_io_using_mask : SOC_RTCIO_PIN_COUNT;
} rtc_io_status_t;

/**
 * @brief Enable/Disable LP_IO peripheral clock
 *
 * @param gpio_num GPIO number
 * @param enable   true to enable the clock / false to disable the clock
 */
void io_mux_enable_lp_io_clock(gpio_num_t gpio_num, bool enable);

/**
 * @brief Force disable one LP_IO to clock dependency
 *
 * @param gpio_num GPIO number
 */
void io_mux_force_disable_lp_io_clock(gpio_num_t gpio_num);
#endif

#if SOC_RTCIO_PIN_COUNT > 0
/**
 * @brief Check if the LP_IO is in use
 *
 * @param gpio_num GPIO number
 *
 * @return true if the LP_IO is in use, false otherwise
 */
bool io_mux_is_lp_io_in_use(gpio_num_t gpio_num);
#endif

#ifdef __cplusplus
}
#endif
