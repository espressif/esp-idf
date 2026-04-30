/*
 * SPDX-FileCopyrightText: 2020 Intel Corporation
 * SPDX-FileCopyrightText: 2020-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_COMMON_API_H_
#define ESP_BLE_AUDIO_COMMON_API_H_

#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "sdkconfig.h"
#include "esp_err.h"

#include "common/init.h"
#include "common/host.h"
#include "common/app/gatt.h"

#include "esp_ble_iso_common_api.h"

#include "esp_ble_audio_defs.h"
#include "esp_ble_audio_csip_api.h"
#include "esp_ble_audio_vcp_api.h"
#include "esp_ble_audio_micp_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Helper for parsing length-type-value data.
 *
 * @param   ltv         Length-type-value (LTV) encoded data.
 * @param   size        Size of the @p ltv data.
 * @param   func        Callback function which will be called for each element
 *                      that's found in the data. The callback should return
 *                      true to continue parsing, or false to stop parsing.
 * @param   user_data   User data to be passed to the callback.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_data_parse(const uint8_t ltv[], size_t size,
                                   bool (*func)(uint8_t type,
                                                const uint8_t *data,
                                                uint8_t data_len,
                                                void *user_data),
                                   void *user_data);

/**
 * @brief   Get the value of a specific data type in an length-type-value data array.
 *
 * @param   ltv_data    The array containing the length-type-value tuples.
 * @param   size        The size of @p ltv_data.
 * @param   type        The type to get the value for.
 * @param   data        Pointer to the data-pointer to update when item is found.
 *                      Any found data will be little endian.
 * @param   data_len    Length The length of found @p data (may be 0).
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_data_get_val(const uint8_t ltv_data[],
                                     size_t size, uint8_t type,
                                     const uint8_t **data,
                                     uint8_t *data_len);

/**
 * @brief   Function to get the number of channels from the channel allocation.
 *
 * @param   chan_allocation The channel allocation.
 *
 * @return  The number of channels.
 */
uint8_t esp_ble_audio_get_chan_count(esp_ble_audio_location_t chan_allocation);

/**
 * @brief   Start GATT service discovery.
 *
 * @note    This function is only needed while using NimBLE Host.
 *
 * @param   conn_handle Connection Handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_gattc_disc_start(uint16_t conn_handle);

/*!< Audio GAP Extended Scan Recv event */
#define ESP_BLE_AUDIO_GAP_EVENT_EXT_SCAN_RECV       BT_LE_GAP_APP_EVENT_EXT_SCAN_RECV
/*!< Audio GAP Periodic Sync Established event */
#define ESP_BLE_AUDIO_GAP_EVENT_PA_SYNC             BT_LE_GAP_APP_EVENT_PA_SYNC
/*!< Audio GAP Periodic Sync Lost event */
#define ESP_BLE_AUDIO_GAP_EVENT_PA_SYNC_LOST        BT_LE_GAP_APP_EVENT_PA_SYNC_LOST
/*!< Audio GAP Connection Complete event */
#define ESP_BLE_AUDIO_GAP_EVENT_ACL_CONNECT         BT_LE_GAP_APP_EVENT_ACL_CONNECT
/*!< Audio GAP Disconnection Complete event */
#define ESP_BLE_AUDIO_GAP_EVENT_ACL_DISCONNECT      BT_LE_GAP_APP_EVENT_ACL_DISCONNECT
/*!< Audio GAP Security Change event */
#define ESP_BLE_AUDIO_GAP_EVENT_SECURITY_CHANGE     BT_LE_GAP_APP_EVENT_SECURITY_CHANGE
/** Audio GAP application event structure */
typedef struct bt_le_gap_app_event                  esp_ble_audio_gap_app_event_t;

/*!< Audio GATT MTU exchange complete event */
#define ESP_BLE_AUDIO_GATT_EVENT_GATT_MTU_CHANGE    BT_LE_GATT_APP_EVENT_GATT_MTU_CHANGE
/*!< Audio GATT client discovery complete event */
#define ESP_BLE_AUDIO_GATT_EVENT_GATTC_DISC_CMPL    BT_LE_GATT_APP_EVENT_GATTC_DISC_CMPL
/** Audio GATT application event structure */
typedef struct bt_le_gatt_app_event                 esp_ble_audio_gatt_app_event_t;

/** Contains callback functions for GAP and GATT events */
typedef struct {
    bt_le_gap_app_cb  gap_cb;   /*!< GAP event callbacks */
    bt_le_gatt_app_cb gatt_cb;  /*!< GATT event callbacks */
} esp_ble_audio_init_info_t;

/**
 * @brief   Post an application-layer GAP event for audio internal usage.
 *
 * @note    This function is only needed while using NimBLE Host.
 *
 * @param   type    Event type.
 * @param   param   Event parameters.
 */
void esp_ble_audio_gap_app_post_event(uint8_t type, void *param);

/**
 * @brief   Post an application-layer GATT event for audio internal usage.
 *
 * @note    This function is only needed while using NimBLE Host.
 *
 * @param   type    Event type.
 * @param   param   Event parameters.
 */
void esp_ble_audio_gatt_app_post_event(uint8_t type, void *param);

/**
 * @brief   Initialize BLE Audio common functionality.
 *
 * @param   info    Initialization parameters.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_common_init(esp_ble_audio_init_info_t *info);

/** Contains service instances for audio profiles */
typedef struct {
#if CONFIG_BT_CSIP_SET_MEMBER
    struct {
        esp_ble_audio_csip_set_member_svc_inst_t *svc_inst;     /*!< Pointer of CSIS Service instance */
        bool included_by_cas;                                   /*!< Indicate if the CSIS Service is included by CAS */
    } csis_insts[CONFIG_BT_CSIP_SET_MEMBER_MAX_INSTANCE_COUNT]; /*!< Structures of CSIS Service instances */
#endif /* CONFIG_BT_CSIP_SET_MEMBER */
    uint8_t dummy;                                              /*!< Dummy field to avoid empty struct */
} esp_ble_audio_start_info_t;

/**
 * @brief   Start BLE Audio services.
 *
 * @param   info    Service instances to start.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_common_start(esp_ble_audio_start_info_t *info);

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_COMMON_API_H_ */
