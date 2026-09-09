/*
 * SPDX-FileCopyrightText: 2023 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "sdkconfig.h"

#if CONFIG_BT_BLUEDROID_ENABLED
#include "esp_bt_defs.h"
#else
#include "nimble/ble.h"
#endif

#include "esp_ble_audio_bap_api.h"

#include "ble_audio_example_init.h"
#include "ble_audio_example_utils.h"

#include "assistant.h"

/* Peers are matched by name substring; an empty string matches any device. */
#define TARGET_SINK_NAME        "BAP Broadcast Sink"
#define TARGET_SOURCE_NAME      "BAP Broadcast Source"

#define TARGET_BROADCAST_CODE   "1234"

#define CONN_HANDLE_INIT        UINT16_MAX
#define PA_SYNC_HANDLE_INIT     UINT16_MAX

/* One extended scanner, used first to find a Scan Delegator and then
 * the Broadcast Source to point it at. */
enum scan_mode {
    SCAN_MODE_IDLE,
    SCAN_MODE_SINK,
    SCAN_MODE_SOURCE,
};

static enum scan_mode scan_mode;

static uint16_t conn_handle = CONN_HANDLE_INIT;
static bool mtu_exchanged;
static bool disc_completed;
static uint8_t recv_state_count;
static uint8_t recv_state_read_idx;
static bool recv_states_synced;

/* Selected Broadcast Source. `src_addr` is in host byte order for
 * pa_sync_create(); addr_host_to_le() converts it for BASS Add Source. */
static uint8_t src_addr_type;
static uint8_t src_addr[6];
static uint8_t src_sid;
static uint32_t src_broadcast_id;
static uint16_t src_pa_interval;

static uint16_t sync_handle = PA_SYNC_HANDLE_INIT;
static bool pa_syncing;
static bool base_received;

/* The sink republishes its receive state on every change, so a bad
 * code would otherwise have us resend forever. */
#define BROADCAST_CODE_MAX_ATTEMPTS 2

static uint8_t code_attempts;

/* Which source the delegator carries, -1 for none. Needed because the Add-vs-
 * Modify decision is made when a BASE arrives, with no receive state in hand;
 * BAP 6.5.4 forbids adding a source it already has. */
static int16_t known_src_id = -1;
static uint32_t known_broadcast_id;

/* A SyncInfo the delegator asked for while we held no sync, -1 for none. The
 * only request that has to outlive the hunt for the source. */
static int16_t past_pending_src_id = -1;

static esp_ble_audio_bap_bass_subgroup_t subgroups[CONFIG_BT_BAP_BASS_MAX_SUBGROUPS];
static uint8_t subgroup_meta[CONFIG_BT_BAP_BASS_MAX_SUBGROUPS]
                            [CONFIG_BT_AUDIO_CODEC_CFG_MAX_METADATA_SIZE];
/* Add Source carries no codec cfg LTVs, but the decoder writes them anyway. */
static uint8_t codec_cfg_data[CONFIG_BT_AUDIO_CODEC_CFG_MAX_DATA_SIZE];
static uint8_t subgroup_count;

static bool name_matches(const char *want, const uint8_t *data, uint8_t data_len)
{
    char name[64];

    if (want[0] == '\0') {
        return true;
    }

    if (data_len >= sizeof(name)) {
        data_len = sizeof(name) - 1;
    }

    memcpy(name, data, data_len);
    name[data_len] = '\0';

    return example_is_substring(want, name);
}

struct sink_adv_data {
    bool name_matched;
    bool has_bass;
};

