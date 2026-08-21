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

/* MTU exchange and service discovery complete independently, so both are tracked.
 * set_discovered is per member rather than a count, so a member that keeps its
 * link while another one is replaced does not have to be discovered again. */
struct set_member {
    uint16_t handle;
    uint8_t  dst[6];
    bool     mtu_exchanged;
    bool     disc_completed;
    bool     set_discovered;
};

static struct set_member members[TMAP_CEN_PEER_COUNT];
static size_t member_count;

/* One connect at a time. members[] only learns a peer at acl_connect, and
 * ext_scan_stop() does not discard reports already queued for us, so without
 * this a second report starts an overlapping conn_create that the stack
 * rejects ("L2CAP - LE - cannot start new connection at conn st: 1"). */
static bool conn_pending;

/* Lock state: pending until the callback, held until stream setup completes. */
static bool lock_pending;
static bool set_locked;

size_t set_member_count(void)
{
    return member_count;
}

uint16_t set_member_handle(size_t index)
{
    return (index < member_count) ? members[index].handle : CONN_HANDLE_INIT;
}

const uint8_t *set_member_addr(uint16_t conn_handle)
{
    for (size_t i = 0; i < member_count; i++) {
        if (members[i].handle == conn_handle) {
            return members[i].dst;
        }
    }

    return NULL;
}

static struct set_member *member_by_handle(uint16_t handle)
{
    for (size_t i = 0; i < member_count; i++) {
        if (members[i].handle == handle) {
            return &members[i];
        }
    }

    return NULL;
}

static void member_remove(uint16_t handle)
{
    for (size_t i = 0; i < member_count; i++) {
        if (members[i].handle != handle) {
            continue;
        }

        for (size_t j = i + 1; j < member_count; j++) {
            members[j - 1] = members[j];
        }

        member_count--;
        memset(&members[member_count], 0, sizeof(members[member_count]));

        return;
    }
}

static bool member_is_known(const uint8_t addr[6])
{
    for (size_t i = 0; i < member_count; i++) {
        if (memcmp(members[i].dst, addr, sizeof(members[i].dst)) == 0) {
            return true;
        }
    }

    return false;
}

/* The library looks members up by handle, not by slot. */
static const uint16_t *lock_handles(void)
{
    static uint16_t handles[TMAP_CEN_PEER_COUNT];

    for (size_t i = 0; i < member_count; i++) {
        handles[i] = members[i].handle;
    }

    return handles;
}

/* Index 0 must be the rank 1 device; unranked members keep connection order. */
static void set_members_sort_by_rank(void)
{
    for (size_t i = 1; i < member_count; i++) {
        struct set_member tmp = members[i];
        uint8_t rank = csip_member_rank_get(tmp.handle);
        size_t j = i;

        if (rank == 0) {
            continue;
        }

        while (j > 0) {
            uint8_t prev = csip_member_rank_get(members[j - 1].handle);

            if (prev != 0 && prev <= rank) {
                break;
            }

            members[j] = members[j - 1];
            j--;
        }

        members[j] = tmp;
    }
}

static void tmap_discovery_complete(esp_ble_audio_tmap_role_t role,
                                    esp_ble_conn_t *conn, int err)
{
    if (err) {
        ESP_LOGE(TAG, "TMAP discovery completed, err %d", err);
        return;
    }

    if (member_by_handle(conn->handle) == NULL) {
        return;
    }

    /* Audio setup waits for set_lock_complete(). */
    ESP_LOGI(TAG, "TMAP discovery done: handle %u", conn->handle);
}

static esp_ble_audio_tmap_cb_t tmap_callbacks = {
    .discovery_complete = tmap_discovery_complete,
};

/* TMAP role and set identity are separate AD elements; decide after parsing. */
struct adv_match {
    bool tmap_umr;
    bool rsi_present;
    bool rsi_ours;
};

static bool collect_adv_match(uint8_t type, const uint8_t *data,
                              uint8_t data_len, void *user_data)
{
    struct adv_match *match;
    uint16_t tmap_role;
    uint16_t uuid_val;

    match = user_data;
    assert(match);

    if (type == EXAMPLE_AD_TYPE_CSIS_RSI) {
        /* Resolves the advertised RSI against our SIRK. */
        match->rsi_present = true;
        match->rsi_ours = csip_adv_is_set_member(type, data, data_len);
        return true;    /* Continue parsing to next AD data type */
    }

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

    if (data_len < sizeof(uuid_val) + sizeof(tmap_role)) {
        ESP_LOGW(TAG, "Invalid ad size %u (tmap role)", data_len);
        return true;    /* Continue parsing to next AD data type */
    }

    tmap_role = sys_get_le16(data + sizeof(uuid_val));
    match->tmap_umr = (tmap_role & ESP_BLE_AUDIO_TMAP_ROLE_UMR) != 0;

    return true;    /* Continue parsing to next AD data type */
}

