/*
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_CCP_API_H_
#define ESP_BLE_AUDIO_CCP_API_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sdkconfig.h"
#include "esp_err.h"

#include <zephyr/bluetooth/audio/audio.h>
#include <zephyr/bluetooth/audio/ccp.h>

#include "common/init.h"
#include "common/host.h"

#include "esp_ble_audio_tbs_api.h"
#include "esp_ble_audio_common_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Abstract Call Control Server Telephone Bearer structure */
typedef struct bt_ccp_call_control_server_bearer \
    esp_ble_audio_ccp_call_control_server_bearer_t;

/** Abstract Call Control Client structure. */
typedef struct bt_ccp_call_control_client \
    esp_ble_audio_ccp_call_control_client_t;

/** Abstract Call Control Client bearer structure. */
typedef struct bt_ccp_call_control_client_bearer \
    esp_ble_audio_ccp_call_control_client_bearer_t;

/** Struct with information about bearers of a client */
typedef struct bt_ccp_call_control_client_bearers \
    esp_ble_audio_ccp_call_control_client_bearers_t;

/** Struct to hold the Telephone Bearer Service client callbacks */
typedef struct bt_ccp_call_control_client_cb \
    esp_ble_audio_ccp_call_control_client_cb_t;

/**
 * @brief   Register a Telephone Bearer.
 *
 * This will register a Telephone Bearer Service (TBS) (or a Generic Telephone
 * Bearer service (GTBS)) with the provided parameters.
 *
 * As per the TBS specification, the GTBS shall be instantiated for the feature,
 * and as such a GTBS shall always be registered before any TBS can be registered.
 * Similarly, all TBS shall be unregistered before the GTBS can be unregistered
 * with esp_ble_audio_ccp_call_control_server_unregister_bearer().
 *
 * @param   param   The parameters to initialize the bearer.
 * @param   bearer  Pointer to the initialized bearer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_ccp_call_control_server_register_bearer(
    const esp_ble_audio_tbs_register_param_t *param,
    esp_ble_audio_ccp_call_control_server_bearer_t **bearer);

/**
 * @brief   Unregister a Telephone Bearer.
 *
 * This will unregister a Telephone Bearer Service (TBS) (or a Generic Telephone
 * Bearer service (GTBS)) with the provided parameters. The bearer shall be
 * registered first by esp_ble_audio_ccp_call_control_server_register_bearer()
 * before it can be unregistered.
 *
 * All TBS shall be unregistered before the GTBS can be unregistered with.
 *
 * @param   bearer  The bearer to unregister.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_ccp_call_control_server_unregister_bearer(
    esp_ble_audio_ccp_call_control_server_bearer_t *bearer);

/**
 * @brief   Set a new bearer provider name.
 *
 * @param   bearer  The bearer to set the name for.
 * @param   name    The new bearer provider name.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_ccp_call_control_server_set_bearer_provider_name(
    esp_ble_audio_ccp_call_control_server_bearer_t *bearer, const char *name);

/**
 * @brief   Get the bearer provider name.
 *
 * @param   bearer  The bearer to get the name for.
 * @param   name    Pointer that will be updated to be the bearer provider name.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_ccp_call_control_server_get_bearer_provider_name(
    esp_ble_audio_ccp_call_control_server_bearer_t *bearer, const char **name);

/**
 * @brief   Discovers the Telephone Bearer Service (TBS) support on a remote device.
 *
 * This will discover the Telephone Bearer Service (TBS) and Generic Telephone Bearer
 * Service (GTBS) on the remote device.
 *
 * @param   conn        Connection to a remote server.
 * @param   out_client  Pointer to client instance on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_ccp_call_control_client_discover(
    esp_ble_conn_t *conn, esp_ble_audio_ccp_call_control_client_t **out_client);

/**
 * @brief   Register callbacks for the Call Control Client.
 *
 * @param   cb  The callback struct.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_ccp_call_control_client_register_cb(
    esp_ble_audio_ccp_call_control_client_cb_t *cb);

/**
 * @brief   Unregister callbacks for the Call Control Client.
 *
 * @param   cb  The callback struct.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_ccp_call_control_client_unregister_cb(
    esp_ble_audio_ccp_call_control_client_cb_t *cb);

/**
 * @brief   Get the bearers of a client instance.
 *
 * @param   client  The client to get the bearers of.
 * @param   bearers The bearers struct that will be populated
 *                  with the bearers of @p client.

 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_ccp_call_control_client_get_bearers(
    esp_ble_audio_ccp_call_control_client_t *client,
    esp_ble_audio_ccp_call_control_client_bearers_t *bearers);

/**
 * @brief   Read the bearer provider name of a remote TBS bearer.
 *
 * @param   bearer  The bearer to read the name from.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_ccp_call_control_client_read_bearer_provider_name(
    esp_ble_audio_ccp_call_control_client_bearer_t *bearer);

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_CCP_API_H_ */
