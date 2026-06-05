/*
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>

#include "nvs_flash.h"

#include "tmap_central.h"

static uint16_t default_conn_handle = CONN_HANDLE_INIT;
static bool disc_completed;
static bool mtu_exchanged;

uint16_t default_conn_handle_get(void)
{
    return default_conn_handle;
}

static void tmap_discovery_complete(esp_ble_audio_tmap_role_t role,
                                    esp_ble_conn_t *conn, int err)
{
    if (err) {
        ESP_LOGE(TAG, "TMAP discovery completed, err %d", err);
        return;
    }

    if (conn->handle != default_conn_handle) {
        return;
    }

    ESP_LOGI(TAG, "TMAP discovery done");

    err = cap_initiator_setup();
    if (err) {
        return;
    }
}

static esp_ble_audio_tmap_cb_t tmap_callbacks = {
    .discovery_complete = tmap_discovery_complete,
};

static bool check_and_connect(uint8_t type, const uint8_t *data,
                              uint8_t data_len, void *user_data)
{
    esp_ble_audio_gap_app_event_t *event;
    uint16_t tmap_role;
    uint16_t uuid_val;
    int err;

    event = user_data;
    assert(event);

    if (type != EXAMPLE_AD_TYPE_SERVICE_DATA16) {
        return true;    /* Continue parsing to next AD data type */
    }

    if (data_len < sizeof(uuid_val)) {
        ESP_LOGW(TAG, "Invalid ad size %u (tmap uuid)", data_len);
        return true;    /* Continue parsing to next AD data type */
    }

    uuid_val = sys_get_le16(data);

    if (uuid_val != ESP_BLE_AUDIO_UUID_TMAS_VAL) {
        /* We are looking for the TMAS service data */
        return true;    /* Continue parsing to next AD data type */
    }

    ESP_LOGI(TAG, "Found TMAS in peer adv data!");

    if (data_len < sizeof(uuid_val) + sizeof(tmap_role)) {
        ESP_LOGW(TAG, "Invalid ad size %u (tmap role)", data_len);
        return false;   /* Stop parsing */
    }

    tmap_role = sys_get_le16(data + sizeof(uuid_val));

    if ((tmap_role & ESP_BLE_AUDIO_TMAP_ROLE_UMR) == 0) {
        ESP_LOGW(TAG, "No TMAS UMR support!");
        return false;   /* Stop parsing */
    }

    err = ext_scan_stop();
    if (err) {
        ESP_LOGE(TAG, "Failed to stop scanning, err %d", err);
        return false;
    }

    err = conn_create(event->ext_scan_recv.addr.type,
                      event->ext_scan_recv.addr.val);
    if (err) {
        ESP_LOGE(TAG, "Failed to create conn, err %d", err);
        ext_scan_start();
    }

    return false;   /* Stop parsing */
}

static void ext_scan_recv(esp_ble_audio_gap_app_event_t *event)
{
    if (default_conn_handle != CONN_HANDLE_INIT) {
        return;
    }

    /* Check if the advertising is connectable and if TMAS is supported */
    if (event->ext_scan_recv.event_type & EXAMPLE_ADV_PROP_CONNECTABLE) {
        esp_ble_audio_data_parse(event->ext_scan_recv.data,
                                 event->ext_scan_recv.data_len,
                                 check_and_connect, (void *)event);
    }
}

static void acl_connect(esp_ble_audio_gap_app_event_t *event)
{
    int err;

    if (event->acl_connect.status) {
        ESP_LOGE(TAG, "Connection failed, status %d", event->acl_connect.status);
        return;
    }

    ESP_LOGI(TAG, "Connected: handle %u role %u peer %02x:%02x:%02x:%02x:%02x:%02x",
             event->acl_connect.conn_handle, event->acl_connect.role,
             EXAMPLE_BT_ADDR_PRINT_ARGS(event->acl_connect.dst.val));

    default_conn_handle = event->acl_connect.conn_handle;

    err = pairing_start(event->acl_connect.conn_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to initiate security, err %d", err);
        return;
    }
}

static void acl_disconnect(esp_ble_audio_gap_app_event_t *event)
{
    ESP_LOGI(TAG, "Disconnected: handle %u reason 0x%02x",
             event->acl_disconnect.conn_handle, event->acl_disconnect.reason);

    default_conn_handle = CONN_HANDLE_INIT;
    disc_completed = false;
    mtu_exchanged = false;

    unicast_group_delete();

    ext_scan_start();
}

static void security_change(esp_ble_iso_gap_app_event_t *event)
{
    int err;

    if (event->security_change.status) {
        security_failed_recover(event->security_change.conn_handle,
                                event->security_change.status);
        return;
    }

    ESP_LOGI(TAG, "Security: handle %u level %u bonded %u",
             event->security_change.conn_handle,
             event->security_change.sec_level,
             event->security_change.bonded);

    err = exchange_mtu(event->security_change.conn_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to exchange MTU, err %d", err);
        return;
    }
}