static bool sink_data_cb(uint8_t type, const uint8_t *data,
                         uint8_t data_len, void *user_data)
{
    struct sink_adv_data *sink = user_data;

    switch (type) {
    case EXAMPLE_AD_TYPE_NAME_SHORTENED:
    case EXAMPLE_AD_TYPE_NAME_COMPLETE:
        sink->name_matched = name_matches(TARGET_SINK_NAME, data, data_len);
        return sink->name_matched;
    case EXAMPLE_AD_TYPE_UUID16_SOME:
    case EXAMPLE_AD_TYPE_UUID16_ALL:
        /* BASS shows up in the UUID list, not as service data, and it alone
         * identifies a Scan Delegator — PACS is not required. */
        for (uint8_t i = 0; i + ESP_BLE_AUDIO_UUID_SIZE_16 <= data_len;
                i += ESP_BLE_AUDIO_UUID_SIZE_16) {
            if (sys_get_le16(data + i) == ESP_BLE_AUDIO_UUID_BASS_VAL) {
                sink->has_bass = true;
                break;
            }
        }
        return true;
    default:
        return true;
    }
}

struct source_adv_data {
    bool name_matched;
    bool broadcast_id_found;
    uint32_t broadcast_id;
};

static bool source_data_cb(uint8_t type, const uint8_t *data,
                           uint8_t data_len, void *user_data)
{
    struct source_adv_data *src = user_data;

    switch (type) {
    case EXAMPLE_AD_TYPE_NAME_SHORTENED:
    case EXAMPLE_AD_TYPE_NAME_COMPLETE:
    case EXAMPLE_AD_TYPE_BROADCAST_NAME:
        src->name_matched = name_matches(TARGET_SOURCE_NAME, data, data_len);
        return src->name_matched;
    case EXAMPLE_AD_TYPE_SERVICE_DATA16:
        if (data_len < ESP_BLE_AUDIO_UUID_SIZE_16 + ESP_BLE_AUDIO_BROADCAST_ID_SIZE) {
            return true;
        }
        if (sys_get_le16(data) != ESP_BLE_AUDIO_UUID_BROADCAST_AUDIO_VAL) {
            return true;
        }
        src->broadcast_id = sys_get_le24(data + ESP_BLE_AUDIO_UUID_SIZE_16);
        src->broadcast_id_found = true;
        return true;
    default:
        return true;
    }
}

static bool base_subgroup_cb(const esp_ble_audio_bap_base_subgroup_t *subgroup,
                             void *user_data)
{
    esp_ble_audio_codec_cfg_t codec_cfg = {0};
    uint32_t bis_indexes = 0;
    esp_err_t err;

    (void)user_data;

    if (subgroup_count >= ARRAY_SIZE(subgroups)) {
        ESP_LOGW(TAG, "More than %u subgroup(s), ignoring the rest",
                 (unsigned)ARRAY_SIZE(subgroups));
        return false;
    }

    err = esp_ble_audio_bap_base_subgroup_get_bis_indexes(subgroup, &bis_indexes);
    if (err) {
        ESP_LOGE(TAG, "Failed to get subgroup BIS indexes, err %d", err);
        return false;
    }

    /* Caller-owned buffers, memcpy'd into unconditionally: a zeroed codec_cfg
     * faults, and anything under the Kconfig maximum overflows — that is what
     * the decoder bounds-checks against, not the real size. */
    codec_cfg.data = codec_cfg_data;
    codec_cfg.meta = subgroup_meta[subgroup_count];

    err = esp_ble_audio_bap_base_subgroup_codec_to_codec_cfg(subgroup, &codec_cfg);
    if (err) {
        ESP_LOGE(TAG, "Failed to convert subgroup codec cfg, err %d", err);
        return false;
    }

    subgroups[subgroup_count].bis_sync = bis_indexes;
    subgroups[subgroup_count].metadata = codec_cfg.meta_len ? codec_cfg.meta : NULL;
    subgroups[subgroup_count].metadata_len = (uint8_t)codec_cfg.meta_len;

    ESP_LOGI(TAG, "Subgroup %u: bis_sync 0x%08lx meta_len %u",
             subgroup_count, (unsigned long)bis_indexes,
             subgroups[subgroup_count].metadata_len);

    subgroup_count++;

    return true;
}

