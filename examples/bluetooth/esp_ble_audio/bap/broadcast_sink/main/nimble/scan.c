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

int pa_sync_terminate(uint16_t sync_handle)
{
    return ble_gap_periodic_adv_sync_terminate(sync_handle);
}
