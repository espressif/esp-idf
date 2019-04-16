// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
 *          atleast till provisioning is started. Upon start, the
 *          manager will store an internal copy of this UUID, and
 *          this data can be freed or invalidated afterwords.
 *
 * @param[in] uuid128  A custom 128 bit UUID
 *
 * @return
 *  - ESP_OK              : Success
 *  - ESP_ERR_INVALID_ARG : Null argument
 */
esp_err_t wifi_prov_scheme_ble_set_service_uuid(uint8_t *uuid128);

#ifdef __cplusplus
}
#endif
