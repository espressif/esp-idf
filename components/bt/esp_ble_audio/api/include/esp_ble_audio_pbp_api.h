/*
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_PBP_API_H_
#define ESP_BLE_AUDIO_PBP_API_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sdkconfig.h"
#include "esp_err.h"

#include <zephyr/bluetooth/audio/pbp.h>

#include "common/init.h"
#include "common/host.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Minimum size of the Public Broadcast Announcement */
#define ESP_BLE_AUDIO_PBP_MIN_PBA_SIZE      BT_PBP_MIN_PBA_SIZE

/*!< Broadcast Streams encryption status */
#define ESP_BLE_AUDIO_PBP_ANNOUNCEMENT_FEATURE_ENCRYPTION       BT_PBP_ANNOUNCEMENT_FEATURE_ENCRYPTION
/*!< Standard Quality Public Broadcast Audio configuration */
#define ESP_BLE_AUDIO_PBP_ANNOUNCEMENT_FEATURE_STANDARD_QUALITY BT_PBP_ANNOUNCEMENT_FEATURE_STANDARD_QUALITY
/*!< High Quality Public Broadcast Audio configuration */
#define ESP_BLE_AUDIO_PBP_ANNOUNCEMENT_FEATURE_HIGH_QUALITY     BT_PBP_ANNOUNCEMENT_FEATURE_HIGH_QUALITY
/** Public Broadcast Announcement features */
typedef enum bt_pbp_announcement_feature                        esp_ble_audio_pbp_announcement_feature_t;

/**
 * @brief   Creates a Public Broadcast Announcement based on the information received
 *          in the features parameter.
 *
 * @param   meta            Metadata to be included in the advertising data.
 * @param   meta_len        Size of the metadata fields to be included in the advertising data.
 * @param   features        Public Broadcast Announcement features.
 * @param   pba_data_buf    Pointer to store the PBA advertising data. Buffer size needs to be
 *                          meta_len + ESP_BLE_AUDIO_PBP_MIN_PBA_SIZE.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_pbp_get_announcement(const uint8_t meta[], size_t meta_len,
                                             esp_ble_audio_pbp_announcement_feature_t features,
                                             struct net_buf_simple *pba_data_buf);

/**
 * @brief   Parses the received advertising data corresponding to a Public Broadcast
 *          Announcement.
 *          Returns the advertised Public Broadcast Announcement features and metadata.
 *
 * @param   data_type   Type of advertising data to be checked.
 * @param   data        Pointer of advertising data to be checked.
 * @param   data_len    Length of advertising data to be checked.
 * @param   features    Pointer to public broadcast source features to store the parsed features in.
 * @param   meta        Pointer to the metadata present in the advertising data.
 * @param   meta_len    Size of the metadata fields to be included in the advertising data.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_pbp_parse_announcement(uint8_t data_type, const uint8_t *data, uint8_t data_len,
                                               esp_ble_audio_pbp_announcement_feature_t *features,
                                               uint8_t **meta, uint8_t *meta_len);

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_PBP_API_H_ */
