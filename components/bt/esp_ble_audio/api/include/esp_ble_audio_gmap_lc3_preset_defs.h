/*
 * SPDX-FileCopyrightText: 2023-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_GMAP_LC3_PRESET_DEFS_H_
#define ESP_BLE_AUDIO_GMAP_LC3_PRESET_DEFS_H_

#include <zephyr/bluetooth/audio/gmap_lc3_preset.h>

#include "esp_ble_audio_bap_lc3_preset_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Base macro for defining GMAP LC3 presets.
 *
 * @param   _name           Preset name.
 * @param   _freq           Sampling frequency (16, 32, or 48 kHz).
 * @param   _duration       Frame duration (7.5 or 10 ms).
 * @param   _loc            Audio channel location.
 * @param   _len            Frame length in bytes.
 * @param   _frames_per_sdu Number of frames per SDU.
 * @param   _stream_context Stream context type.
 * @param   _interval       Interval between packets in microseconds.
 * @param   _sdu            SDU size in bytes.
 * @param   _rtn            Maximum number of retransmissions.
 * @param   _latency        Maximum latency in milliseconds.
 * @param   _pd             Presentation delay in microseconds.
 */
#define ESP_BLE_AUDIO_GMAP_LC3_PRESET_DEFINE(_name, _freq, _duration, _loc, _len, \
                                             _frames_per_sdu, _stream_context, \
                                             _interval, _sdu, _rtn, _latency, _pd) \
    static uint8_t codec_cfg_data_##_name[] = \
        ESP_BLE_AUDIO_CODEC_CFG_LC3_DATA(_freq, _duration, _loc, _len, _frames_per_sdu); \
    static uint8_t codec_cfg_meta_##_name[] = \
        ESP_BLE_AUDIO_CODEC_CFG_LC3_META(_stream_context); \
    static esp_ble_audio_bap_lc3_preset_t _name = \
        ESP_BLE_AUDIO_BAP_LC3_PRESET(ESP_BLE_AUDIO_CODEC_CFG_LC3(codec_cfg_data_##_name, \
                                                                 codec_cfg_meta_##_name), \
                                     ESP_BLE_AUDIO_BAP_QOS_CFG_UNFRAMED(_interval, _sdu, _rtn, \
                                                                        _latency, _pd));

/**
 * @brief   Gaming Reliable (GR) preset at 32kHz, 7.5ms frame duration.
 *
 * Optimized for reliable gaming audio with low latency (15ms) and standard presentation delay (10ms).
 *
 * @param   _name           Preset name.
 * @param   _loc            Audio channel location.
 * @param   _stream_context Stream context type.
 */
#define ESP_BLE_AUDIO_GMAP_LC3_PRESET_32_1_GR_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_GMAP_LC3_PRESET_DEFINE(_name, \
                                         BT_AUDIO_CODEC_CFG_FREQ_32KHZ, \
                                         BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                         _loc, 60U, 1, \
                                         _stream_context, \
                                         7500u, 60U, 1U, 15U, 10000U)

/**
 * @brief   Gaming Reliable (GR) preset at 32kHz, 10ms frame duration.
 *
 * Higher quality variant with increased frame duration for better audio quality.
 *
 * @param   _name           Preset name.
 * @param   _loc            Audio channel location.
 * @param   _stream_context Stream context type.
 */
#define ESP_BLE_AUDIO_GMAP_LC3_PRESET_32_2_GR_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_GMAP_LC3_PRESET_DEFINE(_name, \
                                         BT_AUDIO_CODEC_CFG_FREQ_32KHZ, \
                                         BT_AUDIO_CODEC_CFG_DURATION_10, \
                                         _loc, 80U, 1, \
                                         _stream_context, \
                                         10000u, 80U, 1U, 20U, 10000U)

/**
 * @brief   Gaming Reliable (GR) preset at 48kHz, 7.5ms frame duration.
 *
 * High-quality gaming audio with minimal latency.
 *
 * @param   _name           Preset name.
 * @param   _loc            Audio channel location.
 * @param   _stream_context Stream context type.
 */
#define ESP_BLE_AUDIO_GMAP_LC3_PRESET_48_1_GR_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_GMAP_LC3_PRESET_DEFINE(_name, \
                                         BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                         BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                         _loc, 75U, 1, \
                                         _stream_context, \
                                         7500u, 75U, 1U, 15U, 10000U)

