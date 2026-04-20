/*
 * SPDX-FileCopyrightText: 2023-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFS_H_
#define ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFS_H_

#include "sdkconfig.h"

#include <zephyr/bluetooth/audio/bap_lc3_preset.h>

#include "esp_ble_audio_lc3_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Struct to hold a BAP defined LC3 preset */
typedef struct bt_bap_lc3_preset    esp_ble_audio_bap_lc3_preset_t;

/*!< Helper to declare an LC3 preset structure */
#define ESP_BLE_AUDIO_BAP_LC3_PRESET(_codec, _qos) \
    BT_BAP_LC3_PRESET(_codec, _qos)

/**
 * @brief   Define an LC3 preset configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _freq           Sample frequency (in Hz).
 * @param   _duration       Frame duration (in microseconds).
 * @param   _loc            Audio channel location.
 * @param   _len            Frame length in octets.
 * @param   _frames_per_sdu Number of frames per SDU.
 * @param   _stream_context Stream context (e.g., Media, Conversational, etc.).
 * @param   _interval       Audio frame interval (in microseconds).
 * @param   _sdu            SDU size in octets.
 * @param   _rtn            Number of retransmissions.
 * @param   _latency        Maximum transport latency (in milliseconds).
 * @param   _pd             Presentation delay (in microseconds).
 */
#define ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, _freq, _duration, _loc, _len, \
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
 * @brief   Helper to declare LC3 Unicast 8_1_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_8_1_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_8KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 26u, 1, \
                                        _stream_context, \
                                        7500u, 26u, 2u, 8u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 8_2_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_8_2_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_8KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 30U, 1, \
                                        _stream_context, \
                                        10000u, 30u, 2u, 10u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 16_1_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_16_1_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_16KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 30U, 1, \
                                        _stream_context, \
                                        7500u, 30u, 2u, 8u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 16_2_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_16_2_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_16KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 40U, 1, \
                                        _stream_context, \
                                        10000u, 40u, 2u, 10u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 24_1_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_24_1_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_24KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 45U, 1, \
                                        _stream_context, \
                                        7500u, 45u, 2u, 8u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 24_2_1 codec configuration.
 *
 * Mandatory to support as unicast server.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_24_2_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_24KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 60U, 1, \
                                        _stream_context, \
                                        10000u, 60u, 2u, 10u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 32_1_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_32_1_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_32KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 60U, 1, \
                                        _stream_context, \
                                        7500u, 60u, 2u, 8u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 32_2_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_32_2_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_32KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 80U, 1, \
                                        _stream_context, \
                                        10000u, 80u, 2u, 10u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 441_1_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_441_1_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_44KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 97U, 1, \
                                        _stream_context, \
                                        8163u, 97u, 5u, 24u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 441_2_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_441_2_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_44KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 130U, 1, \
                                        _stream_context, \
                                        10884u, 130u, 5u, 31u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 48_1_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_48_1_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 75U, 1, \
                                        _stream_context, \
                                        7500u, 75u, 5u, 15u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 48_2_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_48_2_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 100U, 1, \
                                        _stream_context, \
                                        10000u, 100u, 5u, 20u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 48_3_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_48_3_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 90U, 1, \
                                        _stream_context, \
                                        7500u, 90u, 5u, 15u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 48_4_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_48_4_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 120u, 1, \
                                        _stream_context, \
                                        10000u, 120u, 5u, 20u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 48_5_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_48_5_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 117u, 1, \
                                        _stream_context, \
                                        7500u, 117u, 5u, 15u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 48_6_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_48_6_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 155u, 1, \
                                        _stream_context, \
                                        10000u, 155u, 5u, 20u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 8_1_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_8_1_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_8KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 26u, 1, \
                                        _stream_context, \
                                        7500u, 26u, 13u, 75u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 8_2_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_8_2_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_8KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 30U, 1, \
                                        _stream_context, \
                                        10000u, 30u, 13u, 95u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 16_1_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_16_1_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_16KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 30U, 1, \
                                        _stream_context, \
                                        7500u, 30u, 13u, 75u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 16_2_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_16_2_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_16KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 40U, 1, \
                                        _stream_context, \
                                        10000u, 40u, 13u, 95u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 24_1_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_24_1_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_24KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 45U, 1, \
                                        _stream_context, \
                                        7500u, 45u, 13u, 75u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 24_2_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_24_2_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_24KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 60U, 1, \
                                        _stream_context, \
                                        10000u, 60u, 13u, 95u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 32_1_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_32_1_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_32KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 60U, 1, \
                                        _stream_context, \
                                        7500u, 60u, 13u, 75u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 32_2_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_32_2_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_32KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 80U, 1, \
                                        _stream_context, \
                                        10000u, 80u, 13u, 95u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 441_1_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_441_1_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_44KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 97U, 1, \
                                        _stream_context, \
                                        8163u, 97u, 13u, 80u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 441_2_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_441_2_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_44KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 130U, 1, \
                                        _stream_context, \
                                        10884u, 130u, 13u, 85u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 48_1_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_48_1_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 75U, 1, \
                                        _stream_context, \
                                        7500u, 75u, 13u, 75u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 48_2_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_48_2_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 100U, 1, \
                                        _stream_context, \
                                        10000u, 100u, 13u, 95u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 48_3_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_48_3_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 90U, 1, \
                                        _stream_context, \
                                        7500u, 90u, 13u, 75u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 48_4_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_48_4_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 120u, 1, \
                                        _stream_context, \
                                        10000u, 120u, 13u, 100u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 48_5_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_48_5_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 117u, 1, \
                                        _stream_context, \
                                        7500u, 117u, 13u, 75u, 40000u)

