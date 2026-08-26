/**
 * @file
 * @brief Bluetooth LC3 codec handling
 */

/*
 * SPDX-FileCopyrightText: 2020 Intel Corporation
 * SPDX-FileCopyrightText: 2022-2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_LC3_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_LC3_H_

/**
 * @brief LC3
 * @defgroup bt_lc3 Bluetooth LC3 codec

 * @since 3.0
 * @version 0.8.0

 * @ingroup bluetooth
 * @{
 */

#include <zephyr/sys/util_macro.h>
#include <zephyr/bluetooth/byteorder.h>
#include <zephyr/bluetooth/hci_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Helper to declare LC3 codec capability
 *
 * @p _max_frames_per_sdu is optional and will be included only if != 1
 *
 * @ref COND_CODE_1 is used to omit an LTV entry in case the @p _frames_per_sdu is 1.
 * @ref COND_CODE_1 will evaluate to second argument if the flag parameter(first argument) is 1
 * - removing one layer of paranteses.
 * If the flags argument is != 1 it will evaluate to the third argument which inserts a LTV
 * entry for the max_frames_per_sdu value.

 * @param _freq Supported Sampling Frequencies bitfield (see ``BT_AUDIO_CODEC_CAP_FREQ_*``)
 * @param _duration Supported Frame Durations bitfield (see ``BT_AUDIO_CODEC_CAP_DURATION_*``)
 * @param _chan_count Supported channels (see @ref BT_AUDIO_CODEC_CAP_CHAN_COUNT_SUPPORT)
 * @param _len_min Minimum number of octets supported per codec frame
 * @param _len_max Maximum number of octets supported per codec frame
 * @param _max_frames_per_sdu Supported maximum codec frames per SDU
 */
#define BT_AUDIO_CODEC_CAP_LC3_DATA(_freq, _duration, _chan_count, _len_min, _len_max,             \
                    _max_frames_per_sdu)                                           \
    {                                                                                          \
        BT_AUDIO_CODEC_DATA(BT_AUDIO_CODEC_CAP_TYPE_FREQ, BT_BYTES_LIST_LE16(_freq)),      \
        BT_AUDIO_CODEC_DATA(BT_AUDIO_CODEC_CAP_TYPE_DURATION, (_duration)),                \
        BT_AUDIO_CODEC_DATA(BT_AUDIO_CODEC_CAP_TYPE_CHAN_COUNT, (_chan_count)),            \
        BT_AUDIO_CODEC_DATA(BT_AUDIO_CODEC_CAP_TYPE_FRAME_LEN,                             \
                    BT_BYTES_LIST_LE16(_len_min),                                  \
                    BT_BYTES_LIST_LE16(_len_max)),                                 \
        COND_CODE_1(_max_frames_per_sdu, (),                                               \
                (BT_AUDIO_CODEC_DATA(BT_AUDIO_CODEC_CAP_TYPE_FRAME_COUNT,              \
                         (_max_frames_per_sdu))))                          \
    }

/**
 * @brief Helper to declare LC3 codec metadata
 *
 * @param _prefer_context Preferred contexts (@ref bt_audio_context)
 */
#define BT_AUDIO_CODEC_CAP_LC3_META(_prefer_context)                                               \
    {                                                                                          \
        BT_AUDIO_CODEC_DATA(BT_AUDIO_METADATA_TYPE_PREF_CONTEXT,                           \
                    BT_BYTES_LIST_LE16(_prefer_context))                           \
    }

/**
 * @brief Helper to declare LC3 codec
 *
 * @param _freq Supported Sampling Frequencies bitfield (see ``BT_AUDIO_CODEC_CAP_FREQ_*``)
 * @param _duration Supported Frame Durations bitfield (see ``BT_AUDIO_CODEC_CAP_DURATION_*``)
 * @param _chan_count Supported channels (see @ref BT_AUDIO_CODEC_CAP_CHAN_COUNT_SUPPORT)
 * @param _len_min Minimum number of octets supported per codec frame
 * @param _len_max Maximum number of octets supported per codec frame
 * @param _max_frames_per_sdu Supported maximum codec frames per SDU
 * @param _prefer_context Preferred contexts (@ref bt_audio_context)
 */
