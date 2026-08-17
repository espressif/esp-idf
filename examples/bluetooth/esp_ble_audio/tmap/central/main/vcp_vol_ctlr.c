/*
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "tmap_central.h"

/* A TWS pair must change volume on both earbuds together, so commands go through
 * the CAP commander. It refuses until every member has a volume controller, and a
 * VCS write needs the change counter synced, hence the per-member state read.
 *
 * Kept per member rather than as a count: a member that keeps its link while
 * another one is replaced keeps its controller and its synced counter too. */
struct vol_ctlr_slot {
    uint16_t                      conn_handle;
    esp_ble_audio_vcp_vol_ctlr_t *ctlr;
    bool                          state_read;
};

static struct vol_ctlr_slot vol_ctlrs[TMAP_CEN_PEER_COUNT];
static bool streams_started;

/* Walks the set through one commander procedure per tick, so the log shows both
 * earbuds following every step of a single procedure rather than being written
 * one at a time.
 *
 * This is application behaviour, so it gets an application task rather than
 * borrowing one of the stack's. Not an esp_timer callback either: that task is
 * shared with the BLE stack's own timers — the ISO TX schedulers post from it
 * every SDU interval — and it is serial, so a GATT write chain dispatched there
 * would sit in front of them. The audio APIs take the host lock themselves
 * (esp_ble_audio_cap_commander_* -> bt_cap_commander_*_safe -> k_lock), which is
 * what makes calling them from here safe. */
#define VOLUME_PERIOD_MS      (5 * 60 * 1000)   /* 5 minutes */
#define VOLUME_TASK_STACK     3072
#define VOLUME_TASK_PRIO      5

static const struct {
    bool    is_mute;
    uint8_t value;      /* volume level, or mute on/off */
} volume_steps[] = {
    { false, 32  },
    { false, 96  },
    { false, 160 },
    { false, 224 },
    { true,  1   },
    { true,  0   },
};

static size_t volume_index;

/* Written here, cleared from the commander callback in the audio task. */
static volatile bool volume_busy;
static volatile bool volume_running;

static struct vol_ctlr_slot *slot_by_handle(uint16_t conn_handle)
{
    for (size_t i = 0; i < ARRAY_SIZE(vol_ctlrs); i++) {
        if (vol_ctlrs[i].ctlr != NULL && vol_ctlrs[i].conn_handle == conn_handle) {
            return &vol_ctlrs[i];
        }
    }

    return NULL;
}

static struct vol_ctlr_slot *slot_by_ctlr(const esp_ble_audio_vcp_vol_ctlr_t *ctlr)
{
    for (size_t i = 0; i < ARRAY_SIZE(vol_ctlrs); i++) {
        if (vol_ctlrs[i].ctlr == ctlr) {
            return &vol_ctlrs[i];
        }
    }

    return NULL;
}

static int set_volume_mute(bool mute)
{
    esp_ble_audio_cap_set_member_t members[TMAP_CEN_PEER_COUNT] = {0};
    esp_ble_audio_cap_commander_change_volume_mute_state_param_t param = {0};
    size_t count;
    int err;

    count = set_member_count();
    if (count == 0) {
        return -ENOTCONN;
    }

    param.type = set_type_get();

    for (size_t i = 0; i < count; i++) {
        if (!set_member_fill(&members[i], param.type, set_member_handle(i))) {
            ESP_LOGE(TAG, "No set member for handle %u", set_member_handle(i));
            return -ENOENT;
        }
    }

    param.members = members;
    param.count = count;
    param.mute = mute;

    err = esp_ble_audio_cap_commander_change_volume_mute_state(&param);
    if (err) {
        ESP_LOGE(TAG, "Failed to %s the set, err %d", mute ? "mute" : "unmute", err);
        return err;
    }

    ESP_LOGI(TAG, "Changing volume mute state of %zu member(s) to %u", count, mute);

    return 0;
}

