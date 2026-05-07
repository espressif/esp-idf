/*
 * SPDX-FileCopyrightText: 2020 Intel Corporation
 * SPDX-FileCopyrightText: 2020-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_CODEC_API_H_
#define ESP_BLE_AUDIO_CODEC_API_H_

#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "sdkconfig.h"

#include "esp_ble_audio_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Helper to declare elements of bt_audio_codec_cap arrays.
 *
 * This macro is mainly for creating an array of struct bt_audio_codec_cap data arrays.
 *
 * @param   _type   Type of advertising data field.
 * @param   _bytes  Variable number of single-byte parameters.
 */
#define ESP_BLE_AUDIO_CODEC_DATA    BT_AUDIO_CODEC_DATA

/*!< Target low latency */
#define ESP_BLE_AUDIO_CODEC_CFG_TARGET_LATENCY_LOW      BT_AUDIO_CODEC_CFG_TARGET_LATENCY_LOW
/*!< Target balanced latency */
#define ESP_BLE_AUDIO_CODEC_CFG_TARGET_LATENCY_BALANCED BT_AUDIO_CODEC_CFG_TARGET_LATENCY_BALANCED
/*!< Target high latency */
#define ESP_BLE_AUDIO_CODEC_CFG_TARGET_LATENCY_HIGH     BT_AUDIO_CODEC_CFG_TARGET_LATENCY_HIGH
/** Codec configuration target latency */
typedef enum bt_audio_codec_cfg_target_latency          esp_ble_audio_codec_cfg_target_latency_t;

/*!< LE 1M PHY */
#define ESP_BLE_AUDIO_CODEC_CFG_TARGET_PHY_1M           BT_AUDIO_CODEC_CFG_TARGET_PHY_1M
/*!< LE 2M PHY */
#define ESP_BLE_AUDIO_CODEC_CFG_TARGET_PHY_2M           BT_AUDIO_CODEC_CFG_TARGET_PHY_2M
/*!< LE Coded PHY */
#define ESP_BLE_AUDIO_CODEC_CFG_TARGET_PHY_CODED        BT_AUDIO_CODEC_CFG_TARGET_PHY_CODED
/** Codec configuration target PHY */
typedef enum bt_audio_codec_cfg_target_phy              esp_ble_audio_codec_cfg_target_phy_t;

/**
 * @brief   Helper to declare esp_ble_audio_codec_cfg_t.
 *
 * @param   _id     Codec ID.
 * @param   _cid    Company ID.
 * @param   _vid    Vendor ID.
 * @param   _data   Codec Specific Data in LVT format.
 * @param   _meta   Codec Specific Metadata in LVT format.
 */
#define ESP_BLE_AUDIO_CODEC_CFG(_id, _cid, _vid, _data, _meta) \
    ((esp_ble_audio_codec_cfg_t){ \
        /* Use HCI data path as default, can be overwritten by application */ \
        .path_id        = ESP_BLE_ISO_DATA_PATH_HCI, \
        .ctlr_transcode = false, \
        COND_CODE_1(IS_ENABLED(CONFIG_BT_BAP_UNICAST), \
                    (.target_latency = ESP_BLE_AUDIO_CODEC_CFG_TARGET_LATENCY_BALANCED, \
                     .target_phy = ESP_BLE_AUDIO_CODEC_CFG_TARGET_PHY_2M,), \
                    ()) \
        .id             = (_id), \
        .cid            = (_cid), \
        .vid            = (_vid), \
        .data_len       = sizeof(_data), \
        .data           = (_data), \
        .meta_len       = sizeof(_meta), \
        .meta           = (_meta), \
    })

/**
 * @brief   Helper to declare esp_ble_audio_codec_cap_t.
 *
 * @param   _id     Codec ID.
 * @param   _cid    Company ID.
 * @param   _vid    Vendor-specific codec ID.
 * @param   _data   Codec-specific capability data.
 * @param   _meta   Metadata capabilities.
 */
#define ESP_BLE_AUDIO_CODEC_CAP(_id, _cid, _vid, _data, _meta) \
    ((esp_ble_audio_codec_cap_t){ \
        /* Use HCI data path as default, can be overwritten by application */ \
        .path_id        = ESP_BLE_ISO_DATA_PATH_HCI, \
        .ctlr_transcode = false, \
        .id             = (_id), \
        .cid            = (_cid), \
        .vid            = (_vid), \
        .data_len       = sizeof(_data), \
        .data           = (_data), \
        .meta_len       = sizeof(_meta), \
        .meta           = (_meta), \
    })

/*!< Supported sampling frequencies */
#define ESP_BLE_AUDIO_CODEC_CAP_TYPE_FREQ           BT_AUDIO_CODEC_CAP_TYPE_FREQ
/*!< Supported frame durations */
#define ESP_BLE_AUDIO_CODEC_CAP_TYPE_DURATION       BT_AUDIO_CODEC_CAP_TYPE_DURATION
/*!< Supported audio channel counts */
#define ESP_BLE_AUDIO_CODEC_CAP_TYPE_CHAN_COUNT     BT_AUDIO_CODEC_CAP_TYPE_CHAN_COUNT
/*!< Supported octets per codec frame */
#define ESP_BLE_AUDIO_CODEC_CAP_TYPE_FRAME_LEN      BT_AUDIO_CODEC_CAP_TYPE_FRAME_LEN
/*!< Supported maximum codec frames per SDU  */
#define ESP_BLE_AUDIO_CODEC_CAP_TYPE_FRAME_COUNT    BT_AUDIO_CODEC_CAP_TYPE_FRAME_COUNT
/** Codec capability types */
typedef enum bt_audio_codec_cap_type                esp_ble_audio_codec_cap_type_t;