static void ext_scan_recv(esp_ble_audio_gap_app_event_t *event)
{
    struct adv_match match = {0};
    int err;

    if (member_count >= ARRAY_SIZE(members) || conn_pending) {
        return;
    }

    if ((event->ext_scan_recv.event_type & EXAMPLE_ADV_PROP_CONNECTABLE) == 0) {
        return;
    }

    if (member_is_known(event->ext_scan_recv.addr.val)) {
        return;
    }

    esp_ble_audio_data_parse(event->ext_scan_recv.data,
                             event->ext_scan_recv.data_len,
                             collect_adv_match, &match);

    if (!match.tmap_umr) {
        return;
    }

    /* A coordinated set build only accepts members carrying our SIRK. */
    if (TMAP_CEN_PEER_COUNT > 1 && !(match.rsi_present && match.rsi_ours)) {
        return;
    }

    ESP_LOGI(TAG, "Found peer %zu/%u (TMAS UMR%s)", member_count + 1, TMAP_CEN_PEER_COUNT,
             match.rsi_present ? ", set member" : "");

    err = ext_scan_stop();
    if (err) {
        ESP_LOGE(TAG, "Failed to stop scanning, err %d", err);
        return;
    }

    err = conn_create(event->ext_scan_recv.addr.type,
                      event->ext_scan_recv.addr.val);
    if (err) {
        ESP_LOGE(TAG, "Failed to create conn, err %d", err);
        ext_scan_start();
        return;
    }

    conn_pending = true;
}

static void acl_connect(esp_ble_audio_gap_app_event_t *event)
{
    int err;

    conn_pending = false;

    if (event->acl_connect.status) {
        ESP_LOGE(TAG, "Connection failed, status %d", event->acl_connect.status);
        ext_scan_start();
        return;
    }

    ESP_LOGI(TAG, "Connected: handle %u role %u peer %02x:%02x:%02x:%02x:%02x:%02x",
             event->acl_connect.conn_handle, event->acl_connect.role,
             EXAMPLE_BT_ADDR_PRINT_ARGS(event->acl_connect.dst.val));

    if (member_count >= ARRAY_SIZE(members)) {
        ESP_LOGE(TAG, "Set member table full");
        return;
    }

    memset(&members[member_count], 0, sizeof(members[member_count]));
    members[member_count].handle = event->acl_connect.conn_handle;
    memcpy(members[member_count].dst, event->acl_connect.dst.val,
           sizeof(members[member_count].dst));
    member_count++;

    err = pairing_start(event->acl_connect.conn_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to initiate security, err %d", err);
        return;
    }
}

/* Nothing is connected any more: forget the whole session. */
static void set_teardown_finish(void)
{
    lock_pending = false;
    set_locked = false;

    cap_initiator_reset();

    vcp_vol_ctlr_reset();

    ext_scan_start();
}

