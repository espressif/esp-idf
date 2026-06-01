/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>

#include "esp_log.h"
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"

#include "esp_ble_iso_common_api.h"

#include "central.h"

static SemaphoreHandle_t scan_sem;

/* Controller status latched by gap_event_handler for EXAMPLE_WAIT_API_CHECK. */
static esp_bt_status_t scan_op_status;

#define WAIT_API(_call) EXAMPLE_WAIT_API_CHECK(_call, scan_sem, portMAX_DELAY, scan_op_status)

/* Cached peer address. Bluedroid's pairing and disconnect APIs key off
 * bd_addr rather than conn_handle, so we stash the addr at conn_create
 * time and reuse it in pairing_start / security_failed_recover. */
static esp_bd_addr_t peer_bda;

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

static void gap_event_handler(esp_gap_ble_cb_event_t event,
                              esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT:
        scan_op_status = param->set_ext_scan_params.status;
        xSemaphoreGive(scan_sem);
        break;
    case ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT:
        scan_op_status = param->ext_scan_start.status;
        xSemaphoreGive(scan_sem);
        break;
    case ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT:
        scan_op_status = param->ext_scan_stop.status;
        xSemaphoreGive(scan_sem);
        break;

    /* SMP request handling for Just Works pairing (IO_CAP=NONE). NC_REQ
     * still fires under LE Secure Connections — auto-accept. We never get
     * SEC_REQ here because the central initiates via esp_ble_set_encryption. */
    case ESP_GAP_BLE_NC_REQ_EVT:
        esp_ble_confirm_reply(param->ble_security.ble_req.bd_addr, true);
        break;

    case ESP_GAP_BLE_AUTH_CMPL_EVT:
        esp_ble_iso_gap_app_post_event(event, param);
        break;

    default:
        break;
    }
}

int app_host_init(void)
{
    esp_err_t err;

    scan_sem = xSemaphoreCreateBinary();
    if (scan_sem == NULL) {
        ESP_LOGE(TAG, "Failed to create scan semaphore");
        return -1;
    }

    err = esp_ble_gap_register_callback(gap_event_handler);
    if (err) {
        ESP_LOGE(TAG, "Failed to register GAP callback, err %d", err);
        vSemaphoreDelete(scan_sem);
        return err;
    }

    return 0;
}

int set_device_name(void)
{
    return esp_ble_gap_set_device_name(LOCAL_DEVICE_NAME);
}

int ext_scan_start(void)
{
    WAIT_API(esp_ble_gap_set_ext_scan_params(&ext_scan_params));
    WAIT_API(esp_ble_gap_start_ext_scan(0, 0));

    ESP_LOGI(TAG, "Scanning for peripheral...");
    return 0;
}

int ext_scan_stop(void)
{
    WAIT_API(esp_ble_gap_stop_ext_scan());
    return 0;
}

int conn_create(uint8_t addr_type, const uint8_t addr[6])
{
    /* Values shared with NimBLE side via central.h for behavioral parity.
     * Without prefer_ext_connect_params_set, L2CAP falls back to defaults
     * and logs "No extend connection parameters set". */
    const esp_ble_gap_conn_params_t conn_params = {
        .scan_interval       = INIT_SCAN_INTERVAL,
        .scan_window         = INIT_SCAN_WINDOW,
        .interval_min        = CONN_INTERVAL,
        .interval_max        = CONN_INTERVAL,
        .latency             = CONN_LATENCY,
        .supervision_timeout = CONN_TIMEOUT,
        .min_ce_len          = CONN_MIN_CE_LEN,
        .max_ce_len          = CONN_MAX_CE_LEN,
    };
    esp_gatt_if_t gattc_if;
    esp_err_t err;

    /* `addr` is in BD_ADDR (Bluedroid native, MSB-first) — direct copy. */
    memcpy(peer_bda, addr, sizeof(peer_bda));

    err = esp_ble_gap_prefer_ext_connect_params_set(
              peer_bda, ESP_BLE_GAP_PHY_1M_PREF_MASK, &conn_params, NULL, NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to set ext conn params, err %d", err);
        return err;
    }

    /* aux_open initiates an ACL against an extended advertiser. Use the
     * engine's gattc_if so the resulting CONNECT/OPEN events route back to
     * the engine — no separate BTA GATTC app needed.
     *
     * engine returns ESP_GATT_IF_NONE (0xFF) when GATTC is not yet registered;
     * feeding that into aux_open silently no-ops in BTC and no acl_connect
     * event ever fires, leaving the caller in a no-scan / no-conn dead state. */
    gattc_if = esp_ble_iso_bluedroid_get_gattc_if();
    if (gattc_if == ESP_GATT_IF_NONE) {
        ESP_LOGE(TAG, "GATTC not registered");
        return ESP_ERR_INVALID_STATE;
    }

    return esp_ble_gattc_aux_open(gattc_if, peer_bda,
                                  (esp_ble_addr_type_t)addr_type, true);
}

int pairing_start(uint16_t conn_handle)
{
    (void)conn_handle;
    return esp_ble_set_encryption(peer_bda, ESP_BLE_SEC_ENCRYPT_NO_MITM);
}

void security_failed_recover(uint16_t conn_handle, uint8_t status)
{
    (void)conn_handle;

    /* Asymmetric bond state: we still hold an LTK for this peer but it
     * cleared its side, so encrypt-with-cached-key times out. Drop the bond
     * and tear down the link; the next reconnect runs fresh pairing. Same
     * reasoning as the NimBLE side. */
    ESP_LOGE(TAG, "Security change failed, status %u, clearing local bond and reconnecting", status);

    esp_ble_remove_bond_device(peer_bda);
    esp_ble_gap_disconnect(peer_bda);
}
