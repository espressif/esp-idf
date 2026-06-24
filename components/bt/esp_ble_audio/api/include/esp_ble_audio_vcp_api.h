/*
 * SPDX-FileCopyrightText: 2020-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_VCP_API_H_
#define ESP_BLE_AUDIO_VCP_API_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sdkconfig.h"
#include "esp_err.h"

#include <zephyr/bluetooth/audio/vcp.h>

#include "common/init.h"
#include "common/host.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!< The volume state is unmuted */
#define ESP_BLE_AUDIO_VCP_STATE_UNMUTED     BT_VCP_STATE_UNMUTED
/*!< The volume state is muted */
#define ESP_BLE_AUDIO_VCP_STATE_MUTED       BT_VCP_STATE_MUTED

/** Volume Control Service included services */
typedef struct bt_vcp_included      esp_ble_audio_vcp_included_t;

/** Volume Control Service instance */
typedef struct bt_vcp_vol_ctlr      esp_ble_audio_vcp_vol_ctlr_t;

/** Struct to hold the Volume Controller callbacks */
typedef struct bt_vcp_vol_ctlr_cb   esp_ble_audio_vcp_vol_ctlr_cb_t;

/** Struct to hold the Volume Renderer callbacks */
typedef struct bt_vcp_vol_rend_cb   esp_ble_audio_vcp_vol_rend_cb_t;

/** Register structure for Volume Control Service */
typedef struct bt_vcp_vol_rend_register_param   esp_ble_audio_vcp_vol_rend_register_param_t;

/**
 * @brief   Get Volume Control Service included services.
 *
 * Returns a pointer to a struct that contains information about the
 * Volume Control Service included service instances, such as pointers
 * to the Volume Offset Control Service (Volume Offset Control Service)
 * or Audio Input Control Service (AICS) instances.
 *
 * @param   included    Pointer to store the result in.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_rend_included_get(esp_ble_audio_vcp_included_t *included);

/**
 * @brief   Register the Volume Control Service.
 *
 * This will register and enable the service and make it discoverable by
 * clients.
 *
 * @param   param   Volume Control Service register parameters.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_rend_register(esp_ble_audio_vcp_vol_rend_register_param_t *param);

/**
 * @brief   Set the Volume Control Service volume step size.
 *
 * This can only be done as the server.
 *
 * @param   volume_step The volume step size (1-255).
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_rend_set_step(uint8_t volume_step);

/**
 * @brief   Get the Volume Control Service volume state.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_rend_get_state(void);

/**
 * @brief   Get the Volume Control Service flags.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_rend_get_flags(void);

/**
 * @brief   Turn the volume down by one step on the server.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_rend_vol_down(void);

/**
 * @brief   Turn the volume up by one step on the server.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_rend_vol_up(void);

/**
 * @brief   Turn the volume down and unmute the server.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_rend_unmute_vol_down(void);

/**
 * @brief   Turn the volume up and unmute the server.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_rend_unmute_vol_up(void);

/**
 * @brief   Set the volume on the server.
 *
 * @param   volume  The absolute volume to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_rend_set_vol(uint8_t volume);

/**
 * @brief   Unmute the server.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_rend_unmute(void);

/**
 * @brief   Mute the server.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_rend_mute(void);

/**
 * @brief   Registers the callbacks used by the Volume Controller.
 *
 * @param   cb  The callback structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_ctlr_cb_register(esp_ble_audio_vcp_vol_ctlr_cb_t *cb);

/**
 * @brief   Unregisters the callbacks used by the Volume Controller.
 *
 * @param   cb  The callback structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_ctlr_cb_unregister(esp_ble_audio_vcp_vol_ctlr_cb_t *cb);

/**
 * @brief   Discover Volume Control Service and included services.
 *
 * This will start a GATT discovery and setup handles and subscriptions.
 * This shall be called once before any other actions can be executed
 * for the peer device.
 *
 * This shall only be done as the client,
 *
 * @param   conn_handle Connection handle.
 * @param   vol_ctlr    Valid remote instance object on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_ctlr_discover(uint16_t conn_handle,
                                              esp_ble_audio_vcp_vol_ctlr_t **vol_ctlr);

/**
 * @brief   Get the volume controller from a connection pointer
 *
 * Get the Volume Control Profile Volume Controller pointer from a connection.
 * Only volume controllers that have been initiated can be retrieved.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  Pointer to a Volume Control Profile Volume Controller instance.
 *          NULL if the connection has not been found has not done discovery yet.
 */
esp_ble_audio_vcp_vol_ctlr_t *esp_ble_audio_vcp_vol_ctlr_get_by_conn(uint16_t conn_handle);

/**
 * @brief   Get Volume Control Service included services.
 *
 * Returns a pointer to a struct that contains information about the
 * Volume Control Service included service instances, such as pointers
 * to the Volume Offset Control Service (Volume Offset Control Service)
 * or Audio Input Control Service (AICS) instances.
 *
 * @param   vol_ctlr    Volume Controller instance pointer.
 * @param   included    Pointer to store the result in.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_ctlr_included_get(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr,
                                                  esp_ble_audio_vcp_included_t *included);

/**
 * @brief   Read the volume state of a remote Volume Renderer.
 *
 * @param   vol_ctlr    Volume Controller instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_ctlr_read_state(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr);

/**
 * @brief   Read the volume flags of a remote Volume Renderer.
 *
 * @param   vol_ctlr    Volume Controller instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_ctlr_read_flags(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr);

/**
 * @brief   Turn the volume down one step on a remote Volume Renderer.
 *
 * @param   vol_ctlr    Volume Controller instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_ctlr_vol_down(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr);

/**
 * @brief   Turn the volume up one step on a remote Volume Renderer.
 *
 * @param   vol_ctlr    Volume Controller instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_ctlr_vol_up(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr);

/**
 * @brief   Turn the volume down one step and unmute on a remote Volume Renderer.
 *
 * @param   vol_ctlr    Volume Controller instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_ctlr_unmute_vol_down(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr);

/**
 * @brief   Turn the volume up one step and unmute on a remote Volume Renderer.
 *
 * @param   vol_ctlr    Volume Controller instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_ctlr_unmute_vol_up(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr);

/**
 * @brief   Set the absolute volume on a remote Volume Renderer.
 *
 * @param   vol_ctlr    Volume Controller instance pointer.
 * @param   volume      The absolute volume to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_ctlr_set_vol(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr, uint8_t volume);

/**
 * @brief   Unmute a remote Volume Renderer.
 *
 * @param   vol_ctlr    Volume Controller instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_ctlr_unmute(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr);

/**
 * @brief   Mute a remote Volume Renderer.
 *
 * @param   vol_ctlr    Volume Controller instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vcp_vol_ctlr_mute(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr);

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_VCP_API_H_ */
