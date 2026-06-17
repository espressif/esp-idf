/**
 * @file
 * @brief Public Broadcast Profile (PBP) APIs.
 */
/*
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_PBP_
#define ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_PBP_

/**
 * @brief Public Broadcast Profile (PBP)
 *
 * @defgroup bt_pbp Public Broadcast Profile (PBP)
 *
 * @since 3.5
 * @version 0.8.0
 *
 * @ingroup bluetooth
 * @{
 *
 * The Public Broadcast Profile (PBP) is used for public broadcasts by providing additional
 * information in the advertising data.
 */
#include <stddef.h>
#include <stdint.h>

#include <zephyr/bluetooth/audio/audio.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/net_buf.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/util_macro.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Minimum size of the Public Broadcast Announcement
 *
 * It contains the Public Broadcast Announcement UUID (2), the Public Broadcast Announcement
 * features (1) and the metadata length (1)
 */
#define BT_PBP_MIN_PBA_SIZE     (BT_UUID_SIZE_16 + 1 + 1)

/** Public Broadcast Announcement features */
enum bt_pbp_announcement_feature {
    /** Broadcast Streams encryption status */
    BT_PBP_ANNOUNCEMENT_FEATURE_ENCRYPTION = BIT(0),
    /** Standard Quality Public Broadcast Audio configuration */
    BT_PBP_ANNOUNCEMENT_FEATURE_STANDARD_QUALITY = BIT(1),
    /** High Quality Public Broadcast Audio configuration */
    BT_PBP_ANNOUNCEMENT_FEATURE_HIGH_QUALITY = BIT(2),
};

/**
 * @brief Creates a Public Broadcast Announcement based on the information received
 * in the features parameter.
 *
 * @param meta      Metadata to be included in the advertising data
 * @param meta_len  Size of the metadata fields to be included in the advertising data
 * @param features  Public Broadcast Announcement features
 * @param pba_data_buf  Pointer to store the PBA advertising data. Buffer size needs to be
 *          meta_len + @ref BT_PBP_MIN_PBA_SIZE.
 *
 * @return 0 on success or an appropriate error code.
 */
int bt_pbp_get_announcement(const uint8_t meta[], size_t meta_len,
                            enum bt_pbp_announcement_feature features,
                            struct net_buf_simple *pba_data_buf);

/**
 * @brief Parses the received advertising data corresponding to a Public Broadcast
 * Announcement. Returns the advertised Public Broadcast Announcement features and metadata.
 *
 * @param[in]  data     Advertising data to be checked
 * @param[out] features Pointer to public broadcast source features to store the parsed features in
 * @param[out] meta     Pointer to the metadata present in the advertising data
 *
 * @return parsed metadata length on success.
 * @retval -EINVAL if @p data, @p features or @p meta are NULL.
 * @retval -ENOENT if @p data is not of type @ref BT_DATA_SVC_DATA16 or if the UUID in the service
 * data is not @ref BT_UUID_PBA.
 * @retval -EMSGSIZE if @p data is not large enough to contain a PBP announcement.
 * @retval -EBADMSG if the @p data contains invalid data.
 */
int bt_pbp_parse_announcement(struct bt_data *data, enum bt_pbp_announcement_feature *features,
                              uint8_t **meta);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_PBP_ */
