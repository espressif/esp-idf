/*
 * SPDX-FileCopyrightText: 2020-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_VOCS_API_H_
#define ESP_BLE_AUDIO_VOCS_API_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sdkconfig.h"
#include "esp_err.h"

#include <zephyr/bluetooth/audio/vocs.h>

#include "common/init.h"
#include "common/host.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!< Minimum offset value */
#define ESP_BLE_AUDIO_VOCS_MIN_OFFSET   BT_VOCS_MIN_OFFSET
/*!< Maximum offset value */
#define ESP_BLE_AUDIO_VOCS_MAX_OFFSET   BT_VOCS_MAX_OFFSET

/** Volume Offset Control Service instance */
typedef struct bt_vocs      esp_ble_audio_vocs_t;

/** Struct to hold the Volume Offset Control Service callbacks */
typedef struct bt_vocs_cb   esp_ble_audio_vocs_cb_t;

/** Structure for registering a Volume Offset Control Service instance */
typedef struct bt_vocs_register_param   esp_ble_audio_vocs_register_param_t;

/** Structure for discovering a Volume Offset Control Service instance */
typedef struct bt_vocs_discover_param   esp_ble_audio_vocs_discover_param_t;

/**
 * @brief   Get a free service instance of Volume Offset Control Service from the pool.
 *
 * @return  Volume Offset Control Service instance in case of success or NULL in case of error.
 */
esp_ble_audio_vocs_t *esp_ble_audio_vocs_free_instance_get(void);

/**
 * @brief   Get the service declaration attribute.
 *
 * The first service attribute returned can be included in any other GATT service.
 *
 * @param   vocs    Volume Offset Control Service instance.
 *
 * @return  Pointer to the attributes of the service.
 */
void *esp_ble_audio_vocs_svc_decl_get(esp_ble_audio_vocs_t *vocs);

/**
 * @brief   Register the Volume Offset Control Service instance.
 *
 * @param   vocs    Volume Offset Control Service instance.
 * @param   param   Volume Offset Control Service register parameters.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vocs_register(esp_ble_audio_vocs_t *vocs,
                                      const esp_ble_audio_vocs_register_param_t *param);

/**
 * @brief   Read the Volume Offset Control Service offset state.
 *
 * @param   inst    Pointer to the Volume Offset Control Service instance.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vocs_state_get(esp_ble_audio_vocs_t *inst);

/**
 * @brief   Set the Volume Offset Control Service offset state.
 *
 * @param   inst    Pointer to the Volume Offset Control Service instance.
 * @param   offset  The offset to set (-255 to 255).
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vocs_state_set(esp_ble_audio_vocs_t *inst, int16_t offset);

/**
 * @brief   Read the Volume Offset Control Service location.
 *
 * @param   inst    Pointer to the Volume Offset Control Service instance.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vocs_location_get(esp_ble_audio_vocs_t *inst);

/**
 * @brief   Set the Volume Offset Control Service location.
 *
 * @param   inst        Pointer to the Volume Offset Control Service instance.
 * @param   location    The location to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vocs_location_set(esp_ble_audio_vocs_t *inst, uint32_t location);

/**
 * @brief   Read the Volume Offset Control Service output description.
 *
 * @param   inst    Pointer to the Volume Offset Control Service instance.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vocs_description_get(esp_ble_audio_vocs_t *inst);

/**
 * @brief   Set the Volume Offset Control Service description.
 *
 * @param   inst        Pointer to the Volume Offset Control Service instance.
 * @param   description The UTF-8 encoded string description to set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vocs_description_set(esp_ble_audio_vocs_t *inst,
                                             const char *description);

/**
 * @brief   Registers the callbacks for the Volume Offset Control Service client.
 *
 * @param   inst    Pointer to the Volume Offset Control Service client instance.
 * @param   cb      Pointer to the callback structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vocs_client_cb_register(esp_ble_audio_vocs_t *inst,
                                                esp_ble_audio_vocs_cb_t *cb);

/**
 * @brief   Returns a pointer to a Volume Offset Control Service client instance.
 *
 * @return  Pointer to the instance, or NULL if no free instances are left.
 */
esp_ble_audio_vocs_t *esp_ble_audio_vocs_client_free_instance_get(void);

/**
 * @brief   Discover a Volume Offset Control Service.
 *
 * Attempts to discover a Volume Offset Control Service on a server given the @p param.
 *
 * @param   conn_handle Connection handle.
 * @param   inst        Pointer to the Volume Offset Control Service client instance.
 * @param   param       Pointer to the parameters.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_vocs_discover(uint16_t conn_handle,
                                      esp_ble_audio_vocs_t *inst,
                                      const esp_ble_audio_vocs_discover_param_t *param);

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_VOCS_API_H_ */
