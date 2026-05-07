/*
 * SPDX-FileCopyrightText: 2020 Intel Corporation
 * SPDX-FileCopyrightText: 2022-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_LC3_DEFS_H_
#define ESP_BLE_AUDIO_LC3_DEFS_H_

#include <zephyr/bluetooth/audio/lc3.h>

#include "esp_ble_audio_codec_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Helper to declare LC3 codec capability.
 *
 * @param   _freq               Supported Sampling Frequencies bitfield.
 * @param   _duration           Supported Frame Durations bitfield.
 * @param   _chan_count         Supported channels.
 * @param   _len_min            Minimum number of octets supported per codec frame.
 * @param   _len_max            Maximum number of octets supported per codec frame.
 * @param   _max_frames_per_sdu Supported maximum codec frames per SDU.
 *                              Optional and will be included only if != 1.
 */
#define ESP_BLE_AUDIO_CODEC_CAP_LC3_DATA(_freq, _duration, _chan_count, \
                                         _len_min, _len_max, \
                                         _max_frames_per_sdu) \
    BT_AUDIO_CODEC_CAP_LC3_DATA(_freq, _duration, _chan_count, \
                                _len_min, _len_max, \
                                _max_frames_per_sdu)

/**
 * @brief   Helper to declare LC3 codec metadata.
 *
 * @param   _prefer_context Preferred contexts.
 */
#define ESP_BLE_AUDIO_CODEC_CAP_LC3_META(_prefer_context) \
    BT_AUDIO_CODEC_CAP_LC3_META(_prefer_context)

/**
 * @brief   Helper to declare LC3 codec capability.
 *
 * @param   _data   LC3 codec capability data.
 * @param   _meta   LC3 codec capability metadata.
 */
#define ESP_BLE_AUDIO_CODEC_CAP_LC3(_data, _meta) \
    ESP_BLE_AUDIO_CODEC_CAP(ESP_BLE_ISO_CODING_FORMAT_LC3, 0x0000, 0x0000, _data, _meta)

/**
 * @brief   Helper to declare LC3 codec data configuration.
 *
 * @param   _freq           Sampling frequency.
 * @param   _duration       Frame duration.
 * @param   _loc            Audio channel location bitfield.
 * @param   _len            Octets per frame (16-bit integer).
 * @param   _frames_per_sdu Frames per SDU (8-bit integer). This value is
 *                          optional and will be included only if != 1.
 */
#define ESP_BLE_AUDIO_CODEC_CFG_LC3_DATA(_freq, _duration, _loc, _len, _frames_per_sdu) \
    BT_AUDIO_CODEC_CFG_LC3_DATA(_freq, _duration, _loc, _len, _frames_per_sdu)

/**
 * @brief   Helper to declare LC3 codec metadata configuration.
 *
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_CODEC_CFG_LC3_META(_stream_context) \
    BT_AUDIO_CODEC_CFG_LC3_META(_stream_context)

/**
 * @brief   Helper to declare LC3 codec configuration.
 *
 * @param   _data   LC3 codec configuration data.
 * @param   _meta   LC3 codec configuration metadata.
 */
#define ESP_BLE_AUDIO_CODEC_CFG_LC3(_data, _meta) \
    ESP_BLE_AUDIO_CODEC_CFG(ESP_BLE_ISO_CODING_FORMAT_LC3, 0x0000, 0x0000, _data, _meta)

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_LC3_DEFS_H_ */
