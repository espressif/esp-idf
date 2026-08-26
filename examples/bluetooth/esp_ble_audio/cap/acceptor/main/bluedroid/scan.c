/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"

#include "cap_acceptor.h"

/* Owned by peripheral.c (single host_sem, serialized init). */
extern SemaphoreHandle_t cap_sem;
extern esp_bt_status_t cap_op_status;

#define WAIT_API(_call) EXAMPLE_WAIT_API_CHECK(_call, cap_sem, portMAX_DELAY, cap_op_status)

#if CONFIG_EXAMPLE_SCAN_SELF
static esp_ble_ext_scan_params_t ext_scan_params = {
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_duplicate = BLE_SCAN_DUPLICATE_DISABLE,
    .cfg_mask = ESP_BLE_GAP_EXT_SCAN_CFG_UNCODE_MASK,
    .uncoded_cfg = {
        .scan_type = BLE_SCAN_TYPE_PASSIVE,
        .scan_interval = SCAN_INTERVAL,
        .scan_window = SCAN_WINDOW,
    },
};
#endif /* CONFIG_EXAMPLE_SCAN_SELF */

#if CONFIG_EXAMPLE_SCAN_SELF
int ext_scan_start(void)
{
    WAIT_API(esp_ble_gap_set_ext_scan_params(&ext_scan_params));
    WAIT_API(esp_ble_gap_start_ext_scan(0, 0));

    ESP_LOGI(TAG, "Scanning for broadcast source...");
    return 0;
}

int ext_scan_stop(void)
{
    WAIT_API(esp_ble_gap_stop_ext_scan());
    return 0;
}
#endif /* CONFIG_EXAMPLE_SCAN_SELF */

int pa_sync_create(uint8_t addr_type, const uint8_t addr[6], uint8_t sid)
{
    esp_ble_gap_periodic_adv_sync_params_t params = {
        .filter_policy = 0,
        .sid = sid,
        .addr_type = addr_type,
        .skip = PA_SYNC_SKIP,
        .sync_timeout = PA_SYNC_TIMEOUT,
    };

    memcpy(params.addr, addr, sizeof(params.addr));

    /* Fire-and-forget: sync establishment (PERIODIC_ADV_SYNC_ESTAB_EVT) is
     * air-dependent and surfaces asynchronously. */
    return esp_ble_gap_periodic_adv_create_sync(&params);
}

int pa_sync_with_past(uint16_t conn_handle, uint8_t addr_type, const uint8_t addr[6])
{
    /* PAST receive params. Mode 0x02 = sync + report events, no dedup.
     * mode>=1 also implicitly enables the periodic adv report stream after the
     * controller auto-syncs via PAST — no separate recv_enable needed. */
    esp_ble_gap_past_params_t params = {
        .mode = ESP_BLE_GAP_PAST_MODE_DUP_FILTER_DISABLED,
        .skip = PA_SYNC_SKIP,
        .sync_timeout = PA_SYNC_TIMEOUT,
        .cte_type = 0,
    };
    /* Use Assistant's BD addr (peer.dst from ACL_CONNECT), not the broadcast
     * source addr from `addr` — BTM dispatches the HCI cmd over the
     * Assistant's ACL. */
    esp_bd_addr_t peer_addr;

    (void)conn_handle;
    (void)addr_type;
    (void)addr;

    memcpy(peer_addr, peer.dst, sizeof(peer_addr));

    WAIT_API(esp_ble_gap_set_periodic_adv_sync_trans_params(peer_addr, &params));

    /* Actual sync handle is delivered later via PA_SYNC_PAST app event. */
    return 0;
}

int pa_past_cancel(uint16_t conn_handle)
{
    /* mode=0 → controller no longer auto-syncs incoming PAST from Assistant.
     * Like pa_sync_with_past, addressed by Assistant's BD addr (peer.dst). */
    esp_ble_gap_past_params_t params = {
        .mode = ESP_BLE_GAP_PAST_MODE_NO_SYNC_EVT,
        .skip = PA_SYNC_SKIP,
        .sync_timeout = PA_SYNC_TIMEOUT,
        .cte_type = 0,
    };
    esp_bd_addr_t peer_addr;

    (void)conn_handle;

    memcpy(peer_addr, peer.dst, sizeof(peer_addr));

    WAIT_API(esp_ble_gap_set_periodic_adv_sync_trans_params(peer_addr, &params));

    return 0;
}

int pa_sync_terminate(uint16_t sync_handle)
{
    /* iso bluedroid/gap.c synthesizes a PA_SYNC_LOST event on
     * BTM_BLE_5_GAP_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT, matching
     * NimBLE host behavior — the app's pa_sync_lost handler runs
     * broadcast_pa_lost automatically. */
    return esp_ble_gap_periodic_adv_sync_terminate(sync_handle);
}
