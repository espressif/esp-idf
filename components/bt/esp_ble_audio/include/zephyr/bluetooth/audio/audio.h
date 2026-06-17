/**
 * @file
 * @brief Bluetooth Audio handling
 */

/*
 * SPDX-FileCopyrightText: 2020 Intel Corporation
 * SPDX-FileCopyrightText: 2020-2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_AUDIO_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_AUDIO_H_

/**
 * @brief Bluetooth Audio
 * @defgroup bt_audio Bluetooth Audio
 * @ingroup bluetooth
 * @{
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/autoconf.h>
#include <zephyr/bluetooth/audio/lc3.h>
#include <zephyr/bluetooth/assigned_numbers.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/buf.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/iso.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/util_macro.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Size of the broadcast ID in octets */
#define BT_AUDIO_BROADCAST_ID_SIZE               3
/** Maximum broadcast ID value */
#define BT_AUDIO_BROADCAST_ID_MAX                0xFFFFFFU
/** Indicates that the server have no preference for the presentation delay */
#define BT_AUDIO_PD_PREF_NONE                    0x000000U
/** Maximum presentation delay in microseconds */
#define BT_AUDIO_PD_MAX                          0xFFFFFFU
/** Indicates that the unicast server does not have a preference for any retransmission number */
#define BT_AUDIO_RTN_PREF_NONE                   0xFFU

/** Size of the stream language value, e.g. "eng" */
#define BT_AUDIO_LANG_SIZE 3

/**
 * @brief Channel count support capability
 *
 * Macro accepts variable number of channel counts.
 * The allowed channel counts are defined by specification and have to be in range from
 * @ref BT_AUDIO_CODEC_CAP_CHAN_COUNT_MIN to @ref BT_AUDIO_CODEC_CAP_CHAN_COUNT_MAX inclusive.
 *
 * Example to support 1 and 3 channels:
 *   BT_AUDIO_CODEC_CAP_CHAN_COUNT_SUPPORT(1, 3)
 */
#define BT_AUDIO_CODEC_CAP_CHAN_COUNT_SUPPORT(...)                                                 \
    ((enum bt_audio_codec_cap_chan_count)((FOR_EACH(BIT, (|), __VA_ARGS__)) >> 1))

/** struct to hold minimum and maximum supported codec frame sizes */
struct bt_audio_codec_octets_per_codec_frame {
    /** Minimum number of octets supported per codec frame */
    uint16_t min;
    /** Maximum number of octets supported per codec frame */
    uint16_t max;
};

/**
 * @brief Helper to check whether metadata type is known by the stack.
 *
 * @note @p _type is evaluated thrice.
 */
#define BT_AUDIO_METADATA_TYPE_IS_KNOWN(_type)                                                     \
    (IN_RANGE((_type), BT_AUDIO_METADATA_TYPE_PREF_CONTEXT,                                    \
          BT_AUDIO_METADATA_TYPE_BROADCAST_IMMEDIATE) ||                                   \
     (_type) == BT_AUDIO_METADATA_TYPE_EXTENDED || (_type) == BT_AUDIO_METADATA_TYPE_VENDOR)

/**
 * @name Unicast Announcement Type
 * @{
 */
/** Unicast Server is connectable and is requesting a connection. */
#define BT_AUDIO_UNICAST_ANNOUNCEMENT_GENERAL    0x00
/** Unicast Server is connectable but is not requesting a connection. */
#define BT_AUDIO_UNICAST_ANNOUNCEMENT_TARGETED   0x01
/** @} */

/**
 * @brief Helper to declare elements of bt_audio_codec_cap arrays
 *
 * This macro is mainly for creating an array of struct bt_audio_codec_cap data arrays.
 *
 * @param _type Type of advertising data field
 * @param _bytes Variable number of single-byte parameters
 */
#define BT_AUDIO_CODEC_DATA(_type, _bytes...)                                                      \
    (sizeof((uint8_t)_type) + sizeof((uint8_t[]){_bytes})), (_type), _bytes

/**
 * @brief Helper to declare @ref bt_audio_codec_cfg
 *
 * @param _id Codec ID
 * @param _cid Company ID
 * @param _vid Vendor ID
 * @param _data Codec Specific Data in LVT format
 * @param _meta Codec Specific Metadata in LVT format
 */
#define BT_AUDIO_CODEC_CFG(_id, _cid, _vid, _data, _meta)                                          \
    ((struct bt_audio_codec_cfg){                                                              \
        /* Use HCI data path as default, can be overwritten by application */              \
        .path_id = BT_ISO_DATA_PATH_HCI,                                                   \
        .ctlr_transcode = false,                                                           \
        COND_CODE_1(IS_ENABLED(CONFIG_BT_BAP_UNICAST),                                     \
                (.target_latency = BT_AUDIO_CODEC_CFG_TARGET_LATENCY_BALANCED,         \
                 .target_phy = BT_AUDIO_CODEC_CFG_TARGET_PHY_2M,),                     \
                ())                                                                    \
        .id = _id,                                                                         \
        .cid = _cid,                                                                       \
        .vid = _vid,                                                                       \
        .data_len = sizeof((uint8_t[])_data),                                              \
        .data = _data,                                                                     \
        .meta_len = sizeof((uint8_t[])_meta),                                              \
        .meta = _meta,                                                                     \
    })

/**
 * @brief Helper to declare @ref bt_audio_codec_cap structure
 *
 * @param _id Codec ID
 * @param _cid Company ID
 * @param _vid Vendor ID
 * @param _data Codec Specific Data in LVT format
 * @param _meta Codec Specific Metadata in LVT format
 */
#define BT_AUDIO_CODEC_CAP(_id, _cid, _vid, _data, _meta)                                          \
    ((struct bt_audio_codec_cap){                                                              \
        /* Use HCI data path as default, can be overwritten by application */              \
        .path_id = BT_ISO_DATA_PATH_HCI,                                                   \
        .ctlr_transcode = false,                                                           \
        .id = (_id),                                                                       \
        .cid = (_cid),                                                                     \
        .vid = (_vid),                                                                     \
        .data_len = sizeof((uint8_t[])_data),                                              \
        .data = _data,                                                                     \
        .meta_len = sizeof((uint8_t[])_meta),                                              \
        .meta = _meta,                                                                     \
    })

/** @brief Codec capability structure. */
struct bt_audio_codec_cap {
    /** Data path ID
     *
     * @ref BT_ISO_DATA_PATH_HCI for HCI path, or any other value for
     * vendor specific ID.
     */
    uint8_t path_id;
    /** Whether or not the local controller should transcode
     *
     * This effectively sets the coding format for the ISO data path to @ref
     * BT_HCI_CODING_FORMAT_TRANSPARENT if false, else uses the @ref bt_audio_codec_cfg.id.
     */
    bool ctlr_transcode;
    /** Codec ID */
    uint8_t id;
    /** Codec Company ID */
    uint16_t cid;
    /** Codec Company Vendor ID */
    uint16_t vid;
    /** Codec Specific Capabilities Data count */
    size_t data_len;
    /** Codec Specific Capabilities Data */
    uint8_t *data;
    /** Codec Specific Capabilities Metadata count */
    size_t meta_len;
    /** Codec Specific Capabilities Metadata */
    uint8_t *meta;
};

/**
 * @brief Codec configuration target latency
 *
 * Set by the BAP Unicast Client to provide context for the BAP Unicast Server for the server to
 * set its QoS preferences.
 */
enum bt_audio_codec_cfg_target_latency {
    /** Target low latency */
    BT_AUDIO_CODEC_CFG_TARGET_LATENCY_LOW = 0x01,

    /** Target balanced latency */
    BT_AUDIO_CODEC_CFG_TARGET_LATENCY_BALANCED = 0x02,

    /** Target high latency */
    BT_AUDIO_CODEC_CFG_TARGET_LATENCY_HIGH = 0x03,
};

/**
 * @brief Codec configuration target PHY
 *
 * The target PHY to achieve the target latency (@ref bt_audio_codec_cfg_target_latency).
 */
enum bt_audio_codec_cfg_target_phy {
    /** LE 1M PHY */
    BT_AUDIO_CODEC_CFG_TARGET_PHY_1M = 0x01,