#define BT_AUDIO_CODEC_CAP_LC3(_freq, _duration, _chan_count, _len_min, _len_max,                  \
                   _max_frames_per_sdu, _prefer_context)                               \
    BT_AUDIO_CODEC_CAP(BT_HCI_CODING_FORMAT_LC3, 0x0000, 0x0000,                               \
               BT_AUDIO_CODEC_CAP_LC3_DATA(_freq, _duration, _chan_count, _len_min,    \
                               _len_max, _max_frames_per_sdu),             \
               BT_AUDIO_CODEC_CAP_LC3_META(_prefer_context))

/**
 * @brief Helper to declare LC3 codec data configuration
 *
 * @param _freq            Sampling frequency (``BT_AUDIO_CODEC_CFG_FREQ_*``)
 * @param _duration        Frame duration (``BT_AUDIO_CODEC_CFG_DURATION_*``)
 * @param _loc             Audio channel location bitfield (@ref bt_audio_location)
 * @param _len             Octets per frame (16-bit integer)
 * @param _frames_per_sdu  Frames per SDU (8-bit integer). This value is optional and will be
 *                         included only if != 1
 */
#define BT_AUDIO_CODEC_CFG_LC3_DATA(_freq, _duration, _loc, _len, _frames_per_sdu)                 \
    {                                                                                          \
        BT_AUDIO_CODEC_DATA(BT_AUDIO_CODEC_CFG_FREQ, (_freq)),                             \
        BT_AUDIO_CODEC_DATA(BT_AUDIO_CODEC_CFG_DURATION, (_duration)),                     \
        BT_AUDIO_CODEC_DATA(BT_AUDIO_CODEC_CFG_CHAN_ALLOC, BT_BYTES_LIST_LE32(_loc)),      \
        BT_AUDIO_CODEC_DATA(BT_AUDIO_CODEC_CFG_FRAME_LEN, BT_BYTES_LIST_LE16(_len)),       \
        COND_CODE_1(_frames_per_sdu, (),                                                   \
                (BT_AUDIO_CODEC_DATA(BT_AUDIO_CODEC_CFG_FRAME_BLKS_PER_SDU,            \
                         (_frames_per_sdu))))                              \
    }

/** @brief Helper to declare LC3 codec metadata configuration */
#define BT_AUDIO_CODEC_CFG_LC3_META(_stream_context)                                               \
    {                                                                                          \
        BT_AUDIO_CODEC_DATA(BT_AUDIO_METADATA_TYPE_STREAM_CONTEXT,                         \
                    BT_BYTES_LIST_LE16(_stream_context))                           \
    }

/**
 * @brief Helper to declare LC3 codec configuration.
 *
 * @param _freq            Sampling frequency (``BT_AUDIO_CODEC_CFG_FREQ_*``)
 * @param _duration        Frame duration (``BT_AUDIO_CODEC_CFG_DURATION_*``)
 * @param _loc             Audio channel location bitfield (@ref bt_audio_location)
 * @param _len             Octets per frame (16-bit integer)
 * @param _frames_per_sdu  Frames per SDU (8-bit integer)
 * @param _stream_context  Stream context (``BT_AUDIO_CONTEXT_*``)
 */
#define BT_AUDIO_CODEC_LC3_CONFIG(_freq, _duration, _loc, _len, _frames_per_sdu, _stream_context)  \
    BT_AUDIO_CODEC_CFG(                                                                        \
        BT_HCI_CODING_FORMAT_LC3, 0x0000, 0x0000,                                          \
        BT_AUDIO_CODEC_CFG_LC3_DATA(_freq, _duration, _loc, _len, _frames_per_sdu),        \
        BT_AUDIO_CODEC_CFG_LC3_META(_stream_context))

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_LC3_H_ */
