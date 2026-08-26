/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "esp_log.h"

#include "host/ble_gap.h"
#include "host/ble_hs.h"

#include "os/os_mbuf.h"

#include "cap_handover.h"

static int gap_event_cb(struct ble_gap_event *event, void *arg)
{
    return 0;
}

static int adv_data_set(const uint8_t *payload, uint8_t payload_len, bool periodic)
{
    struct os_mbuf *data;
    int err;

    data = os_msys_get_pkthdr(payload_len, 0);
    if (data == NULL) {
        ESP_LOGE(TAG, "Failed to get %s adv mbuf", periodic ? "per" : "ext");
        return -1;
    }

    err = os_mbuf_append(data, payload, payload_len);
    if (err) {
        ESP_LOGE(TAG, "Failed to append %s adv data, err %d", periodic ? "per" : "ext", err);
        os_mbuf_free_chain(data);
        return err;
    }

    err = periodic ? ble_gap_periodic_adv_set_data(ADV_HANDLE, data)
          : ble_gap_ext_adv_set_data(ADV_HANDLE, data);
    if (err) {
        ESP_LOGE(TAG, "Failed to set %s adv data, err %d", periodic ? "per" : "ext", err);
        return err;
    }

    return 0;
}

static int adv_set_configure(const uint8_t *ext_data, uint8_t ext_len)
{
    struct ble_gap_periodic_adv_params per_params = {0};
    struct ble_gap_ext_adv_params ext_params = {0};
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

    err = adv_data_set(ext_data, ext_len, false);
    if (err) {
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

    return 0;
}

int per_adv_data_start(const uint8_t *per_data, uint8_t per_len)
{
    int err;

    err = adv_data_set(per_data, per_len, true);
    if (err) {
        return err;
    }

    err = ble_gap_periodic_adv_start(ADV_HANDLE);
    if (err) {
        ESP_LOGE(TAG, "Failed to start per advertising, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Periodic advertising started (handle %u)", ADV_HANDLE);
    return 0;
}


int ext_adv_start_without_base(const uint8_t *ext_data, uint8_t ext_len)
{
    int err;

    err = adv_set_configure(ext_data, ext_len);
    if (err) {
        return err;
    }

    err = ble_gap_ext_adv_start(ADV_HANDLE, 0, 0);
    if (err) {
        ESP_LOGE(TAG, "Failed to start ext advertising, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Advertising started, BASE pending (handle %u)", ADV_HANDLE);
    return 0;
}

int adv_stop(void)
{
    int err;

    err = ble_gap_periodic_adv_stop(ADV_HANDLE);
    if (err) {
        ESP_LOGE(TAG, "Failed to stop per advertising, err %d", err);
        return err;
    }

    err = ble_gap_ext_adv_stop(ADV_HANDLE);
    if (err) {
        ESP_LOGE(TAG, "Failed to stop ext advertising, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Advertising stopped (handle %u)", ADV_HANDLE);
    return 0;
}

int local_public_addr_get(uint8_t addr[6])
{
    int err = ble_hs_id_copy_addr(BLE_ADDR_PUBLIC, addr, NULL);

    if (err) {
        ESP_LOGE(TAG, "Local BD address unavailable, err %d", err);
        return err;
    }

    return 0;
}

int pa_set_info_transfer(uint16_t conn_handle, const uint8_t peer_addr[6],
                         uint16_t service_data)
{
    (void)peer_addr;

    return ble_gap_periodic_adv_sync_set_info(ADV_HANDLE, conn_handle, service_data);
}