    /** LE 2M PHY */
    BT_AUDIO_CODEC_CFG_TARGET_PHY_2M = 0x02,

    /** LE Coded PHY */
    BT_AUDIO_CODEC_CFG_TARGET_PHY_CODED = 0x03,
};

/** @brief Codec specific configuration structure. */
struct bt_audio_codec_cfg {
    /** Data path ID
     *
     * @ref BT_ISO_DATA_PATH_HCI for HCI path, or any other value for
     * vendor specific ID.
     */
    uint8_t path_id;
    /** Whether or not the local controller should transcode
     *
     * This effectively sets the coding format for the ISO data path to @ref
     * BT_HCI_CODING_FORMAT_TRANSPARENT if false, else uses the @ref bt_audio_codec_cfg.id.
     */
    bool ctlr_transcode;
    /** Target latency
     *
     * Unused for broadcast streams.
     */
    enum bt_audio_codec_cfg_target_latency target_latency;
    /** Target PHY
     *
     * Unused for broadcast streams.
     */
    enum bt_audio_codec_cfg_target_phy target_phy;
    /** Codec ID */
    uint8_t  id;
    /** Codec Company ID */
    uint16_t cid;
    /** Codec Company Vendor ID */
    uint16_t vid;
    /** Codec Specific Capabilities Data count */
    size_t data_len;
    /** Codec Specific Capabilities Data */
    uint8_t *data;
    /** Codec Specific Capabilities Metadata count */
    size_t meta_len;
    /** Codec Specific Capabilities Metadata */
    uint8_t *meta;
};

/**
 * @brief Helper for parsing length-type-value data.
 *
 * @param ltv       Length-type-value (LTV) encoded data.
 * @param size      Size of the @p ltv data.
 * @param func      Callback function which will be called for each element
 *                  that's found in the data. The callback should return
 *                  true to continue parsing, or false to stop parsing.
 * @param user_data User data to be passed to the callback.
 *
 * @retval 0 All entries were parsed.
 * @retval -EINVAL The data is incorrectly encoded
 * @retval -ECANCELED Parsing was prematurely cancelled by the callback
 */
int bt_audio_data_parse(const uint8_t ltv[], size_t size,
                        bool (*func)(uint8_t type, const uint8_t *data, uint8_t data_len, void *user_data),
                        void *user_data);

/**
 * @brief Get the value of a specific data type in an length-type-value data array
 *
 * @param[in]  ltv_data The array containing the length-type-value tuples
 * @param[in]  size The size of @p ltv_data
 * @param[in]  type The type to get the value for. May be any type, but typically either
 *             @ref bt_audio_codec_cap_type, @ref bt_audio_codec_cfg_type or
 *             @ref bt_audio_metadata_type.
 * @param[out] data Pointer to the data-pointer to update when item is found.
 *                  Any found data will be little endian.
 *
 * @retval length The length of found @p data (may be 0).
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 */
int bt_audio_data_get_val(const uint8_t ltv_data[], size_t size, uint8_t type,
                          const uint8_t **data);

/**
 * @brief Function to get the number of channels from the channel allocation
 *
 * @param chan_allocation The channel allocation
 *
 * @return The number of channels
 */
uint8_t bt_audio_get_chan_count(enum bt_audio_location chan_allocation);

/** @brief Audio direction from the perspective of the BAP Unicast Server / BAP Broadcast Sink */
enum bt_audio_dir {
    /**
     * @brief Audio direction sink
     *
     * For a BAP Unicast Client or Broadcast Source this is considered outgoing audio (TX).
     * For a BAP Unicast Server or Broadcast Sink this is considered incoming audio (RX).
     */
    BT_AUDIO_DIR_SINK = 0x01,
    /**
     * @brief Audio direction source
     *
     * For a BAP Unicast Client or Broadcast Source this is considered incoming audio (RX).
     * For a BAP Unicast Server or Broadcast Sink this is considered outgoing audio (TX).
     */
    BT_AUDIO_DIR_SOURCE = 0x02,
};

/**
 * @brief Audio codec Config APIs
 * @defgroup bt_audio_codec_cfg Codec config parsing APIs
 *
 * Functions to parse codec config data when formatted as LTV wrapped into @ref bt_audio_codec_cfg.
 *
 * @{
 */

/**
 * @brief Convert assigned numbers frequency to frequency value.
 *
 * @param freq The assigned numbers frequency to convert.
 *
 * @retval frequency The converted frequency value in Hz.
 * @retval -EINVAL Arguments are invalid.
 */
int bt_audio_codec_cfg_freq_to_freq_hz(enum bt_audio_codec_cfg_freq freq);

/**
 * @brief Convert frequency value to assigned numbers frequency.
 *
 * @param freq_hz The frequency value to convert.
 *
 * @retval frequency The assigned numbers frequency (@ref bt_audio_codec_cfg_freq).
 * @retval -EINVAL Arguments are invalid.
 */
int bt_audio_codec_cfg_freq_hz_to_freq(uint32_t freq_hz);

/**
 * @brief Extract the frequency from a codec configuration.
 *
 * @param codec_cfg The codec configuration to extract data from.
 *
 * @retval frequency A @ref bt_audio_codec_cfg_freq value
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size or value
 */
int bt_audio_codec_cfg_get_freq(const struct bt_audio_codec_cfg *codec_cfg);

/**
 * @brief Set the frequency of a codec configuration.
 *
 * @param codec_cfg The codec configuration to set data for.
 * @param freq      The assigned numbers frequency to set.
 *
 * @retval data_len The @p codec_cfg.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cfg_set_freq(struct bt_audio_codec_cfg *codec_cfg,
                                enum bt_audio_codec_cfg_freq freq);

/**
 * @brief Convert assigned numbers frame duration to duration in microseconds.
 *
 * @param frame_dur The assigned numbers frame duration to convert.
 *
 * @retval duration The converted frame duration value in microseconds.
 * @retval -EINVAL Arguments are invalid.
 */
int bt_audio_codec_cfg_frame_dur_to_frame_dur_us(enum bt_audio_codec_cfg_frame_dur frame_dur);

/**
 * @brief Convert frame duration in microseconds to assigned numbers frame duration.
 *
 * @param frame_dur_us The frame duration in microseconds to convert.
 *
 * @retval duration The assigned numbers frame duration (@ref bt_audio_codec_cfg_frame_dur).
 * @retval -EINVAL Arguments are invalid.
 */
int bt_audio_codec_cfg_frame_dur_us_to_frame_dur(uint32_t frame_dur_us);

/**
 * @brief Extract frame duration from BT codec config
 *
 * @param codec_cfg The codec configuration to extract data from.
 *
 * @retval frequency A @ref bt_audio_codec_cfg_frame_dur value
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size or value
 */
int bt_audio_codec_cfg_get_frame_dur(const struct bt_audio_codec_cfg *codec_cfg);

/**
 * @brief Set the frame duration of a codec configuration.
 *
 * @param codec_cfg  The codec configuration to set data for.
 * @param frame_dur  The assigned numbers frame duration to set.
 *
 * @retval data_len The @p codec_cfg.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cfg_set_frame_dur(struct bt_audio_codec_cfg *codec_cfg,
                                     enum bt_audio_codec_cfg_frame_dur frame_dur);

/**
 * @brief Extract channel allocation from BT codec config
 *
 * The value returned is a bit field representing one or more audio locations as
 * specified by @ref bt_audio_location
 * Shall match one or more of the bits set in BT_PAC_SNK_LOC/BT_PAC_SRC_LOC.
 *
 * Up to the configured @ref BT_AUDIO_CODEC_CAP_TYPE_CHAN_COUNT number of channels can be present.
 *
 * @param codec_cfg The codec configuration to extract data from.
 * @param chan_allocation Pointer to the variable to store the extracted value in.
 * @param fallback_to_default If true this function will provide the default value of
 *        @ref BT_AUDIO_LOCATION_MONO_AUDIO if the type is not found when @p codec_cfg.id is @ref
 *        BT_HCI_CODING_FORMAT_LC3.
 *
 * @retval 0 Value is found and stored in the pointer provided
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size or value
 */
