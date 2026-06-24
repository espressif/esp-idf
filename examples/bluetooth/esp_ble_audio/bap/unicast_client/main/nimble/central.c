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
#include "host/ble_store.h"
#include "services/gap/ble_svc_gap.h"

#include "esp_ble_audio_common_api.h"

#include "central.h"

/* Init/conn parameters are shared with the bluedroid wrapper via central.h.
 * CONN_DURATION is NimBLE-specific (ble_gap_connect's discovery timeout). */
#define CONN_DURATION           10000   /* 10s */

static int gap_event_cb(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_EXT_DISC:
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

int ext_scan_start(void)
{
    struct ble_gap_disc_params params = {0};
    uint8_t own_addr_type;
    int err;

    err = ble_hs_id_infer_auto(0, &own_addr_type);
    if (err) {
        ESP_LOGE(TAG, "Failed to determine address type, err %d", err);
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

    ESP_LOGI(TAG, "Scanning for unicast server...");
    return 0;
}

int ext_scan_stop(void)
{
    return ble_gap_disc_cancel();
}

int conn_create(uint8_t addr_type, const uint8_t addr[6])
{
    struct ble_gap_conn_params params = {0};
    uint8_t own_addr_type = 0;
    ble_addr_t dst = {0};
    int err;

    err = ble_hs_id_infer_auto(0, &own_addr_type);
    if (err) {
        ESP_LOGE(TAG, "Failed to determine address type, err %d", err);
        return err;
    }

    params.scan_itvl = INIT_SCAN_INTERVAL;
    params.scan_window = INIT_SCAN_WINDOW;
    params.itvl_min = CONN_INTERVAL;
    params.itvl_max = CONN_INTERVAL;
    params.latency = CONN_LATENCY;
    params.supervision_timeout = CONN_TIMEOUT;
    params.max_ce_len = CONN_MAX_CE_LEN;
    params.min_ce_len = CONN_MIN_CE_LEN;

    dst.type = addr_type;
    memcpy(dst.val, addr, sizeof(dst.val));

    return ble_gap_connect(own_addr_type, &dst, CONN_DURATION,
                           &params, gap_event_cb, NULL);
}

int pairing_start(uint16_t conn_handle)
{
    return ble_gap_security_initiate(conn_handle);
}

int exchange_mtu(uint16_t conn_handle)
{
    return ble_gattc_exchange_mtu(conn_handle, NULL, NULL);
}

void security_failed_recover(uint16_t conn_handle, uint8_t status)
{
    struct ble_gap_conn_desc desc = {0};
    int rc;

    /* status 13 = BLE_HS_ETIMEOUT: SMP exchange did not complete. Typical
     * cause is asymmetric bond state — drop the stale entry and tear down
     * the link so the next reconnect runs fresh pairing. */
    ESP_LOGE(TAG, "Security change failed, status %u, clearing local bond and reconnecting", status);

    rc = ble_gap_conn_find(conn_handle, &desc);
    if (rc == 0) {
        ble_store_util_delete_peer(&desc.peer_id_addr);
    }

    ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
}
