/*
 * SPDX-FileCopyrightText: 2021-2022 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "nvs_flash.h"

#include "cap_acceptor.h"

static uint8_t codec_data[] =
    ESP_BLE_AUDIO_CODEC_CAP_LC3_DATA(
        ESP_BLE_AUDIO_CODEC_CAP_FREQ_ANY,               /* Sampling frequency Any */
        ESP_BLE_AUDIO_CODEC_CAP_DURATION_7_5 | \
        ESP_BLE_AUDIO_CODEC_CAP_DURATION_10,            /* Frame duration 7.5ms/10ms */
        ESP_BLE_AUDIO_CODEC_CAP_CHAN_COUNT_SUPPORT(2),  /* Supported channels 2 */
        30,                                             /* Minimum 30 octets per frame */
        155,                                            /* Maximum 155 octets per frame */
        2);                                             /* Maximum 2 codec frames per SDU */

static uint8_t codec_meta[] =
    ESP_BLE_AUDIO_CODEC_CAP_LC3_META(SINK_CONTEXT | SOURCE_CONTEXT);

static const esp_ble_audio_codec_cap_t codec_cap =
    ESP_BLE_AUDIO_CODEC_CAP_LC3(codec_data, codec_meta);

static esp_ble_audio_pacs_cap_t sink_cap = {
    .codec_cap = &codec_cap,
};

static esp_ble_audio_pacs_cap_t source_cap = {
    .codec_cap = &codec_cap,
};

struct peer_config peer = {
    .conn_handle = CONN_HANDLE_INIT,
};

static uint8_t ext_adv_data[] = {
    /* Flags */
    0x02, EXAMPLE_AD_TYPE_FLAGS, (EXAMPLE_AD_FLAGS_GENERAL | EXAMPLE_AD_FLAGS_NO_BREDR),
    /* Incomplete List of 16-bit Service UUIDs:
     *   CAS always; ASCS only when unicast role is built;
     *   BASS only when broadcast role is built.
     */
#if CONFIG_EXAMPLE_UNICAST && CONFIG_EXAMPLE_BROADCAST
    0x07, EXAMPLE_AD_TYPE_UUID16_SOME,
    (ESP_BLE_AUDIO_UUID_ASCS_VAL & 0xFF), ((ESP_BLE_AUDIO_UUID_ASCS_VAL >> 8) & 0xFF),
    (ESP_BLE_AUDIO_UUID_CAS_VAL  & 0xFF), ((ESP_BLE_AUDIO_UUID_CAS_VAL  >> 8) & 0xFF),
    (ESP_BLE_AUDIO_UUID_BASS_VAL & 0xFF), ((ESP_BLE_AUDIO_UUID_BASS_VAL >> 8) & 0xFF),
#elif CONFIG_EXAMPLE_UNICAST
    0x05, EXAMPLE_AD_TYPE_UUID16_SOME,
    (ESP_BLE_AUDIO_UUID_ASCS_VAL & 0xFF), ((ESP_BLE_AUDIO_UUID_ASCS_VAL >> 8) & 0xFF),
    (ESP_BLE_AUDIO_UUID_CAS_VAL  & 0xFF), ((ESP_BLE_AUDIO_UUID_CAS_VAL  >> 8) & 0xFF),
#elif CONFIG_EXAMPLE_BROADCAST
    0x05, EXAMPLE_AD_TYPE_UUID16_SOME,
    (ESP_BLE_AUDIO_UUID_CAS_VAL  & 0xFF), ((ESP_BLE_AUDIO_UUID_CAS_VAL  >> 8) & 0xFF),
    (ESP_BLE_AUDIO_UUID_BASS_VAL & 0xFF), ((ESP_BLE_AUDIO_UUID_BASS_VAL >> 8) & 0xFF),
#endif
    /* Service Data - 16-bit UUID */
    0x04, EXAMPLE_AD_TYPE_SERVICE_DATA16, (ESP_BLE_AUDIO_UUID_CAS_VAL & 0xFF),
    ((ESP_BLE_AUDIO_UUID_CAS_VAL >> 8) & 0xFF),
    ESP_BLE_AUDIO_UNICAST_ANNOUNCEMENT_TARGETED,
#if CONFIG_EXAMPLE_UNICAST
    /* Service Data - 16-bit UUID */
    0x09, EXAMPLE_AD_TYPE_SERVICE_DATA16, (ESP_BLE_AUDIO_UUID_ASCS_VAL & 0xFF),
    ((ESP_BLE_AUDIO_UUID_ASCS_VAL >> 8) & 0xFF),
    ESP_BLE_AUDIO_UNICAST_ANNOUNCEMENT_TARGETED,
    (SINK_CONTEXT & 0xFF),
    ((SINK_CONTEXT >> 8) & 0xFF),
    (SOURCE_CONTEXT & 0xFF),
    ((SOURCE_CONTEXT >> 8) & 0xFF),
    0x00, /* Metadata length */
#endif /* CONFIG_EXAMPLE_UNICAST */
#if CONFIG_EXAMPLE_BROADCAST
    /* Service Data - 16-bit UUID */
    0x03, EXAMPLE_AD_TYPE_SERVICE_DATA16, (ESP_BLE_AUDIO_UUID_BASS_VAL & 0xFF),
    ((ESP_BLE_AUDIO_UUID_BASS_VAL >> 8) & 0xFF),
#endif /* CONFIG_EXAMPLE_BROADCAST */
    /* Complete Device Name */
    0x0d, EXAMPLE_AD_TYPE_NAME_COMPLETE, 'c', 'a', 'p', '_', 'a', 'c', 'c', 'e', 'p', 't', 'o', 'r',
};

