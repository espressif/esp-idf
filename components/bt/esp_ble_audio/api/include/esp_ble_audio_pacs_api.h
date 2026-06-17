/*
 * SPDX-FileCopyrightText: 2021 Intel Corporation
 * SPDX-FileCopyrightText: 2021-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_PACS_API_H_
#define ESP_BLE_AUDIO_PACS_API_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sdkconfig.h"
#include "esp_err.h"

#include <zephyr/bluetooth/audio/pacs.h>

#include "common/init.h"
#include "common/host.h"

#include "esp_ble_audio_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Structure for registering PACS */
typedef struct bt_pacs_register_param   esp_ble_audio_pacs_register_param_t;

/** Published Audio Capability structure. */
typedef struct bt_pacs_cap  esp_ble_audio_pacs_cap_t;

/**
 * @brief   Register the Published Audio Capability Service instance.
 *
 * @param   param   PACS register parameters.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_pacs_register(const esp_ble_audio_pacs_register_param_t *param);

/**
 * @brief   Unregister the Published Audio Capability Service instance.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_pacs_unregister(void);

/**
 * @brief   Register Published Audio Capability.
 *
 * Register Audio Local Capability.
 *
 * @param   dir Direction of the endpoint to register capability for.
 * @param   cap Capability structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_pacs_cap_register(esp_ble_audio_dir_t dir,
                                          esp_ble_audio_pacs_cap_t *cap);

/**
 * @brief   Unregister Published Audio Capability.
 *
 * Unregister Audio Local Capability.
 *
 * @param   dir Direction of the endpoint to unregister capability for.
 * @param   cap Capability structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_pacs_cap_unregister(esp_ble_audio_dir_t dir,
                                            esp_ble_audio_pacs_cap_t *cap);

/**
 * @brief   Set the location for an endpoint type.
 *
 * @param   dir         Direction of the endpoints to change location for.
 * @param   location    The location to be set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_pacs_set_location(esp_ble_audio_dir_t dir,
                                          esp_ble_audio_location_t location);

/**
 * @brief   Set the available contexts for an endpoint type.
 *
 * @param   dir         Direction of the endpoints to change available contexts for.
 * @param   contexts    The contexts to be set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_pacs_set_available_contexts(esp_ble_audio_dir_t dir,
                                                    esp_ble_audio_context_t contexts);

/**
 * @brief   Get the available contexts for an endpoint type.
 *
 * @param   dir Direction of the endpoints to get contexts for.
 *
 * @return  Bitmask of available contexts.
 */
esp_ble_audio_context_t esp_ble_audio_pacs_get_available_contexts(esp_ble_audio_dir_t dir);

/**
 * @brief   Set the available contexts for a given connection.
 *
 * This function sets the available contexts value for a given connection.
 * The Available Context Value is reset to default on ACL disconnection.
 *
 * @param   conn_handle Connection handle.
 * @param   dir         Direction of the endpoints to change available contexts for.
 * @param   contexts    The contexts to be set or NULL to reset to default.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_pacs_conn_set_available_contexts_for_conn(uint16_t conn_handle,
                                                                  esp_ble_audio_dir_t dir,
                                                                  esp_ble_audio_context_t *contexts);

/**
 * @brief   Get the available contexts for a given connection.
 *
 * This server function returns the available contexts value for a given connection.
 *
 * @param   conn_handle Connection handle.
 * @param   dir         Direction of the endpoints to get contexts for.
 *
 * @return  Bitmask of available contexts.
 */
esp_ble_audio_context_t esp_ble_audio_pacs_get_available_contexts_for_conn(uint16_t conn_handle,
                                                                           esp_ble_audio_dir_t dir);

/**
 * @brief   Set the supported contexts for an endpoint type.
 *
 * @param   dir         Direction of the endpoints to change available contexts for.
 * @param   contexts    The contexts to be set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_pacs_set_supported_contexts(esp_ble_audio_dir_t dir,
                                                    esp_ble_audio_context_t contexts);

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_PACS_API_H_ */
