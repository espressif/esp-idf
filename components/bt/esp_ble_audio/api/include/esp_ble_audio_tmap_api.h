/*
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_TMAP_API_H_
#define ESP_BLE_AUDIO_TMAP_API_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sdkconfig.h"
#include "esp_err.h"

#include <zephyr/bluetooth/audio/tmap.h>

#include "common/init.h"
#include "common/host.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!< TMAP Call Gateway role */
#define ESP_BLE_AUDIO_TMAP_ROLE_CG          BT_TMAP_ROLE_CG
/*!< TMAP Call Terminal role */
#define ESP_BLE_AUDIO_TMAP_ROLE_CT          BT_TMAP_ROLE_CT
/*!< TMAP Unicast Media Sender role */
#define ESP_BLE_AUDIO_TMAP_ROLE_UMS         BT_TMAP_ROLE_UMS
/*!< TMAP Unicast Media Receiver role */
#define ESP_BLE_AUDIO_TMAP_ROLE_UMR         BT_TMAP_ROLE_UMR
/*!< TMAP Broadcast Media Sender role */
#define ESP_BLE_AUDIO_TMAP_ROLE_BMS         BT_TMAP_ROLE_BMS
/*!< TMAP Broadcast Media Receiver role */
#define ESP_BLE_AUDIO_TMAP_ROLE_BMR         BT_TMAP_ROLE_BMR

/** TMAP Role characteristic */
typedef enum bt_tmap_role   esp_ble_audio_tmap_role_t;

/** TMAP callback structure */
typedef struct bt_tmap_cb   esp_ble_audio_tmap_cb_t;

/**
 * @brief   Adds TMAS instance to database and sets the received TMAP role(s).
 *
 * @param   role    TMAP role(s) of the device (one or multiple).
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tmap_register(esp_ble_audio_tmap_role_t role);

/**
 * @brief   Perform service discovery as TMAP Client.
 *
 * @param   conn_handle Connection handle.
 * @param   tmap_cb     Pointer to struct of TMAP callbacks.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_tmap_discover(uint16_t conn_handle,
                                      const esp_ble_audio_tmap_cb_t *tmap_cb);

/**
 * @brief   Set one or multiple TMAP roles dynamically.
 *          Previously registered value will be overwritten.
 *
 * @param   role    TMAP role(s).
 */
void esp_ble_audio_tmap_set_role(esp_ble_audio_tmap_role_t role);

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_TMAP_API_H_ */