static void volume_tick(void)
{
    size_t step = volume_index;
    int err;

    /* The commander runs one procedure at a time, so a tick is skipped rather
     * than piled on top of a set that has not answered yet. */
    if (volume_busy) {
        ESP_LOGW(TAG, "Previous volume procedure still running, skipping a step");
        return;
    }

    volume_index = (volume_index + 1) % ARRAY_SIZE(volume_steps);

    /* Marked busy before issuing: a procedure that fails early reports back
     * from inside the call, and that callback must not be overwritten here. */
    volume_busy = true;

    if (volume_steps[step].is_mute) {
        err = set_volume_mute(volume_steps[step].value != 0);
    } else {
        err = vcp_vol_ctlr_set_volume(volume_steps[step].value);
    }

    if (err) {
        volume_busy = false;
    }
}

static void volume_task(void *arg)
{
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(VOLUME_PERIOD_MS));

        if (volume_running) {
            volume_tick();
        }
    }
}

static void volume_stop(void)
{
    volume_running = false;
    volume_busy = false;
}

/* Needs every VCS state read and the streams up, in either order. */
static void volume_try_start(void)
{
    size_t count = set_member_count();

    if (volume_running || !streams_started || count == 0) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        const struct vol_ctlr_slot *slot = slot_by_handle(set_member_handle(i));

        if (slot == NULL || !slot->state_read) {
            return;
        }
    }

    volume_index = 0;
    volume_busy = false;
    volume_running = true;

    ESP_LOGI(TAG, "Driving the volume of %zu member(s) every %u s",
             count, VOLUME_PERIOD_MS / 1000);
}

