/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "esp_log.h"

#include "host/ble_gap.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"

#include "os/os_mbuf.h"

#include "cap_initiator.h"

/* Non-connectable broadcaster: no GAP events for the application. */
static int gap_event_cb(struct ble_gap_event *event, void *arg)
{
    return 0;
}

int app_host_init(void)
{
    return 0;
}

int set_device_name(void)
{
    return ble_svc_gap_device_name_set(LOCAL_DEVICE_NAME);
}

int ext_adv_start(const uint8_t *ext_data, uint8_t ext_len,
                  const uint8_t *per_data, uint8_t per_len)
{
    struct ble_gap_periodic_adv_params per_params = {0};
    struct ble_gap_ext_adv_params ext_params = {0};
    struct os_mbuf *data = NULL;
    int err;

    ext_params.connectable = 0;
    ext_params.scannable = 0;
    ext_params.legacy_pdu = 0;
    ext_params.own_addr_type = BLE_OWN_ADDR_PUBLIC;
    ext_params.primary_phy = BLE_HCI_LE_PHY_1M;
    ext_params.secondary_phy = BLE_HCI_LE_PHY_2M;
    ext_params.tx_power = ADV_TX_POWER;
    ext_params.sid = ADV_SID;
    ext_params.itvl_min = BLE_GAP_ADV_ITVL_MS(ADV_INTERVAL_MS);
    ext_params.itvl_max = BLE_GAP_ADV_ITVL_MS(ADV_INTERVAL_MS);

    err = ble_gap_ext_adv_configure(ADV_HANDLE, &ext_params, NULL,
                                    gap_event_cb, NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to configure ext adv params, err %d", err);
        return err;
    }

    data = os_msys_get_pkthdr(ext_len, 0);
    if (data == NULL) {
        ESP_LOGE(TAG, "Failed to get ext adv mbuf");
        return -1;
    }

    err = os_mbuf_append(data, ext_data, ext_len);
    if (err) {
        ESP_LOGE(TAG, "Failed to append ext adv data, err %d", err);
        os_mbuf_free_chain(data);
        return err;
    }

    err = ble_gap_ext_adv_set_data(ADV_HANDLE, data);
    if (err) {
        ESP_LOGE(TAG, "Failed to set ext adv data, err %d", err);
        return err;
    }

    per_params.include_tx_power = 0;
    per_params.itvl_min = BLE_GAP_PERIODIC_ITVL_MS(PER_ADV_INTERVAL_MS);
    per_params.itvl_max = BLE_GAP_PERIODIC_ITVL_MS(PER_ADV_INTERVAL_MS);

    err = ble_gap_periodic_adv_configure(ADV_HANDLE, &per_params);
    if (err) {
        ESP_LOGE(TAG, "Failed to configure per adv params, err %d", err);
        return err;
    }

    data = os_msys_get_pkthdr(per_len, 0);
    if (data == NULL) {
        ESP_LOGE(TAG, "Failed to get per adv mbuf");
        return -1;
    }

    err = os_mbuf_append(data, per_data, per_len);
    if (err) {
        ESP_LOGE(TAG, "Failed to append per adv data, err %d", err);
        os_mbuf_free_chain(data);
        return err;
    }

    err = ble_gap_periodic_adv_set_data(ADV_HANDLE, data);
    if (err) {
        ESP_LOGE(TAG, "Failed to set per adv data, err %d", err);
        return err;
    }

    err = ble_gap_periodic_adv_start(ADV_HANDLE);
    if (err) {
        ESP_LOGE(TAG, "Failed to start per advertising, err %d", err);
        return err;
    }

    err = ble_gap_ext_adv_start(ADV_HANDLE, 0, 0);
    if (err) {
        ESP_LOGE(TAG, "Failed to start ext advertising, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Advertising started (handle %u)", ADV_HANDLE);
    return 0;
}