int bt_audio_codec_cfg_get_chan_allocation(const struct bt_audio_codec_cfg *codec_cfg,
                                           enum bt_audio_location *chan_allocation,
                                           bool fallback_to_default);

/**
 * @brief Set the channel allocation of a codec configuration.
 *
 * @param codec_cfg       The codec configuration to set data for.
 * @param chan_allocation The channel allocation to set.
 *
 * @retval data_len The @p codec_cfg.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cfg_set_chan_allocation(struct bt_audio_codec_cfg *codec_cfg,
                                           enum bt_audio_location chan_allocation);

/**
 * @brief Extract frame size in octets from BT codec config
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
 * @param codec_cfg The codec configuration to extract data from.
 *
 * @retval frame_length Frame length in octets
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size or value
 */
int bt_audio_codec_cfg_get_octets_per_frame(const struct bt_audio_codec_cfg *codec_cfg);

/**
 * @brief Set the octets per codec frame of a codec configuration.
 *
 * @param codec_cfg        The codec configuration to set data for.
 * @param octets_per_frame The octets per codec frame to set.
 *
 * @retval data_len The @p codec_cfg.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cfg_set_octets_per_frame(struct bt_audio_codec_cfg *codec_cfg,
                                            uint16_t octets_per_frame);

/**
 * @brief Extract number of audio frame blocks in each SDU from BT codec config
 *
 * The overall SDU size will be octets_per_frame * frame_blocks_per_sdu * number-of-channels.
 *
 * If this value is not present a default value of 1 shall be used.
 *
 * A frame block is one or more frames that represents data for the same period of time but
 * for different channels. If the stream have two audio channels and this value is two
 * there will be four frames in the SDU.
 *
 * @param codec_cfg The codec configuration to extract data from.
 * @param fallback_to_default If true this function will return the default value of 1
 *         if the type is not found when @p codec_cfg.id is @ref BT_HCI_CODING_FORMAT_LC3.
 *
 * @retval frame_blocks_per_sdu The count of codec frame blocks in each SDU.
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size or value
 */
int bt_audio_codec_cfg_get_frame_blocks_per_sdu(const struct bt_audio_codec_cfg *codec_cfg,
                                                bool fallback_to_default);

/**
 * @brief Set the frame blocks per SDU of a codec configuration.
 *
 * @param codec_cfg    The codec configuration to set data for.
 * @param frame_blocks The frame blocks per SDU to set.
 *
 * @retval data_len The @p codec_cfg.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cfg_set_frame_blocks_per_sdu(struct bt_audio_codec_cfg *codec_cfg,
                                                uint8_t frame_blocks);

/**
 * @brief Lookup a specific codec configuration value
 *
 * @param[in] codec_cfg The codec data to search in.
 * @param[in] type The type id to look for
 * @param[out] data Pointer to the data-pointer to update when item is found
 *
 * @retval len Length of found @p data (may be 0)
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 */
int bt_audio_codec_cfg_get_val(const struct bt_audio_codec_cfg *codec_cfg,
                               enum bt_audio_codec_cfg_type type, const uint8_t **data);

/**
 * @brief Set or add a specific codec configuration value
 *
 * @param codec_cfg  The codec data to set the value in.
 * @param type       The type id to set
 * @param data       Pointer to the data-pointer to set
 * @param data_len   Length of @p data
 *
 * @retval data_len The @p codec_cfg.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cfg_set_val(struct bt_audio_codec_cfg *codec_cfg,
                               enum bt_audio_codec_cfg_type type, const uint8_t *data,
                               size_t data_len);

/**
 * @brief Unset a specific codec configuration value
 *
 * The type and the value will be removed from the codec configuration.
 *
 * @param codec_cfg  The codec data to set the value in.
 * @param type       The type id to unset.
 *
 * @retval data_len The @p codec_cfg.data_len on success
 * @retval -EINVAL Arguments are invalid
 */
int bt_audio_codec_cfg_unset_val(struct bt_audio_codec_cfg *codec_cfg,
                                 enum bt_audio_codec_cfg_type type);

/**
 * @brief Lookup a specific metadata value based on type
 *
 *
 * @param[in]  codec_cfg The codec data to search in.
 * @param[in]  type      The type id to look for
 * @param[out] data      Pointer to the data-pointer to update when item is found
 *
 * @retval len Length of found @p data (may be 0)
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 */
int bt_audio_codec_cfg_meta_get_val(const struct bt_audio_codec_cfg *codec_cfg, uint8_t type,
                                    const uint8_t **data);

/**
 * @brief Set or add a specific codec configuration metadata value.
 *
 * @param codec_cfg  The codec configuration to set the value in.
 * @param type       The type id to set.
 * @param data       Pointer to the data-pointer to set.
 * @param data_len   Length of @p data.
 *
 * @retval meta_len The @p codec_cfg.meta_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cfg_meta_set_val(struct bt_audio_codec_cfg *codec_cfg,
                                    enum bt_audio_metadata_type type, const uint8_t *data,
                                    size_t data_len);

/**
 * @brief Unset a specific codec configuration metadata value
 *
 * The type and the value will be removed from the codec configuration metadata.
 *
 * @param codec_cfg  The codec data to set the value in.
 * @param type       The type id to unset.
 *
 * @retval meta_len The of @p codec_cfg.meta_len success
 * @retval -EINVAL Arguments are invalid
 */
int bt_audio_codec_cfg_meta_unset_val(struct bt_audio_codec_cfg *codec_cfg,
                                      enum bt_audio_metadata_type type);
/**
 * @brief Extract preferred contexts
 *
 * See @ref BT_AUDIO_METADATA_TYPE_PREF_CONTEXT for more information about this value.
 *
 * @param codec_cfg The codec data to search in.
 * @param fallback_to_default If true this function will provide the default value of
 *        @ref BT_AUDIO_CONTEXT_TYPE_UNSPECIFIED if the type is not found when @p codec_cfg.id is
 *        @ref BT_HCI_CODING_FORMAT_LC3.
 *
 * @retval context The preferred context type if positive or 0
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size
 */
int bt_audio_codec_cfg_meta_get_pref_context(const struct bt_audio_codec_cfg *codec_cfg,
                                             bool fallback_to_default);

/**
 * @brief Set the preferred context of a codec configuration metadata.
 *
 * @param codec_cfg The codec configuration to set data for.
 * @param ctx       The preferred context to set.
 *
 * @retval data_len The @p codec_cfg.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cfg_meta_set_pref_context(struct bt_audio_codec_cfg *codec_cfg,
                                             enum bt_audio_context ctx);

/**
 * @brief Extract stream contexts
 *
 * See @ref BT_AUDIO_METADATA_TYPE_STREAM_CONTEXT for more information about this value.
 *
 * @param codec_cfg The codec data to search in.
 *
 * @retval context The stream context type if positive or 0
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size
 */
int bt_audio_codec_cfg_meta_get_stream_context(const struct bt_audio_codec_cfg *codec_cfg);

/**
 * @brief Set the stream context of a codec configuration metadata.
 *
 * @param codec_cfg The codec configuration to set data for.
 * @param ctx       The stream context to set.
 *
 * @retval data_len The @p codec_cfg.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cfg_meta_set_stream_context(struct bt_audio_codec_cfg *codec_cfg,
                                               enum bt_audio_context ctx);

/**
 * @brief Extract program info
 *
 * See @ref BT_AUDIO_METADATA_TYPE_PROGRAM_INFO for more information about this value.
 *
 * @param[in]  codec_cfg    The codec data to search in.
 * @param[out] program_info Pointer to the UTF-8 formatted program info.
 *
 * @retval len The length of the @p program_info (may be 0)
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 */
int bt_audio_codec_cfg_meta_get_program_info(const struct bt_audio_codec_cfg *codec_cfg,
                                             const uint8_t **program_info);

