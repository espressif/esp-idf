/*
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "cap_handover.h"

/* One preset per sink stream: each owns its codec cfg buffer, which is what
 * lets the streams carry different channel allocations.
 */
HANDOVER_LC3_PRESET_DEFINE(unicast_preset_left, ESP_BLE_AUDIO_LOCATION_FRONT_LEFT);
HANDOVER_LC3_PRESET_DEFINE(unicast_preset_right, ESP_BLE_AUDIO_LOCATION_FRONT_RIGHT);
static esp_ble_audio_bap_lc3_preset_t *const sink_presets[] = {
    &unicast_preset_left,
    &unicast_preset_right,
};

_Static_assert(ARRAY_SIZE(sink_presets) >= SINK_STREAM_COUNT,
               "Need one preset (one channel allocation) per sink stream");

static esp_ble_audio_cap_unicast_group_t *unicast_group;

struct peer_config peer = {
    .conn_handle = CONN_HANDLE_INIT,
};

esp_ble_audio_cap_unicast_group_t *unicast_group_get(void)
{
    return unicast_group;
}

void unicast_group_set(esp_ble_audio_cap_unicast_group_t *group)
{
    unicast_group = group;
}

esp_ble_audio_bap_lc3_preset_t *sink_preset_get(size_t idx)
{
    return (idx < ARRAY_SIZE(sink_presets)) ? sink_presets[idx] : NULL;
}

static const char *dir_str(esp_ble_audio_dir_t dir)
{
    return dir == ESP_BLE_AUDIO_DIR_SINK ? "SNK" : "SRC";
}

static const char *stream_dir_str(const esp_ble_audio_bap_stream_t *stream)
{
    for (size_t i = 0; i < ARRAY_SIZE(peer.sink_streams); i++) {
        if (stream == &peer.sink_streams[i].bap_stream) {
            return "SNK";
        }
    }

    return "???";
}

static int stream_index(const esp_ble_audio_bap_stream_t *stream)
{
    /* Index within its own direction's pool, so logs read "SNK #0" / "SNK #1". */
    for (size_t i = 0; i < ARRAY_SIZE(peer.sink_streams); i++) {
        if (stream == &peer.sink_streams[i].bap_stream) {
            return (int)i;
        }
    }

    return -1;
}

/* Ask whether the endpoint transmits; do not derive it from dir. These streams
 * migrate, and dir means opposite things on the two endpoint kinds: the peer's
 * direction on a unicast ASE (SINK), our own on a broadcast source (SOURCE).
 * Testing dir == SINK therefore goes silent the moment a stream moves to a BIS.
 */
static bool is_tx_stream(esp_ble_audio_bap_stream_t *stream)
{
    esp_ble_audio_bap_ep_info_t ep_info = {0};
    esp_err_t err;

    /* Detached already: only teardown sees this, and unregistering is a no-op. */
    if (stream->ep == NULL) {
        return true;
    }

    err = esp_ble_audio_bap_ep_get_info(stream->ep, &ep_info);
    if (err) {
        ESP_LOGE(TAG, "Failed to get ep info, err %d", err);
        return false;
    }

    return ep_info.can_send;
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

    if (is_tx_stream(stream)) {
        cap_stream = CONTAINER_OF(stream, esp_ble_audio_cap_stream_t, bap_stream);

        err = cap_handover_tx_register_stream(cap_stream, false);
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

        (void)cap_handover_tx_unregister_stream(cap_stream);
    }
}

static void unicast_stream_disconnected_cb(esp_ble_audio_bap_stream_t *stream, uint8_t reason)
{
    esp_ble_audio_cap_stream_t *cap_stream;

    ESP_LOGI(TAG, "[%s #%d] ISO disconnected, reason 0x%02x",
             stream_dir_str(stream), stream_index(stream), reason);

    if (is_tx_stream(stream)) {
        cap_stream = CONTAINER_OF(stream, esp_ble_audio_cap_stream_t, bap_stream);

        (void)cap_handover_tx_unregister_stream(cap_stream);
    }
}

static void unicast_stream_released_cb(esp_ble_audio_bap_stream_t *stream)
{
    ESP_LOGI(TAG, "[%s #%d] Stream released",
             stream_dir_str(stream), stream_index(stream));
}