/* GAP events hand out addresses in the active host's own byte order (Bluedroid
 * MSB-first, NimBLE on-air/LSB-first), while the BASS Add Source PDU always
 * carries them on-air. Host APIs such as pa_sync_create() want the event bytes
 * unchanged; only the BASS parameter needs this conversion. */
static void addr_host_to_le(uint8_t dst[6], const uint8_t src[6])
{
#if CONFIG_BT_BLUEDROID_ENABLED
    for (size_t i = 0; i < 6; i++) {
        dst[i] = src[5 - i];
    }
#else
    memcpy(dst, src, 6);
#endif
}

/* The address type needs the same treatment. BASS 3.1.1.4 defines only two
 * values for Advertiser_Address_Type, each covering its identity form as well:
 * 0x00 public (device or identity), 0x01 random (device or static identity). */
static uint8_t addr_type_host_to_le(uint8_t type)
{
#if CONFIG_BT_BLUEDROID_ENABLED
    return (type == BLE_ADDR_TYPE_PUBLIC ||
            type == BLE_ADDR_TYPE_RPA_PUBLIC) ? BT_ADDR_LE_PUBLIC : BT_ADDR_LE_RANDOM;
#else
    return (type == BLE_ADDR_PUBLIC ||
            type == BLE_ADDR_PUBLIC_ID) ? BT_ADDR_LE_PUBLIC : BT_ADDR_LE_RANDOM;
#endif
}

static int add_source(void)
{
    esp_ble_audio_bap_broadcast_assistant_add_src_param_t param = {0};
    esp_err_t err;

    param.addr.type = addr_type_host_to_le(src_addr_type);
    addr_host_to_le(param.addr.a.val, src_addr);
    param.adv_sid = src_sid;
    param.pa_sync = true;
    param.broadcast_id = src_broadcast_id;
    /* The measured interval: delegators that check it reject
     * ESP_BLE_AUDIO_BAP_PA_INTERVAL_UNKNOWN. */
    param.pa_interval = src_pa_interval;
    param.num_subgroups = subgroup_count;
    param.subgroups = subgroups;

    err = esp_ble_audio_bap_broadcast_assistant_add_src(conn_handle, &param);
    if (err) {
        ESP_LOGE(TAG, "Failed to add source, err %d", err);
        return -EIO;
    }

    ESP_LOGI(TAG, "Add Source sent: id 0x%06lx sid %u pa_interval %u subgroups %u",
             (unsigned long)src_broadcast_id, src_sid, src_pa_interval,
             subgroup_count);

    return 0;
}

/* Re-drive a state the delegator still carries but is not receiving, with the
 * BIS indexes from the BASE we just decoded. */
static int modify_source(uint8_t src_id)
{
    esp_ble_audio_bap_broadcast_assistant_mod_src_param_t param = {0};
    esp_err_t err;

    param.src_id = src_id;
    param.pa_sync = true;
    param.pa_interval = src_pa_interval;
    param.num_subgroups = subgroup_count;
    param.subgroups = subgroups;

    err = esp_ble_audio_bap_broadcast_assistant_mod_src(conn_handle, &param);
    if (err) {
        ESP_LOGE(TAG, "Failed to modify source, err %d", err);
        return -EIO;
    }

    ESP_LOGI(TAG, "Modify Source sent: src_id %u pa_interval %u subgroups %u",
             src_id, src_pa_interval, subgroup_count);

    return 0;
}

static void scan_restart(enum scan_mode mode)
{
    scan_mode = mode;

    ESP_LOGI(TAG, "Scanning for broadcast %s...",
             mode == SCAN_MODE_SINK ? "sink" : "source");

    ext_scan_start();
}

/* Get the delegator receiving. With a local sync and a decoded BASE in hand the
 * source can be written straight away; otherwise go find it first. */
