/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "esp_log.h"

#include "host/ble_gap.h"
#include "host/ble_store.h"
#include "services/gap/ble_svc_gap.h"

#include "esp_ble_audio_common_api.h"

#include "peripheral.h"

static int gap_event_cb(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
    case BLE_GAP_EVENT_DISCONNECT:
    case BLE_GAP_EVENT_ENC_CHANGE:
        esp_ble_audio_gap_app_post_event(event->type, event);
        break;
    case BLE_GAP_EVENT_MTU:
    case BLE_GAP_EVENT_NOTIFY_RX:
    case BLE_GAP_EVENT_NOTIFY_TX:
    case BLE_GAP_EVENT_SUBSCRIBE:
        esp_ble_audio_gatt_app_post_event(event->type, event);
        break;
    case BLE_GAP_EVENT_REPEAT_PAIRING: {
        /* Peer wants to re-pair on top of an existing bond — delete our stale
         * bond entry and tell NimBLE to retry pairing. */
        struct ble_gap_conn_desc desc = {0};
        int rc = ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc);
        if (rc == 0) {
            ble_store_util_delete_peer(&desc.peer_id_addr);
        }
        return BLE_GAP_REPEAT_PAIRING_RETRY;
    }
    default:
        break;
    }

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

int ext_adv_start(const uint8_t *ext_data, uint16_t ext_len)
{
    struct ble_gap_ext_adv_params ext_params = {0};
    struct os_mbuf *data;
    int err;

    ext_params.connectable = 1;
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
        /* ble_gap_ext_adv_set_data takes ownership of `data` and frees it via
         * its 'done' label on both success and failure paths — do NOT free
         * here (double-free). API header doesn't document this contract. */
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