/*!< 8 Khz sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CAP_FREQ_8KHZ           BT_AUDIO_CODEC_CAP_FREQ_8KHZ
/*!< 11.025 Khz sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CAP_FREQ_11KHZ          BT_AUDIO_CODEC_CAP_FREQ_11KHZ
/*!< 16 Khz sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CAP_FREQ_16KHZ          BT_AUDIO_CODEC_CAP_FREQ_16KHZ
/*!< 22.05 Khz sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CAP_FREQ_22KHZ          BT_AUDIO_CODEC_CAP_FREQ_22KHZ
/*!< 24 Khz sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CAP_FREQ_24KHZ          BT_AUDIO_CODEC_CAP_FREQ_24KHZ
/*!< 32 Khz sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CAP_FREQ_32KHZ          BT_AUDIO_CODEC_CAP_FREQ_32KHZ
/*!< 44.1 Khz sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CAP_FREQ_44KHZ          BT_AUDIO_CODEC_CAP_FREQ_44KHZ
/*!< 48 Khz sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CAP_FREQ_48KHZ          BT_AUDIO_CODEC_CAP_FREQ_48KHZ
/*!< 88.2 Khz sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CAP_FREQ_88KHZ          BT_AUDIO_CODEC_CAP_FREQ_88KHZ
/*!< 96 Khz sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CAP_FREQ_96KHZ          BT_AUDIO_CODEC_CAP_FREQ_96KHZ
/*!< 176.4 Khz sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CAP_FREQ_176KHZ         BT_AUDIO_CODEC_CAP_FREQ_176KHZ
/*!< 192 Khz sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CAP_FREQ_192KHZ         BT_AUDIO_CODEC_CAP_FREQ_192KHZ
/*!< 384 Khz sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CAP_FREQ_384KHZ         BT_AUDIO_CODEC_CAP_FREQ_384KHZ
/*!< Any frequency capability */
#define ESP_BLE_AUDIO_CODEC_CAP_FREQ_ANY            BT_AUDIO_CODEC_CAP_FREQ_ANY
/** Supported frequencies bitfield */
typedef enum bt_audio_codec_cap_freq                esp_ble_audio_codec_cap_freq_t;

/*!< 7.5 msec frame duration capability */
#define ESP_BLE_AUDIO_CODEC_CAP_DURATION_7_5        BT_AUDIO_CODEC_CAP_DURATION_7_5
/*!< 10 msec frame duration capability */
#define ESP_BLE_AUDIO_CODEC_CAP_DURATION_10         BT_AUDIO_CODEC_CAP_DURATION_10
/*!< Any frame duration capability */
#define ESP_BLE_AUDIO_CODEC_CAP_DURATION_ANY        BT_AUDIO_CODEC_CAP_DURATION_ANY
/*!< 7.5 msec preferred frame duration capability */
#define ESP_BLE_AUDIO_CODEC_CAP_DURATION_PREFER_7_5 BT_AUDIO_CODEC_CAP_DURATION_PREFER_7_5
/*!< 10 msec preferred frame duration capability */
#define ESP_BLE_AUDIO_CODEC_CAP_DURATION_PREFER_10  BT_AUDIO_CODEC_CAP_DURATION_PREFER_10
/** Supported frame durations bitfield */
typedef enum bt_audio_codec_cap_frame_dur           esp_ble_audio_codec_cap_frame_dur_t;

/*!< Supporting 1 channel */
#define ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_1        BT_AUDIO_CODEC_CAP_CHAN_COUNT_1
/*!< Supporting 2 channel */
#define ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_2        BT_AUDIO_CODEC_CAP_CHAN_COUNT_2
/*!< Supporting 3 channel */
#define ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_3        BT_AUDIO_CODEC_CAP_CHAN_COUNT_3
/*!< Supporting 4 channel */
#define ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_4        BT_AUDIO_CODEC_CAP_CHAN_COUNT_4
/*!< Supporting 5 channel */
#define ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_5        BT_AUDIO_CODEC_CAP_CHAN_COUNT_5
/*!< Supporting 6 channel */
#define ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_6        BT_AUDIO_CODEC_CAP_CHAN_COUNT_6
/*!< Supporting 7 channel */
#define ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_7        BT_AUDIO_CODEC_CAP_CHAN_COUNT_7
/*!< Supporting 8 channel */
#define ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_8        BT_AUDIO_CODEC_CAP_CHAN_COUNT_8
/*!< Supporting all channels */
#define ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_ANY      BT_AUDIO_CODEC_CAP_CHAN_COUNT_ANY

/*!< Minimum supported channel counts */
#define ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_MIN      BT_AUDIO_CODEC_CAP_CHAN_COUNT_MIN
/*!< Maximum supported channel counts */
#define ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_MAX      BT_AUDIO_CODEC_CAP_CHAN_COUNT_MAX

/**
 * @brief   Channel count support capability.
 *
 * Macro accepts variable number of channel counts.
 * The allowed channel counts are defined by specification and have to be in range from
 * ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_MIN to ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_MAX
 * inclusive.
 *
 * Example to support 1 and 3 channels:
 *   ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_SUPPORT(1, 3)
 */
#define ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_SUPPORT  BT_AUDIO_CODEC_CAP_CHAN_COUNT_SUPPORT
/** Supported audio capabilities channel count bitfield */
typedef enum bt_audio_codec_cap_chan_count          esp_ble_audio_codec_cap_chan_count_t;

/*!< Sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CFG_FREQ                BT_AUDIO_CODEC_CFG_FREQ
/*!< Frame duration */
#define ESP_BLE_AUDIO_CODEC_CFG_DURATION            BT_AUDIO_CODEC_CFG_DURATION
/*!< Audio channel allocation */
#define ESP_BLE_AUDIO_CODEC_CFG_CHAN_ALLOC          BT_AUDIO_CODEC_CFG_CHAN_ALLOC
/*!< Octets per codec frame */
#define ESP_BLE_AUDIO_CODEC_CFG_FRAME_LEN           BT_AUDIO_CODEC_CFG_FRAME_LEN
/*!< Codec frame blocks per SDU */
#define ESP_BLE_AUDIO_CODEC_CFG_FRAME_BLKS_PER_SDU  BT_AUDIO_CODEC_CFG_FRAME_BLKS_PER_SDU
/** Codec configuration types */
typedef enum bt_audio_codec_cfg_type                esp_ble_audio_codec_cfg_type_t;

