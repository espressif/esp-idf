/*
 * SPDX-FileCopyrightText: 2022 Codecoup
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_HAS_API_H_
#define ESP_BLE_AUDIO_HAS_API_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sdkconfig.h"
#include "esp_err.h"

#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/audio/has.h>

#include "common/init.h"
#include "common/host.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!< No index */
#define ESP_BLE_AUDIO_HAS_PRESET_INDEX_NONE     BT_HAS_PRESET_INDEX_NONE
/*!< First preset index */
#define ESP_BLE_AUDIO_HAS_PRESET_INDEX_FIRST    BT_HAS_PRESET_INDEX_FIRST
/*!< Last preset index */
#define ESP_BLE_AUDIO_HAS_PRESET_INDEX_LAST     BT_HAS_PRESET_INDEX_LAST

/*!< Preset name minimum length */
#define ESP_BLE_AUDIO_HAS_PRESET_NAME_MIN       BT_HAS_PRESET_NAME_MIN
/*!< Preset name maximum length */
#define ESP_BLE_AUDIO_HAS_PRESET_NAME_MAX       BT_HAS_PRESET_NAME_MAX

/*!< No properties set */
#define ESP_BLE_AUDIO_HAS_PROP_NONE             BT_HAS_PROP_NONE
/*!< Preset name can be written by the client */
#define ESP_BLE_AUDIO_HAS_PROP_WRITABLE         BT_HAS_PROP_WRITABLE
/*!< Preset availability */
#define ESP_BLE_AUDIO_HAS_PROP_AVAILABLE        BT_HAS_PROP_AVAILABLE
/** Preset Properties values */
typedef enum bt_has_properties                  esp_ble_audio_has_properties_t;

/*!< Indicate support for presets */
#define ESP_BLE_AUDIO_HAS_PRESET_SUPPORT        BT_HAS_PRESET_SUPPORT
/** Hearing Aid device capabilities */
typedef enum bt_has_capabilities                esp_ble_audio_has_capabilities_t;

/**
 * Two hearing aids that form a Coordinated Set, one for the right ear and one for the left
 * ear of the user. Typically used by a user with bilateral hearing loss.
 */
#define ESP_BLE_AUDIO_HAS_HEARING_AID_TYPE_BINAURAL BT_HAS_HEARING_AID_TYPE_BINAURAL
/**
 * A single hearing aid for the left or the right ear. Typically used by a user with
 * unilateral hearing loss.
 */
#define ESP_BLE_AUDIO_HAS_HEARING_AID_TYPE_MONAURAL BT_HAS_HEARING_AID_TYPE_MONAURAL
/**
 * Two hearing aids with a connection to one another that expose a single Bluetooth radio
 * interface.
 */
#define ESP_BLE_AUDIO_HAS_HEARING_AID_TYPE_BANDED   BT_HAS_HEARING_AID_TYPE_BANDED
/** Hearing Aid device type */
typedef enum bt_has_hearing_aid_type                esp_ble_audio_has_hearing_aid_type_t;

/** Hearing Access Service object */
typedef struct bt_has                   esp_ble_audio_has_t;

/** Hearing Access Service Client callback structure */
typedef struct bt_has_client_cb         esp_ble_audio_has_client_cb_t;

/** Structure for registering features of a Hearing Access Service instance */
typedef struct bt_has_features_param    esp_ble_audio_has_features_param_t;

/** Preset operations structure */
typedef struct bt_has_preset_ops        esp_ble_audio_has_preset_ops_t;

/** Preset record definition */
typedef struct bt_has_preset_record     esp_ble_audio_has_preset_record_t;

/** Register structure for preset */
typedef struct bt_has_preset_register_param esp_ble_audio_has_preset_register_param_t;

/**
 * @brief   Registers the callbacks used by the Hearing Access Service client.
 *
 * @param   cb  The callback structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_has_client_cb_register(const esp_ble_audio_has_client_cb_t *cb);

/**
 * @brief   Discover Hearing Access Service on a remote device.
 *
 * Client method to find a Hearing Access Service on a server identified by @p conn_handle.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_has_client_discover(uint16_t conn_handle);

/**
 * @brief   Read Preset Records.
 *
 * Client method to read up to @p max_count presets starting from given @p index.
 *
 * @param   has         Pointer to the Hearing Access Service object.
 * @param   index       The index to start with.
 * @param   max_count   Maximum number of presets to read.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_has_client_presets_read(esp_ble_audio_has_t *has,
                                                uint8_t index,
                                                uint8_t max_count);

/**
 * @brief   Set Active Preset.
 *
 * Client procedure to set preset identified by @p index as active.
 *
 * @param   has     Pointer to the Hearing Access Service object.
 * @param   index   Preset index to activate.
 * @param   sync    Request active preset synchronization in set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_has_client_preset_set(esp_ble_audio_has_t *has,
                                              uint8_t index, bool sync);

/**
 * @brief   Activate Next Preset.
 *
 * Client procedure to set next available preset as active.
 *
 * @param   has     Pointer to the Hearing Access Service object.
 * @param   sync    Request active preset synchronization in set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_has_client_preset_next(esp_ble_audio_has_t *has, bool sync);

/**
 * @brief   Activate Previous Preset.
 *
 * Client procedure to set previous available preset as active.
 *
 * @param   has     Pointer to the Hearing Access Service object.
 * @param   sync    Request active preset synchronization in set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_has_client_preset_prev(esp_ble_audio_has_t *has, bool sync);

/**
 * @brief   Register the Hearing Access Service instance.
 *
 * @param   features    Hearing Access Service register parameters.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_has_register(const esp_ble_audio_has_features_param_t *features);

/**
 * @brief   Register preset.
 *
 * Register preset. The preset will be added to the list of exposed preset records.
 *
 * @param   param   Preset registration parameter.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_has_preset_register(const esp_ble_audio_has_preset_register_param_t *param);

/**
 * @brief   Unregister Preset.
 *
 * Unregister preset. The preset will be removed from the list of preset records.
 *
 * @param   index   The index of preset that's being requested to unregister.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_has_preset_unregister(uint8_t index);

/**
 * @brief   Set the preset as available.
 *
 * This will notify preset availability to peer devices. Only available preset can
 * be selected as active preset.
 *
 * @param   index   The index of preset that's became available.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_has_preset_available(uint8_t index);

/**
 * @brief   Set the preset as unavailable.
 *
 * This will notify preset availability to peer devices. Unavailable preset cannot
 * be selected as active preset.
 *
 * @param   index   The index of preset that's became unavailable.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_has_preset_unavailable(uint8_t index);

/**
 * @brief   Set active preset.
 *
 * Function used to set the preset identified by the @p index as the active preset.
 * The preset index will be notified to peer devices.
 *
 * @param   index   Preset index.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_has_preset_active_set(uint8_t index);

/**
 * @brief   Get active preset.
 *
 * Function used to get the currently active preset index.
 *
 * @return  Active preset index.
 */
uint8_t esp_ble_audio_has_preset_active_get(void);

/**
 * @brief   Change the Preset Name.
 *
 * Change the name of the preset identified by @p index.
 *
 * @param   index   The index of the preset to change the name of.
 * @param   name    Name to write.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_has_preset_name_change(uint8_t index, const char *name);

/**
 * @brief   Change the Hearing Aid Features.
 *
 * Change the hearing aid features.
 *
 * @param   features    The features to be set.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_has_features_set(const esp_ble_audio_has_features_param_t *features);

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_HAS_API_H_ */
