/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tmap_central.h"

#if TMAP_CEN_PEER_COUNT > 1

/* Must match the SIRK the peripherals register (peripheral/main/csip_set_member.c). */
#define CSIP_SIRK_DEBUG     { 0xcd, 0xcc, 0x72, 0xdd, 0x86, 0x8c, 0xcd, 0xce, \
                              0x22, 0xfd, 0xa1, 0x21, 0x09, 0x7d, 0x7d, 0x45 }

static const uint8_t set_sirk[ESP_BLE_AUDIO_CSIP_SIRK_SIZE] = CSIP_SIRK_DEBUG;

/* Look up by the caller's handles: the library keys on the controller-assigned
 * ACL handle, not a slot number. */
static uint8_t collect_members(const uint16_t *conn_handles, size_t count,
                               const esp_ble_audio_csip_set_coordinator_set_member_t *out[])
{
    uint8_t found = 0;

    for (size_t i = 0; i < count; i++) {
        const esp_ble_audio_csip_set_coordinator_set_member_t *member =
            esp_ble_audio_csip_set_coordinator_set_member_by_conn(conn_handles[i]);

        if (member != NULL) {
            out[found++] = member;
        }
    }

    return found;
}

static void csip_discover_cb(struct bt_conn *conn,
                             const esp_ble_audio_csip_set_coordinator_set_member_t *member,
                             int err, size_t set_count)
{
    uint16_t conn_handle = (conn != NULL) ? conn->handle : CONN_HANDLE_INIT;

    if (err) {
        ESP_LOGE(TAG, "CSIP discovery failed on handle %u, err %d", conn_handle, err);
        set_member_discovered(conn_handle, err);
        return;
    }

    if (set_count == 0 || member == NULL) {
        ESP_LOGE(TAG, "No coordinated set on handle %u", conn_handle);
        set_member_discovered(conn_handle, -ENOENT);
        return;
    }

    ESP_LOGI(TAG, "CSIP discovered: handle %u set_size %u rank %u lockable %u",
             conn_handle, member->insts[0].info.set_size,
             member->insts[0].info.rank, member->insts[0].info.lockable);

    if (member->insts[0].info.set_size != TMAP_CEN_PEER_COUNT) {
        ESP_LOGW(TAG, "Peer reports set size %u, this build drives %u member(s)",
                 member->insts[0].info.set_size, TMAP_CEN_PEER_COUNT);
    }

    set_member_discovered(conn_handle, 0);
}

static void csip_lock_set_cb(int err)
{
    /* A failure is reported by set_lock_complete(), which also says what it
     * does about it. */
    if (!err) {
        ESP_LOGI(TAG, "Set locked");
    }

    set_lock_complete(err);
}

static void csip_release_set_cb(int err)
{
    if (err) {
        ESP_LOGE(TAG, "Failed to release set, err %d", err);
    } else {
        ESP_LOGI(TAG, "Set released");
    }
}

static esp_ble_audio_csip_set_coordinator_cb_t csip_cb = {
    .discover    = csip_discover_cb,
    .lock_set    = csip_lock_set_cb,
    .release_set = csip_release_set_cb,
};

int csip_set_coordinator_init(void)
{
    int err;

    err = esp_ble_audio_csip_set_coordinator_register_cb(&csip_cb);
    if (err) {
        ESP_LOGE(TAG, "Failed to register csip callbacks, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "CSIP set coordinator initialized");

    return 0;
}

bool csip_adv_is_set_member(uint8_t ad_type, const uint8_t *data, uint8_t data_len)
{
    return esp_ble_audio_csip_set_coordinator_is_set_member(set_sirk, ad_type, data, data_len);
}

int csip_discover_start(uint16_t conn_handle)
{
    int err;

    err = esp_ble_audio_csip_set_coordinator_discover(conn_handle);
    if (err) {
        ESP_LOGE(TAG, "Failed to start csip discovery on handle %u, err %d", conn_handle, err);
        return err;
    }

    return 0;
}

uint8_t csip_member_rank_get(uint16_t conn_handle)
{
    const esp_ble_audio_csip_set_coordinator_set_member_t *member =
        esp_ble_audio_csip_set_coordinator_set_member_by_conn(conn_handle);

    return (member != NULL) ? member->insts[0].info.rank : 0;
}

int csip_lock_set(const uint16_t *conn_handles, size_t count)
{
    const esp_ble_audio_csip_set_coordinator_set_member_t *members[TMAP_CEN_PEER_COUNT];
    uint8_t found;
    int err;

    if (conn_handles == NULL || count == 0 || count > ARRAY_SIZE(members)) {
        return -EINVAL;
    }

    found = collect_members(conn_handles, count, members);
    if (found == 0) {
        ESP_LOGE(TAG, "No set member to lock");
        return -ENOENT;
    }

    /* The library locks ascending by rank, releases descending. */
    err = esp_ble_audio_csip_set_coordinator_lock(members, found, &members[0]->insts[0].info);
    if (err) {
        ESP_LOGE(TAG, "Failed to issue set lock, err %d", err);
        return err;
    }

    return 0;
}

int csip_release_set(const uint16_t *conn_handles, size_t count)
{
    const esp_ble_audio_csip_set_coordinator_set_member_t *members[TMAP_CEN_PEER_COUNT];
    uint8_t found;
    int err;

    if (conn_handles == NULL || count == 0 || count > ARRAY_SIZE(members)) {
        return -EINVAL;
    }

    found = collect_members(conn_handles, count, members);
    if (found == 0) {
        return -ENOENT;
    }

    err = esp_ble_audio_csip_set_coordinator_release(members, found, &members[0]->insts[0].info);
    if (err) {
        ESP_LOGE(TAG, "Failed to issue set release, err %d", err);
        return err;
    }

    return 0;
}

#endif /* TMAP_CEN_PEER_COUNT > 1 */
