/*
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "cap_initiator.h"

#define SCAN_INTERVAL       160     /* 100ms */
#define SCAN_WINDOW         160     /* 100ms */

#define INIT_SCAN_INTERVAL  16      /* 10ms */
#define INIT_SCAN_WINDOW    16      /* 10ms */
#define CONN_INTERVAL       64      /* 64 * 1.25 = 80ms */
#define CONN_LATENCY        0
#define CONN_TIMEOUT        500     /* 500 * 10ms = 5s */
#define CONN_MAX_CE_LEN     0xFFFF
#define CONN_MIN_CE_LEN     0xFFFF
#define CONN_DURATION       10000   /* 10s */

ESP_BLE_AUDIO_BAP_LC3_UNICAST_PRESET_16_2_1_DEFINE(unicast_preset_16_2_1,
                                                   ESP_BLE_AUDIO_LOCATION_MONO_AUDIO,
                                                   ESP_BLE_AUDIO_CONTEXT_TYPE_UNSPECIFIED);
static esp_ble_audio_cap_unicast_group_t *unicast_group;

static struct peer_config {
    esp_ble_audio_cap_stream_t source_stream;
    esp_ble_audio_cap_stream_t sink_stream;
    esp_ble_audio_bap_ep_t *source_ep;
    esp_ble_audio_bap_ep_t *sink_ep;

    esp_ble_conn_t *conn;
    uint16_t conn_handle;
    bool disc_completed;
    bool disc_cancelled;
    bool mtu_exchanged;
} peer = {
    .conn_handle = CONN_HANDLE_INIT,
};

static example_audio_rx_metrics_t rx_metrics;

static const char *dir_str(esp_ble_audio_dir_t dir)
{
    return dir == ESP_BLE_AUDIO_DIR_SINK ? "SNK" : "SRC";
}

static const char *stream_dir_str(const esp_ble_audio_bap_stream_t *stream)
{
    if (stream == &peer.sink_stream.bap_stream) {
        return "SNK";
    } else if (stream == &peer.source_stream.bap_stream) {
        return "SRC";
    }
    return "???";
}

static int stream_index(const esp_ble_audio_bap_stream_t *stream)
{
    /* Only one sink and one source per peer in this example. */
    (void)stream;
    return 0;
}

static bool is_tx_stream(esp_ble_audio_bap_stream_t *stream)
{
    esp_ble_audio_bap_ep_info_t ep_info = {0};
    esp_err_t err;

    err = esp_ble_audio_bap_ep_get_info(stream->ep, &ep_info);
    if (err) {
        ESP_LOGE(TAG, "Failed to get ep info, err %d", err);
        return false;
    }

    return (ep_info.dir == ESP_BLE_AUDIO_DIR_SINK);
}

static void unicast_stream_configured_cb(esp_ble_audio_bap_stream_t *stream,
                                         const esp_ble_audio_bap_qos_cfg_pref_t *pref)
{
    ESP_LOGI(TAG, "[%s #%d] Stream configured, QoS preference:",
             stream_dir_str(stream), stream_index(stream));

    example_print_qos_pref(TAG, pref);
}

static void unicast_stream_qos_set_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] QoS set",
             stream_dir_str(stream), stream_index(stream));
}

static void unicast_stream_enabled_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] Stream enabled",
             stream_dir_str(stream), stream_index(stream));
}

static void unicast_stream_started_cb(esp_ble_audio_bap_stream_t *stream)
{
    esp_ble_audio_cap_stream_t *cap_stream;
    int err;

    ESP_LOGI(TAG, "[%s #%d] Stream started",
             stream_dir_str(stream), stream_index(stream));

    example_audio_rx_metrics_reset(&rx_metrics);

    if (is_tx_stream(stream)) {
        cap_stream = CONTAINER_OF(stream, esp_ble_audio_cap_stream_t, bap_stream);

        err = cap_initiator_tx_register_stream(cap_stream, false);
        if (err) {
            ESP_LOGE(TAG, "[%s #%d] Failed to register TX, err %d",
                     stream_dir_str(stream), stream_index(stream), err);
        }
    }
}