/**
 * @brief Set the program info of a codec configuration metadata.
 *
 * @param codec_cfg        The codec configuration to set data for.
 * @param program_info     The program info to set.
 * @param program_info_len The length of @p program_info.
 *
 * @retval data_len The @p codec_cfg.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cfg_meta_set_program_info(struct bt_audio_codec_cfg *codec_cfg,
                                             const uint8_t *program_info, size_t program_info_len);

/**
 * @brief Extract language
 *
 * See @ref BT_AUDIO_METADATA_TYPE_LANG for more information about this value.
 *
 * @param[in]  codec_cfg The codec data to search in.
 * @param[out] lang      Pointer to the language bytes (of length BT_AUDIO_LANG_SIZE)
 *
 * @retval 0 Success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size
 */
int bt_audio_codec_cfg_meta_get_lang(const struct bt_audio_codec_cfg *codec_cfg,
                                     const uint8_t **lang);

/**
 * @brief Set the language of a codec configuration metadata.
 *
 * @param codec_cfg   The codec configuration to set data for.
 * @param lang        The 24-bit language to set.
 *
 * @retval data_len The @p codec_cfg.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cfg_meta_set_lang(struct bt_audio_codec_cfg *codec_cfg,
                                     const uint8_t lang[BT_AUDIO_LANG_SIZE]);

/**
 * @brief Extract CCID list
 *
 * See @ref BT_AUDIO_METADATA_TYPE_CCID_LIST for more information about this value.
 *
 * @param[in]  codec_cfg The codec data to search in.
 * @param[out] ccid_list Pointer to the array containing 8-bit CCIDs.
 *
 * @retval len The length of the @p ccid_list (may be 0)
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 */
int bt_audio_codec_cfg_meta_get_ccid_list(const struct bt_audio_codec_cfg *codec_cfg,
                                          const uint8_t **ccid_list);

/**
 * @brief Set the CCID list of a codec configuration metadata.
 *
 * @param codec_cfg     The codec configuration to set data for.
 * @param ccid_list     The program info to set.
 * @param ccid_list_len The length of @p ccid_list.
 *
 * @retval data_len The @p codec_cfg.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cfg_meta_set_ccid_list(struct bt_audio_codec_cfg *codec_cfg,
                                          const uint8_t *ccid_list, size_t ccid_list_len);

/**
 * @brief Extract parental rating
 *
 * See @ref BT_AUDIO_METADATA_TYPE_PARENTAL_RATING for more information about this value.
 *
 * @param codec_cfg The codec data to search in.
 *
 * @retval parental_rating The parental rating if positive or 0
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size
 */
int bt_audio_codec_cfg_meta_get_parental_rating(const struct bt_audio_codec_cfg *codec_cfg);

/**
 * @brief Set the parental rating of a codec configuration metadata.
 *
 * @param codec_cfg       The codec configuration to set data for.
 * @param parental_rating The parental rating to set.
 *
 * @retval data_len The @p codec_cfg.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cfg_meta_set_parental_rating(struct bt_audio_codec_cfg *codec_cfg,
                                                enum bt_audio_parental_rating parental_rating);

/**
 * @brief Extract program info URI
 *
 * See @ref BT_AUDIO_METADATA_TYPE_PROGRAM_INFO_URI for more information about this value.
 *
 * @param[in]  codec_cfg The codec data to search in.
 * @param[out] program_info_uri Pointer to the UTF-8 formatted program info URI.
 *
 * @retval len The length of the @p program_info_uri (may be 0)
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 */
int bt_audio_codec_cfg_meta_get_program_info_uri(const struct bt_audio_codec_cfg *codec_cfg,
                                                 const uint8_t **program_info_uri);

/**
 * @brief Set the program info URI of a codec configuration metadata.
 *
 * @param codec_cfg            The codec configuration to set data for.
 * @param program_info_uri     The program info URI to set.
 * @param program_info_uri_len The length of @p program_info_uri.
 *
 * @retval data_len The @p codec_cfg.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cfg_meta_set_program_info_uri(struct bt_audio_codec_cfg *codec_cfg,
                                                 const uint8_t *program_info_uri,
                                                 size_t program_info_uri_len);

/**
 * @brief Extract audio active state
 *
 * See @ref BT_AUDIO_METADATA_TYPE_AUDIO_STATE for more information about this value.
 *
 * @param codec_cfg The codec data to search in.
 *
 * @retval context The preferred context type if positive or 0
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size
 */
int bt_audio_codec_cfg_meta_get_audio_active_state(const struct bt_audio_codec_cfg *codec_cfg);

/**
 * @brief Set the audio active state of a codec configuration metadata.
 *
 * @param codec_cfg The codec configuration to set data for.
 * @param state     The audio active state to set.
 *
 * @retval data_len The @p codec_cfg.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cfg_meta_set_audio_active_state(struct bt_audio_codec_cfg *codec_cfg,
                                                   enum bt_audio_active_state state);

/**
 * @brief Extract broadcast audio immediate rendering flag
 *
 * See @ref BT_AUDIO_METADATA_TYPE_BROADCAST_IMMEDIATE for more information about this value.
 *
 * @param codec_cfg The codec data to search in.
 *
 * @retval 0 The flag was found
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA The flag was not found
 */
int bt_audio_codec_cfg_meta_get_bcast_audio_immediate_rend_flag(
    const struct bt_audio_codec_cfg *codec_cfg);

/**
 * @brief Set the broadcast audio immediate rendering flag of a codec configuration metadata.
 *
 * @param codec_cfg The codec configuration to set data for.
 *
 * @retval data_len The @p codec_cfg.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cfg_meta_set_bcast_audio_immediate_rend_flag(
    struct bt_audio_codec_cfg *codec_cfg);

/**
 * @brief Extract assisted listening stream
 *
 * See @ref BT_AUDIO_METADATA_TYPE_ASSISTED_LISTENING_STREAM for more information about this value.
 *
 * @param codec_cfg The codec data to search in.
 *
 * @retval value The assisted listening stream value if positive or 0
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size
 */
int bt_audio_codec_cfg_meta_get_assisted_listening_stream(
    const struct bt_audio_codec_cfg *codec_cfg);

/**
 * @brief Set the assisted listening stream value of a codec configuration metadata.
 *
 * @param codec_cfg The codec configuration to set data for.
 * @param val       The assisted listening stream value to set.
 *
 * @retval data_len The @p codec_cap.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cfg_meta_set_assisted_listening_stream(
    struct bt_audio_codec_cfg *codec_cfg, enum bt_audio_assisted_listening_stream val);

/**
 * @brief Extract broadcast name
 *
 * See @ref BT_AUDIO_METADATA_TYPE_BROADCAST_NAME for more information about this value.
 *
 * @param[in]  codec_cfg      The codec data to search in.
 * @param[out] broadcast_name Pointer to the UTF-8 formatted broadcast name.
 *
 * @retval length The length of the @p broadcast_name (may be 0)
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 */
int bt_audio_codec_cfg_meta_get_broadcast_name(const struct bt_audio_codec_cfg *codec_cfg,
                                               const uint8_t **broadcast_name);

/**
 * @brief Set the broadcast name of a codec configuration metadata.
 *
 * @param codec_cfg          The codec configuration to set data for.
 * @param broadcast_name     The broadcast name to set.
 * @param broadcast_name_len The length of @p broadcast_name.
 *
 * @retval data_len The @p codec_cfg.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cfg_meta_set_broadcast_name(struct bt_audio_codec_cfg *codec_cfg,
                                               const uint8_t *broadcast_name,
                                               size_t broadcast_name_len);

/**
 * @brief Extract extended metadata
 *
 * See @ref BT_AUDIO_METADATA_TYPE_EXTENDED for more information about this value.
 *
 * @param[in]  codec_cfg     The codec data to search in.
 * @param[out] extended_meta Pointer to the extended metadata.
 *
 * @retval len The length of the @p extended_meta (may be 0)
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 */
int bt_audio_codec_cfg_meta_get_extended(const struct bt_audio_codec_cfg *codec_cfg,
                                         const uint8_t **extended_meta);