/*!< 8 Khz codec sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CFG_FREQ_8KHZ           BT_AUDIO_CODEC_CFG_FREQ_8KHZ
/*!< 11.025 Khz codec sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CFG_FREQ_11KHZ          BT_AUDIO_CODEC_CFG_FREQ_11KHZ
/*!< 16 Khz codec sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CFG_FREQ_16KHZ          BT_AUDIO_CODEC_CFG_FREQ_16KHZ
/*!< 22.05 Khz codec sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CFG_FREQ_22KHZ          BT_AUDIO_CODEC_CFG_FREQ_22KHZ
/*!< 24 Khz codec sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CFG_FREQ_24KHZ          BT_AUDIO_CODEC_CFG_FREQ_24KHZ
/*!< 32 Khz codec sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CFG_FREQ_32KHZ          BT_AUDIO_CODEC_CFG_FREQ_32KHZ
/*!< 44.1 Khz codec sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CFG_FREQ_44KHZ          BT_AUDIO_CODEC_CFG_FREQ_44KHZ
/*!< 48 Khz codec sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CFG_FREQ_48KHZ          BT_AUDIO_CODEC_CFG_FREQ_48KHZ
/*!< 88.2 Khz codec sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CFG_FREQ_88KHZ          BT_AUDIO_CODEC_CFG_FREQ_88KHZ
/*!< 96 Khz codec sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CFG_FREQ_96KHZ          BT_AUDIO_CODEC_CFG_FREQ_96KHZ
/*!< 176.4 Khz codec sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CFG_FREQ_176KHZ         BT_AUDIO_CODEC_CFG_FREQ_176KHZ
/*!< 192 Khz codec sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CFG_FREQ_192KHZ         BT_AUDIO_CODEC_CFG_FREQ_192KHZ
/*!< 384 Khz codec sampling frequency */
#define ESP_BLE_AUDIO_CODEC_CFG_FREQ_384KHZ         BT_AUDIO_CODEC_CFG_FREQ_384KHZ
/** Codec configuration sampling frequency */
typedef enum bt_audio_codec_cfg_freq                esp_ble_audio_codec_cfg_freq_t;

/*!< 7.5 msec Frame Duration configuration */
#define ESP_BLE_AUDIO_CODEC_CFG_DURATION_7_5        BT_AUDIO_CODEC_CFG_DURATION_7_5
/*!< 10 msec Frame Duration configuration */
#define ESP_BLE_AUDIO_CODEC_CFG_DURATION_10         BT_AUDIO_CODEC_CFG_DURATION_10
/** Codec configuration frame duration */
typedef enum bt_audio_codec_cfg_frame_dur           esp_ble_audio_codec_cfg_frame_dur_t;

/** Codec specific configuration structure */
typedef struct bt_audio_codec_cfg   esp_ble_audio_codec_cfg_t;

/** Codec capability structure */
typedef struct bt_audio_codec_cap   esp_ble_audio_codec_cap_t;

/** Struct to hold minimum and maximum supported codec frame sizes */
typedef struct bt_audio_codec_octets_per_codec_frame    esp_ble_audio_codec_octets_per_codec_frame_t;