static void unicast_stream_metadata_updated_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] Metadata updated",
             stream_dir_str(stream), stream_index(stream));
}

static void unicast_stream_disabled_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] Stream disabled",
             stream_dir_str(stream), stream_index(stream));
}

static void unicast_stream_stopped_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    esp_ble_audio_cap_stream_t *cap_stream;

    ESP_LOGI(TAG, "[%s #%d] Stream stopped, reason 0x%02x",
             stream_dir_str(stream), stream_index(stream), reason);

    if (is_tx_stream(stream)) {
        cap_stream = CONTAINER_OF(stream, esp_ble_audio_cap_stream_t, bap_stream);

        (void)cap_initiator_tx_unregister_stream(cap_stream);
    }
}

static void unicast_stream_disconnected_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    esp_ble_audio_cap_stream_t *cap_stream;

    ESP_LOGI(TAG, "[%s #%d] ISO disconnected, reason 0x%02x",
             stream_dir_str(stream), stream_index(stream), reason);

    if (is_tx_stream(stream)) {
        cap_stream = CONTAINER_OF(stream, esp_ble_audio_cap_stream_t, bap_stream);

        (void)cap_initiator_tx_unregister_stream(cap_stream);
    }
}

static void unicast_stream_released_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] Stream released",
             stream_dir_str(stream), stream_index(stream));
}

static void unicast_stream_recv_cb(esp_ble_audio_bap_stream_t *stream,
                                   const esp_ble_iso_recv_info_t *info,
                                   const uint8_t *data, uint16_t len)
{
    char name[24];

    snprintf(name, sizeof(name), "%s #%d",
             stream_dir_str(stream), stream_index(stream));
    rx_metrics.last_sdu_len = len;
    example_audio_rx_metrics_on_recv(info, &rx_metrics, TAG, name);
}

static void unicast_stream_sent_cb(esp_ble_audio_bap_stream_t *stream, void *user_data)
{
    cap_initiator_tx_stream_sent(stream, user_data);
}

static esp_ble_audio_bap_stream_ops_t unicast_stream_ops = {
    .configured       = unicast_stream_configured_cb,
    .qos_set          = unicast_stream_qos_set_cb,
    .enabled          = unicast_stream_enabled_cb,
    .started          = unicast_stream_started_cb,
    .metadata_updated = unicast_stream_metadata_updated_cb,
    .disabled         = unicast_stream_disabled_cb,
    .stopped          = unicast_stream_stopped_cb,
    .released         = unicast_stream_released_cb,
    .recv             = unicast_stream_recv_cb,
    .sent             = unicast_stream_sent_cb,
    .disconnected     = unicast_stream_disconnected_cb,
};