/**
 * @brief   Gaming Reliable (GR) preset at 48kHz, 10ms frame duration.
 *
 * High-quality gaming audio with balanced latency and quality.
 *
 * @param   _name           Preset name.
 * @param   _loc            Audio channel location.
 * @param   _stream_context Stream context type.
 */
#define ESP_BLE_AUDIO_GMAP_LC3_PRESET_48_2_GR_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_GMAP_LC3_PRESET_DEFINE(_name, \
                                         BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                         BT_AUDIO_CODEC_CFG_DURATION_10, \
                                         _loc, 100U, 1, \
                                         _stream_context, \
                                         10000u, 100U, 1U, 20U, 10000U)

/**
 * @brief   Gaming Reliable (GR) preset at 48kHz, 7.5ms frame duration, higher bitrate.
 *
 * Enhanced quality gaming audio with minimal latency.
 *
 * @param   _name           Preset name.
 * @param   _loc            Audio channel location.
 * @param   _stream_context Stream context type.
 */
#define ESP_BLE_AUDIO_GMAP_LC3_PRESET_48_3_GR_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_GMAP_LC3_PRESET_DEFINE(_name, \
                                         BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                         BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                         _loc, 90U, 1, \
                                         _stream_context, \
                                         7500u, 90U, 1U, 15U, 10000U)

/**
 * @brief   Gaming Reliable (GR) preset at 48kHz, 10ms frame duration, higher bitrate.
 *
 * Premium quality gaming audio with balanced latency.
 *
 * @param   _name           Preset name.
 * @param   _loc            Audio channel location.
 * @param   _stream_context Stream context type.
 */
#define ESP_BLE_AUDIO_GMAP_LC3_PRESET_48_4_GR_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_GMAP_LC3_PRESET_DEFINE(_name, \
                                         BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                         BT_AUDIO_CODEC_CFG_DURATION_10, \
                                         _loc, 120u, 1, \
                                         _stream_context, \
                                         10000u, 120U, 1U, 20U, 10000U)

/**
 * @brief   Gaming Standard (GS) preset at 16kHz, 7.5ms frame duration.
 *
 * Basic gaming audio with longer presentation delay (60ms).
 *
 * @param   _name           Preset name.
 * @param   _loc            Audio channel location.
 * @param   _stream_context Stream context type.
 */
#define ESP_BLE_AUDIO_GMAP_LC3_PRESET_16_1_GS_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_GMAP_LC3_PRESET_DEFINE(_name, \
                                         BT_AUDIO_CODEC_CFG_FREQ_16KHZ, \
                                         BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                         _loc, 30U, 1, \
                                         _stream_context, \
                                         7500u, 30U, 1U, 15U, 60000U)

/**
 * @brief   Gaming Standard (GS) preset at 16kHz, 10ms frame duration.
 *
 * Basic gaming audio with better quality and longer presentation delay.
 *
 * @param   _name           Preset name.
 * @param   _loc            Audio channel location.
 * @param   _stream_context Stream context type.
 */
#define ESP_BLE_AUDIO_GMAP_LC3_PRESET_16_2_GS_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_GMAP_LC3_PRESET_DEFINE(_name, \
                                         BT_AUDIO_CODEC_CFG_FREQ_16KHZ, \
                                         BT_AUDIO_CODEC_CFG_DURATION_10, \
                                         _loc, 40U, 1, \
                                         _stream_context, \
                                         10000u, 40U, 1U, 20U, 60000U)

/**
 * @brief   Gaming Standard (GS) preset at 32kHz, 7.5ms frame duration.
 *
 * Medium quality gaming audio with standard latency.
 *
 * @param   _name           Preset name.
 * @param   _loc            Audio channel location.
 * @param   _stream_context Stream context type.
 */
#define ESP_BLE_AUDIO_GMAP_LC3_PRESET_32_1_GS_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_GMAP_LC3_PRESET_DEFINE(_name, \
                                         BT_AUDIO_CODEC_CFG_FREQ_32KHZ, \
                                         BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                         _loc, 60U, 1, \
                                         _stream_context, \
                                         7500u, 60U, 1U, 15U, 60000U)

/**
 * @brief   Gaming Standard (GS) preset at 32kHz, 10ms frame duration.
 *
 * Enhanced medium quality gaming audio.
 *
 * @param   _name           Preset name.
 * @param   _loc            Audio channel location.
 * @param   _stream_context Stream context type.
 */
