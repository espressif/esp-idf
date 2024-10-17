/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <protocomm.h>
#include <protocomm_ble.h>

#include "wifi_provisioning/manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Scheme that can be used by manager for provisioning
 *          over BLE transport with GATT server
 */
extern const wifi_prov_scheme_t wifi_prov_scheme_ble;

/* This scheme specific event handler is to be used when application
 * doesn't require BT and BLE after provisioning has finished */
#define WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM {    \
    .event_cb  = wifi_prov_scheme_ble_event_cb_free_btdm, \
    .user_data = NULL                                     \
}

/* This scheme specific event handler is to be used when application
 * doesn't require BLE to be active after provisioning has finished */
#define WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BLE {    \
    .event_cb  = wifi_prov_scheme_ble_event_cb_free_ble, \
    .user_data = NULL                                    \
}

/* This scheme specific event handler is to be used when application
 * doesn't require BT to be active after provisioning has finished */
#define WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BT {    \
    .event_cb  = wifi_prov_scheme_ble_event_cb_free_bt, \
    .user_data = NULL                                   \
}

void wifi_prov_scheme_ble_event_cb_free_btdm(void *user_data, wifi_prov_cb_event_t event, void *event_data);
void wifi_prov_scheme_ble_event_cb_free_ble (void *user_data, wifi_prov_cb_event_t event, void *event_data);
void wifi_prov_scheme_ble_event_cb_free_bt  (void *user_data, wifi_prov_cb_event_t event, void *event_data);

/**
 * @brief   Set the 128 bit GATT service UUID used for provisioning
 *
 * This API is used to override the default 128 bit provisioning
 * service UUID, which is 0000ffff-0000-1000-8000-00805f9b34fb.
 *
 * This must be called before starting provisioning, i.e. before
 * making a call to wifi_prov_mgr_start_provisioning(), otherwise
 * the default UUID will be used.
 *
 * @note    The data being pointed to by the argument must be valid
 *          at least till provisioning is started. Upon start, the
 *          manager will store an internal copy of this UUID, and
 *          this data can be freed or invalidated afterwards.
 *
 * @param[in] uuid128  A custom 128 bit UUID
 *
 * @return
 *  - ESP_OK              : Success
 *  - ESP_ERR_INVALID_ARG : Null argument
 */
esp_err_t wifi_prov_scheme_ble_set_service_uuid(uint8_t *uuid128);

/**
 * @brief   Keep the BLE on after provisioning
 *
 * This API is used to specify whether the BLE should remain on
 * after the provisioning process has stopped.
 *
 * This must be called before starting provisioning, i.e. before
 * making a call to wifi_prov_mgr_start_provisioning(), otherwise
 * the default behavior will be used.
 *
 * @note    The value being pointed to by the argument must be valid
 *          at least until provisioning is started. Upon start, the
 *          manager will store an internal copy of this value, and
 *          this data can be freed or invalidated afterwards.
 *
 * @param[in] is_on_after_ble_stop  A boolean indicating if BLE should remain on after provisioning
 *
 * @return
 *  - ESP_OK              : Success
 *  - ESP_ERR_INVALID_ARG : Null argument
 */
esp_err_t wifi_prov_mgr_keep_ble_on(uint8_t is_on_after_ble_stop);

/**
 * @brief   Set manufacturer specific data in scan response
 *
 * This must be called before starting provisioning, i.e. before
 * making a call to wifi_prov_mgr_start_provisioning().
 *
 * @note    It is important to understand that length of custom manufacturer
 *          data should be within limits. The manufacturer data goes into scan
 *          response along with BLE device name. By default, BLE device name
 *          length is of 11 Bytes, however it can vary as per application use
 *          case. So, one has to honour the scan response data size limits i.e.
 *          (mfg_data_len + 2) < 31 - (device_name_length + 2 ). If the
 *          mfg_data length exceeds this limit, the length will be truncated.
 *
 * @param[in] mfg_data      Custom manufacturer data
 * @param[in] mfg_data_len  Manufacturer data length
 *
 * @return
 *  - ESP_OK              : Success
 *  - ESP_ERR_INVALID_ARG : Null argument
 */
esp_err_t wifi_prov_scheme_ble_set_mfg_data(uint8_t *mfg_data, ssize_t mfg_data_len);

/**
 * @brief   Set Bluetooth Random address
 *
 * This must be called before starting provisioning, i.e. before
 * making a call to wifi_prov_mgr_start_provisioning().
 *
 * This API can be used in cases where a new identity address is to be used during
 * provisioning. This will result in this device being treated as a new device by remote
 * devices.
 *
 * @note    This API will change the existing BD address for the device. The address once
 *          set will remain unchanged until BLE stack tear down happens when
 *          wifi_prov_mgr_deinit is invoked.
 *
 *          This API is only to be called to set random address. Re-invoking this API
 *          after provisioning is started will have no effect.
 *
 * @param[in] rand_addr     The static random address to be set of length 6 bytes.
 *
 * @return
 *  - ESP_OK              : Success
 *  - ESP_ERR_INVALID_ARG : Null argument
 */
esp_err_t wifi_prov_scheme_ble_set_random_addr(const uint8_t *rand_addr);

#ifdef __cplusplus
}
#endif