/**
 * @brief Set the extended metadata of a codec configuration metadata.
 *
 * @param codec_cfg         The codec configuration to set data for.
 * @param extended_meta     The extended metadata to set.
 * @param extended_meta_len The length of @p extended_meta.
 *
 * @retval data_len The @p codec_cfg.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cfg_meta_set_extended(struct bt_audio_codec_cfg *codec_cfg,
                                         const uint8_t *extended_meta, size_t extended_meta_len);

/**
 * @brief Extract vendor specific metadata
 *
 * See @ref BT_AUDIO_METADATA_TYPE_VENDOR for more information about this value.
 *
 * @param[in]  codec_cfg   The codec data to search in.
 * @param[out] vendor_meta Pointer to the vendor specific metadata.
 *
 * @retval len The length of the @p vendor_meta (may be 0)
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 */
int bt_audio_codec_cfg_meta_get_vendor(const struct bt_audio_codec_cfg *codec_cfg,
                                       const uint8_t **vendor_meta);

/**
 * @brief Set the vendor specific metadata of a codec configuration metadata.
 *
 * @param codec_cfg       The codec configuration to set data for.
 * @param vendor_meta     The vendor specific metadata to set.
 * @param vendor_meta_len The length of @p vendor_meta.
 *
 * @retval data_len The @p codec_cfg.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cfg_meta_set_vendor(struct bt_audio_codec_cfg *codec_cfg,
                                       const uint8_t *vendor_meta, size_t vendor_meta_len);
/** @} */ /* End of bt_audio_codec_cfg */

/**
 * @brief Audio codec capabilities APIs
 * @defgroup bt_audio_codec_cap Codec capability parsing APIs
 *
 * Functions to parse codec capability data when formatted as LTV wrapped into @ref
 * bt_audio_codec_cap.
 *
 * @{
 */

/**
 * @brief Lookup a specific value based on type
 *
 * @param[in]  codec_cap The codec data to search in.
 * @param[in]  type The type id to look for
 * @param[out] data Pointer to the data-pointer to update when item is found
 *
 * @retval len Length of found @p data (may be 0)
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 */
int bt_audio_codec_cap_get_val(const struct bt_audio_codec_cap *codec_cap,
                               enum bt_audio_codec_cap_type type, const uint8_t **data);

/**
 * @brief Set or add a specific codec capability value
 *
 * @param codec_cap  The codec data to set the value in.
 * @param type       The type id to set
 * @param data       Pointer to the data-pointer to set
 * @param data_len   Length of @p data
 *
 * @retval data_len The @p codec_cap.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cap_set_val(struct bt_audio_codec_cap *codec_cap,
                               enum bt_audio_codec_cap_type type, const uint8_t *data,
                               size_t data_len);

/**
 * @brief Unset a specific codec capability value
 *
 * The type and the value will be removed from the codec capability.
 *
 * @param codec_cap  The codec data to set the value in.
 * @param type       The type id to unset.
 *
 * @retval data_len The @p codec_cap.data_len on success
 * @retval -EINVAL Arguments are invalid
 */
int bt_audio_codec_cap_unset_val(struct bt_audio_codec_cap *codec_cap,
                                 enum bt_audio_codec_cap_type type);

/**
 * @brief Extract the frequency from a codec capability.
 *
 * @param codec_cap The codec capabilities to extract data from.
 *
 * @retval frequencies Bitfield of supported frequencies (@ref bt_audio_codec_cap_freq) if 0 or
 *                     positive
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size or value
 */
int bt_audio_codec_cap_get_freq(const struct bt_audio_codec_cap *codec_cap);

/**
 * @brief Set the supported frequencies of a codec capability.
 *
 * @param codec_cap The codec capabilities to set data for.
 * @param freq      The supported frequencies to set.
 *
 * @retval data_len The @p codec_cap.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cap_set_freq(struct bt_audio_codec_cap *codec_cap,
                                enum bt_audio_codec_cap_freq freq);

/**
 * @brief Extract the frequency from a codec capability.
 *
 * @param codec_cap The codec capabilities to extract data from.
 *
 * @retval durations Bitfield of supported frame durations if 0 or positive
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size or value
 */
int bt_audio_codec_cap_get_frame_dur(const struct bt_audio_codec_cap *codec_cap);

/**
 * @brief Set the frame duration of a codec capability.
 *
 * @param codec_cap The codec capabilities to set data for.
 * @param frame_dur The frame duration to set.
 *
 * @retval data_len The @p codec_cap.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cap_set_frame_dur(struct bt_audio_codec_cap *codec_cap,
                                     enum bt_audio_codec_cap_frame_dur frame_dur);

/**
 * @brief Extract the frequency from a codec capability.
 *
 * @param codec_cap The codec capabilities to extract data from.
 * @param fallback_to_default If true this function will provide the default value of 1
 *        if the type is not found when @p codec_cap.id is @ref BT_HCI_CODING_FORMAT_LC3.
 *
 * @retval channel_counts Number of supported channel counts if 0 or positive
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size or value
 */
int bt_audio_codec_cap_get_supported_audio_chan_counts(const struct bt_audio_codec_cap *codec_cap,
                                                       bool fallback_to_default);

/**
 * @brief Set the channel count of a codec capability.
 *
 * @param codec_cap The codec capabilities to set data for.
 * @param chan_count The channel count frequency to set.
 *
 * @retval data_len The @p codec_cap.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cap_set_supported_audio_chan_counts(
    struct bt_audio_codec_cap *codec_cap, enum bt_audio_codec_cap_chan_count chan_count);

/**
 * @brief Extract the supported octets per codec frame from a codec capability.
 *
 * @param[in]  codec_cap   The codec capabilities to extract data from.
 * @param[out] codec_frame Struct to place the resulting values in
 *
 * @retval 0 Success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size or value
 */
int bt_audio_codec_cap_get_octets_per_frame(
    const struct bt_audio_codec_cap *codec_cap,
    struct bt_audio_codec_octets_per_codec_frame *codec_frame);

/**
 * @brief Set the octets per codec frame of a codec capability.
 *
 * @param codec_cap   The codec capabilities to set data for.
 * @param codec_frame The octets per codec frame to set.
 *
 * @retval data_len The @p codec_cap.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cap_set_octets_per_frame(
    struct bt_audio_codec_cap *codec_cap,
    const struct bt_audio_codec_octets_per_codec_frame *codec_frame);

/**
 * @brief Extract the maximum codec frames per SDU from a codec capability.
 *
 * @param codec_cap The codec capabilities to extract data from.
 * @param fallback_to_default If true this function will provide the default value of 1
 *        if the type is not found when @p codec_cap.id is @ref BT_HCI_CODING_FORMAT_LC3.
 *
 * @retval codec_frames_per_sdu Maximum number of codec frames per SDU supported
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size or value
 */
int bt_audio_codec_cap_get_max_codec_frames_per_sdu(const struct bt_audio_codec_cap *codec_cap,
                                                    bool fallback_to_default);

/**
 * @brief Set the maximum codec frames per SDU of a codec capability.
 *
 * @param codec_cap            The codec capabilities to set data for.
 * @param codec_frames_per_sdu The maximum codec frames per SDU to set.
 *
 * @retval data_len The @p codec_cap.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cap_set_max_codec_frames_per_sdu(struct bt_audio_codec_cap *codec_cap,
                                                    uint8_t codec_frames_per_sdu);

/**
 * @brief Lookup a specific metadata value based on type
 *
 * @param[in]  codec_cap The codec data to search in.
 * @param[in]  type      The type id to look for
 * @param[out] data      Pointer to the data-pointer to update when item is found
 *
 * @retval len Length of found @p data (may be 0)
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 */
int bt_audio_codec_cap_meta_get_val(const struct bt_audio_codec_cap *codec_cap, uint8_t type,
                                    const uint8_t **data);

/**
 * @brief Set or add a specific codec capability metadata value.
 *
 * @param codec_cap  The codec capability to set the value in.
 * @param type       The type id to set.
 * @param data       Pointer to the data-pointer to set.
 * @param data_len   Length of @p data.
 *
 * @retval meta_len The @p codec_cap.meta_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cap_meta_set_val(struct bt_audio_codec_cap *codec_cap,
                                    enum bt_audio_metadata_type type, const uint8_t *data,
                                    size_t data_len);

/**
 * @brief Unset a specific codec capability metadata value
 *
 * The type and the value will be removed from the codec capability metadata.
 *
 * @param codec_cap  The codec data to set the value in.
 * @param type       The type id to unset.
 *
 * @retval meta_len The of @p codec_cap.meta_len on success
 * @retval -EINVAL Arguments are invalid
 */
