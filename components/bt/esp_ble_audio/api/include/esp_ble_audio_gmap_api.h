/*
 * SPDX-FileCopyrightText: 2023-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_GMAP_API_H_
#define ESP_BLE_AUDIO_GMAP_API_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sdkconfig.h"
#include "esp_err.h"

#include <zephyr/bluetooth/audio/gmap.h>

#include "common/init.h"
#include "common/host.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!< Gaming Role Unicast Game Gateway */
#define ESP_BLE_AUDIO_GMAP_ROLE_UGG     BT_GMAP_ROLE_UGG
/*!< Gaming Role Unicast Game Terminal */
#define ESP_BLE_AUDIO_GMAP_ROLE_UGT     BT_GMAP_ROLE_UGT
/*!< Gaming Role Broadcast Game Sender */
#define ESP_BLE_AUDIO_GMAP_ROLE_BGS     BT_GMAP_ROLE_BGS
/*!< Gaming Role Broadcast Game Receiver */
#define ESP_BLE_AUDIO_GMAP_ROLE_BGR     BT_GMAP_ROLE_BGR
/** Gaming Role bitfield */
typedef enum bt_gmap_role               esp_ble_audio_gmap_role_t;

/*!< Support transmitting multiple LC3 codec frames per block in an SDU */
#define ESP_BLE_AUDIO_GMAP_UGG_FEAT_MULTIPLEX       BT_GMAP_UGG_FEAT_MULTIPLEX
/*!< 96 kbps source support */
#define ESP_BLE_AUDIO_GMAP_UGG_FEAT_96KBPS_SOURCE   BT_GMAP_UGG_FEAT_96KBPS_SOURCE
/*!< Support for receiving at least two channels of audio, each in a separate CIS */
#define ESP_BLE_AUDIO_GMAP_UGG_FEAT_MULTISINK       BT_GMAP_UGG_FEAT_MULTISINK
/** Unicast Game Gateway Feature bitfield */
typedef enum bt_gmap_ugg_feat                       esp_ble_audio_gmap_ugg_feat_t;

/*!< Source support */
#define ESP_BLE_AUDIO_GMAP_UGT_FEAT_SOURCE          BT_GMAP_UGT_FEAT_SOURCE
/*!< 80 kbps source support */
#define ESP_BLE_AUDIO_GMAP_UGT_FEAT_80KBPS_SOURCE   BT_GMAP_UGT_FEAT_80KBPS_SOURCE
/*!< Sink support */
#define ESP_BLE_AUDIO_GMAP_UGT_FEAT_SINK            BT_GMAP_UGT_FEAT_SINK
/*!< 64 kbps sink support */
#define ESP_BLE_AUDIO_GMAP_UGT_FEAT_64KBPS_SINK     BT_GMAP_UGT_FEAT_64KBPS_SINK
/*!< Support for receiving multiple LC3 codec frames per block in an SDU */
#define ESP_BLE_AUDIO_GMAP_UGT_FEAT_MULTIPLEX       BT_GMAP_UGT_FEAT_MULTIPLEX
/*!< Support for receiving at least two audio channels, each in a separate CIS */
#define ESP_BLE_AUDIO_GMAP_UGT_FEAT_MULTISINK       BT_GMAP_UGT_FEAT_MULTISINK
/*!< Support for sending at least two audio channels, each in a separate CIS */
#define ESP_BLE_AUDIO_GMAP_UGT_FEAT_MULTISOURCE     BT_GMAP_UGT_FEAT_MULTISOURCE
/** Unicast Game Terminal Feature bitfield */
typedef enum bt_gmap_ugt_feat                       esp_ble_audio_gmap_ugt_feat_t;

/*!< 96 kbps support */
#define ESP_BLE_AUDIO_GMAP_BGS_FEAT_96KBPS          BT_GMAP_BGS_FEAT_96KBPS
/** Broadcast Game Receiver Feature bitfield */
typedef enum bt_gmap_bgs_feat                       esp_ble_audio_gmap_bgs_feat_t;

/*!< Support for receiving at least two audio channels, each in a separate BIS */
#define ESP_BLE_AUDIO_GMAP_BGR_FEAT_MULTISINK       BT_GMAP_BGR_FEAT_MULTISINK
/*!< Support for receiving multiple LC3 codec frames per block in an SDU */
#define ESP_BLE_AUDIO_GMAP_BGR_FEAT_MULTIPLEX       BT_GMAP_BGR_FEAT_MULTIPLEX
/** Broadcast Game Receiver Feature bitfield */
typedef enum bt_gmap_bgr_feat                       esp_ble_audio_gmap_bgr_feat_t;

/** Hearing Access Service Client callback structure */
typedef struct bt_gmap_cb   esp_ble_audio_gmap_cb_t;

/** Broadcast Game Receiver Feature bitfield */
typedef struct bt_gmap_feat esp_ble_audio_gmap_feat_t;

/**
 * @brief   Registers the callbacks used by the Gaming Audio Profile.
 *
 * @param   cb  The callback structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_gmap_cb_register(const esp_ble_audio_gmap_cb_t *cb);

/**
 * @brief   Discover Gaming Service on a remote device.
 *
 * Procedure to find a Gaming Service on a server identified by @p conn_handle.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_gmap_discover(uint16_t conn_handle);

/**
 * @brief   Adds GMAS instance to database and sets the received Gaming Audio Profile role(s).
 *
 * @param   role        Gaming Audio Profile role(s) of the device (one or multiple).
 * @param   features    Features of the roles. If a role is not in the @p role parameter, then
 *                      the feature value for that role is simply ignored.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_gmap_register(esp_ble_audio_gmap_role_t role,
                                      esp_ble_audio_gmap_feat_t features);

/**
 * @brief   Set one or multiple Gaming Audio Profile roles and features dynamically.
 *
 * Previously registered value will be overwritten. If there is a role change, this will trigger
 * a Gaming Audio Service (GMAS) service change. If there is only a feature change, no service
 * change will happen.
 *
 * @param   role        Gaming Audio Profile role(s).
 * @param   features    Features of the roles. If a role is not in the @p role parameter, then
 *                      the feature value for that role is simply ignored.

 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_gmap_set_role(esp_ble_audio_gmap_role_t role,
                                      esp_ble_audio_gmap_feat_t features);

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_GMAP_API_H_ */