static void acl_disconnect(esp_ble_audio_gap_app_event_t *event)
{
    ESP_LOGI(TAG, "Disconnected: handle %u reason 0x%02x",
             event->acl_disconnect.conn_handle, event->acl_disconnect.reason);

    if (member_by_handle(event->acl_disconnect.conn_handle) == NULL) {
        return;
    }

    member_remove(event->acl_disconnect.conn_handle);
    peer_forget(event->acl_disconnect.conn_handle);
    vcp_vol_ctlr_forget(event->acl_disconnect.conn_handle);

    /* An incomplete set is no longer something to drive as one. */
    vcp_vol_ctlr_streams_stopped();

    /* Give the lock back on whatever is still connected, and drop a lock that
     * is still in flight: the set it was taken for no longer exists. */
    set_release_lock();
    lock_pending = false;

    if (member_count == 0) {
        set_teardown_finish();
        return;
    }

    /* Nothing else to undo: the members that are left keep their links and
     * keep streaming. Only the departed member's own streams were released,
     * by the stack, and its slot in the CIG stays reserved for whatever comes
     * back to fill it. */
    ESP_LOGI(TAG, "Lost a member; %zu left, scanning for the other", member_count);

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

/* MTU exchange and service discovery complete in either order. */
static void member_discover_profiles(struct set_member *member)
{
    esp_err_t err;

    if (!member->mtu_exchanged || !member->disc_completed) {
        return;
    }

    err = esp_ble_audio_tmap_discover(member->handle, &tmap_callbacks);
    if (err) {
        ESP_LOGE(TAG, "Failed to discover tmap, err %d", err);
        return;
    }

    /* The CAP commander needs a volume controller on every member. */
    vcp_vol_ctlr_discover(member->handle);

    /* Look for the remaining members while this one is discovered. */
    if (member_count < ARRAY_SIZE(members)) {
        ESP_LOGI(TAG, "Peer %zu/%u connected, scanning for the rest",
                 member_count, TMAP_CEN_PEER_COUNT);
        ext_scan_start();
    }

    /* Rank before endpoints, so members can be ordered. No CSIS, no callback. */
    if (csip_discover_start(member->handle) != 0) {
        set_member_discovered(member->handle, -ENOTSUP);
    }
}

static void gatt_mtu_change(esp_ble_audio_gatt_app_event_t *event)
{
    struct set_member *member;
    esp_err_t err;

    ESP_LOGI(TAG, "MTU updated: handle %u mtu %u",
             event->gatt_mtu_change.conn_handle, event->gatt_mtu_change.mtu);

    if (event->gatt_mtu_change.mtu < ESP_BLE_AUDIO_ATT_MTU_MIN) {
        ESP_LOGW(TAG, "MTU %u below minimum %u",
                 event->gatt_mtu_change.mtu, ESP_BLE_AUDIO_ATT_MTU_MIN);
        return;
    }

    member = member_by_handle(event->gatt_mtu_change.conn_handle);
    if (member == NULL) {
        return;
    }

    err = esp_ble_audio_gattc_disc_start(event->gatt_mtu_change.conn_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to start service discovery, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Service discovery started: handle %u",
             event->gatt_mtu_change.conn_handle);

    member->mtu_exchanged = true;

    member_discover_profiles(member);
}

static void gattc_disc_cmpl(esp_ble_audio_gatt_app_event_t *event)
{
    struct set_member *member;

    ESP_LOGI(TAG, "Service discovery complete: handle %u status %u",
             event->gattc_disc_cmpl.conn_handle,
             event->gattc_disc_cmpl.status);

    if (event->gattc_disc_cmpl.status) {
        return;
    }

    member = member_by_handle(event->gattc_disc_cmpl.conn_handle);
    if (member == NULL) {
        return;
    }

    member->disc_completed = true;

    member_discover_profiles(member);
}

/* Ordered access needs all ranks 0 or all unique: a duplicate locks one member
 * twice and skips the other. */
static bool set_ranks_are_valid(void)
{
    size_t unranked = 0;

    for (size_t i = 0; i < member_count; i++) {
        uint8_t rank = csip_member_rank_get(members[i].handle);

        if (rank == 0) {
            unranked++;
            continue;
        }

        for (size_t j = i + 1; j < member_count; j++) {
            if (csip_member_rank_get(members[j].handle) == rank) {
                ESP_LOGE(TAG, "Handle %u and handle %u both report rank %u; flash the "
                         "peripherals for different ears so their ranks differ",
                         members[i].handle, members[j].handle, rank);
                return false;
            }
        }
    }

    if (unranked != 0 && unranked != member_count) {
        ESP_LOGE(TAG, "Set mixes %zu ranked and %zu unranked member(s)",
                 member_count - unranked, unranked);
        return false;
    }

    return true;
}

/* One peripheral finished set discovery, or has no CSIS at all. */
void set_member_discovered(uint16_t conn_handle, int err)
{
    struct set_member *member = member_by_handle(conn_handle);

    /* A standalone headset exposes no CSIS; keep going with what is connected. */
    if (err) {
        ESP_LOGI(TAG, "Handle %u is not a coordinated set member (err %d)", conn_handle, err);
    }

    if (member == NULL) {
        return;
    }

    member->set_discovered = true;

    if (member_count < ARRAY_SIZE(members)) {
        return;
    }

    for (size_t i = 0; i < member_count; i++) {
        if (!members[i].set_discovered) {
            return;
        }
    }

    ESP_LOGI(TAG, "All %zu peer(s) ready", member_count);

    set_members_sort_by_rank();

    /* Streams still work unlocked, so report and carry on. */
    if (!set_ranks_are_valid()) {
        set_lock_complete(-EINVAL);
        return;
    }

    /* Nothing to lock: go straight to audio setup. */
    if (csip_lock_set(lock_handles(), member_count) != 0) {
        set_lock_complete(-ENOTSUP);
        return;
    }

    lock_pending = true;
}

void set_lock_complete(int err)
{
    /* A set that cannot be locked may still be operated on. */
    if (err && lock_pending) {
        ESP_LOGW(TAG, "Proceeding without the set lock, err %d", err);
    }

    set_locked = (err == 0) && lock_pending;
    lock_pending = false;

    /* A member may have gone while the lock was in flight; the set that is left
     * is not the one to set up. */
    if (member_count < ARRAY_SIZE(members)) {
        set_release_lock();
        return;
    }

    if (cap_initiator_setup() != 0) {
        set_release_lock();
    }
}

/* Give the lock back once stream setup is done. Called from cap_initiator.c. */
void set_release_lock(void)
{
    if (!set_locked) {
        return;
    }

    if (csip_release_set(lock_handles(), member_count) == 0) {
        set_locked = false;
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

    err = csip_set_coordinator_init();
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