static void drive_delegator(void)
{
    esp_err_t err;

    if (base_received && subgroup_count > 0) {
        if (known_src_id >= 0 && known_broadcast_id == src_broadcast_id) {
            modify_source((uint8_t)known_src_id);
        } else {
            add_source();
        }
        return;
    }

    /* An operation is already under way: a scan report, or the PA sync event
     * that brings the BASE, carries on from here. */
    if (scan_mode != SCAN_MODE_IDLE || pa_syncing ||
            sync_handle != PA_SYNC_HANDLE_INIT) {
        return;
    }

    /* Tell the delegator we are scanning on its behalf. `start_scan` stays
     * false: we parse the reports ourselves. */
    err = esp_ble_audio_bap_broadcast_assistant_scan_start(conn_handle, false);
    if (err) {
        ESP_LOGE(TAG, "Failed to send Remote Scan Started, err %d", err);
    }

    scan_restart(SCAN_MODE_SOURCE);
}

static bool base_store_cb(uint8_t type, const uint8_t *data,
                          uint8_t data_len, void *user_data)
{
    const esp_ble_audio_bap_base_t *base;
    uint8_t base_subgroup_count = 0;
    esp_err_t err;

    (void)user_data;

    base = esp_ble_audio_bap_base_get_base_from_ad(type, data, data_len);
    if (base == NULL) {
        /* Not the BASE — keep walking the rest of the periodic advertising. */
        return true;
    }

    err = esp_ble_audio_bap_base_get_subgroup_count(base, &base_subgroup_count);
    if (err) {
        ESP_LOGE(TAG, "Failed to get subgroup count, err %d", err);
        return false;
    }

    ESP_LOGI(TAG, "BASE received (%u subgroup(s))", base_subgroup_count);

    subgroup_count = 0;
    memset(subgroups, 0, sizeof(subgroups));

    err = esp_ble_audio_bap_base_foreach_subgroup(base, base_subgroup_cb, NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to walk BASE subgroups, err %d", err);
        return false;
    }

    if (subgroup_count == 0) {
        ESP_LOGW(TAG, "BASE carries no usable subgroup");
        return false;
    }

    base_received = true;

    drive_delegator();

    return false;
}

static void read_next_recv_state(void)
{
    esp_err_t err;

    while (recv_state_read_idx < recv_state_count) {
        err = esp_ble_audio_bap_broadcast_assistant_read_recv_state(
                  conn_handle, recv_state_read_idx);
        recv_state_read_idx++;
        if (err == ESP_OK) {
            return;
        }

        ESP_LOGE(TAG, "Failed to read receive state %u, err %d",
                 recv_state_read_idx - 1, err);
    }

    recv_states_synced = true;

    /* Every slot was empty, so it has no source at all. Anything else was
     * dispatched from delegator_state_changed() as the states came in. */
    if (known_src_id < 0) {
        drive_delegator();
    }
}

