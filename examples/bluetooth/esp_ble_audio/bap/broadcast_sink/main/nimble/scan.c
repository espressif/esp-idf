/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>

#include "esp_log.h"

#include "host/ble_gap.h"
#include "host/ble_hs.h"
#if CONFIG_EXAMPLE_SCAN_OFFLOAD
#include "host/ble_store.h"
#include "services/gap/ble_svc_gap.h"
#include "os/os_mbuf.h"
#endif /* CONFIG_EXAMPLE_SCAN_OFFLOAD */

#include "esp_ble_audio_common_api.h"

#include "scan.h"

/* Forward only the GAP events the application consumes. */
static int gap_event_cb(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_EXT_DISC:
    case BLE_GAP_EVENT_PERIODIC_SYNC:
    case BLE_GAP_EVENT_PERIODIC_REPORT:
    case BLE_GAP_EVENT_PERIODIC_SYNC_LOST:
        esp_ble_audio_gap_app_post_event(event->type, event);
        break;
#if CONFIG_EXAMPLE_SCAN_OFFLOAD
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
        struct ble_gap_conn_desc desc = {0};
        int rc = ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc);
        if (rc == 0) {
            ble_store_util_delete_peer(&desc.peer_id_addr);
        }
        return BLE_GAP_REPEAT_PAIRING_RETRY;
    }
#endif /* CONFIG_EXAMPLE_SCAN_OFFLOAD */
    default:
        break;
    }

    return 0;
}

int app_host_init(void)
{
    return 0;
}

int ext_scan_start(void)
{
    struct ble_gap_disc_params params = {0};
    uint8_t own_addr_type;
    int err;

    err = ble_hs_id_infer_auto(0, &own_addr_type);
    if (err) {
        ESP_LOGE(TAG, "Failed to determine own addr type, err %d", err);
        return err;
    }

    params.passive = 1;
    params.itvl = SCAN_INTERVAL;
    params.window = SCAN_WINDOW;

    err = ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &params,
                       gap_event_cb, NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to start scanning, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Scanning for broadcast source...");
    return 0;
}

int ext_scan_stop(void)
{
    return ble_gap_disc_cancel();
}

int pa_sync_create(uint8_t addr_type, const uint8_t addr[6], uint8_t sid)
{
    struct ble_gap_periodic_sync_params params = {0};
    ble_addr_t sync_addr = {0};

    sync_addr.type = addr_type;
    memcpy(sync_addr.val, addr, sizeof(sync_addr.val));
    params.skip = PA_SYNC_SKIP;
    params.sync_timeout = PA_SYNC_TIMEOUT;

    return ble_gap_periodic_adv_sync_create(&sync_addr, sid, &params,
                                            gap_event_cb, NULL);
}

#if CONFIG_EXAMPLE_PAST
int pa_sync_with_past(uint16_t conn_handle, const uint8_t addr[6])
{
    (void)addr;

    struct ble_gap_periodic_sync_params params = {
        .skip = PA_SYNC_SKIP,
        .sync_timeout = PA_SYNC_TIMEOUT,
    };

    return ble_gap_periodic_adv_sync_receive(conn_handle, &params,
                                             gap_event_cb, NULL);
}
#endif /* CONFIG_EXAMPLE_PAST */

int pa_sync_terminate(uint16_t sync_handle)
{
    return ble_gap_periodic_adv_sync_terminate(sync_handle);
}

#if CONFIG_EXAMPLE_SCAN_OFFLOAD
int set_device_name(void)
{
    return ble_svc_gap_device_name_set(LOCAL_DEVICE_NAME);
}

int ext_adv_start(const uint8_t *ext_data, uint8_t ext_len)
{
    struct ble_gap_ext_adv_params ext_params = {0};
    struct os_mbuf *data = NULL;
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
#endif /* CONFIG_EXAMPLE_SCAN_OFFLOAD */
