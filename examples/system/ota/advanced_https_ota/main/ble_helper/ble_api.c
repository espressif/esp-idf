// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#include "sdkconfig.h"

#if CONFIG_BT_BLE_ENABLED || CONFIG_BT_NIMBLE_ENABLED

#include "ble_api.h"
#include "esp_log.h"
#ifdef CONFIG_BT_NIMBLE_ENABLED
#include "nimble_gatts.h"
#elif CONFIG_BT_BLE_ENABLED
#include "bluedroid_gatts.h"
#endif

#if CONFIG_BT_BLE_ENABLED
static const char *TAG = "BLE_API";
#endif

void esp_ble_helper_init(void)
{
    esp_err_t err;
#if CONFIG_BT_BLE_ENABLED
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    err = esp_bt_controller_init(&bt_cfg);
    if (err) {
        ESP_LOGE(TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(err));
        return;
    }

    err = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (err) {
        ESP_LOGE(TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(err));
        return;
    }
    err = esp_bluedroid_init();
    if (err) {
        ESP_LOGE(TAG, "%s init bluetooth failed: %s\n", __func__, esp_err_to_name(err));
        return;
    }
    err = esp_bluedroid_enable();
    if (err) {
        ESP_LOGE(TAG, "%s enable bluetooth failed: %s\n", __func__, esp_err_to_name(err));
        return;
    }

    err = esp_ble_gatts_register_callback(gatts_event_handler);
    if (err) {
        ESP_LOGE(TAG, "gatts register error, error code = %x", err);
        return;
    }
    err = esp_ble_gap_register_callback(gap_event_handler);
    if (err) {
        ESP_LOGE(TAG, "gap register error, error code = %x", err);
        return;
    }
    err = esp_ble_gatts_app_register(PROFILE_A_APP_ID);
    if (err) {
        ESP_LOGE(TAG, "gatts app register error, error code = %x", err);
        return;
    }
    esp_err_t local_mtu_err = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_err) {
        ESP_LOGE(TAG, "set local  MTU failed, error code = %x", local_mtu_err);
    }

#elif CONFIG_BT_NIMBLE_ENABLED

    ESP_ERROR_CHECK(esp_nimble_hci_and_controller_init());

    nimble_port_init();
    /* Initialize the NimBLE host configuration. */
    ble_hs_cfg.reset_cb = bleprph_on_reset;
    ble_hs_cfg.sync_cb = bleprph_on_sync;
    ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    err = nimble_gatt_svr_init();
    assert(err == 0);

    /* Set the default device name. */
    err = ble_svc_gap_device_name_set("ESP_OTA_GATTS");
    assert(err == 0);

    ble_store_config_init();

    nimble_port_freertos_init(bleprph_host_task);

#endif
}

#endif