static void iso_gap_app_cb(esp_ble_audio_gap_app_event_t *event)
{
    switch (event->type) {
    case ESP_BLE_AUDIO_GAP_EVENT_EXT_SCAN_RECV:
        ext_scan_recv(event);
        break;
    case ESP_BLE_AUDIO_GAP_EVENT_ACL_CONNECT:
        acl_connect(event);
        break;
    case ESP_BLE_AUDIO_GAP_EVENT_ACL_DISCONNECT:
        acl_disconnect(event);
        break;
    case ESP_BLE_AUDIO_GAP_EVENT_SECURITY_CHANGE:
        security_change(event);
        break;
    default:
        break;
    }
}

static void gatt_mtu_change(esp_ble_audio_gatt_app_event_t *event)
{
    esp_err_t err;

    ESP_LOGI(TAG, "MTU updated: handle %u mtu %u",
             event->gatt_mtu_change.conn_handle, event->gatt_mtu_change.mtu);

    if (event->gatt_mtu_change.mtu < ESP_BLE_AUDIO_ATT_MTU_MIN) {
        ESP_LOGW(TAG, "MTU %u below minimum %u",
                 event->gatt_mtu_change.mtu, ESP_BLE_AUDIO_ATT_MTU_MIN);
        return;
    }

    err = esp_ble_audio_gattc_disc_start(event->gatt_mtu_change.conn_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to start service discovery, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Service discovery started: handle %u",
             event->gatt_mtu_change.conn_handle);

    /* Note:
     * MTU exchanged event may arrived after discover completed event.
     */
    mtu_exchanged = true;

    if (disc_completed) {
        err = esp_ble_audio_tmap_discover(default_conn_handle, &tmap_callbacks);
        if (err) {
            ESP_LOGE(TAG, "Failed to discover tmap, err %d", err);
            return;
        }

        vcp_vol_ctlr_discover();
    }
}

static void gattc_disc_cmpl(esp_ble_audio_gatt_app_event_t *event)
{
    esp_err_t err;

    ESP_LOGI(TAG, "Service discovery complete: handle %u status %u",
             event->gattc_disc_cmpl.conn_handle,
             event->gattc_disc_cmpl.status);

    if (event->gattc_disc_cmpl.status) {
        return;
    }

    /* Note:
     * Discover completed event may arrived before MTU exchanged event.
     */
    disc_completed = true;

    if (mtu_exchanged) {
        err = esp_ble_audio_tmap_discover(default_conn_handle, &tmap_callbacks);
        if (err) {
            ESP_LOGE(TAG, "Failed to discover tmap, err %d", err);
            return;
        }

        vcp_vol_ctlr_discover();
    }
}

static void iso_gatt_app_cb(esp_ble_audio_gatt_app_event_t *event)
{
    switch (event->type) {
    case ESP_BLE_AUDIO_GATT_EVENT_GATT_MTU_CHANGE:
        gatt_mtu_change(event);
        break;
    case ESP_BLE_AUDIO_GATT_EVENT_GATTC_DISC_CMPL:
        gattc_disc_cmpl(event);
        break;
    default:
        break;
    }
}

void app_main(void)
{
    esp_ble_audio_start_info_t start_info = {0};
    esp_ble_audio_init_info_t init_info = {
        .gap_cb  = iso_gap_app_cb,
        .gatt_cb = iso_gatt_app_cb,
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

    err = app_host_init();
    if (err) {
        ESP_LOGE(TAG, "Failed to init host, err %d", err);
        return;
    }

    err = esp_ble_audio_common_init(&init_info);
    if (err) {
        ESP_LOGE(TAG, "Failed to initialize audio, err %d", err);
        return;
    }

    err = esp_ble_audio_tmap_register(ESP_BLE_AUDIO_TMAP_ROLE_CG | ESP_BLE_AUDIO_TMAP_ROLE_UMS);
    if (err) {
        ESP_LOGE(TAG, "Failed to register tmap, err %d", err);
        return;
    }

    err = cap_initiator_init();
    if (err) {
        return;
    }

    err = vcp_vol_ctlr_init();
    if (err) {
        return;
    }

    err = mcp_server_init();
    if (err) {
        return;
    }

    err = ccp_server_init();
    if (err) {
        return;
    }

    err = esp_ble_audio_common_start(&start_info);
    if (err) {
        ESP_LOGE(TAG, "Failed to start audio, err %d", err);
        return;
    }

    err = set_device_name();
    if (err) {
        ESP_LOGE(TAG, "Failed to set device name, err %d", err);
        return;
    }

    ext_scan_start();
}
