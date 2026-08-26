/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "hal/glitch_filter_types.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Typedef of GPIO glitch filter handle
 */
typedef struct gpio_glitch_filter_t *gpio_glitch_filter_handle_t;

/**
 * @brief Configuration of GPIO pin glitch filter
 */
typedef struct {
    glitch_filter_clock_source_t clk_src; /*!< Clock source for the glitch filter */
    gpio_num_t gpio_num;    /*!< GPIO number */
} gpio_pin_glitch_filter_config_t;

/**
 * @brief Create a pin glitch filter
 *
 * @note Pin glitch filter parameters are fixed, pulses shorter than two sample clocks (IO-MUX's source clock) will be filtered out.
 *       It's independent with "flex" glitch filter. See also `gpio_new_flex_glitch_filter`.
 * @note The created filter handle can later be deleted by `gpio_del_glitch_filter`.
 *
 * @param[in] config Glitch filter configuration
 * @param[out] ret_filter Returned glitch filter handle
 * @return
 *      - ESP_OK: Create a pin glitch filter successfully
 *      - ESP_ERR_INVALID_ARG: Create a pin glitch filter failed because of invalid arguments (e.g. wrong GPIO number)
 *      - ESP_ERR_NO_MEM: Create a pin glitch filter failed because of out of memory
 *      - ESP_FAIL: Create a pin glitch filter failed because of other error
 */
esp_err_t gpio_new_pin_glitch_filter(const gpio_pin_glitch_filter_config_t *config, gpio_glitch_filter_handle_t *ret_filter);

/**
 * @brief Configuration of GPIO flex glitch filter
 */
typedef struct {
    glitch_filter_clock_source_t clk_src; /*!< Clock source for the glitch filter */
    gpio_num_t gpio_num;      /*!< GPIO number */
    uint32_t window_width_ns; /*!< Sample window width (in ns) */
    uint32_t window_thres_ns; /*!< Sample window threshold (in ns), during the `window_width_ns` sample window, any pulse whose width < window_thres_ns will be discarded. */
} gpio_flex_glitch_filter_config_t;

/**
 * @brief Allocate a flex glitch filter
 *
 * @note "flex" means the filter parameters (window, threshold) are adjustable. It's independent with pin glitch filter.
 *       See also `gpio_new_pin_glitch_filter`.
 * @note The created filter handle can later be deleted by `gpio_del_glitch_filter`.
 *
 * @param[in] config Glitch filter configuration
 * @param[out] ret_filter Returned glitch filter handle
 * @return
 *      - ESP_OK: Allocate a flex glitch filter successfully
 *      - ESP_ERR_INVALID_ARG: Allocate a flex glitch filter failed because of invalid arguments (e.g. wrong GPIO number, filter parameters out of range)
 *      - ESP_ERR_NO_MEM: Allocate a flex glitch filter failed because of out of memory
 *      - ESP_ERR_NOT_FOUND: Allocate a flex glitch filter failed because the underlying hardware resources are used up
 *      - ESP_FAIL: Allocate a flex glitch filter failed because of other error
 */
esp_err_t gpio_new_flex_glitch_filter(const gpio_flex_glitch_filter_config_t *config, gpio_glitch_filter_handle_t *ret_filter);

/**
 * @brief Delete a glitch filter
 *
 * @param[in] filter Glitch filter handle returned from `gpio_new_flex_glitch_filter` or `gpio_new_pin_glitch_filter`
 * @return
 *      - ESP_OK: Delete glitch filter successfully
 *      - ESP_ERR_INVALID_ARG: Delete glitch filter failed because of invalid arguments
 *      - ESP_ERR_INVALID_STATE: Delete glitch filter failed because the glitch filter is still in working
 *      - ESP_FAIL: Delete glitch filter failed because of other error
 */
esp_err_t gpio_del_glitch_filter(gpio_glitch_filter_handle_t filter);

/**
 * @brief Enable a glitch filter
 *
 * @param[in] filter Glitch filter handle returned from `gpio_new_flex_glitch_filter` or `gpio_new_pin_glitch_filter`
 * @return
 *      - ESP_OK: Enable glitch filter successfully
 *      - ESP_ERR_INVALID_ARG: Enable glitch filter failed because of invalid arguments
 *      - ESP_ERR_INVALID_STATE: Enable glitch filter failed because the glitch filter is already enabled
 *      - ESP_FAIL: Enable glitch filter failed because of other error
 */
esp_err_t gpio_glitch_filter_enable(gpio_glitch_filter_handle_t filter);

/**
 * @brief Disable a glitch filter
 *
 * @param[in] filter Glitch filter handle returned from `gpio_new_flex_glitch_filter` or `gpio_new_pin_glitch_filter`
 * @return
 *      - ESP_OK: Disable glitch filter successfully
 *      - ESP_ERR_INVALID_ARG: Disable glitch filter failed because of invalid arguments
 *      - ESP_ERR_INVALID_STATE: Disable glitch filter failed because the glitch filter is not enabled yet
 *      - ESP_FAIL: Disable glitch filter failed because of other error
 */
esp_err_t gpio_glitch_filter_disable(gpio_glitch_filter_handle_t filter);

#ifdef __cplusplus
}
#endif
