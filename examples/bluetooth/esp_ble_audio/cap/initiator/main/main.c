/*
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "nvs_flash.h"
#include "esp_system.h"

#include "cap_initiator.h"

void app_main(void)
{
    esp_ble_audio_init_info_t info = {
#if CONFIG_EXAMPLE_UNICAST
        .gap_cb  = cap_initiator_unicast_gap_cb,
        .gatt_cb = cap_initiator_unicast_gatt_cb,
#endif /* CONFIG_EXAMPLE_UNICAST */
    };
    esp_err_t err;

    /* Initialize NVS — it is used to store PHY calibration data */
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    err = bluetooth_init();
    if (err) {
        ESP_LOGE(TAG, "Failed to initialize BLE, err %d", err);
        return;
    }

    err = esp_ble_audio_common_init(&info);
    if (err) {
        ESP_LOGE(TAG, "Failed to initialize audio, err %d", err);
        return;
    }

#if CONFIG_EXAMPLE_UNICAST
    err = cap_initiator_unicast_init();
    if (err) {
        return;
    }
#endif /* CONFIG_EXAMPLE_UNICAST */

#if CONFIG_EXAMPLE_BROADCAST
    err = cap_initiator_broadcast_init();
    if (err) {
        return;
    }
#endif /* CONFIG_EXAMPLE_BROADCAST */

    cap_initiator_tx_init();

    err = esp_ble_audio_common_start(NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to start audio, err %d", err);
        return;
    }

    /* Match the GAP device name to the role this build was compiled for, so
     * the name advertised in GAP reads matches the local-name field embedded
     * in extended advertising (broadcast mode uses "CAP Broadcast Source",
     * which is what the acceptor's self-scan path matches against).
     */
#if CONFIG_EXAMPLE_BROADCAST
    err = ble_svc_gap_device_name_set("CAP Broadcast Source");
#else
    err = ble_svc_gap_device_name_set("CAP Initiator");
#endif
    if (err) {
        ESP_LOGE(TAG, "Failed to set device name, err %d", err);
        return;
    }

#if CONFIG_EXAMPLE_UNICAST
    err = cap_initiator_unicast_start();
    if (err) {
        return;
    }
#endif /* CONFIG_EXAMPLE_UNICAST */

#if CONFIG_EXAMPLE_BROADCAST
    err = cap_initiator_broadcast_start();
    if (err) {
        return;
    }
#endif /* CONFIG_EXAMPLE_BROADCAST */
}