int bt_audio_codec_cap_meta_unset_val(struct bt_audio_codec_cap *codec_cap,
                                      enum bt_audio_metadata_type type);

/**
 * @brief Extract preferred contexts
 *
 * See @ref BT_AUDIO_METADATA_TYPE_PREF_CONTEXT for more information about this value.
 *
 * @param codec_cap The codec data to search in.
 *
 * @retval The preferred context type if positive or 0
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size
 */
int bt_audio_codec_cap_meta_get_pref_context(const struct bt_audio_codec_cap *codec_cap);

/**
 * @brief Set the preferred context of a codec capability metadata.
 *
 * @param codec_cap The codec capability to set data for.
 * @param ctx       The preferred context to set.
 *
 * @retval data_len The @p codec_cap.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cap_meta_set_pref_context(struct bt_audio_codec_cap *codec_cap,
                                             enum bt_audio_context ctx);

/**
 * @brief Extract stream contexts
 *
 * See @ref BT_AUDIO_METADATA_TYPE_STREAM_CONTEXT for more information about this value.
 *
 * @param codec_cap The codec data to search in.
 *
 * @retval context The stream context type if positive or 0
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size
 */
int bt_audio_codec_cap_meta_get_stream_context(const struct bt_audio_codec_cap *codec_cap);

/**
 * @brief Set the stream context of a codec capability metadata.
 *
 * @param codec_cap The codec capability to set data for.
 * @param ctx       The stream context to set.
 *
 * @retval data_len The @p codec_cap.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cap_meta_set_stream_context(struct bt_audio_codec_cap *codec_cap,
                                               enum bt_audio_context ctx);

/**
 * @brief Extract program info
 *
 * See @ref BT_AUDIO_METADATA_TYPE_PROGRAM_INFO for more information about this value.
 *
 * @param[in]  codec_cap    The codec data to search in.
 * @param[out] program_info Pointer to the UTF-8 formatted program info.
 *
 * @retval len The length of the @p program_info (may be 0)
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 */
int bt_audio_codec_cap_meta_get_program_info(const struct bt_audio_codec_cap *codec_cap,
                                             const uint8_t **program_info);

/**
 * @brief Set the program info of a codec capability metadata.
 *
 * @param codec_cap        The codec capability to set data for.
 * @param program_info     The program info to set.
 * @param program_info_len The length of @p program_info.
 *
 * @retval data_len The @p codec_cap.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cap_meta_set_program_info(struct bt_audio_codec_cap *codec_cap,
                                             const uint8_t *program_info, size_t program_info_len);

/**
 * @brief Extract language
 *
 * See @ref BT_AUDIO_METADATA_TYPE_LANG for more information about this value.
 *
 * @param[in]  codec_cap The codec data to search in.
 * @param[out] lang      Pointer to the language bytes (of length BT_AUDIO_LANG_SIZE)
 *
 * @retval 0 Success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size
 */
int bt_audio_codec_cap_meta_get_lang(const struct bt_audio_codec_cap *codec_cap,
                                     const uint8_t **lang);

/**
 * @brief Set the language of a codec capability metadata.
 *
 * @param codec_cap   The codec capability to set data for.
 * @param lang        The 24-bit language to set.
 *
 * @retval data_len The @p codec_cap.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cap_meta_set_lang(struct bt_audio_codec_cap *codec_cap,
                                     const uint8_t lang[BT_AUDIO_LANG_SIZE]);

/**
 * @brief Extract CCID list
 *
 * See @ref BT_AUDIO_METADATA_TYPE_CCID_LIST for more information about this value.
 *
 * @param[in]  codec_cap The codec data to search in.
 * @param[out] ccid_list Pointer to the array containing 8-bit CCIDs.
 *
 * @retval len The length of the @p ccid_list (may be 0)
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 */
int bt_audio_codec_cap_meta_get_ccid_list(const struct bt_audio_codec_cap *codec_cap,
                                          const uint8_t **ccid_list);

/**
 * @brief Set the CCID list of a codec capability metadata.
 *
 * @param codec_cap     The codec capability to set data for.
 * @param ccid_list     The program info to set.
 * @param ccid_list_len The length of @p ccid_list.
 *
 * @retval data_len The @p codec_cap.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cap_meta_set_ccid_list(struct bt_audio_codec_cap *codec_cap,
                                          const uint8_t *ccid_list, size_t ccid_list_len);

/**
 * @brief Extract parental rating
 *
 * See @ref BT_AUDIO_METADATA_TYPE_PARENTAL_RATING for more information about this value.
 *
 * @param codec_cap The codec data to search in.
 *
 * @retval The parental rating if positive or 0
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size
 */
int bt_audio_codec_cap_meta_get_parental_rating(const struct bt_audio_codec_cap *codec_cap);

/**
 * @brief Set the parental rating of a codec capability metadata.
 *
 * @param codec_cap       The codec capability to set data for.
 * @param parental_rating The parental rating to set.
 *
 * @retval data_len The @p codec_cap.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cap_meta_set_parental_rating(struct bt_audio_codec_cap *codec_cap,
                                                enum bt_audio_parental_rating parental_rating);

/**
 * @brief Extract program info URI
 *
 * See @ref BT_AUDIO_METADATA_TYPE_PROGRAM_INFO_URI for more information about this value.
 *
 * @param[in]  codec_cap        The codec data to search in.
 * @param[out] program_info_uri Pointer to the UTF-8 formatted program info URI.
 *
 * @retval len The length of the @p program_info_uri (may be 0)
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 */
int bt_audio_codec_cap_meta_get_program_info_uri(const struct bt_audio_codec_cap *codec_cap,
                                                 const uint8_t **program_info_uri);

/**
 * @brief Set the program info URI of a codec capability metadata.
 *
 * @param codec_cap            The codec capability to set data for.
 * @param program_info_uri     The program info URI to set.
 * @param program_info_uri_len The length of @p program_info_uri.
 *
 * @retval data_len The @p codec_cap.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cap_meta_set_program_info_uri(struct bt_audio_codec_cap *codec_cap,
                                                 const uint8_t *program_info_uri,
                                                 size_t program_info_uri_len);

/**
 * @brief Extract audio active state
 *
 * See @ref BT_AUDIO_METADATA_TYPE_AUDIO_STATE for more information about this value.
 *
 * @param codec_cap The codec data to search in.
 *
 * @retval context The preferred context type if positive or 0
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size
 */
int bt_audio_codec_cap_meta_get_audio_active_state(const struct bt_audio_codec_cap *codec_cap);

/**
 * @brief Set the audio active state of a codec capability metadata.
 *
 * @param codec_cap The codec capability to set data for.
 * @param state     The audio active state to set.
 *
 * @retval data_len The @p codec_cap.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cap_meta_set_audio_active_state(struct bt_audio_codec_cap *codec_cap,
                                                   enum bt_audio_active_state state);

/**
 * @brief Extract broadcast audio immediate rendering flag
 *
 * See @ref BT_AUDIO_METADATA_TYPE_BROADCAST_IMMEDIATE for more information about this value.
 *
 * @param codec_cap The codec data to search in.
 *
 * @retval 0 The flag was found
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA The flag was not found
 */
int bt_audio_codec_cap_meta_get_bcast_audio_immediate_rend_flag(
    const struct bt_audio_codec_cap *codec_cap);

/**
 * @brief Set the broadcast audio immediate rendering flag of a codec capability metadata.
 *
 * @param codec_cap The codec capability to set data for.
 *
 * @retval data_len The @p codec_cap.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cap_meta_set_bcast_audio_immediate_rend_flag(
    struct bt_audio_codec_cap *codec_cap);

/**
 * @brief Extract assisted listening stream
 *
 * See @ref BT_AUDIO_METADATA_TYPE_ASSISTED_LISTENING_STREAM for more information about this value.
 *
 * @param codec_cap The codec data to search in.
 *
 * @retval value The assisted listening stream value if positive or 0
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 * @retval -EBADMSG The found value has invalid size
 */
