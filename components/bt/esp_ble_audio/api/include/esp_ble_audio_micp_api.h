/*
 * SPDX-FileCopyrightText: 2020-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_MICP_API_H_
#define ESP_BLE_AUDIO_MICP_API_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sdkconfig.h"
#include "esp_err.h"

#include <zephyr/bluetooth/audio/micp.h>

#include "common/init.h"
#include "common/host.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!< The microphone state is unmuted */
#define ESP_BLE_AUDIO_MICP_MUTE_UNMUTED         BT_MICP_MUTE_UNMUTED
/*!< The microphone state is muted */
#define ESP_BLE_AUDIO_MICP_MUTE_MUTED           BT_MICP_MUTE_MUTED
/*!< The microphone state is disabled and cannot be muted or unmuted */
#define ESP_BLE_AUDIO_MICP_MUTE_DISABLED        BT_MICP_MUTE_DISABLED

/** Microphone Controller instance */
typedef struct bt_micp_mic_ctlr                 esp_ble_audio_micp_mic_ctlr_t;

/** Struct to hold the Microphone Controller callbacks */
typedef struct bt_micp_mic_ctlr_cb              esp_ble_audio_micp_mic_ctlr_cb_t;

/** Struct to hold the Microphone Device callbacks */
typedef struct bt_micp_mic_dev_cb               esp_ble_audio_micp_mic_dev_cb_t;

/** Register parameters structure for Microphone Control Service */
typedef struct bt_micp_mic_dev_register_param   esp_ble_audio_micp_mic_dev_register_param_t;

/** Microphone Control Profile included services */
typedef struct bt_micp_included                 esp_ble_audio_micp_included_t;

/**
 * @brief   Initialize the Microphone Control Profile Microphone Device.
 *
 * This will enable the Microphone Control Service instance and make it
 * discoverable by Microphone Controllers.
 *
 * @param   param   Pointer to an initialization structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_micp_mic_dev_register(esp_ble_audio_micp_mic_dev_register_param_t *param);

/**
 * @brief   Get Microphone Device included services.
 *
 * Returns a pointer to a struct that contains information about the
 * Microphone Device included Audio Input Control Service instances.
 *
 * @param   included    Pointer to store the result in.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_micp_mic_dev_included_get(esp_ble_audio_micp_included_t *included);

/**
 * @brief   Unmute the Microphone Device.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_micp_mic_dev_unmute(void);

/**
 * @brief   Mute the Microphone Device.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_micp_mic_dev_mute(void);

/**
 * @brief   Disable the mute functionality on the Microphone Device.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_micp_mic_dev_mute_disable(void);

/**
 * @brief   Read the mute state on the Microphone Device.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_micp_mic_dev_mute_get(void);

/**
 * @brief   Get Microphone Control Profile included services.
 *
 * Returns a pointer to a struct that contains information about the
 * Microphone Control Profile included services instances, such as
 * pointers to the Audio Input Control Service instances.
 *
 * @param   mic_ctlr    Microphone Controller instance pointer.
 * @param   included    Pointer to store the result in.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_micp_mic_ctlr_included_get(esp_ble_audio_micp_mic_ctlr_t *mic_ctlr,
                                                   esp_ble_audio_micp_included_t *included);

/**
 * @brief   Get the Microphone controller from a connection pointer.
 *
 * Only Microphone controllers that have been initiated can be retrieved.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  Pointer to a Microphone controller instance or NULL if not found.
 */
esp_ble_audio_micp_mic_ctlr_t *esp_ble_audio_micp_mic_ctlr_get_by_conn(uint16_t conn_handle);

/**
 * @brief   Discover Microphone Control Service.
 *
 * This will start a GATT discovery and setup handles and subscriptions.
 * This shall be called once before any other actions can be executed
 * for the peer device.
 *
 * @param   conn_handle Connection handle.
 * @param   mic_ctlr    Valid remote instance object on success.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_micp_mic_ctlr_discover(uint16_t conn_handle,
                                               esp_ble_audio_micp_mic_ctlr_t **mic_ctlr);

/**
 * @brief   Unmute a remote Microphone Device.
 *
 * @param   mic_ctlr    Microphone Controller instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_micp_mic_ctlr_unmute(esp_ble_audio_micp_mic_ctlr_t *mic_ctlr);

/**
 * @brief   Mute a remote Microphone Device.
 *
 * @param   mic_ctlr    Microphone Controller instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_micp_mic_ctlr_mute(esp_ble_audio_micp_mic_ctlr_t *mic_ctlr);

/**
 * @brief   Read the mute state of a remote Microphone Device.
 *
 * @param   mic_ctlr    Microphone Controller instance pointer.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_micp_mic_ctlr_mute_get(esp_ble_audio_micp_mic_ctlr_t *mic_ctlr);

/**
 * @brief   Registers the callbacks used by Microphone Controller.
 *
 * This can only be done as the client.
 *
 * @param   cb  The callback structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_micp_mic_ctlr_cb_register(esp_ble_audio_micp_mic_ctlr_cb_t *cb);

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_MICP_API_H_ */
