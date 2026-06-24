/*
 * SPDX-FileCopyrightText: 2020-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_AICS_API_H_
#define ESP_BLE_AUDIO_AICS_API_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sdkconfig.h"
#include "esp_err.h"

#include <zephyr/bluetooth/audio/audio.h>
#include <zephyr/bluetooth/audio/aics.h>

#include "common/init.h"
#include "common/host.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!< The mute state is unmuted */
#define ESP_BLE_AUDIO_AICS_STATE_UNMUTED                BT_AICS_STATE_UNMUTED
/*!< The mute state is muted */
#define ESP_BLE_AUDIO_AICS_STATE_MUTED                  BT_AICS_STATE_MUTED
/*!< The mute state is disabled */
#define ESP_BLE_AUDIO_AICS_STATE_MUTE_DISABLED          BT_AICS_STATE_MUTE_DISABLED

/*!< The gain mode is manual only and cannot be changed to automatic */
#define ESP_BLE_AUDIO_AICS_MODE_MANUAL_ONLY             BT_AICS_MODE_MANUAL_ONLY
/*!< The gain mode is automatic only and cannot be changed to manual */
#define ESP_BLE_AUDIO_AICS_MODE_AUTO_ONLY               BT_AICS_MODE_AUTO_ONLY
/*!< The gain mode is manual */
#define ESP_BLE_AUDIO_AICS_MODE_MANUAL                  BT_AICS_MODE_MANUAL
/*!< The gain mode is automatic */
#define ESP_BLE_AUDIO_AICS_MODE_AUTO                    BT_AICS_MODE_AUTO

/*!< The input is unspecified */
#define ESP_BLE_AUDIO_AICS_INPUT_TYPE_UNSPECIFIED       BT_AICS_INPUT_TYPE_UNSPECIFIED
/*!< The input is a Bluetooth Audio Stream */
#define ESP_BLE_AUDIO_AICS_INPUT_TYPE_BLUETOOTH         BT_AICS_INPUT_TYPE_BLUETOOTH
/*!< The input is a microphone */
#define ESP_BLE_AUDIO_AICS_INPUT_TYPE_MICROPHONE        BT_AICS_INPUT_TYPE_MICROPHONE
/*!< The input is analog */
#define ESP_BLE_AUDIO_AICS_INPUT_TYPE_ANALOG            BT_AICS_INPUT_TYPE_ANALOG
/*!< The input is digital */
#define ESP_BLE_AUDIO_AICS_INPUT_TYPE_DIGITAL           BT_AICS_INPUT_TYPE_DIGITAL
/*!< The input is a radio (AM/FM/XM/etc.) */
#define ESP_BLE_AUDIO_AICS_INPUT_TYPE_RADIO             BT_AICS_INPUT_TYPE_RADIO
/*!< The input is a Streaming Audio Source */
#define ESP_BLE_AUDIO_AICS_INPUT_TYPE_STREAMING         BT_AICS_INPUT_TYPE_STREAMING
/*!< The input is transparent / pass-through */
#define ESP_BLE_AUDIO_AICS_INPUT_TYPE_AMBIENT           BT_AICS_INPUT_TYPE_AMBIENT

/** Audio Input Control Service instance. */
typedef struct bt_aics                  esp_ble_audio_aics_t;

/** Structure to hold callbacks for Audio Input Control Service. */
typedef struct bt_aics_cb               esp_ble_audio_aics_cb_t;

/** Structure for initializing an Audio Input Control Service instance. */
typedef struct bt_aics_register_param   esp_ble_audio_aics_register_param_t;

/** Structure for discovering an Audio Input Control Service instance. */
typedef struct bt_aics_discover_param   esp_ble_audio_aics_discover_param_t;

/**
 * @brief   Get a free instance of Audio Input Control Service from the pool.
 *
 * @return  Audio Input Control Service instance in case of success or NULL in case of error.
 */
esp_ble_audio_aics_t *esp_ble_audio_aics_free_instance_get(void);

/**
 * @brief   Get the service declaration attribute.
 *
 * The first service attribute returned can be included in any other GATT service.
 *
 * @param   aics    Audio Input Control Service instance.
 *
 * @return  Pointer to the attributes of the service.
 */
void *esp_ble_audio_aics_svc_decl_get(esp_ble_audio_aics_t *aics);

