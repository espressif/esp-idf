/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/touch_sens_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Touch channel information
 *
 */
typedef struct {
    int chan_id;                    /*!< Touch channel number */
    int chan_gpio;                  /*!< Corresponding GPIO of this channel */
    struct {
        uint32_t can_wake_dp_slp: 1;/*!< Whether this channel can wakeup from deep sleep */
        uint32_t is_proxi: 1;       /*!< Whether this channel is used for proximity sensing */
        uint32_t is_guard: 1;       /*!< Whether this channel is used for waterproof guard channel */
        uint32_t is_shield: 1;      /*!< Whether this channel is used for waterproof shield channel */
    } flags;                        /*!< Channel sub-feature flags */
} touch_chan_info_t;

/**
 * @brief Get the touch channel information by the channel handle
 *
 * @param[in]  chan_handle  Touch channel handle
 * @param[out] chan_info    Touch channel information
 * @return
 *      - ESP_OK:               Success to get the channel information
 *      - ESP_ERR_INVALID_ARG:  NULL pointer
 */
esp_err_t touch_sensor_get_channel_info(touch_channel_handle_t chan_handle, touch_chan_info_t *chan_info);

#ifdef __cplusplus
}
#endif