esp_ble_audio_cap_stream_t *stream_alloc(esp_ble_audio_dir_t dir)
{
    if (dir == ESP_BLE_AUDIO_DIR_SINK) {
        return &peer.sink_stream;
    }

    if (dir == ESP_BLE_AUDIO_DIR_SOURCE) {
        return &peer.source_stream;
    }

    return NULL;
}

void stream_released(const esp_ble_audio_cap_stream_t *cap_stream)
{
    if (cap_stream == &peer.source_stream) {
        ESP_LOGI(TAG, "Source stream released");
    } else if (cap_stream == &peer.sink_stream) {
        ESP_LOGI(TAG, "Sink stream released");
    }
}

static int register_pac(esp_ble_audio_dir_t dir,
                        esp_ble_audio_context_t context,
                        esp_ble_audio_pacs_cap_t *cap)
{
    esp_err_t err;

    err = esp_ble_audio_pacs_cap_register(dir, cap);
    if (err) {
        ESP_LOGE(TAG, "Failed to register pacs capabilities, err %d", err);
        return err;
    }

    /* Note:
     * If using ESP_BLE_AUDIO_LOCATION_MONO_AUDIO, Samsung S24 will not perform
     * the BAP unicast related procedures.
     */
    err = esp_ble_audio_pacs_set_location(dir, (ESP_BLE_AUDIO_LOCATION_FRONT_LEFT |
                                                ESP_BLE_AUDIO_LOCATION_FRONT_RIGHT));
    if (err) {
        ESP_LOGE(TAG, "Failed to set pacs location, err %d", err);
        return err;
    }

    err = esp_ble_audio_pacs_set_supported_contexts(dir, context);
    if (err) {
        ESP_LOGE(TAG, "Failed to set supported contexts, err %d", err);
        return err;
    }

    err = esp_ble_audio_pacs_set_available_contexts(dir, context);
    if (err) {
        ESP_LOGE(TAG, "Failed to set available contexts, err %d", err);
        return err;
    }

    return 0;
}

static int init_cap_acceptor(void)
{
    const esp_ble_audio_pacs_register_param_t pacs_param = {
        .snk_pac = true,
        .snk_loc = true,
        .src_pac = true,
        .src_loc = true,
    };
    int err;

    err = esp_ble_audio_pacs_register(&pacs_param);
    if (err) {
        ESP_LOGE(TAG, "Failed to register pacs, err %d", err);
        return -1;
    }

    err = register_pac(ESP_BLE_AUDIO_DIR_SINK, SINK_CONTEXT, &sink_cap);
    if (err) {
        ESP_LOGE(TAG, "Failed to register sink capabilities, err %d", err);
        return -1;
    }

    err = register_pac(ESP_BLE_AUDIO_DIR_SOURCE, SOURCE_CONTEXT, &source_cap);
    if (err) {
        ESP_LOGE(TAG, "Failed to register source capabilities, err %d", err);
        return -1;
    }

    return 0;
}

#if CONFIG_EXAMPLE_BROADCAST
#if CONFIG_EXAMPLE_SCAN_SELF
static void ext_scan_recv(esp_ble_audio_gap_app_event_t *event)
{
    broadcast_scan_recv(event);
}
#endif /* CONFIG_EXAMPLE_SCAN_SELF */

static void pa_sync(esp_ble_audio_gap_app_event_t *event)
{
    if (event->pa_sync.status) {
        ESP_LOGE(TAG, "PA sync failed, status %d", event->pa_sync.status);
        broadcast_pa_sync_failed(event);
        return;
    }

    broadcast_pa_synced(event);
}

static void pa_sync_lost(esp_ble_audio_gap_app_event_t *event)
{
    ESP_LOGI(TAG, "PA sync lost: sync_handle %u reason 0x%02x",
             event->pa_sync_lost.sync_handle, event->pa_sync_lost.reason);

    broadcast_pa_lost(event->pa_sync_lost.sync_handle);
}
#endif /* CONFIG_EXAMPLE_BROADCAST */