#define ESP_BLE_AUDIO_GMAP_LC3_PRESET_32_2_GS_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_GMAP_LC3_PRESET_DEFINE(_name, \
                                         BT_AUDIO_CODEC_CFG_FREQ_32KHZ, \
                                         BT_AUDIO_CODEC_CFG_DURATION_10, \
                                         _loc, 80U, 1, \
                                         _stream_context, \
                                         10000u, 80U, 1U, 20U, 60000U)

/**
 * @brief   Gaming Standard (GS) preset at 48kHz, 7.5ms frame duration.
 *
 * High quality gaming audio with standard latency.
 *
 * @param   _name           Preset name.
 * @param   _loc            Audio channel location.
 * @param   _stream_context Stream context type.
 */
#define ESP_BLE_AUDIO_GMAP_LC3_PRESET_48_1_GS_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_GMAP_LC3_PRESET_DEFINE(_name, \
                                         BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                         BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                         _loc, 75U, 1, \
                                         _stream_context, \
                                         7500u, 75U, 1U, 15U, 60000U)

/**
 * @brief   Gaming Standard (GS) preset at 48kHz, 10ms frame duration.
 *
 * Premium quality gaming audio with standard latency.
 *
 * @param   _name           Preset name.
 * @param   _loc            Audio channel location.
 * @param   _stream_context Stream context type.
 */
#define ESP_BLE_AUDIO_GMAP_LC3_PRESET_48_2_GS_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_GMAP_LC3_PRESET_DEFINE(_name, \
                                         BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                         BT_AUDIO_CODEC_CFG_DURATION_10, \
                                         _loc, 100U, 1, \
                                         _stream_context, \
                                         10000u, 100U, 1U, 20U, 60000U)

/**
 * @brief   Generic Gaming (G) preset at 48kHz, 7.5ms frame duration.
 *
 * High quality gaming audio with ultra-low latency (8ms).
 *
 * @param   _name           Preset name.
 * @param   _loc            Audio channel location.
 * @param   _stream_context Stream context type.
 */
#define ESP_BLE_AUDIO_GMAP_LC3_PRESET_48_1_G_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_GMAP_LC3_PRESET_DEFINE(_name, \
                                         BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                         BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                         _loc, 75U, 1, \
                                         _stream_context, \
                                         7500u, 75U, 1U, 8U, 10000U)

/**
 * @brief   Generic Gaming (G) preset at 48kHz, 10ms frame duration.
 *
 * High quality gaming audio with very low latency (10ms).
 *
 * @param   _name           Preset name.
 * @param   _loc            Audio channel location.
 * @param   _stream_context Stream context type.
 */
#define ESP_BLE_AUDIO_GMAP_LC3_PRESET_48_2_G_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_GMAP_LC3_PRESET_DEFINE(_name, \
                                         BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                         BT_AUDIO_CODEC_CFG_DURATION_10, \
                                         _loc, 100U, 1, \
                                         _stream_context, \
                                         10000u, 100U, 1U, 10U, 10000U)

/**
 * @brief   Generic Gaming (G) preset at 48kHz, 7.5ms frame duration, higher bitrate.
 *
 * Premium quality gaming audio with ultra-low latency.
 *
 * @param   _name           Preset name.
 * @param   _loc            Audio channel location.
 * @param   _stream_context Stream context type.
 */
#define ESP_BLE_AUDIO_GMAP_LC3_PRESET_48_3_G_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_GMAP_LC3_PRESET_DEFINE(_name, \
                                         BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                         BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                         _loc, 90U, 1, \
                                         _stream_context, \
                                         7500u, 90U, 1U, 8U, 10000U)

/**
 * @brief   Generic Gaming (G) preset at 48kHz, 10ms frame duration, higher bitrate.
 *
 * Highest quality gaming audio with very low latency.
 *
 * @param   _name           Preset name.
 * @param   _loc            Audio channel location.
 * @param   _stream_context Stream context type.
 */
#define ESP_BLE_AUDIO_GMAP_LC3_PRESET_48_4_G_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_GMAP_LC3_PRESET_DEFINE(_name, \
                                         BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                         BT_AUDIO_CODEC_CFG_DURATION_10, \
                                         _loc, 120u, 1, \
                                         _stream_context, \
                                         10000u, 120U, 1U, 10U, 10000U)

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_GMAP_LC3_PRESET_DEFS_H_ */
