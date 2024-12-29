/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdio.h>
#include "esp_err.h"
#include "hal/ldo_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of LDO regulator channel handle
 */
typedef struct ldo_regulator_channel_t *esp_ldo_channel_handle_t;

/**
 * @brief LDO channel configurations
 */
typedef struct {
    int chan_id;      /*!< You must set the LDO channel ID according to the datasheet, e.g., set it to 1 for LDO_VO1 */
    int voltage_mv;   /*!< The voltage value to be set to the LDO channel */

    /// Extra flags of a LDO channel
    struct ldo_extra_flags {
        uint32_t adjustable : 1; /*!< Whether the LDO channel is adjustable, and the voltage can be updated by `esp_ldo_channel_adjust_voltage` */
        uint32_t owned_by_hw: 1; /*!< If the LDO channel is owned by hardware, then software configurations can be overridden by hardware (e.g. eFuse) */
        uint32_t bypass: 1;      /*!< Whether to bypass the regulator, i.e., the input voltage is sourced directly to the output */
    } flags; /*!< Flags for the LDO channel */
} esp_ldo_channel_config_t;

/**
 * @brief Acquire an LDO channel with the specified configuration
 *
 * @note This function can't automatically search a LDO channel for you, you must specify a LDO channel ID manually, based on your schematic.
 * @note The same channel can be acquired multiple times in different places of the application code, however,
 *       if the LDO channel is adjustable, you can't acquire it multiple times, in case user A changes the voltage and breaks the voltage setting of user B.
 * @note You should release the channel by `esp_ldo_release_channel` when it's no longer needed.
 *
 * @param[in] config The configuration of the LDO channel
 * @param[out] out_handle The returned LDO channel handle
 * @return
 *      - ESP_OK: Acquire the LDO channel successfully
 *      - ESP_ERR_INVALID_ARG: Acquire the LDO channel failed due to invalid arguments
 *      - ESP_FAIL: Acquire the LDO channel failed due to other reasons
 */
esp_err_t esp_ldo_acquire_channel(const esp_ldo_channel_config_t *config, esp_ldo_channel_handle_t *out_handle);

/**
 * @brief Release the LDO channel
 *
 * @param[in] chan The LDO channel handle returned from `esp_ldo_acquire_channel`
 * @return
 *      - ESP_OK: Release the LDO channel successfully
 *      - ESP_ERR_INVALID_ARG: Release the LDO channel failed due to invalid arguments
 *      - ESP_ERR_INVALID_STATE: Release the LDO channel failed due to invalid state, e.g., the channel handle is double released
 *      - ESP_FAIL: Release the LDO channel failed due to other reasons
 */
esp_err_t esp_ldo_release_channel(esp_ldo_channel_handle_t chan);

/**
 * @brief Adjust the voltage of the LDO channel
 *
 * @param[in] chan The LDO channel handle returned from `esp_ldo_acquire_channel`
 * @param[in] voltage_mv The voltage value to be set to the LDO channel, in millivolts
 * @return
 *      - ESP_OK: Adjust the voltage of the LDO channel successfully
 *      - ESP_ERR_INVALID_ARG: Adjust the voltage of the LDO channel failed due to invalid arguments
 *      - ESP_ERR_NOT_SUPPORTED: Adjust the voltage of the LDO channel failed due to the channel is not adjustable
 *      - ESP_FAIL: Adjust the voltage of the LDO channel failed due to other reasons
 */
esp_err_t esp_ldo_channel_adjust_voltage(esp_ldo_channel_handle_t chan, int voltage_mv);

/**
 * @brief Dump LDO channel status to the specified stream
 *
 * @param[in] stream IO stream. Can be stdout, stderr, or a file/string stream.
 * @return
 *      - ESP_OK: Dump the LDO channel status successfully
 *      - ESP_FAIL: Dump the LDO channel status failed
 */
esp_err_t esp_ldo_dump(FILE *stream);

#ifdef __cplusplus
}
#endif