int bt_audio_codec_cap_meta_get_assisted_listening_stream(
    const struct bt_audio_codec_cap *codec_cap);

/**
 * @brief Set the assisted listening stream value of a codec capability metadata.
 *
 * @param codec_cap The codec capability to set data for.
 * @param val       The assisted listening stream value to set.
 *
 * @retval data_len The @p codec_cap.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cap_meta_set_assisted_listening_stream(
    struct bt_audio_codec_cap *codec_cap, enum bt_audio_assisted_listening_stream val);

/**
 * @brief Extract broadcast name
 *
 * See @ref BT_AUDIO_METADATA_TYPE_BROADCAST_NAME for more information about this value.
 *
 * @param[in]  codec_cap      The codec data to search in.
 * @param[out] broadcast_name Pointer to the UTF-8 formatted broadcast name.
 *
 * @retval length The length of the @p broadcast_name (may be 0)
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 */
int bt_audio_codec_cap_meta_get_broadcast_name(const struct bt_audio_codec_cap *codec_cap,
                                               const uint8_t **broadcast_name);

/**
 * @brief Set the broadcast name of a codec capability metadata.
 *
 * @param codec_cap          The codec capability to set data for.
 * @param broadcast_name     The broadcast name to set.
 * @param broadcast_name_len The length of @p broadcast_name.
 *
 * @retval data_len The @p codec_cap.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cap_meta_set_broadcast_name(struct bt_audio_codec_cap *codec_cap,
                                               const uint8_t *broadcast_name,
                                               size_t broadcast_name_len);
/**
 * @brief Extract extended metadata
 *
 * See @ref BT_AUDIO_METADATA_TYPE_EXTENDED for more information about this value.
 *
 * @param[in]  codec_cap     The codec data to search in.
 * @param[out] extended_meta Pointer to the extended metadata.
 *
 * @retval len The length of the @p extended_meta (may be 0)
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 */
int bt_audio_codec_cap_meta_get_extended(const struct bt_audio_codec_cap *codec_cap,
                                         const uint8_t **extended_meta);

/**
 * @brief Set the extended metadata of a codec capability metadata.
 *
 * @param codec_cap         The codec capability to set data for.
 * @param extended_meta     The extended metadata to set.
 * @param extended_meta_len The length of @p extended_meta.
 *
 * @retval data_len The @p codec_cap.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cap_meta_set_extended(struct bt_audio_codec_cap *codec_cap,
                                         const uint8_t *extended_meta, size_t extended_meta_len);

/**
 * @brief Extract vendor specific metadata
 *
 * See @ref BT_AUDIO_METADATA_TYPE_VENDOR for more information about this value.
 *
 * @param[in]  codec_cap   The codec data to search in.
 * @param[out] vendor_meta Pointer to the vendor specific metadata.
 *
 * @retval len The length of the @p vendor_meta (may be 0)
 * @retval -EINVAL Arguments are invalid
 * @retval -ENODATA Data not found
 */
int bt_audio_codec_cap_meta_get_vendor(const struct bt_audio_codec_cap *codec_cap,
                                       const uint8_t **vendor_meta);

/**
 * @brief Set the vendor specific metadata of a codec capability metadata.
 *
 * @param codec_cap       The codec capability to set data for.
 * @param vendor_meta     The vendor specific metadata to set.
 * @param vendor_meta_len The length of @p vendor_meta.
 *
 * @retval data_len The @p codec_cap.data_len on success
 * @retval -EINVAL Arguments are invalid
 * @retval -ENOMEM The new value could not be set or added due to lack of memory
 */
int bt_audio_codec_cap_meta_set_vendor(struct bt_audio_codec_cap *codec_cap,
                                       const uint8_t *vendor_meta, size_t vendor_meta_len);

/** @} */ /* End of bt_audio_codec_cap */

/**
 * @brief Assigned numbers to string API
 * @defgroup bt_audio_to_str Assigned numbers to string API
 *
 * Functions to return string representation of Bluetooth Audio assigned number values.
 *
 * @{
 */

/**
 * @brief Returns a string representation of a specific @ref bt_audio_context bit
 *
 * If @p context contains multiple bits, it will return "Unknown context"
 *
 * @param context A single context bit
 *
 * @return String representation of the supplied bit
 */
static inline char *bt_audio_context_bit_to_str(enum bt_audio_context context)
{
    switch (context) {
    case BT_AUDIO_CONTEXT_TYPE_NONE:
        return "None";
    case BT_AUDIO_CONTEXT_TYPE_UNSPECIFIED:
        return "Unspecified";
    case BT_AUDIO_CONTEXT_TYPE_CONVERSATIONAL:
        return "Conversational";
    case BT_AUDIO_CONTEXT_TYPE_MEDIA:
        return "Media";
    case BT_AUDIO_CONTEXT_TYPE_GAME:
        return "Game";
    case BT_AUDIO_CONTEXT_TYPE_INSTRUCTIONAL:
        return "Instructional";
    case BT_AUDIO_CONTEXT_TYPE_VOICE_ASSISTANTS:
        return "Voice assistant";
    case BT_AUDIO_CONTEXT_TYPE_LIVE:
        return "Live";
    case BT_AUDIO_CONTEXT_TYPE_SOUND_EFFECTS:
        return "Sound effects";
    case BT_AUDIO_CONTEXT_TYPE_NOTIFICATIONS:
        return "Notifications";
    case BT_AUDIO_CONTEXT_TYPE_RINGTONE:
        return "Ringtone";
    case BT_AUDIO_CONTEXT_TYPE_ALERTS:
        return "Alerts";
    case BT_AUDIO_CONTEXT_TYPE_EMERGENCY_ALARM:
        return "Emergency alarm";
    default:
        return "Unknown context";
    }
}

/**
 * @brief Returns a string representation of a @ref bt_audio_parental_rating value
 *
 * @param parental_rating The parental rating value
 *
 * @return String representation of the supplied parental rating value
 */
static inline char *bt_audio_parental_rating_to_str(enum bt_audio_parental_rating parental_rating)
{
    switch (parental_rating) {
    case BT_AUDIO_PARENTAL_RATING_NO_RATING:
        return "No rating";
    case BT_AUDIO_PARENTAL_RATING_AGE_ANY:
        return "Any";
    case BT_AUDIO_PARENTAL_RATING_AGE_5_OR_ABOVE:
        return "Age 5 or above";
    case BT_AUDIO_PARENTAL_RATING_AGE_6_OR_ABOVE:
        return "Age 6 or above";
    case BT_AUDIO_PARENTAL_RATING_AGE_7_OR_ABOVE:
        return "Age 7 or above";
    case BT_AUDIO_PARENTAL_RATING_AGE_8_OR_ABOVE:
        return "Age 8 or above";
    case BT_AUDIO_PARENTAL_RATING_AGE_9_OR_ABOVE:
        return "Age 9 or above";
    case BT_AUDIO_PARENTAL_RATING_AGE_10_OR_ABOVE:
        return "Age 10 or above";
    case BT_AUDIO_PARENTAL_RATING_AGE_11_OR_ABOVE:
        return "Age 11 or above";
    case BT_AUDIO_PARENTAL_RATING_AGE_12_OR_ABOVE:
        return "Age 12 or above";
    case BT_AUDIO_PARENTAL_RATING_AGE_13_OR_ABOVE:
        return "Age 13 or above";
    case BT_AUDIO_PARENTAL_RATING_AGE_14_OR_ABOVE:
        return "Age 14 or above";
    case BT_AUDIO_PARENTAL_RATING_AGE_15_OR_ABOVE:
        return "Age 15 or above";
    case BT_AUDIO_PARENTAL_RATING_AGE_16_OR_ABOVE:
        return "Age 16 or above";
    case BT_AUDIO_PARENTAL_RATING_AGE_17_OR_ABOVE:
        return "Age 17 or above";
    case BT_AUDIO_PARENTAL_RATING_AGE_18_OR_ABOVE:
        return "Age 18 or above";
    default:
        return "Unknown rating";
    }
}