static void assistant_discover_cb(esp_ble_conn_t *conn, int err,
                                  uint8_t state_count)
{
    (void)conn;

    if (err) {
        ESP_LOGE(TAG, "BASS discovery failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "BASS discovered, %u receive state(s)", state_count);

    recv_state_count = state_count;
    recv_state_read_idx = 0;
    recv_states_synced = false;
    known_src_id = -1;
    past_pending_src_id = -1;

    read_next_recv_state();
}

static void send_broadcast_code(uint8_t src_id)
{
    uint8_t code[ESP_BLE_ISO_BROADCAST_CODE_SIZE] = {0};
    esp_err_t err;

    /* Left-aligned and zero-padded, as the sink expects it over BASS. This is
     * not the HCI Create BIG path, which takes the code byte-reversed. */
    memcpy(code, TARGET_BROADCAST_CODE,
           MIN(sizeof(code), strlen(TARGET_BROADCAST_CODE)));

    err = esp_ble_audio_bap_broadcast_assistant_set_broadcast_code(conn_handle,
                                                                   src_id, code);
    if (err) {
        ESP_LOGE(TAG, "Failed to send broadcast code, err %d", err);
        return;
    }

    code_attempts++;

    ESP_LOGI(TAG, "Broadcast code sent for src_id %u", src_id);
}

#if CONFIG_EXAMPLE_PAST
/* Hands over the sync already used to read the BASE, so the sink skips a scan
 * and a sync of its own. */
static void send_past(uint8_t src_id)
{
    int err;

    past_pending_src_id = -1;

    err = pa_past_transfer(conn_handle, sync_handle, src_id);
    if (err) {
        ESP_LOGE(TAG, "Failed to transfer PA sync, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "SyncInfo transferred for src_id %u", src_id);
}
#endif /* CONFIG_EXAMPLE_PAST */

#if CONFIG_EXAMPLE_PAST
static void request_past(uint8_t src_id)
{
    if (sync_handle != PA_SYNC_HANDLE_INIT) {
        send_past(src_id);
        return;
    }

    /* Ours lapsed with the last broadcast — get it back, then hand it over. */
    ESP_LOGI(TAG, "Sink wants SyncInfo, re-syncing to the source");
    past_pending_src_id = src_id;
    drive_delegator();
}
#endif /* CONFIG_EXAMPLE_PAST */

/* The receive state says everything about what the delegator needs, so the same
 * dispatch serves the read-back and every later notification. */
static void delegator_state_changed(const esp_ble_audio_bap_scan_delegator_recv_state_t *state)
{
    switch (state->pa_sync_state) {
    case ESP_BLE_AUDIO_BAP_PA_STATE_INFO_REQ:
#if CONFIG_EXAMPLE_PAST
        request_past(state->src_id);
#else
        ESP_LOGW(TAG, "Sink wants SyncInfo but PAST is not built in");
#endif /* CONFIG_EXAMPLE_PAST */
        return;

    case ESP_BLE_AUDIO_BAP_PA_STATE_NOT_SYNCED:
    case ESP_BLE_AUDIO_BAP_PA_STATE_FAILED:
    case ESP_BLE_AUDIO_BAP_PA_STATE_NO_PAST:
        /* Not on the source — whether it never got there or the broadcast
         * stopped and its sync lapsed, the answer is the same. */
        drive_delegator();
        return;

    default:
        break;
    }

    /* Synced. Only the broadcast code is ours to supply; whether a BIS is up
     * yet is between the delegator and the air. */
    if (state->encrypt_state == ESP_BLE_AUDIO_BAP_BIG_ENC_STATE_BAD_CODE) {
        ESP_LOGW(TAG, "Sink rejected the broadcast code");
    }

    if (state->encrypt_state == ESP_BLE_AUDIO_BAP_BIG_ENC_STATE_BCODE_REQ ||
            state->encrypt_state == ESP_BLE_AUDIO_BAP_BIG_ENC_STATE_BAD_CODE) {
        if (code_attempts < BROADCAST_CODE_MAX_ATTEMPTS) {
            send_broadcast_code(state->src_id);
        }
    }
}

static void assistant_recv_state_cb(esp_ble_conn_t *conn, int err,
                                    const esp_ble_audio_bap_scan_delegator_recv_state_t *state)
{
    (void)conn;

    if (err) {
        ESP_LOGE(TAG, "Receive state read failed, err %d", err);
    } else if (state == NULL) {
        /* An empty slot, not an error. */
        ESP_LOGI(TAG, "Receive state empty");
    } else {
        ESP_LOGI(TAG, "Receive state: src_id %u id 0x%06lx pa 0x%02x enc 0x%02x",
                 state->src_id, (unsigned long)state->broadcast_id,
                 state->pa_sync_state, state->encrypt_state);

        for (uint8_t i = 0; i < state->num_subgroups && state->subgroups; i++) {
            ESP_LOGI(TAG, "  subgroup %u bis_sync 0x%08lx",
                     i, (unsigned long)state->subgroups[i].bis_sync);
        }

        known_src_id = state->src_id;
        known_broadcast_id = state->broadcast_id;

        delegator_state_changed(state);
    }

    /* Chain the initial read-back; its tail covers a delegator with no source
     * at all, which produces no state to dispatch on. */
    if (!recv_states_synced) {
        read_next_recv_state();
    }
}

static void assistant_recv_state_removed_cb(esp_ble_conn_t *conn, uint8_t src_id)
{
    (void)conn;

    ESP_LOGI(TAG, "Receive state removed: src_id %u", src_id);

    /* The delegator no longer carries it, so it is addable again. */
    known_src_id = -1;
    code_attempts = 0;
}

static void assistant_scan_start_cb(esp_ble_conn_t *conn, int err)
{
    (void)conn;

    ESP_LOGI(TAG, "Remote Scan Started: %s", err ? "failed" : "ok");
}

static void assistant_scan_stop_cb(esp_ble_conn_t *conn, int err)
{
    (void)conn;

    ESP_LOGI(TAG, "Remote Scan Stopped: %s", err ? "failed" : "ok");
}

static void assistant_add_src_cb(esp_ble_conn_t *conn, int err)
{
    esp_err_t rc;

    (void)conn;

    if (err) {
        /* The BASE is still good, so a retry would write the same rejected
         * request; wait for the delegator's state to change instead. */
        ESP_LOGE(TAG, "Add Source failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Add Source OK");

    rc = esp_ble_audio_bap_broadcast_assistant_scan_stop(conn_handle);
    if (rc) {
        ESP_LOGE(TAG, "Failed to send Remote Scan Stopped, err %d", rc);
    }
}

static void assistant_mod_src_cb(esp_ble_conn_t *conn, int err)
{
    (void)conn;

    ESP_LOGI(TAG, "Modify Source: %s", err ? "failed" : "ok");
}

static void assistant_broadcast_code_cb(esp_ble_conn_t *conn, int err)
{
    (void)conn;

    ESP_LOGI(TAG, "Set Broadcast Code: %s", err ? "failed" : "ok");
}

static void assistant_rem_src_cb(esp_ble_conn_t *conn, int err)
{
    (void)conn;

    ESP_LOGI(TAG, "Remove Source: %s", err ? "failed" : "ok");
}

static esp_ble_audio_bap_broadcast_assistant_cb_t assistant_cbs = {
    .discover           = assistant_discover_cb,
    .recv_state         = assistant_recv_state_cb,
    .recv_state_removed = assistant_recv_state_removed_cb,
    .scan_start         = assistant_scan_start_cb,
    .scan_stop          = assistant_scan_stop_cb,
    .add_src            = assistant_add_src_cb,
    .mod_src            = assistant_mod_src_cb,
    .broadcast_code     = assistant_broadcast_code_cb,
    .rem_src            = assistant_rem_src_cb,
};

static void reset_source_state(void)
{
    sync_handle = PA_SYNC_HANDLE_INIT;
    pa_syncing = false;
    base_received = false;
    code_attempts = 0;
    subgroup_count = 0;
}

static void ext_scan_recv_sink(esp_ble_audio_gap_app_event_t *event)
{
    struct sink_adv_data sink = {0};
    int err;

    if (conn_handle != CONN_HANDLE_INIT) {
        return;
    }

    if ((event->ext_scan_recv.event_type & EXAMPLE_ADV_PROP_CONNECTABLE) == 0) {
        return;
    }

    esp_ble_audio_data_parse(event->ext_scan_recv.data,
                             event->ext_scan_recv.data_len,
                             sink_data_cb, &sink);

    if (!sink.has_bass || !sink.name_matched) {
        return;
    }

    ESP_LOGI(TAG, "Broadcast sink found: %02x:%02x:%02x:%02x:%02x:%02x",
             EXAMPLE_BT_ADDR_PRINT_ARGS(event->ext_scan_recv.addr.val));

    /* NimBLE rejects ble_gap_connect while a discovery procedure is running. */
    err = ext_scan_stop();
    if (err) {
        ESP_LOGE(TAG, "Failed to stop scanning, err %d", err);
        return;
    }

    scan_mode = SCAN_MODE_IDLE;

    err = conn_create(event->ext_scan_recv.addr.type,
                      event->ext_scan_recv.addr.val);
    if (err) {
        ESP_LOGE(TAG, "Failed to create conn, err %d", err);
        scan_restart(SCAN_MODE_SINK);
    }
}

static void ext_scan_recv_source(esp_ble_audio_gap_app_event_t *event)
{
    struct source_adv_data src = {0};
    int err;

    /* No periodic advertising means no BASE and nothing to sync to. */
    if (event->ext_scan_recv.per_adv_itvl == 0) {
        return;
    }

    if (pa_syncing || sync_handle != PA_SYNC_HANDLE_INIT) {
        return;
    }

    esp_ble_audio_data_parse(event->ext_scan_recv.data,
                             event->ext_scan_recv.data_len,
                             source_data_cb, &src);

    if (!src.broadcast_id_found || !src.name_matched) {
        return;
    }

    src_addr_type = event->ext_scan_recv.addr.type;
    memcpy(src_addr, event->ext_scan_recv.addr.val, sizeof(src_addr));
    src_sid = event->ext_scan_recv.sid;
    src_broadcast_id = src.broadcast_id;
    src_pa_interval = event->ext_scan_recv.per_adv_itvl;

    ESP_LOGI(TAG, "Broadcast source found: %02x:%02x:%02x:%02x:%02x:%02x "
             "type %u id 0x%06lx sid %u pa_interval %u",
             EXAMPLE_BT_ADDR_PRINT_ARGS(src_addr), src_addr_type,
             (unsigned long)src_broadcast_id, src_sid, src_pa_interval);

    /* Sync locally to read the BASE, so the sink gets exact BIS indexes rather
     * than BIS_SYNC_NO_PREF. pa_sync() stops the scanner once established;
     * until then the pa_syncing guard above suppresses further reports. */
    err = pa_sync_create(src_addr_type, src_addr, src_sid);
    if (err) {
        ESP_LOGE(TAG, "Failed to create PA sync, err %d", err);
        return;
    }

    pa_syncing = true;
}

static void ext_scan_recv(esp_ble_audio_gap_app_event_t *event)
{
    switch (scan_mode) {
    case SCAN_MODE_SINK:
        ext_scan_recv_sink(event);
        break;
    case SCAN_MODE_SOURCE:
        ext_scan_recv_source(event);
        break;
    default:
        break;
    }
}

static void acl_connect(esp_ble_audio_gap_app_event_t *event)
{
    int err;

    if (event->acl_connect.status) {
        ESP_LOGE(TAG, "Connection failed, status %d", event->acl_connect.status);
        /* acl_disconnect only fires for an established link, so resume here. */
        scan_restart(SCAN_MODE_SINK);
        return;
    }

    ESP_LOGI(TAG, "Connected: handle %u peer %02x:%02x:%02x:%02x:%02x:%02x",
             event->acl_connect.conn_handle,
             EXAMPLE_BT_ADDR_PRINT_ARGS(event->acl_connect.dst.val));

    conn_handle = event->acl_connect.conn_handle;

    err = pairing_start(event->acl_connect.conn_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to initiate security, err %d", err);
    }
}

static void acl_disconnect(esp_ble_audio_gap_app_event_t *event)
{
    ESP_LOGI(TAG, "Disconnected: handle %u reason 0x%02x",
             event->acl_disconnect.conn_handle, event->acl_disconnect.reason);

    conn_handle = CONN_HANDLE_INIT;
    mtu_exchanged = false;
    disc_completed = false;
    recv_state_count = 0;
    recv_state_read_idx = 0;
    recv_states_synced = false;

    if (sync_handle != PA_SYNC_HANDLE_INIT) {
        pa_sync_terminate(sync_handle);
    }

    reset_source_state();

    scan_restart(SCAN_MODE_SINK);
}

static void security_change(esp_ble_audio_gap_app_event_t *event)
{
    int err;

    /* A failure the adapter could not attribute to a connection arrives with
     * conn_handle unset, and after a dropped link it arrives late. Recovering
     * from either would throw away a bond that is still good. */
    if (conn_handle == CONN_HANDLE_INIT ||
            event->security_change.conn_handle != conn_handle) {
        return;
    }

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
    }
}

static void pa_sync(esp_ble_audio_gap_app_event_t *event)
{
    int err;

    pa_syncing = false;

    if (event->pa_sync.status) {
        /* Still scanning, so the next report of this source retries. */
        ESP_LOGE(TAG, "PA sync failed, status %d", event->pa_sync.status);
        return;
    }

    sync_handle = event->pa_sync.sync_handle;

    ESP_LOGI(TAG, "PA synced: sync_handle %u, waiting for BASE", sync_handle);

#if CONFIG_EXAMPLE_PAST
    if (past_pending_src_id >= 0) {
        send_past((uint8_t)past_pending_src_id);
    }
#endif /* CONFIG_EXAMPLE_PAST */

    /* The BASE arrives over the PA channel now; pa_sync_lost() re-arms. */
    err = ext_scan_stop();
    if (err) {
        ESP_LOGW(TAG, "Failed to stop scanning, err %d", err);
    }

    scan_mode = SCAN_MODE_IDLE;
}

static void pa_sync_recv(esp_ble_audio_gap_app_event_t *event)
{
    if (base_received || event->pa_sync_recv.sync_handle != sync_handle) {
        return;
    }

    esp_ble_audio_data_parse(event->pa_sync_recv.data,
                             event->pa_sync_recv.data_len,
                             base_store_cb, NULL);
}

static void pa_sync_lost(esp_ble_audio_gap_app_event_t *event)
{
    ESP_LOGI(TAG, "PA sync lost: sync_handle %u reason 0x%02x",
             event->pa_sync_lost.sync_handle, event->pa_sync_lost.reason);

    if (sync_handle != event->pa_sync_lost.sync_handle) {
        return;
    }

    reset_source_state();

    /* Only re-scan while the delegator link is up; otherwise acl_disconnect()
     * has already restarted the sink scan. */
    if (conn_handle != CONN_HANDLE_INIT) {
        scan_restart(SCAN_MODE_SOURCE);
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
    case ESP_BLE_AUDIO_GAP_EVENT_PA_SYNC:
        pa_sync(event);
        break;
    case ESP_BLE_AUDIO_GAP_EVENT_PA_SYNC_RECV:
        pa_sync_recv(event);
        break;
    case ESP_BLE_AUDIO_GAP_EVENT_PA_SYNC_LOST:
        pa_sync_lost(event);
        break;
    default:
        break;
    }
}

static void bass_discover(void)
{
    esp_err_t err;

    err = esp_ble_audio_bap_broadcast_assistant_discover(conn_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to discover BASS, err %d", err);
    }
}

static void gatt_mtu_change(esp_ble_audio_gatt_app_event_t *event)
{
    int err;

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

    /* Note:
     * MTU exchanged event may arrived after discover completed event.
     */
    mtu_exchanged = true;

    if (disc_completed) {
        bass_discover();
    }
}

static void gattc_disc_cmpl(esp_ble_audio_gatt_app_event_t *event)
{
    if (event->gattc_disc_cmpl.status) {
        ESP_LOGE(TAG, "Service discovery failed, status %u",
                 event->gattc_disc_cmpl.status);
        return;
    }

    ESP_LOGI(TAG, "Service discovery complete: handle %u",
             event->gattc_disc_cmpl.conn_handle);

    /* Note:
     * Discover completed event may arrived before MTU exchanged event.
     */
    disc_completed = true;

    if (mtu_exchanged) {
        bass_discover();
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

    err = esp_ble_audio_bap_broadcast_assistant_register_cb(&assistant_cbs);
    if (err) {
        ESP_LOGE(TAG, "Failed to register assistant callbacks, err %d", err);
        return;
    }

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

    scan_restart(SCAN_MODE_SINK);
}
