/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdio.h>
#include "esp_err.h"
#include "soc/gpio_num.h"
#include "debug_probe_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Handle type of a debug probe unit
 */
typedef struct debug_probe_unit_t *debug_probe_unit_handle_t;

/**
 * @brief Handle type of a debug probe channel
 */
typedef struct debug_probe_channel_t *debug_probe_channel_handle_t;

/**
 * @brief Configuration for a debug probe unit
 */
typedef struct {
    gpio_num_t probe_out_gpio_nums[DEBUG_PROBE_MAX_OUTPUT_WIDTH]; ///< GPIO numbers for probe output
} debug_probe_unit_config_t;

/**
 * @brief Create a new debug probe unit
 *
 * @param[in] config Configuration for the debug probe unit
 * @param[out] out_handle Handle of the created debug probe unit
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the configuration is invalid
 *      - ESP_ERR_NOT_FOUND if there is no free unit slot
 *      - ESP_ERR_NO_MEM if memory allocation failed
 *      - ESP_FAIL if an internal error occurred
 */
esp_err_t debug_probe_new_unit(const debug_probe_unit_config_t *config, debug_probe_unit_handle_t *out_handle);

/**
 * @brief Delete a debug probe unit
 *
 * @param[in] unit Handle of the debug probe unit
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the handle is invalid
 *      - ESP_FAIL if an internal error occurred
 */
esp_err_t debug_probe_del_unit(debug_probe_unit_handle_t unit);

/**
 * @brief Configuration for a debug probe channel
 */
typedef struct {
    debug_probe_target_t target_module; ///< Target module of the debug probe channel
} debug_probe_channel_config_t;

/**
 * @brief Create a new debug probe channel in a unit
 *
 * @param[in] unit Handle of the debug probe unit
 * @param[in] config Configuration for the debug probe channel
 * @param[out] out_handle Handle of the created debug probe channel
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the handle or configuration is invalid
 *      - ESP_ERR_NOT_FOUND if there is no free channel slot
 *      - ESP_ERR_NO_MEM if memory allocation failed
 *      - ESP_FAIL if an internal error occurred
 */
esp_err_t debug_probe_new_channel(debug_probe_unit_handle_t unit, const debug_probe_channel_config_t *config, debug_probe_channel_handle_t *out_handle);

/**
 * @brief Delete a debug probe channel
 *
 * @param[in] chan Handle of the debug probe channel
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the handle is invalid
 *      - ESP_FAIL if an internal error occurred
 */
esp_err_t debug_probe_del_channel(debug_probe_channel_handle_t chan);

/**
 * @brief Add signals to a debug probe channel
 *
 * @note The n-th byte of channel output comes from the n-th byte in the sig_group.
 * @note The signals to be added must aligned to the byte boundary.
 *       byte_idx = 0: signal 0-7 in the group
 *       byte_idx = 1: signal 8-15 in the group
 *       ...
 * @note If you add the signals from different groups but with the same byte_idx, only the last added signal will be effective.
 * @note You can save up to 32 signals in a channel, but in the end, only the part of them (e.g. upper or lower 16 signals) can be output to the GPIO pads.
 *
 * @param[in] chan Handle of the debug probe channel
 * @param[in] byte_idx Byte index of the signals, ranges from 0 to 3
 * @param[in] sig_group Signal group of the signal, ranges from 0 to 15
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the parameters are invalid
 *      - ESP_FAIL if an internal error occurred
 */
esp_err_t debug_probe_chan_add_signal_by_byte(debug_probe_channel_handle_t chan, uint8_t byte_idx, uint8_t sig_group);

/**
 * @brief Merge the part of the channel output to the debug probe unit output
 *
 *           +----upper16---+
 * chan_0 ---+              |
 *           +----lower16---+
 *                          |
 *                          +-------unit_output[31:0]
 *                          |
 *           +----upper16---+
 * chan_1 ---+              |
 *           +----lower16---+
 *
 * @param[in] unit Handle of the debug probe unit
 * @param[in] chan0 Handle of the debug probe channel 0, whose output will be merged to the lower 16 signals of the unit output
 * @param[in] split_of_chan0 Part of the channel 0 output to be merged
 * @param[in] chan1 Handle of the debug probe channel 1, whose output will be merged to the upper 16 signals of the unit output
 * @param[in] split_of_chan1 Part of the channel 1 output to be merged
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the parameters are invalid
 *      - ESP_FAIL if an internal error occurred
 */
esp_err_t debug_probe_unit_merge16(debug_probe_unit_handle_t unit, debug_probe_channel_handle_t chan0, debug_probe_split_u16_t split_of_chan0,
                                   debug_probe_channel_handle_t chan1, debug_probe_split_u16_t split_of_chan1);

/**
 * @brief Read the value of the debug probe unit
 *
 * @note Only the lower 16 signals of the probe unit can be routed to the GPIO pads.
 *
 * @param[in] unit Handle of the debug probe unit
 * @param[out] value Current value of the debug probe unit output
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the handle or value is invalid
 *      - ESP_FAIL if an internal error occurred
 */
esp_err_t debug_probe_unit_read(debug_probe_unit_handle_t unit, uint32_t *value);

#ifdef __cplusplus
}
#endif