/**
 * @brief Returns a string representation of a @ref bt_audio_active_state value
 *
 * @param state The active state value
 *
 * @return String representation of the supplied active state value
 */
static inline char *bt_audio_active_state_to_str(enum bt_audio_active_state state)
{
    switch (state) {
    case BT_AUDIO_ACTIVE_STATE_DISABLED:
        return "Disabled";
    case BT_AUDIO_ACTIVE_STATE_ENABLED:
        return "Enabled";
    default:
        return "Unknown active state";
    }
}

/**
 * @brief Returns a string representation of a specific @ref bt_audio_codec_cap_freq bit
 *
 * If @p freq contains multiple bits, it will return "Unknown supported frequency"
 *
 * @param freq A single frequency bit
 *
 * @return String representation of the supplied bit
 */
static inline char *bt_audio_codec_cap_freq_bit_to_str(enum bt_audio_codec_cap_freq freq)
{
    switch (freq) {
    case BT_AUDIO_CODEC_CAP_FREQ_8KHZ:
        return "8000 Hz";
    case BT_AUDIO_CODEC_CAP_FREQ_11KHZ:
        return "11025 Hz";
    case BT_AUDIO_CODEC_CAP_FREQ_16KHZ:
        return "16000 Hz";
    case BT_AUDIO_CODEC_CAP_FREQ_22KHZ:
        return "22050 Hz";
    case BT_AUDIO_CODEC_CAP_FREQ_24KHZ:
        return "24000 Hz";
    case BT_AUDIO_CODEC_CAP_FREQ_32KHZ:
        return "32000 Hz";
    case BT_AUDIO_CODEC_CAP_FREQ_44KHZ:
        return "44100 Hz";
    case BT_AUDIO_CODEC_CAP_FREQ_48KHZ:
        return "48000 Hz";
    case BT_AUDIO_CODEC_CAP_FREQ_88KHZ:
        return "88200 Hz";
    case BT_AUDIO_CODEC_CAP_FREQ_96KHZ:
        return "96000 Hz";
    case BT_AUDIO_CODEC_CAP_FREQ_176KHZ:
        return "176400 Hz";
    case BT_AUDIO_CODEC_CAP_FREQ_192KHZ:
        return "192000 Hz";
    case BT_AUDIO_CODEC_CAP_FREQ_384KHZ:
        return "384000 Hz";
    default:
        return "Unknown supported frequency";
    }
}

/**
 * @brief Returns a string representation of a specific @ref bt_audio_codec_cap_frame_dur bit
 *
 * If @p frame_dur contains multiple bits, it will return "Unknown frame duration"
 *
 * @param frame_dur A single frame duration bit
 *
 * @return String representation of the supplied bit
 */
static inline char *
bt_audio_codec_cap_frame_dur_bit_to_str(enum bt_audio_codec_cap_frame_dur frame_dur)
{
    switch (frame_dur) {
    case BT_AUDIO_CODEC_CAP_DURATION_7_5:
        return "7.5 ms";
    case BT_AUDIO_CODEC_CAP_DURATION_10:
        return "10 ms";
    case BT_AUDIO_CODEC_CAP_DURATION_PREFER_7_5:
        return "7.5 ms preferred";
    case BT_AUDIO_CODEC_CAP_DURATION_PREFER_10:
        return "10 ms preferred";
    default:
        return "Unknown frame duration";
    }
}

/**
 * @brief Returns a string representation of a specific @ref bt_audio_codec_cap_chan_count bit
 *
 * If @p chan_count contains multiple bits, it will return "Unknown channel count"
 *
 * @param chan_count A single frame channel count bit
 *
 * @return String representation of the supplied bit
 */
static inline char *
bt_audio_codec_cap_chan_count_bit_to_str(enum bt_audio_codec_cap_chan_count chan_count)
{
    switch (chan_count) {
    case BT_AUDIO_CODEC_CAP_CHAN_COUNT_1:
        return "1 channel";
    case BT_AUDIO_CODEC_CAP_CHAN_COUNT_2:
        return "2 channels";
    case BT_AUDIO_CODEC_CAP_CHAN_COUNT_3:
        return "3 channels";
    case BT_AUDIO_CODEC_CAP_CHAN_COUNT_4:
        return "4 channels";
    case BT_AUDIO_CODEC_CAP_CHAN_COUNT_5:
        return "5 channels";
    case BT_AUDIO_CODEC_CAP_CHAN_COUNT_6:
        return "6 channels";
    case BT_AUDIO_CODEC_CAP_CHAN_COUNT_7:
        return "7 channels";
    case BT_AUDIO_CODEC_CAP_CHAN_COUNT_8:
        return "8 channels";
    default:
        return "Unknown channel count";
    }
}

/**
 * @brief Returns a string representation of a specific @ref bt_audio_location bit
 *
 * If @p location contains multiple bits, it will return "Unknown location"
 *
 * @param location A single location bit
 *
 * @return String representation of the supplied bit
 */
static inline char *bt_audio_location_bit_to_str(enum bt_audio_location location)
{
    switch (location) {
    case BT_AUDIO_LOCATION_MONO_AUDIO:
        return "Mono";
    case BT_AUDIO_LOCATION_FRONT_LEFT:
        return "Front left";
    case BT_AUDIO_LOCATION_FRONT_RIGHT:
        return "Front right";
    case BT_AUDIO_LOCATION_FRONT_CENTER:
        return "Front center";
    case BT_AUDIO_LOCATION_LOW_FREQ_EFFECTS_1:
        return "Low frequency effects 1";
    case BT_AUDIO_LOCATION_BACK_LEFT:
        return "Back left";
    case BT_AUDIO_LOCATION_BACK_RIGHT:
        return "Back right";
    case BT_AUDIO_LOCATION_FRONT_LEFT_OF_CENTER:
        return "Front left of center";
    case BT_AUDIO_LOCATION_FRONT_RIGHT_OF_CENTER:
        return "Front right of center";
    case BT_AUDIO_LOCATION_BACK_CENTER:
        return "Back center";
    case BT_AUDIO_LOCATION_LOW_FREQ_EFFECTS_2:
        return "Low frequency effects 2";
    case BT_AUDIO_LOCATION_SIDE_LEFT:
        return "Side left";
    case BT_AUDIO_LOCATION_SIDE_RIGHT:
        return "Side right";
    case BT_AUDIO_LOCATION_TOP_FRONT_LEFT:
        return "Top front left";
    case BT_AUDIO_LOCATION_TOP_FRONT_RIGHT:
        return "Top front right";
    case BT_AUDIO_LOCATION_TOP_FRONT_CENTER:
        return "Top front center";
    case BT_AUDIO_LOCATION_TOP_CENTER:
        return "Top center";
    case BT_AUDIO_LOCATION_TOP_BACK_LEFT:
        return "Top back left";
    case BT_AUDIO_LOCATION_TOP_BACK_RIGHT:
        return "Top back right";
    case BT_AUDIO_LOCATION_TOP_SIDE_LEFT:
        return "Top side left";
    case BT_AUDIO_LOCATION_TOP_SIDE_RIGHT:
        return "Top side right";
    case BT_AUDIO_LOCATION_TOP_BACK_CENTER:
        return "Top back center";
    case BT_AUDIO_LOCATION_BOTTOM_FRONT_CENTER:
        return "Bottom front center";
    case BT_AUDIO_LOCATION_BOTTOM_FRONT_LEFT:
        return "Bottom front left";
    case BT_AUDIO_LOCATION_BOTTOM_FRONT_RIGHT:
        return "Bottom front right";
    case BT_AUDIO_LOCATION_FRONT_LEFT_WIDE:
        return "Front left wide";
    case BT_AUDIO_LOCATION_FRONT_RIGHT_WIDE:
        return "Front right wde";
    case BT_AUDIO_LOCATION_LEFT_SURROUND:
        return "Left surround";
    case BT_AUDIO_LOCATION_RIGHT_SURROUND:
        return "Right surround";
    default:
        return "Unknown location";
    }
}

/** @} */ /* End of bt_audio_to_str */
#ifdef __cplusplus
}
#endif

/** @} */ /* end of bt_audio */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_H_ */
