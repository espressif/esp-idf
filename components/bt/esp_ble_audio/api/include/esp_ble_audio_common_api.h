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
/*!< Audio GAP Periodic Sync Transfer Received event */
#define ESP_BLE_AUDIO_GAP_EVENT_PA_SYNC_PAST        BT_LE_GAP_APP_EVENT_PA_SYNC_PAST
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
 * @note    NimBLE-only: NimBLE has no global GAP callback, so the layers below
 *          see only what the application forwards here. An event left out is
 *          dropped silently. Bluedroid's adapter hooks the stack directly.
 *
 * @note    Forward every event below that the application's flows can produce.
 *          Forwarding an unused one is harmless; omitting one leaves the
 *          matching state uncreated and fails later and elsewhere:
 *          - BLE_GAP_EVENT_EXT_DISC             extended advertising report
 *          - BLE_GAP_EVENT_CONNECT              ACL established or failed
 *          - BLE_GAP_EVENT_DISCONNECT           ACL closed
 *          - BLE_GAP_EVENT_ENC_CHANGE           encryption changed
 *          - BLE_GAP_EVENT_PERIODIC_SYNC        PA sync established
 *          - BLE_GAP_EVENT_PERIODIC_SYNC_LOST   PA sync lost
 *          - BLE_GAP_EVENT_PERIODIC_REPORT      periodic advertising report
 *          - BLE_GAP_EVENT_PERIODIC_TRANSFER    PA sync received over PAST
 *          - BLE_GAP_EVENT_PERIODIC_TRANSFER_V2 as above, PAwR
 *
 *          GATT events go to esp_ble_audio_gatt_app_post_event() instead.
 *
 * @note    Shares a sink with esp_ble_iso_gap_app_post_event(): forward a given
 *          event through exactly one of them or it is delivered twice.
 *
 * @param   type    Event type, i.e. ble_gap_event::type.
 * @param   param   Event parameters, i.e. the struct ble_gap_event pointer.
 */
void esp_ble_audio_gap_app_post_event(uint16_t type, void *param);

#if !CONFIG_BT_BLUEDROID_ENABLED
/**
 * @brief   Post an application-layer GATT event for audio internal usage.
 *
 * @note    NimBLE-only: Bluedroid dispatches GATT events inside the adapter, so
 *          no app-level post is needed. Hidden from Bluedroid builds to make
 *          misuse a compile-time error.
 *
 * @note    Forward all four below here rather than through
 *          esp_ble_audio_gap_app_post_event() - GATT has no ISO counterpart.
 *          Omitting one is silent; ASCS and PACS simply never react.
 *          - BLE_GAP_EVENT_MTU        ATT MTU exchanged
 *          - BLE_GAP_EVENT_SUBSCRIBE  peer wrote a CCC descriptor
 *          - BLE_GAP_EVENT_NOTIFY_RX  notification or indication received
 *          - BLE_GAP_EVENT_NOTIFY_TX  notification or indication sent
 *
 * @param   type    Event type, i.e. ble_gap_event::type.
 * @param   param   Event parameters, i.e. the struct ble_gap_event pointer.
 */
void esp_ble_audio_gatt_app_post_event(uint8_t type, void *param);
#endif /* !CONFIG_BT_BLUEDROID_ENABLED */

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
 * @brief   Deinitialize BLE Audio.
 *
 * The only precondition is that the application has stopped its streams: a live
 * stream always has a CIS under it, so one still up makes this return
 * ESP_ERR_INVALID_STATE with each offending item logged at ERROR level.
 *
 * @note    Registered profiles need no prior unregister - this releases them
 *          itself, while the host lock and GATT application are still alive.
 *          The per-profile esp_ble_audio_*_unregister() calls are for dropping
 *          one profile while audio stays up.
 *
 * @note    Also releases the ISO layer, so esp_ble_iso_common_deinit() must not
 *          be called as well. @p info has the same meaning as there.
 *
 * @param   info    Which ISO-layer records to clear, or NULL to clear all.
 *
 * @return  ESP_OK on success,
 *          ESP_ERR_INVALID_STATE if something is still active,
 *          ESP_ERR_TIMEOUT if the ISO task could not be stopped.
 *          Nothing is released unless ESP_OK is returned.
 */
esp_err_t esp_ble_audio_common_deinit(const esp_ble_iso_deinit_info_t *info);

/**
 * @brief   Start BLE Audio services.
 *
 * @param   info    Service instances to start.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_common_start(esp_ble_audio_start_info_t *info);

#if CONFIG_BT_BLUEDROID_ENABLED
/**
 * @brief   Get the engine's internal GATTC interface handle (Bluedroid only).
 *
 * Pass this to esp_ble_gattc_aux_open() / esp_ble_gattc_open() so the
 * resulting ACL events route back to the engine, avoiding the need for the
 * application to register a second BTA GATTC app for connection initiation.
 *
 * @return  Engine's gattc_if (ABI-compatible with esp_gatt_if_t), or
 *          ESP_GATT_IF_NONE (0xFF) if GATTC registration has not completed —
 *          callers must bail rather than pass it to aux_open.
 */
uint8_t esp_ble_audio_bluedroid_get_gattc_if(void);
#endif /* CONFIG_BT_BLUEDROID_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_COMMON_API_H_ */