static int discover_cas(void)
{
    int err;

    err = esp_ble_audio_cap_initiator_unicast_discover(peer.conn_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to discover CAS, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Discovering CAS");

    return 0;
}

static int discover_sinks(void)
{
    int err;

    esp_ble_audio_cap_stream_ops_register(&peer.sink_stream, &unicast_stream_ops);

    err = esp_ble_audio_bap_unicast_client_discover(peer.conn_handle, ESP_BLE_AUDIO_DIR_SINK);
    if (err) {
        ESP_LOGE(TAG, "Failed to discover sinks, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Discovering sinks");

    return 0;
}

static int discover_sources(void)
{
    int err;

    esp_ble_audio_cap_stream_ops_register(&peer.source_stream, &unicast_stream_ops);

    err = esp_ble_audio_bap_unicast_client_discover(peer.conn_handle, ESP_BLE_AUDIO_DIR_SOURCE);
    if (err) {
        ESP_LOGE(TAG, "Failed to discover sources, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Discovering source ASEs");

    return 0;
}

static int unicast_group_create(void)
{
    esp_ble_audio_cap_unicast_group_stream_param_t source_stream_param = {
        .qos_cfg = &unicast_preset_16_2_1.qos,
        .stream  = &peer.source_stream,
    };
    esp_ble_audio_cap_unicast_group_stream_param_t sink_stream_param = {
        .qos_cfg = &unicast_preset_16_2_1.qos,
        .stream  = &peer.sink_stream,
    };
    esp_ble_audio_cap_unicast_group_stream_pair_param_t pair_params = {0};
    esp_ble_audio_cap_unicast_group_param_t group_param = {0};
    int err;

    if (peer.source_ep) {
        pair_params.rx_param = &source_stream_param;
    }

    if (peer.sink_ep) {
        pair_params.tx_param = &sink_stream_param;
    }

    group_param.params_count = 1;
    group_param.params = &pair_params;

    err = esp_ble_audio_cap_unicast_group_create(&group_param, &unicast_group);
    if (err) {
        ESP_LOGE(TAG, "Failed to create unicast group, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Created unicast group");

    return 0;
}

int unicast_group_delete(void)
{
    int err;

    if (unicast_group == NULL) {
        return 0;
    }

    err = esp_ble_audio_cap_unicast_group_delete(unicast_group);
    if (err) {
        ESP_LOGE(TAG, "Failed to delete unicast group, err %d", err);
        return err;
    }

    unicast_group = NULL;

    ESP_LOGI(TAG, "Deleted unicast group");

    return 0;
}

static int unicast_audio_start(void)
{
    esp_ble_audio_cap_unicast_audio_start_stream_param_t stream_param[2] = {0};
    esp_ble_audio_cap_unicast_audio_start_param_t param = {0};
    int err;

    if (peer.sink_ep) {
        stream_param[param.count].member.member = peer.conn;
        stream_param[param.count].stream = &peer.sink_stream;
        stream_param[param.count].ep = peer.sink_ep;
        stream_param[param.count].codec_cfg = &unicast_preset_16_2_1.codec_cfg;
        param.count++;
    }

    if (peer.source_ep) {
        stream_param[param.count].member.member = peer.conn;
        stream_param[param.count].stream = &peer.source_stream;
        stream_param[param.count].ep = peer.source_ep;
        stream_param[param.count].codec_cfg = &unicast_preset_16_2_1.codec_cfg;
        param.count++;
    }

    if (param.count == 0) {
        ESP_LOGW(TAG, "No endpoints available, skip starting unicast audio");
        return 0;
    }

    param.type = ESP_BLE_AUDIO_CAP_SET_TYPE_AD_HOC;
    param.stream_params = stream_param;

    err = esp_ble_audio_cap_initiator_unicast_audio_start(&param);
    if (err) {
        ESP_LOGE(TAG, "Failed to start unicast audio, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Starting unicast streams");

    return 0;
}

static void discover_cb(esp_ble_conn_t *conn, int err, esp_ble_audio_dir_t dir)
{
    if (conn->handle != peer.conn_handle) {
        return;
    }

    peer.conn = conn;

    if (dir == ESP_BLE_AUDIO_DIR_SINK) {
        if (err) {
            ESP_LOGE(TAG, "Discovery sinks failed, err %d", err);
        } else {
            ESP_LOGI(TAG, "Discover sinks complete");

            discover_sources();
        }
    } else if (dir == ESP_BLE_AUDIO_DIR_SOURCE) {
        if (err) {
            ESP_LOGE(TAG, "Discovery sources failed, err %d", err);
        } else {
            ESP_LOGI(TAG, "Discover sources complete");

            err = unicast_group_create();
            if (err) {
                return;
            }

            err = unicast_audio_start();
            if (err) {
                unicast_group_delete();
                return;
            }
        }
    }
}

static void pac_record_cb(esp_ble_conn_t *conn,
                          esp_ble_audio_dir_t dir,
                          const esp_ble_audio_codec_cap_t *codec_cap)
{
    example_print_codec_cap(TAG, codec_cap);
}

static void endpoint_cb(esp_ble_conn_t *conn,
                        esp_ble_audio_dir_t dir,
                        esp_ble_audio_bap_ep_t *ep)
{
    if (dir == ESP_BLE_AUDIO_DIR_SOURCE) {
        if (peer.source_ep == NULL) {
            ESP_LOGI(TAG, "[%s] Endpoint discovered", dir_str(dir));
            peer.source_ep = ep;
        }
    } else if (dir == ESP_BLE_AUDIO_DIR_SINK) {
        if (peer.sink_ep == NULL) {
            ESP_LOGI(TAG, "[%s] Endpoint discovered", dir_str(dir));
            peer.sink_ep = ep;
        }
    }
}

static esp_ble_audio_bap_unicast_client_cb_t unicast_client_cbs = {
    .discover   = discover_cb,
    .pac_record = pac_record_cb,
    .endpoint   = endpoint_cb,
};

static void unicast_discovery_complete_cb(esp_ble_conn_t *conn, int err,
                                          const esp_ble_audio_csip_set_coordinator_set_member_t *member,
                                          const esp_ble_audio_csip_set_coordinator_csis_inst_t *csis_inst)
{
    if (err) {
        ESP_LOGE(TAG, "Unicast discovery completed, err %d", err);
        return;
    }

    if (IS_ENABLED(CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER)) {
        if (csis_inst == NULL) {
            ESP_LOGW(TAG, "Failed to discover CAS CSIS");
            return;
        }

        ESP_LOGI(TAG, "Found CAS with CSIS");

        /* TODO: Do set member discovery */
    } else {
        ESP_LOGI(TAG, "Found CAS");
    }

    (void)discover_sinks();
}

static void unicast_start_complete_cb(int err, esp_ble_conn_t *conn)
{
    if (err) {
        ESP_LOGE(TAG, "Unicast start completed, err %d", err);
    } else {
        ESP_LOGI(TAG, "Unicast start completed");
    }
}

static esp_ble_audio_cap_initiator_cb_t cap_cb = {
    .unicast_discovery_complete = unicast_discovery_complete_cb,
    .unicast_start_complete     = unicast_start_complete_cb,
};

static int conn_create(ble_addr_t *dst)
{
    struct ble_gap_conn_params params = {0};
    uint8_t own_addr_type = 0;
    int err;

    err = ble_hs_id_infer_auto(0, &own_addr_type);
    if (err) {
        ESP_LOGE(TAG, "Failed to determine address type, err %d", err);
        return err;
    }

    err = ble_gap_disc_cancel();
    if (err) {
        ESP_LOGE(TAG, "Failed to stop scanning, err %d", err);
        return err;
    }

    peer.disc_cancelled = true;

    params.scan_itvl = INIT_SCAN_INTERVAL;
    params.scan_window = INIT_SCAN_WINDOW;
    params.itvl_min = CONN_INTERVAL;
    params.itvl_max = CONN_INTERVAL;
    params.latency = CONN_LATENCY;
    params.supervision_timeout = CONN_TIMEOUT;
    params.max_ce_len = CONN_MAX_CE_LEN;
    params.min_ce_len = CONN_MIN_CE_LEN;

    return ble_gap_connect(own_addr_type, dst, CONN_DURATION, &params,
                           example_audio_gap_event_cb, NULL);
}

static int pairing_start(uint16_t conn_handle)
{
    return ble_gap_security_initiate(conn_handle);
}

static int exchange_mtu(uint16_t conn_handle)
{
    return ble_gattc_exchange_mtu(conn_handle, NULL, NULL);
}

static bool check_and_connect(uint8_t type, const uint8_t *data,
                              uint8_t data_len, void *user_data)
{
    esp_ble_audio_gap_app_event_t *event;
    ble_addr_t dst = {0};
    uint16_t uuid_val;
    int err;

    event = user_data;
    assert(event);

    if (type != EXAMPLE_AD_TYPE_SERVICE_DATA16) {
        return true;    /* Continue parsing to next AD data type */
    }

    if (data_len < sizeof(uuid_val)) {
        ESP_LOGW(TAG, "Invalid ad size %u (cas uuid)", data_len);
        return true;    /* Continue parsing to next AD data type */
    }

    uuid_val = sys_get_le16(data);

    if (uuid_val != ESP_BLE_AUDIO_UUID_CAS_VAL) {
        /* We are looking for the TMAS service data */
        return true;    /* Continue parsing to next AD data type */
    }

    ESP_LOGI(TAG, "Found CAS in peer adv data!");

    dst.type = event->ext_scan_recv.addr.type;
    memcpy(dst.val, event->ext_scan_recv.addr.val, sizeof(dst.val));

    err = conn_create(&dst);
    if (err) {
        ESP_LOGE(TAG, "Failed to create conn, err %d", err);

        if (peer.disc_cancelled) {
            peer.disc_cancelled = false;
            cap_initiator_unicast_start();
        }
    }

    return false;   /* Stop parsing */
}

static void ext_scan_recv(esp_ble_audio_gap_app_event_t *event)
{
    if (peer.conn_handle != CONN_HANDLE_INIT) {
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
             event->acl_connect.dst.val[5], event->acl_connect.dst.val[4],
             event->acl_connect.dst.val[3], event->acl_connect.dst.val[2],
             event->acl_connect.dst.val[1], event->acl_connect.dst.val[0]);

    peer.conn_handle = event->acl_connect.conn_handle;

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

    peer.conn_handle = CONN_HANDLE_INIT;
    peer.conn = NULL;
    peer.source_ep = NULL;
    peer.sink_ep = NULL;
    peer.disc_completed = false;
    peer.disc_cancelled = false;
    peer.mtu_exchanged = false;

    unicast_group_delete();

    cap_initiator_unicast_start();
}

static void security_change(esp_ble_iso_gap_app_event_t *event)
{
    int err;

    if (event->security_change.status) {
        example_audio_security_failed_recover(TAG, event->security_change.conn_handle,
                                              event->security_change.status);
        return;
    }

    ESP_LOGI(TAG, "Security: handle %u level %u bonded %u",
             event->security_change.conn_handle, event->security_change.sec_level,
             event->security_change.bonded);

    err = exchange_mtu(event->security_change.conn_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to exchange MTU, err %d", err);
        return;
    }
}

void cap_initiator_unicast_gap_cb(esp_ble_audio_gap_app_event_t *event)
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
    uint16_t conn_handle = event->gatt_mtu_change.conn_handle;
    int err;

    ESP_LOGI(TAG, "MTU updated: handle %u mtu %u",
             conn_handle, event->gatt_mtu_change.mtu);

    if (event->gatt_mtu_change.mtu < ESP_BLE_AUDIO_ATT_MTU_MIN) {
        ESP_LOGW(TAG, "Invalid new mtu %u, shall be at least %u",
                 event->gatt_mtu_change.mtu, ESP_BLE_AUDIO_ATT_MTU_MIN);
        return;
    }

    err = esp_ble_audio_gattc_disc_start(conn_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to start svc disc, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Service discovery started: handle %u", conn_handle);

    /* Note:
     * MTU exchanged event may arrived after discover completed event.
     */
    peer.mtu_exchanged = true;

    if (peer.disc_completed) {
        (void)discover_cas();
    }
}

static void gattc_disc_cmpl(esp_ble_audio_gatt_app_event_t *event)
{
    ESP_LOGI(TAG, "Service discovery complete: handle %u",
             event->gattc_disc_cmpl.conn_handle);

    if (event->gattc_disc_cmpl.status) {
        ESP_LOGE(TAG, "gattc disc failed, status %u", event->gattc_disc_cmpl.status);
        return;
    }

    /* Note:
     * Discover completed event may arrived before MTU exchanged event.
     */
    peer.disc_completed = true;

    if (peer.mtu_exchanged) {
        (void)discover_cas();
    }
}

void cap_initiator_unicast_gatt_cb(esp_ble_audio_gatt_app_event_t *event)
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

int cap_initiator_unicast_start(void)
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
                       example_audio_gap_event_cb, NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to start scanning, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Scanning for CAP Acceptor...");
    return 0;
}

int cap_initiator_unicast_init(void)
{
    int err;

    err = esp_ble_audio_cap_initiator_register_cb(&cap_cb);
    if (err) {
        ESP_LOGE(TAG, "Failed to register CAP callbacks, err %d", err);
        return err;
    }

    err = esp_ble_audio_bap_unicast_client_register_cb(&unicast_client_cbs);
    if (err) {
        ESP_LOGE(TAG, "Failed to register BAP unicast client callbacks, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "CAP initiator unicast initialized");

    return 0;
}