static void acl_connect(esp_ble_audio_gap_app_event_t *event)
{
    if (event->acl_connect.status) {
        ESP_LOGE(TAG, "Connection failed, status %d", event->acl_connect.status);
        return;
    }

    ESP_LOGI(TAG, "Connected: handle %u role %u peer %02x:%02x:%02x:%02x:%02x:%02x",
             event->acl_connect.conn_handle, event->acl_connect.role,
             EXAMPLE_BT_ADDR_PRINT_ARGS(event->acl_connect.dst.val));

    peer.conn_handle = event->acl_connect.conn_handle;
    memcpy(peer.dst, event->acl_connect.dst.val, sizeof(peer.dst));
}

static void acl_disconnect(esp_ble_audio_gap_app_event_t *event)
{
    ESP_LOGI(TAG, "Disconnected: handle %u reason 0x%02x",
             event->acl_disconnect.conn_handle, event->acl_disconnect.reason);

    peer.conn_handle = CONN_HANDLE_INIT;
    memset(peer.dst, 0, sizeof(peer.dst));

    ext_adv_start(ext_adv_data, sizeof(ext_adv_data));
}

static void security_change(esp_ble_audio_gap_app_event_t *event)
{
    if (event->security_change.status) {
        ESP_LOGE(TAG, "Security failed: handle %u status %d",
                 event->security_change.conn_handle,
                 event->security_change.status);
        return;
    }

    ESP_LOGI(TAG, "Security: handle %u level %u bonded %u",
             event->security_change.conn_handle,
             event->security_change.sec_level,
             event->security_change.bonded);
}

static void iso_gap_app_cb(esp_ble_audio_gap_app_event_t *event)
{
    switch (event->type) {
#if CONFIG_EXAMPLE_BROADCAST
#if CONFIG_EXAMPLE_SCAN_SELF
    case ESP_BLE_AUDIO_GAP_EVENT_EXT_SCAN_RECV:
        ext_scan_recv(event);
        break;
#endif /* CONFIG_EXAMPLE_SCAN_SELF */
    case ESP_BLE_AUDIO_GAP_EVENT_PA_SYNC:
    case ESP_BLE_AUDIO_GAP_EVENT_PA_SYNC_PAST:
        pa_sync(event);
        break;
    case ESP_BLE_AUDIO_GAP_EVENT_PA_SYNC_LOST:
        pa_sync_lost(event);
        break;
#endif /* CONFIG_EXAMPLE_BROADCAST */
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
    uint16_t conn_handle = event->gatt_mtu_change.conn_handle;
    int err;

    ESP_LOGI(TAG, "MTU updated: handle %u mtu %u",
             conn_handle, event->gatt_mtu_change.mtu);

    if (event->gatt_mtu_change.mtu < ESP_BLE_AUDIO_ATT_MTU_MIN) {
        ESP_LOGW(TAG, "Invalid new mtu %u, shall be at least %u",
                 event->gatt_mtu_change.mtu, ESP_BLE_AUDIO_ATT_MTU_MIN);
        return;
    }

    /* This function only needs to be invoked when:
     * - The device is a Peripheral (Link Layer role);
     * - The device works as a GATT client.
     */
    err = esp_ble_audio_gattc_disc_start(conn_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to start svc disc, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Service discovery started: handle %u", conn_handle);
}

static void gattc_disc_cmpl(esp_ble_audio_gatt_app_event_t *event)
{
    ESP_LOGI(TAG, "Service discovery complete: handle %u",
             event->gattc_disc_cmpl.conn_handle);
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
    esp_ble_audio_init_info_t info = {
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

    err = esp_ble_audio_common_init(&info);
    if (err) {
        ESP_LOGE(TAG, "Failed to initialize audio, err %d", err);
        return;
    }

    err = init_cap_acceptor();
    if (err) {
        return;
    }

#if CONFIG_EXAMPLE_UNICAST
    err = cap_acceptor_unicast_init(&peer);
    if (err) {
        return;
    }
#endif /* CONFIG_EXAMPLE_UNICAST */

#if CONFIG_EXAMPLE_BROADCAST
    err = cap_acceptor_broadcast_init();
    if (err) {
        return;
    }
#endif /* CONFIG_EXAMPLE_BROADCAST */

    err = esp_ble_audio_common_start(NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to start audio, err %d", err);
        return;
    }

    err = set_device_name();
    if (err) {
        ESP_LOGE(TAG, "Failed to set device name, err %d", err);
        return;
    }

#if CONFIG_EXAMPLE_SCAN_SELF
    err = check_start_scan();
    if (err) {
        return;
    }
#endif /* CONFIG_EXAMPLE_SCAN_SELF */

    /* Advertising will be used by Unicast Server and
     * Broadcast Sink when self-scanning is disabled.
     */
    ext_adv_start(ext_adv_data, sizeof(ext_adv_data));
}