/**
 * @brief   Convert assigned numbers frequency to frequency value.
 *
 * @param   freq    The assigned numbers frequency to convert.
 * @param   freq_hz The converted frequency value in Hz on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_freq_to_freq_hz(esp_ble_audio_codec_cfg_freq_t freq,
                                                  uint32_t *freq_hz);

/**
 * @brief   Convert frequency value to assigned numbers frequency.
 *
 * @param   freq_hz Frequency in Hz.
 * @param   freq    The assigned numbers frequency on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_freq_hz_to_freq(uint32_t freq_hz,
                                                  esp_ble_audio_codec_cfg_freq_t *freq);

/**
 * @brief   Extract the frequency from a codec configuration.
 *
 * @param   codec_cfg   The codec configuration to extract data from.
 * @param   freq        The assigned numbers frequency on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_get_freq(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                           esp_ble_audio_codec_cfg_freq_t *freq);

/**
 * @brief   Set the frequency of a codec configuration.
 *
 * @param   codec_cfg   The codec configuration to set data for.
 * @param   freq        The assigned numbers frequency to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_set_freq(esp_ble_audio_codec_cfg_t *codec_cfg,
                                           esp_ble_audio_codec_cfg_freq_t freq);

/**
 * @brief   Convert assigned numbers frame duration to duration in microseconds.
 *
 * @param   frame_dur       The assigned numbers frame duration to convert.
 * @param   frame_dur_us    The converted frame duration value in microseconds on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_frame_dur_to_frame_dur_us(esp_ble_audio_codec_cfg_frame_dur_t frame_dur,
                                                            uint32_t *frame_dur_us);

/**
 * @brief   Convert frame duration in microseconds to assigned numbers frame duration.
 *
 * @param   frame_dur_us    The frame duration in microseconds to convert.
 * @param   frame_dur       The assigned numbers frame duration on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_frame_dur_us_to_frame_dur(uint32_t frame_dur_us,
                                                            esp_ble_audio_codec_cfg_frame_dur_t *frame_dur);

/**
 * @brief   Extract frame duration from codec config.
 *
 * @param   codec_cfg   The codec configuration to extract data from.
 * @param   frame_dur   The assigned numbers frame duration on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_get_frame_dur(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                esp_ble_audio_codec_cfg_frame_dur_t *frame_dur);

/**
 * @brief   Set the frame duration of a codec configuration.
 *
 * @param   codec_cfg   The codec configuration to set data for.
 * @param   frame_dur   The assigned numbers frame duration to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_set_frame_dur(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                esp_ble_audio_codec_cfg_frame_dur_t frame_dur);

/**
 * @brief   Extract channel allocation from codec config.
 *
 * The value returned is a bit field representing one or more audio locations as
 * specified by esp_ble_audio_location_t.
 * Shall match one or more of the bits set in ESP_BLE_AUDIO_PAC_SNK_LOC or
 * ESP_BLE_AUDIO_PAC_SRC_LOC.
 *
 * Up to the configured ESP_BLE_AUDIO_CODEC_CAP_TYPE_CHAN_COUNT number of
 * channels can be present.
 *
 * @param   codec_cfg           The codec configuration to extract data from.
 * @param   chan_allocation     Pointer to the variable to store the extracted value in.
 * @param   fallback_to_default If true this function will provide the default value of
 *                              ESP_BLE_AUDIO_LOCATION_MONO_AUDIO if the type is not
 *                              found when @p codec_cfg.id is BT_HCI_CODING_FORMAT_LC3.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_get_chan_allocation(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                      esp_ble_audio_location_t *chan_allocation,
                                                      bool fallback_to_default);

/**
 * @brief   Set the channel allocation of a codec configuration.
 *
 * @param   codec_cfg       The codec configuration to set data for.
 * @param   chan_allocation The channel allocation to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_set_chan_allocation(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                      esp_ble_audio_location_t chan_allocation);

/**
 * @brief   Extract frame size in octets from codec config.
 *
 * The overall SDU size will be octets_per_frame * blocks_per_sdu.
 *
 * The Bluetooth specifications are not clear about this value - it does not state that
 * the codec shall use this SDU size only. A codec like LC3 supports variable bit-rate
 * (per SDU) hence it might be allowed for an encoder to reduce the frame size below this
 * value.
 * Hence it is recommended to use the received SDU size and divide by
 * blocks_per_sdu rather than relying on this octets_per_sdu value to be fixed.
 *
 * @param   codec_cfg           The codec configuration to extract data from.
 * @param   octets_per_frame    The frame length in octets on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_get_octets_per_frame(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                       uint16_t *octets_per_frame);

/**
 * @brief   Set the octets per codec frame of a codec configuration.
 *
 * @param   codec_cfg           The codec configuration to set data for.
 * @param   octets_per_frame    The octets per codec frame to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_set_octets_per_frame(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                       uint16_t octets_per_frame);

/**
 * @brief Extract number of audio frame blocks in each SDU from codec config.
 *
 * The overall SDU size will be octets_per_frame * frame_blocks_per_sdu * number-of-channels.
 *
 * If this value is not present a default value of 1 shall be used.
 *
 * A frame block is one or more frames that represents data for the same period of time
 * but for different channels. If the stream have two audio channels and this value is
 * two there will be four frames in the SDU.
 *
 * @param   codec_cfg           The codec configuration to extract data from.
 * @param   frame_blocks        The count of codec frame blocks in each SDU on success.
 * @param   fallback_to_default If true this function will return the default value of 1
 *                              if the type is not found when @p codec_cfg.id is
 *                              BT_HCI_CODING_FORMAT_LC3.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_get_frame_blocks_per_sdu(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                           uint8_t *frame_blocks,
                                                           bool fallback_to_default);

/**
 * @brief   Set the frame blocks per SDU of a codec configuration.
 *
 * @param   codec_cfg       The codec configuration to set data for.
 * @param   frame_blocks    The frame blocks per SDU to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_set_frame_blocks_per_sdu(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                           uint8_t frame_blocks);

/**
 * @brief   Lookup a specific codec configuration value.
 *
 * @param   codec_cfg   The codec data to search in.
 * @param   type        The type id to look for.
 * @param   data        Pointer to the data-pointer to update when item is found.
 * @param   data_len    Length of found @p data (may be 0) on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_get_val(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                          esp_ble_audio_codec_cfg_type_t type,
                                          const uint8_t **data, size_t *data_len);

/**
 * @brief   Set or add a specific codec configuration value.
 *
 * @param   codec_cfg   The codec data to set the value in.
 * @param   type        The type id to set.
 * @param   data        Pointer to the data-pointer to set.
 * @param   data_len    Length of @p data.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_set_val(esp_ble_audio_codec_cfg_t *codec_cfg,
                                          esp_ble_audio_codec_cfg_type_t type,
                                          const uint8_t *data, size_t data_len);

/**
 * @brief   Unset a specific codec configuration value.
 *
 * The type and the value will be removed from the codec configuration.
 *
 * @param   codec_cfg   The codec data to set the value in.
 * @param   type        The type id to unset.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_unset_val(esp_ble_audio_codec_cfg_t *codec_cfg,
                                            esp_ble_audio_codec_cfg_type_t type);

/**
 * @brief   Lookup a specific metadata value based on type.
 *
 * @param   codec_cfg   The codec data to search in.
 * @param   type        The type id to look for
 * @param   data        Pointer to the data-pointer to update when item is found.
 * @param   data_len    Length of found @p data (may be 0) on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_get_val(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                               esp_ble_audio_metadata_type_t type,
                                               const uint8_t **data, size_t *data_len);

/**
 * @brief   Set or add a specific codec configuration metadata value.
 *
 * @param   codec_cfg   The codec configuration to set the value in.
 * @param   type        The type id to set.
 * @param   data        Pointer to the data-pointer to set.
 * @param   data_len    Length of @p data.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_set_val(esp_ble_audio_codec_cfg_t *codec_cfg,
                                               esp_ble_audio_metadata_type_t type,
                                               const uint8_t *data, size_t data_len);

/**
 * @brief   Unset a specific codec configuration metadata value.
 *
 * The type and the value will be removed from the codec configuration metadata.
 *
 * @param   codec_cfg   The codec data to set the value in.
 * @param   type        The type id to unset.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_unset_val(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                 esp_ble_audio_metadata_type_t type);

/**
 * @brief   Extract preferred contexts.
 *
 * @param   codec_cfg           The codec data to search in.
 * @param   ctx                 The preferred context type on success.
 * @param   fallback_to_default If true this function will provide the default value of
 *                              ESP_BLE_AUDIO_CONTEXT_TYPE_UNSPECIFIED if the type
 *                              is not found when @p codec_cfg.id is
 *                              BT_HCI_CODING_FORMAT_LC3.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_get_pref_context(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                        esp_ble_audio_context_t *ctx,
                                                        bool fallback_to_default);

/**
 * @brief   Set the preferred context of a codec configuration metadata.
 *
 * @param   codec_cfg   The codec configuration to set data for.
 * @param   ctx         The preferred context to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_set_pref_context(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                        esp_ble_audio_context_t ctx);

/**
 * @brief   Extract stream contexts.
 *
 * @param   codec_cfg   The codec data to search in.
 * @param   ctx         The stream context type on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_get_stream_context(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                          esp_ble_audio_context_t *ctx);

/**
 * @brief   Set the stream context of a codec configuration metadata.
 *
 * @param   codec_cfg   The codec configuration to set data for.
 * @param   ctx         The stream context to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_set_stream_context(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                          esp_ble_audio_context_t ctx);

/**
 * @brief   Extract program info.
 *
 * @param   codec_cfg           The codec data to search in.
 * @param   program_info        Pointer to the UTF-8 formatted program info.
 * @param   program_info_len    The length of the @p program_info (may be 0) on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_get_program_info(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                        const uint8_t **program_info,
                                                        size_t *program_info_len);

/**
 * @brief   Set the program info of a codec configuration metadata.
 *
 * @param   codec_cfg           The codec configuration to set data for.
 * @param   program_info        The program info to set.
 * @param   program_info_len    The length of @p program_info.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_set_program_info(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                        const uint8_t *program_info,
                                                        size_t program_info_len);

/**
 * @brief   Extract language.
 *
 * @param   codec_cfg   The codec data to search in.
 * @param   language    Pointer to the language bytes (of length ESP_BLE_AUDIO_LANG_SIZE).
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_get_language(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                    const uint8_t **language);

/**
 * @brief   Set the language of a codec configuration metadata.
 *
 * @param   codec_cfg   The codec configuration to set data for.
 * @param   language    The 24-bit language to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_set_language(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                    const uint8_t language[ESP_BLE_AUDIO_LANG_SIZE]);

/**
 * @brief   Extract CCID list.
 *
 * @param   codec_cfg       The codec data to search in.
 * @param   ccid_list       Pointer to the array containing 8-bit CCIDs.
 * @param   ccid_list_len   The length of the @p ccid_list (may be 0) on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_get_ccid_list(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                     const uint8_t **ccid_list,
                                                     size_t *ccid_list_len);

/**
 * @brief   Set the CCID list of a codec configuration metadata.
 *
 * @param   codec_cfg       The codec configuration to set data for.
 * @param   ccid_list       The program info to set.
 * @param   ccid_list_len   The length of @p ccid_list.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_set_ccid_list(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                     const uint8_t *ccid_list,
                                                     size_t ccid_list_len);

/**
 * @brief   Extract parental rating.
 *
 * @param   codec_cfg       The codec data to search in.
 * @param   parental_rating The parental rating on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_get_parental_rating(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                           esp_ble_audio_parental_rating_t *parental_rating);

/**
 * @brief   Set the parental rating of a codec configuration metadata.
 *
 * @param   codec_cfg       The codec configuration to set data for.
 * @param   parental_rating The parental rating to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_set_parental_rating(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                           esp_ble_audio_parental_rating_t parental_rating);

/**
 * @brief   Extract program info URI.
 *
 * @param   codec_cfg               The codec data to search in.
 * @param   program_info_uri        Pointer to the UTF-8 formatted program info URI.
 * @param   program_info_uri_len    The length of the @p program_info_uri (may be 0) on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_get_program_info_uri(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                            const uint8_t **program_info_uri,
                                                            size_t *program_info_uri_len);

/**
 * @brief   Set the program info URI of a codec configuration metadata.
 *
 * @param   codec_cfg               The codec configuration to set data for.
 * @param   program_info_uri        The program info URI to set.
 * @param   program_info_uri_len    The length of @p program_info_uri.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_set_program_info_uri(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                            const uint8_t *program_info_uri,
                                                            size_t program_info_uri_len);

/**
 * @brief   Extract audio active state.
 *
 * @param   codec_cfg   The codec data to search in.
 * @param   state       The preferred context type on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_get_audio_active_state(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                              esp_ble_audio_active_state_t *state);

/**
 * @brief   Set the audio active state of a codec configuration metadata.
 *
 * @param   codec_cfg   The codec configuration to set data for.
 * @param   state       The audio active state to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_set_audio_active_state(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                              esp_ble_audio_active_state_t state);

/**
 * @brief   Extract broadcast audio immediate rendering flag.
 *
 * @param   codec_cfg   The codec data to search in.
 * @param   found       Indicate if the flag is found on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_get_broadcast_audio_immediate_rend_flag(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                                               bool *found);

/**
 * @brief   Set the broadcast audio immediate rendering flag of a codec configuration metadata.
 *
 * @param   codec_cfg   The codec configuration to set data for.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_set_broadcast_audio_immediate_rend_flag(esp_ble_audio_codec_cfg_t *codec_cfg);

/**
 * @brief   Extract assisted listening stream.
 *
 * @param   codec_cfg   The codec data to search in.
 * @param   val         The assisted listening stream value on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_get_assisted_listening_stream(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                                     esp_ble_audio_assisted_listening_stream_t *val);

/**
 * @brief   Set the assisted listening stream value of a codec configuration metadata.
 *
 * @param   codec_cfg   The codec configuration to set data for.
 * @param   val         The assisted listening stream value to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_set_assisted_listening_stream(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                                     esp_ble_audio_assisted_listening_stream_t val);

/**
 * @brief   Extract broadcast name.
 *
 * @param   codec_cfg           The codec data to search in.
 * @param   broadcast_name      Pointer to the UTF-8 formatted broadcast name.
 * @param   broadcast_name_len  The length of the @p broadcast_name (may be 0) on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_get_broadcast_name(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                          const uint8_t **broadcast_name,
                                                          size_t *broadcast_name_len);

/**
 * @brief   Set the broadcast name of a codec configuration metadata.
 *
 * @param   codec_cfg           The codec configuration to set data for.
 * @param   broadcast_name      The broadcast name to set.
 * @param   broadcast_name_len  The length of @p broadcast_name.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_set_broadcast_name(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                          const uint8_t *broadcast_name,
                                                          size_t broadcast_name_len);

/**
 * @brief   Extract extended metadata.
 *
 * @param   codec_cfg           The codec data to search in.
 * @param   extended_meta       Pointer to the extended metadata.
 * @param   extended_meta_len   The length of the @p extended_meta (may be 0) on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_get_extended(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                    const uint8_t **extended_meta,
                                                    size_t *extended_meta_len);

/**
 * @brief   Set the extended metadata of a codec configuration metadata.
 *
 * @param   codec_cfg           The codec configuration to set data for.
 * @param   extended_meta       The extended metadata to set.
 * @param   extended_meta_len   The length of @p extended_meta.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_set_extended(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                    const uint8_t *extended_meta,
                                                    size_t extended_meta_len);

/**
 * @brief   Extract vendor specific metadata.
 *
 * @param   codec_cfg       The codec data to search in.
 * @param   vendor_meta     Pointer to the vendor specific metadata.
 * @param   vendor_meta_len The length of the @p vendor_meta (may be 0) on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_get_vendor(const esp_ble_audio_codec_cfg_t *codec_cfg,
                                                  const uint8_t **vendor_meta,
                                                  size_t *vendor_meta_len);

/**
 * @brief   Set the vendor specific metadata of a codec configuration metadata.
 *
 * @param   codec_cfg       The codec configuration to set data for.
 * @param   vendor_meta     The vendor specific metadata to set.
 * @param   vendor_meta_len The length of @p vendor_meta.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cfg_meta_set_vendor(esp_ble_audio_codec_cfg_t *codec_cfg,
                                                  const uint8_t *vendor_meta,
                                                  size_t vendor_meta_len);

/**
 * @brief   Lookup a specific value based on type.
 *
 * @param   codec_cap   The codec data to search in.
 * @param   type        The type id to look for.
 * @param   data        Pointer to the data-pointer to update when item is found.
 * @param   data_len    Length of found @p data (may be 0) on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_get_val(const esp_ble_audio_codec_cap_t *codec_cap,
                                          esp_ble_audio_codec_cap_type_t type,
                                          const uint8_t **data, size_t *data_len);

/**
 * @brief   Set or add a specific codec capability value.
 *
 * @param   codec_cap   The codec data to set the value in.
 * @param   type        The type id to set.
 * @param   data        Pointer to the data-pointer to set.
 * @param   data_len    Length of @p data.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_set_val(esp_ble_audio_codec_cap_t *codec_cap,
                                          esp_ble_audio_codec_cap_type_t type,
                                          const uint8_t *data, size_t data_len);

/**
 * @brief   Unset a specific codec capability value.
 *
 * The type and the value will be removed from the codec capability.
 *
 * @param   codec_cap   The codec data to set the value in.
 * @param   type        The type id to unset.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_unset_val(esp_ble_audio_codec_cap_t *codec_cap,
                                            esp_ble_audio_codec_cap_type_t type);

/**
 * @brief   Extract the frequency from a codec capability.
 *
 * @param   codec_cap   The codec capabilities to extract data from.
 * @param   freq        Bitfield of supported frequencies on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_get_freq(const esp_ble_audio_codec_cap_t *codec_cap,
                                           esp_ble_audio_codec_cap_freq_t *freq);

/**
 * @brief   Set the supported frequencies of a codec capability.
 *
 * @param   codec_cap   The codec capabilities to set data for.
 * @param   freq        The supported frequencies to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_set_freq(esp_ble_audio_codec_cap_t *codec_cap,
                                           esp_ble_audio_codec_cap_freq_t freq);

/**
 * @brief   Extract the frequency from a codec capability.
 *
 * @param   codec_cap   The codec capabilities to extract data from.
 * @param   frame_dur   Bitfield of supported frame durations on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_get_frame_dur(const esp_ble_audio_codec_cap_t *codec_cap,
                                                esp_ble_audio_codec_cap_frame_dur_t *frame_dur);

/**
 * @brief   Set the frame duration of a codec capability.
 *
 * @param   codec_cap   The codec capabilities to set data for.
 * @param   frame_dur   The frame duration to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_set_frame_dur(esp_ble_audio_codec_cap_t *codec_cap,
                                                esp_ble_audio_codec_cap_frame_dur_t frame_dur);

/**
 * @brief   Extract the frequency from a codec capability.
 *
 * @param   codec_cap           The codec capabilities to extract data from.
 * @param   chan_count          Number of supported channel counts on success.
 * @param   fallback_to_default If true this function will provide the default value of 1
 *                              if the type is not found when @p codec_cap.id is
 *                              BT_HCI_CODING_FORMAT_LC3.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_get_supported_audio_chan_counts(const esp_ble_audio_codec_cap_t *codec_cap,
                                                                  esp_ble_audio_codec_cap_chan_count_t *chan_count,
                                                                  bool fallback_to_default);

/**
 * @brief   Set the channel count of a codec capability.
 *
 * @param   codec_cap   The codec capabilities to set data for.
 * @param   chan_count  The channel count frequency to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_set_supported_audio_chan_counts(esp_ble_audio_codec_cap_t *codec_cap,
                                                                  esp_ble_audio_codec_cap_chan_count_t chan_count);

/**
 * @brief   Extract the supported octets per codec frame from a codec capability.
 *
 * @param   codec_cap   The codec capabilities to extract data from.
 * @param   codec_frame Struct to place the resulting values in.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_get_octets_per_frame(const esp_ble_audio_codec_cap_t *codec_cap,
                                                       esp_ble_audio_codec_octets_per_codec_frame_t *codec_frame);

/**
 * @brief   Set the octets per codec frame of a codec capability.
 *
 * @param   codec_cap   The codec capabilities to set data for.
 * @param   codec_frame The octets per codec frame to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_set_octets_per_frame(esp_ble_audio_codec_cap_t *codec_cap,
                                                       const esp_ble_audio_codec_octets_per_codec_frame_t *codec_frame);

/**
 * @brief   Extract the maximum codec frames per SDU from a codec capability.
 *
 * @param   codec_cap               The codec capabilities to extract data from.
 * @param   codec_frames_per_sdu    Maximum number of codec frames per SDU supported on success.
 * @param   fallback_to_default     If true this function will provide the default value of 1
 *                                  if the type is not found when @p codec_cap.id is
 *                                  BT_HCI_CODING_FORMAT_LC3.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_get_max_codec_frames_per_sdu(const esp_ble_audio_codec_cap_t *codec_cap,
                                                               uint8_t *codec_frames_per_sdu,
                                                               bool fallback_to_default);

/**
 * @brief   Set the maximum codec frames per SDU of a codec capability.
 *
 * @param   codec_cap               The codec capabilities to set data for.
 * @param   codec_frames_per_sdu    The maximum codec frames per SDU to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_set_max_codec_frames_per_sdu(esp_ble_audio_codec_cap_t *codec_cap,
                                                               uint8_t codec_frames_per_sdu);

/**
 * @brief   Lookup a specific metadata value based on type.
 *
 * @param   codec_cap   The codec data to search in.
 * @param   type        The type id to look for.
 * @param   data        Pointer to the data-pointer to update when item is found.
 * @param   data_len    Length of found @p data (may be 0) on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_get_val(const esp_ble_audio_codec_cap_t *codec_cap,
                                               esp_ble_audio_metadata_type_t type,
                                               const uint8_t **data, size_t *data_len);

/**
 * @brief   Set or add a specific codec capability metadata value.
 *
 * @param   codec_cap   The codec capability to set the value in.
 * @param   type        The type id to set.
 * @param   data        Pointer to the data-pointer to set.
 * @param   data_len    Length of @p data.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_set_val(esp_ble_audio_codec_cap_t *codec_cap,
                                               esp_ble_audio_metadata_type_t type,
                                               const uint8_t *data, size_t data_len);

/**
 * @brief   Unset a specific codec capability metadata value.
 *
 * The type and the value will be removed from the codec capability metadata.
 *
 * @param   codec_cap   The codec data to set the value in.
 * @param   type        The type id to unset.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_unset_val(esp_ble_audio_codec_cap_t *codec_cap,
                                                 esp_ble_audio_metadata_type_t type);

/**
 * @brief   Extract preferred contexts.
 *
 * @param   codec_cap   The codec data to search in.
 * @param   ctx         The preferred context type on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_get_pref_context(const esp_ble_audio_codec_cap_t *codec_cap,
                                                        esp_ble_audio_context_t *ctx);

/**
 * @brief   Set the preferred context of a codec capability metadata.
 *
 * @param   codec_cap   The codec capability to set data for.
 * @param   ctx         The preferred context to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_set_pref_context(esp_ble_audio_codec_cap_t *codec_cap,
                                                        esp_ble_audio_context_t ctx);

/**
 * @brief   Extract stream contexts.
 *
 * @param   codec_cap   The codec data to search in.
 * @param   ctx         The stream context type on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_get_stream_context(const esp_ble_audio_codec_cap_t *codec_cap,
                                                          esp_ble_audio_context_t *ctx);

/**
 * @brief   Set the stream context of a codec capability metadata.
 *
 * @param   codec_cap   The codec capability to set data for.
 * @param   ctx         The stream context to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_set_stream_context(esp_ble_audio_codec_cap_t *codec_cap,
                                                          esp_ble_audio_context_t ctx);

/**
 * @brief   Extract program info.
 *
 * @param   codec_cap           The codec data to search in.
 * @param   program_info        Pointer to the UTF-8 formatted program info.
 * @param   program_info_len    The length of the @p program_info (may be 0) on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_get_program_info(const esp_ble_audio_codec_cap_t *codec_cap,
                                                        const uint8_t **program_info,
                                                        size_t *program_info_len);

/**
 * @brief   Set the program info of a codec capability metadata.
 *
 * @param   codec_cap           The codec capability to set data for.
 * @param   program_info        The program info to set.
 * @param   program_info_len    The length of @p program_info.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_set_program_info(esp_ble_audio_codec_cap_t *codec_cap,
                                                        const uint8_t *program_info,
                                                        size_t program_info_len);

/**
 * @brief   Extract language.
 *
 * @param   codec_cap   The codec data to search in.
 * @param   language    Pointer to the language bytes (of length ESP_BLE_AUDIO_LANG_SIZE).
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_get_language(const esp_ble_audio_codec_cap_t *codec_cap,
                                                    const uint8_t **language);

/**
 * @brief   Set the language of a codec capability metadata.
 *
 * @param   codec_cap   The codec capability to set data for.
 * @param   language    The 24-bit language to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_set_language(esp_ble_audio_codec_cap_t *codec_cap,
                                                    const uint8_t language[ESP_BLE_AUDIO_LANG_SIZE]);

/**
 * @brief   Extract CCID list.
 *
 * @param   codec_cap       The codec data to search in.
 * @param   ccid_list       Pointer to the array containing 8-bit CCIDs.
 * @param   ccid_list_len   The length of the @p ccid_list (may be 0) on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_get_ccid_list(const esp_ble_audio_codec_cap_t *codec_cap,
                                                     const uint8_t **ccid_list,
                                                     size_t *ccid_list_len);

/**
 * @brief   Set the CCID list of a codec capability metadata.
 *
 * @param   codec_cap       The codec capability to set data for.
 * @param   ccid_list       The program info to set.
 * @param   ccid_list_len   The length of @p ccid_list.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_set_ccid_list(esp_ble_audio_codec_cap_t *codec_cap,
                                                     const uint8_t *ccid_list,
                                                     size_t ccid_list_len);

/**
 * @brief   Extract parental rating.
 *
 * @param   codec_cap       The codec data to search in.
 * @param   parental_rating The parental rating on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_get_parental_rating(const esp_ble_audio_codec_cap_t *codec_cap,
                                                           esp_ble_audio_parental_rating_t *parental_rating);

/**
 * @brief   Set the parental rating of a codec capability metadata.
 *
 * @param   codec_cap       The codec capability to set data for.
 * @param   parental_rating The parental rating to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_set_parental_rating(esp_ble_audio_codec_cap_t *codec_cap,
                                                           esp_ble_audio_parental_rating_t parental_rating);

/**
 * @brief   Extract program info URI.
 *
 * @param   codec_cap               The codec data to search in.
 * @param   program_info_uri        Pointer to the UTF-8 formatted program info URI.
 * @param   program_info_uri_len    The length of the @p program_info_uri (may be 0) on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_get_program_info_uri(const esp_ble_audio_codec_cap_t *codec_cap,
                                                            const uint8_t **program_info_uri,
                                                            size_t *program_info_uri_len);

/**
 * @brief   Set the program info URI of a codec capability metadata.
 *
 * @param   codec_cap               The codec capability to set data for.
 * @param   program_info_uri        The program info URI to set.
 * @param   program_info_uri_len    The length of @p program_info_uri.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_set_program_info_uri(esp_ble_audio_codec_cap_t *codec_cap,
                                                            const uint8_t *program_info_uri,
                                                            size_t program_info_uri_len);

/**
 * @brief   Extract audio active state.
 *
 * @param   codec_cap   The codec data to search in.
 * @param   state       The preferred context type on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_get_audio_active_state(const esp_ble_audio_codec_cap_t *codec_cap,
                                                              esp_ble_audio_active_state_t *state);

/**
 * @brief   Set the audio active state of a codec capability metadata.
 *
 * @param   codec_cap   The codec capability to set data for.
 * @param   state       The audio active state to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_set_audio_active_state(esp_ble_audio_codec_cap_t *codec_cap,
                                                              esp_ble_audio_active_state_t state);

/**
 * @brief   Extract broadcast audio immediate rendering flag.
 *
 * @param   codec_cap   The codec data to search in.
 * @param   found       Indicate if the flag is found on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_get_broadcast_audio_immediate_rend_flag(const esp_ble_audio_codec_cap_t *codec_cap,
                                                                               bool *found);

/**
 * @brief   Set the broadcast audio immediate rendering flag of a codec capability metadata.
 *
 * @param   codec_cap   The codec capability to set data for.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_set_broadcast_audio_immediate_rend_flag(esp_ble_audio_codec_cap_t *codec_cap);

/**
 * @brief   Extract assisted listening stream.
 *
 * @param   codec_cap   The codec data to search in.
 * @param   val         The assisted listening stream value on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_get_assisted_listening_stream(const esp_ble_audio_codec_cap_t *codec_cap,
                                                                     esp_ble_audio_assisted_listening_stream_t *val);

/**
 * @brief   Set the assisted listening stream value of a codec capability metadata.
 *
 * @param   codec_cap   The codec capability to set data for.
 * @param   val         The assisted listening stream value to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_set_assisted_listening_stream(esp_ble_audio_codec_cap_t *codec_cap,
                                                                     esp_ble_audio_assisted_listening_stream_t val);

/**
 * @brief   Extract broadcast name.
 *
 * @param   codec_cap           The codec data to search in.
 * @param   broadcast_name      Pointer to the UTF-8 formatted broadcast name.
 * @param   broadcast_name_len  The length of the @p broadcast_name (may be 0) on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_get_broadcast_name(const esp_ble_audio_codec_cap_t *codec_cap,
                                                          const uint8_t **broadcast_name,
                                                          size_t *broadcast_name_len);

/**
 * @brief   Set the broadcast name of a codec capability metadata.
 *
 * @param   codec_cap           The codec capability to set data for.
 * @param   broadcast_name      The broadcast name to set.
 * @param   broadcast_name_len  The length of @p broadcast_name.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_set_broadcast_name(esp_ble_audio_codec_cap_t *codec_cap,
                                                          const uint8_t *broadcast_name,
                                                          size_t broadcast_name_len);

/**
 * @brief   Extract extended metadata.
 *
 * @param   codec_cap       The codec data to search in.
 * @param   extended_meta   Pointer to the extended metadata.
 * @param   extended_meta_len   The length of the @p extended_meta (may be 0) on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_get_extended(const esp_ble_audio_codec_cap_t *codec_cap,
                                                    const uint8_t **extended_meta,
                                                    size_t *extended_meta_len);

/**
 * @brief   Set the extended metadata of a codec capability metadata.
 *
 * @param   codec_cap           The codec capability to set data for.
 * @param   extended_meta       The extended metadata to set.
 * @param   extended_meta_len   The length of @p extended_meta.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_set_extended(esp_ble_audio_codec_cap_t *codec_cap,
                                                    const uint8_t *extended_meta,
                                                    size_t extended_meta_len);

/**
 * @brief   Extract vendor specific metadata.
 *
 * @param   codec_cap       The codec data to search in.
 * @param   vendor_meta     Pointer to the vendor specific metadata.
 * @param   vendor_meta_len The length of the @p vendor_meta (may be 0) on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_get_vendor(const esp_ble_audio_codec_cap_t *codec_cap,
                                                  const uint8_t **vendor_meta,
                                                  size_t *vendor_meta_len);

/**
 * @brief   Set the vendor specific metadata of a codec capability metadata.
 *
 * @param   codec_cap       The codec capability to set data for.
 * @param   vendor_meta     The vendor specific metadata to set.
 * @param   vendor_meta_len The length of @p vendor_meta.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_codec_cap_meta_set_vendor(esp_ble_audio_codec_cap_t *codec_cap,
                                                  const uint8_t *vendor_meta,
                                                  size_t vendor_meta_len);

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_CODEC_API_H_ */