/**
 * @brief   Initialize the Audio Input Control Service instance.
 *
 * @param   aics    Audio Input Control Service instance.
 * @param   param   Audio Input Control Service register parameters.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_aics_register(esp_ble_audio_aics_t *aics,
                                      esp_ble_audio_aics_register_param_t *param);

/**
 * @brief   Activates an Audio Input Control Service instance.
 *
 * Audio Input Control Services are activated by default, but this will allow
 * the server reactivate an Audio Input Control Service instance after it has
 * been deactivated.
 *
 * @param   inst    The instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_aics_activate(esp_ble_audio_aics_t *inst);

/**
 * @brief   Deactivates an Audio Input Control Service instance.
 *
 * Audio Input Control Services are activated by default, but this will allow
 * the server to deactivate an Audio Input Control Service.
 *
 * @param   inst    The instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_aics_deactivate(esp_ble_audio_aics_t *inst);

/**
 * @brief   Read the Audio Input Control Service input state.
 *
 * @param   inst    The instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_aics_state_get(esp_ble_audio_aics_t *inst);

/**
 * @brief   Read the Audio Input Control Service gain settings.
 *
 * @param   inst    The instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_aics_gain_setting_get(esp_ble_audio_aics_t *inst);

/**
 * @brief   Read the Audio Input Control Service input type.
 *
 * @param   inst    The instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_aics_type_get(esp_ble_audio_aics_t *inst);

/**
 * @brief   Read the Audio Input Control Service input status.
 *
 * @param   inst    The instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_aics_status_get(esp_ble_audio_aics_t *inst);

/**
 * @brief   Disable mute in the Audio Input Control Service.
 *
 * @param   inst    The instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_aics_disable_mute(esp_ble_audio_aics_t *inst);

/**
 * @brief   Unmute the Audio Input Control Service input.
 *
 * @param   inst    The instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_aics_unmute(esp_ble_audio_aics_t *inst);

/**
 * @brief   Mute the Audio Input Control Service input.
 *
 * @param   inst    The instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_aics_mute(esp_ble_audio_aics_t *inst);

/**
 * @brief   Set manual only gain mode in Audio Input Control Service.
 *
 * @param   inst    The instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_aics_gain_set_manual_only(esp_ble_audio_aics_t *inst);

/**
 * @brief   Set automatic only gain mode in Audio Input Control Service.
 *
 * @param   inst    The instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_aics_gain_set_auto_only(esp_ble_audio_aics_t *inst);

/**
 * @brief   Set input gain to manual.
 *
 * @param   inst    The instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_aics_manual_gain_set(esp_ble_audio_aics_t *inst);

/**
 * @brief   Set the input gain to automatic.
 *
 * @param   inst    The instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_aics_automatic_gain_set(esp_ble_audio_aics_t *inst);

/**
 * @brief   Set the input gain.
 *
 * @param   inst    The instance pointer.
 * @param   gain    The gain to set (-128 to 127) in gain setting units.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_aics_gain_set(esp_ble_audio_aics_t *inst, int8_t gain);

/**
 * @brief   Read the Audio Input Control Service description.
 *
 * @param   inst    The instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_aics_description_get(esp_ble_audio_aics_t *inst);

/**
 * @brief   Set the Audio Input Control Service description.
 *
 * @param   inst        The instance pointer.
 * @param   description The description as an UTF-8 encoded string.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_aics_description_set(esp_ble_audio_aics_t *inst,
                                             const char *description);

/**
 * @brief   Discover an Audio Input Control Service.
 *
 * Attempts to discover an Audio Input Control Service on a server given the
 * @p param.
 *
 * @param   conn_handle Connection handle.
 * @param   inst        The instance pointer.
 * @param   param       Pointer to the parameters.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_aics_discover(uint16_t conn_handle, esp_ble_audio_aics_t *inst,
                                      const esp_ble_audio_aics_discover_param_t *param);

/**
 * @brief   Get a new Audio Input Control Service client instance.
 *
 * @return  Pointer to the instance, or NULL if no free instances are left.
 */
esp_ble_audio_aics_t *esp_ble_audio_aics_client_free_instance_get(void);

/**
 * @brief   Registers the callbacks for the Audio Input Control Service client.
 *
 * @param   inst    The instance pointer.
 * @param   cb      Pointer to the callback structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_aics_client_cb_register(esp_ble_audio_aics_t *inst,
                                                esp_ble_audio_aics_cb_t *cb);

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_AICS_API_H_ */