static void unicast_stream_sent_cb(esp_ble_audio_bap_stream_t *stream, void *user_data)
{
    cap_handover_tx_stream_sent(stream, user_data);
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

    for (size_t i = 0; i < ARRAY_SIZE(peer.sink_streams); i++) {
        esp_ble_audio_cap_stream_ops_register(&peer.sink_streams[i], &unicast_stream_ops);
    }

    err = esp_ble_audio_bap_unicast_client_discover(peer.conn_handle, ESP_BLE_AUDIO_DIR_SINK);
    if (err) {
        ESP_LOGE(TAG, "Failed to discover sinks, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Discovering sinks");

    return 0;
}

static int unicast_group_create(void)
{
    /* Referenced by the group while it exists, so they outlive this call. */
    static esp_ble_audio_cap_unicast_group_stream_param_t sink_stream_params[SINK_STREAM_COUNT];
    static esp_ble_audio_cap_unicast_group_stream_pair_param_t pair_params[SINK_STREAM_COUNT];
    esp_ble_audio_cap_unicast_group_param_t group_param = {0};
    size_t pair_count = 0;
    int err;

    /* One CIS per sink stream. */
    for (size_t i = 0; i < peer.sink_ep_count; i++) {
        sink_stream_params[i].qos_cfg = &sink_presets[i]->qos;
        sink_stream_params[i].stream = &peer.sink_streams[i];

        pair_params[pair_count].rx_param = NULL;
        pair_params[pair_count].tx_param = &sink_stream_params[i];
        pair_count++;
    }

    if (pair_count == 0) {
        ESP_LOGW(TAG, "No endpoints available, skip creating unicast group");
        return -ENODEV;
    }

    group_param.params_count = pair_count;
    group_param.params = pair_params;

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
    /* codec_cfg has to stay valid while the stream is non-idle. */
    static esp_ble_audio_cap_unicast_audio_start_stream_param_t stream_param[SINK_STREAM_COUNT];
    esp_ble_audio_cap_unicast_audio_start_param_t param = {0};
    int err;

    for (size_t i = 0; i < peer.sink_ep_count; i++) {
        stream_param[param.count].member.member = peer.conn;
        stream_param[param.count].stream = &peer.sink_streams[i];
        stream_param[param.count].ep = peer.sink_eps[i];
        stream_param[param.count].codec_cfg = &sink_presets[i]->codec_cfg;
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

int cap_handover_unicast_setup_and_start(void)
{
    int err;

    err = unicast_group_create();
    if (err) {
        return err;
    }

    err = unicast_audio_start();
    if (err) {
        unicast_group_delete();
        return err;
    }

    return 0;
}

static void discover_cb(esp_ble_conn_t *conn, int err, esp_ble_audio_dir_t dir)
{
    if (conn->handle != peer.conn_handle) {
        return;
    }

    peer.conn = conn;

    /* Sink only: a broadcast Audio Stream has no return path, so the Acceptor's
     * source ASEs take no part in a handover.
     */
    if (dir != ESP_BLE_AUDIO_DIR_SINK) {
        return;
    }

    if (err) {
        ESP_LOGE(TAG, "Discovery sinks failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Discover sinks complete");

    /* The collocated Commander writes the broadcast source into the Acceptor's
     * BASS, so discover that too. Its callback continues the unicast setup.
     */
    (void)cap_handover_proc_discover(peer.conn_handle);
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
    /* Keep every sink endpoint: a stereo Acceptor exposes one per channel and
     * they are handed over together.
     */
    if (dir == ESP_BLE_AUDIO_DIR_SINK &&
            peer.sink_ep_count < ARRAY_SIZE(peer.sink_eps)) {
        ESP_LOGI(TAG, "[%s #%zu] Endpoint discovered", dir_str(dir), peer.sink_ep_count);
        peer.sink_eps[peer.sink_ep_count++] = ep;
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
        return;
    }

    ESP_LOGI(TAG, "Unicast start completed");

    cap_handover_proc_unicast_started();
}

static void broadcast_stopped_cb(esp_ble_audio_cap_broadcast_source_t *source, uint8_t reason)
{
    ESP_LOGI(TAG, "Broadcast source stopped, reason 0x%02x", reason);

    cap_handover_proc_broadcast_stopped(source);
}

static esp_ble_audio_cap_initiator_cb_t cap_cb = {
    .unicast_discovery_complete = unicast_discovery_complete_cb,
    .unicast_start_complete     = unicast_start_complete_cb,
    .broadcast_stopped          = broadcast_stopped_cb,
};

static bool check_and_connect(uint8_t type, const uint8_t *data,
                              uint8_t data_len, void *user_data)
{
    esp_ble_audio_gap_app_event_t *event;
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

    /* Stop scanning before connect — NimBLE rejects ble_gap_connect while
     * a discovery procedure is running. On failure restart scanning so we
     * don't stall in no-scan no-conn state. */
    err = ext_scan_stop();
    if (err) {
        ESP_LOGE(TAG, "Failed to stop scanning, err %d", err);
        return false;
    }

    err = conn_create(event->ext_scan_recv.addr.type,
                      event->ext_scan_recv.addr.val);
    if (err) {
        ESP_LOGE(TAG, "Failed to create conn, err %d", err);
        cap_handover_unicast_start();
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
        /* Scanning was stopped before conn_create and acl_disconnect only fires on
         * an established connection, so resume here or nothing runs. */
        cap_handover_unicast_start();
        return;
    }

    ESP_LOGI(TAG, "Connected: handle %u role %u peer %02x:%02x:%02x:%02x:%02x:%02x",
             event->acl_connect.conn_handle, event->acl_connect.role,
             EXAMPLE_BT_ADDR_PRINT_ARGS(event->acl_connect.dst.val));

    peer.conn_handle = event->acl_connect.conn_handle;
    memcpy(peer.dst, event->acl_connect.dst.val, sizeof(peer.dst));

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
    memset(peer.dst, 0, sizeof(peer.dst));
    memset(peer.sink_eps, 0, sizeof(peer.sink_eps));
    peer.sink_ep_count = 0;
    peer.disc_completed = false;
    peer.mtu_exchanged = false;

    /* Drop a broadcast source left running before reusing the stream objects. */
    cap_handover_proc_reset();

    unicast_group_delete();

    cap_handover_unicast_start();
}

static void security_change(esp_ble_audio_gap_app_event_t *event)
{
    int err;

    if (event->security_change.status) {
        security_failed_recover(event->security_change.conn_handle,
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

void cap_handover_unicast_gap_cb(esp_ble_audio_gap_app_event_t *event)
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

void cap_handover_unicast_gatt_cb(esp_ble_audio_gatt_app_event_t *event)
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

int cap_handover_unicast_start(void)
{
    return ext_scan_start();
}

int cap_handover_unicast_init(void)
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