static void vcs_discover_cb(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr, int err,
                            uint8_t vocs_count, uint8_t aics_count)
{
    if (err) {
        ESP_LOGE(TAG, "VCP discovery cb failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "VCP discovery done, vocs %u aics %u", vocs_count, aics_count);

    /* Control-point writes carry a change counter: read the state to sync it. */
    err = esp_ble_audio_vcp_vol_ctlr_read_state(vol_ctlr);
    if (err) {
        ESP_LOGE(TAG, "Failed to read VCS state, err %d", err);
    }
}

static void vcs_write_cb(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr, int err)
{
    if (err) {
        ESP_LOGE(TAG, "VCP write cb failed, err %d", err);
    } else {
        ESP_LOGI(TAG, "VCP write cb done");
    }
}

static void vcs_state_cb(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr,
                         int err, uint8_t volume, uint8_t mute)
{
    struct vol_ctlr_slot *slot;

    if (err) {
        ESP_LOGE(TAG, "VCP state cb failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "VCP state cb done, volume %u mute %u", volume, mute);

    slot = slot_by_ctlr(vol_ctlr);
    if (slot != NULL) {
        slot->state_read = true;
    }

    volume_try_start();
}

static void vcs_flags_cb(esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr,
                         int err, uint8_t flags)
{
    if (err) {
        ESP_LOGE(TAG, "VCP flags cb failed, err %d", err);
    } else {
        ESP_LOGI(TAG, "VCP flags cb done, flags 0x%02x", flags);
    }
}

static esp_ble_audio_vcp_vol_ctlr_cb_t vcp_cbs = {
    .discover        = vcs_discover_cb,
    .vol_down        = vcs_write_cb,
    .vol_up          = vcs_write_cb,
    .mute            = vcs_write_cb,
    .unmute          = vcs_write_cb,
    .vol_down_unmute = vcs_write_cb,
    .vol_up_unmute   = vcs_write_cb,
    .vol_set         = vcs_write_cb,
    .state           = vcs_state_cb,
    .flags           = vcs_flags_cb,
};

/* One callback per procedure, not per member: conn names the member that failed. */
static void volume_changed_cb(esp_ble_conn_t *conn, int err)
{
    if (err) {
        ESP_LOGE(TAG, "Set volume failed on handle %u, err %d",
                 (conn != NULL) ? conn->handle : CONN_HANDLE_INIT, err);
    } else {
        ESP_LOGI(TAG, "Set volume changed");
    }

    volume_busy = false;
}

static void volume_mute_changed_cb(esp_ble_conn_t *conn, int err)
{
    if (err) {
        ESP_LOGE(TAG, "Set volume mute failed on handle %u, err %d",
                 (conn != NULL) ? conn->handle : CONN_HANDLE_INIT, err);
    } else {
        ESP_LOGI(TAG, "Set volume mute changed");
    }

    volume_busy = false;
}

static esp_ble_audio_cap_commander_cb_t cap_commander_cbs = {
    .volume_changed      = volume_changed_cb,
    .volume_mute_changed = volume_mute_changed_cb,
};

int vcp_vol_ctlr_discover(uint16_t conn_handle)
{
    esp_ble_audio_vcp_vol_ctlr_t *vol_ctlr;
    struct vol_ctlr_slot *slot;
    int err;

    if (conn_handle == CONN_HANDLE_INIT) {
        ESP_LOGE(TAG, "%s, not connected", __func__);
        return -ENOTCONN;
    }

    slot = slot_by_ctlr(NULL);
    if (slot == NULL) {
        ESP_LOGE(TAG, "No volume controller slot for handle %u", conn_handle);
        return -ENOMEM;
    }

    err = esp_ble_audio_vcp_vol_ctlr_discover(conn_handle, &vol_ctlr);
    if (err) {
        ESP_LOGE(TAG, "Failed to discover vcp vol ctlr on handle %u, err %d", conn_handle, err);
        return err;
    }

    slot->conn_handle = conn_handle;
    slot->ctlr = vol_ctlr;
    slot->state_read = false;

    ESP_LOGI(TAG, "VCP volume controller discovering: handle %u", conn_handle);

    return 0;
}

void vcp_vol_ctlr_forget(uint16_t conn_handle)
{
    struct vol_ctlr_slot *slot = slot_by_handle(conn_handle);

    if (slot != NULL) {
        memset(slot, 0, sizeof(*slot));
    }
}

void vcp_vol_ctlr_streams_started(void)
{
    streams_started = true;

    volume_try_start();
}

/* The set is no longer whole, so it is no longer something to drive as one. */
void vcp_vol_ctlr_streams_stopped(void)
{
    streams_started = false;

    volume_stop();
}

void vcp_vol_ctlr_reset(void)
{
    volume_stop();

    memset(vol_ctlrs, 0, sizeof(vol_ctlrs));
    streams_started = false;
}

int vcp_vol_ctlr_set_volume(uint8_t volume)
{
    esp_ble_audio_cap_set_member_t members[TMAP_CEN_PEER_COUNT] = {0};
    esp_ble_audio_cap_commander_change_volume_param_t param = {0};
    size_t count;
    int err;

    count = set_member_count();
    if (count == 0) {
        return -ENOTCONN;
    }

    param.type = set_type_get();

    for (size_t i = 0; i < count; i++) {
        if (!set_member_fill(&members[i], param.type, set_member_handle(i))) {
            return -ENOENT;
        }
    }

    param.members = members;
    param.count = count;
    param.volume = volume;

    err = esp_ble_audio_cap_commander_change_volume(&param);
    if (err) {
        ESP_LOGE(TAG, "Failed to set set volume, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "Changing volume of %zu member(s) to %u", count, volume);

    return 0;
}

int vcp_vol_ctlr_init(void)
{
    int err;

    err = esp_ble_audio_vcp_vol_ctlr_cb_register(&vcp_cbs);
    if (err) {
        ESP_LOGE(TAG, "Failed to register vcp vol ctlr cb, err %d", err);
        return err;
    }

    err = esp_ble_audio_cap_commander_register_cb(&cap_commander_cbs);
    if (err) {
        ESP_LOGE(TAG, "Failed to register cap commander cb, err %d", err);
        return err;
    }

    if (xTaskCreate(volume_task, "volume", VOLUME_TASK_STACK, NULL,
                    VOLUME_TASK_PRIO, NULL) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create the volume task");
        return -ENOMEM;
    }

    ESP_LOGI(TAG, "VCP volume controller initialized");

    return 0;
}