/**
 * @brief   Helper to declare LC3 Unicast 48_6_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_48_6_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 155u, 1, \
                                        _stream_context, \
                                        10000u, 155u, 13u, 100u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 8_1_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_8_1_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_8KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 26u, 1, \
                                        _stream_context, \
                                        7500u, 26u, 2u, 8u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 8_2_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_8_2_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_8KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 30U, 1, \
                                        _stream_context, \
                                        10000u, 30u, 2u, 10u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 16_1_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_16_1_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_16KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 30U, 1, \
                                        _stream_context, \
                                        7500u, 30u, 2u, 8u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 16_2_1 codec configuration.
 *
 * Mandatory to support as both broadcast source and sink.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_16_2_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_16KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 40U, 1, \
                                        _stream_context, \
                                        10000u, 40u, 2u, 10u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 24_1_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_24_1_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_24KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 45U, 1, \
                                        _stream_context, \
                                        7500u, 45u, 2u, 8u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 24_2_1 codec configuration.
 *
 * Mandatory to support as broadcast sink.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_24_2_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_24KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 60U, 1, \
                                        _stream_context, \
                                        10000u, 60u, 2u, 10u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 32_1_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_32_1_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_32KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 60U, 1, \
                                        _stream_context, \
                                        7500u, 60u, 2u, 8u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 32_2_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_32_2_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_32KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 80U, 1, \
                                        _stream_context, \
                                        10000u, 80u, 2u, 10u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 441_1_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_441_1_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_44KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 97U, 1, \
                                        _stream_context, \
                                        8163u, 97u, 4u, 24u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 441_2_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_441_2_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_44KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 130U, 1, \
                                        _stream_context, \
                                        10884u, 130u, 4u, 31u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 48_1_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_48_1_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 75U, 1, \
                                        _stream_context, \
                                        7500u, 75u, 4u, 15u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 48_2_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_48_2_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 100U, 1, \
                                        _stream_context, \
                                        10000u, 100u, 4u, 20u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 48_3_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_48_3_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 90U, 1, \
                                        _stream_context, \
                                        7500u, 90u, 4u, 15u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 48_4_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_48_4_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 120u, 1, \
                                        _stream_context, \
                                        10000u, 120u, 4u, 20u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 48_5_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_48_5_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 117u, 1, \
                                        _stream_context, \
                                        7500u, 117u, 4u, 15u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 48_6_1 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_48_6_1_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 155u, 1, \
                                        _stream_context, \
                                        10000u, 155u, 4u, 20u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 8_1_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_8_1_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_8KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 26u, 1, \
                                        _stream_context, \
                                        7500u, 26u, 4u, 45u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 8_2_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_8_2_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_8KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 30U, 1, \
                                        _stream_context, \
                                        10000u, 30u, 4u, 60u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 16_1_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_16_1_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_16KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 30U, 1, \
                                        _stream_context, \
                                        7500u, 30u, 4u, 45u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 16_2_2 codec configuration.
 *
 * Mandatory to support as both broadcast source and sink.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_16_2_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_16KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 40U, 1, \
                                        _stream_context, \
                                        10000u, 40u, 4u, 60u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 24_1_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_24_1_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_24KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 45U, 1, \
                                        _stream_context, \
                                        7500u, 45u, 4u, 45u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 24_2_2 codec configuration.
 *
 * Mandatory to support as broadcast sink.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_24_2_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_24KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 60U, 1, \
                                        _stream_context, \
                                        10000u, 60u, 4u, 60u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 32_1_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_32_1_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_32KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 60U, 1, \
                                        _stream_context, \
                                        7500u, 60u, 4u, 45u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 32_2_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_32_2_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_32KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 80U, 1, \
                                        _stream_context, \
                                        10000u, 80u, 4u, 60u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 441_1_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_441_1_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_44KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 97U, 1, \
                                        _stream_context, \
                                        8163u, 97u, 4u, 54u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 441_2_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_441_2_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_44KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 130U, 1, \
                                        _stream_context, \
                                        10884u, 130u, 4u, 60u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 48_1_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_48_1_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 75U, 1, \
                                        _stream_context, \
                                        7500u, 75u, 4u, 50u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 48_2_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_48_2_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 100U, 1, \
                                        _stream_context, \
                                        10000u, 100u, 4u, 65u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 48_3_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_48_3_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 90U, 1, \
                                        _stream_context, \
                                        7500u, 90u, 4u, 50u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 48_4_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_48_4_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 120u, 1, \
                                        _stream_context, \
                                        10000u, 120u, 4u, 65u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 48_5_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_48_5_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_7_5, \
                                        _loc, 117u, 1, \
                                        _stream_context, \
                                        7500u, 117u, 4u, 50u, 40000u)

/**
 * @brief   Helper to declare LC3 Broadcast 48_6_2 codec configuration.
 *
 * @param   _name           Preset configuration name.
 * @param   _loc            Audio channel location bitfield.
 * @param   _stream_context Stream context.
 */
#define ESP_BLE_AUDIO_BAP_LC3_BROADCAST_PRESET_48_6_2_DEFINE(_name, _loc, _stream_context) \
    ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFINE(_name, \
                                        BT_AUDIO_CODEC_CFG_FREQ_48KHZ, \
                                        BT_AUDIO_CODEC_CFG_DURATION_10, \
                                        _loc, 155u, 1, \
                                        _stream_context, \
                                        10000u, 155u, 4u, 65u, 40000u)

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_BAP_LC3_PRESET_DEFS_H_ */
