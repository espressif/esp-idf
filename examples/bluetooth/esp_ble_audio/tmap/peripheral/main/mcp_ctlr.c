/*
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <errno.h>

#include "tmap_peripheral.h"

static uint16_t conn_handle = CONN_HANDLE_INIT;
static bool is_remote_read;

static void discover_mcs_cb(esp_ble_conn_t *conn, int err)
{
    if (err) {
        ESP_LOGE(TAG, "Discover MCS failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Discover MCS succeeded");

    if (is_remote_read == false) {
        err = esp_ble_audio_mcc_read_player_name(conn_handle);
        if (err) {
            ESP_LOGE(TAG, "Read player name failed, err %d", err);
        }
    }
}

static void read_player_name_cb(esp_ble_conn_t *conn, int err, const char *name)
{
    if (err) {
        ESP_LOGE(TAG, "Read player name failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Read player name succeeded, name %s", name);

    if (is_remote_read == false) {
        err = esp_ble_audio_mcc_read_track_title(conn_handle);
        if (err) {
            ESP_LOGE(TAG, "Read track title failed, err %d", err);
        }
    }
}

static void track_changed_ntf_cb(esp_ble_conn_t *conn, int err)
{
    if (err) {
        ESP_LOGE(TAG, "Invalid track changed ntf received, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Track changed ntf received");
}

static void read_track_title_cb(esp_ble_conn_t *conn, int err, const char *title)
{
    if (err) {
        ESP_LOGE(TAG, "Read track title failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Read track title succeeded, title %s", title);

    if (is_remote_read == false) {
        err = esp_ble_audio_mcc_read_track_duration(conn_handle);
        if (err) {
            ESP_LOGE(TAG, "Read track duration failed, err %d", err);
        }
    }
}

static void read_track_duration_cb(esp_ble_conn_t *conn, int err, int32_t dur)
{
    if (err) {
        ESP_LOGE(TAG, "Read track duration failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Read track duration succeeded, dur %ld", dur);

    if (is_remote_read == false) {
        err = esp_ble_audio_mcc_read_track_position(conn_handle);
        if (err) {
            ESP_LOGE(TAG, "Read track position failed, err %d", err);
        }
    }
}

static void read_track_position_cb(esp_ble_conn_t *conn, int err, int32_t pos)
{
    if (err) {
        ESP_LOGE(TAG, "Read track position failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Read track position succeeded, pos %ld", pos);

    if (is_remote_read == false) {
        err = esp_ble_audio_mcc_read_playback_speed(conn_handle);
        if (err) {
            ESP_LOGE(TAG, "Read playback speed failed, err %d", err);
        }
    }
}

static void set_track_position_cb(esp_ble_conn_t *conn, int err, int32_t pos)
{
    if (err) {
        ESP_LOGE(TAG, "Set track position failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Set track position succeeded, pos %ld", pos);
}

static void read_playback_speed_cb(esp_ble_conn_t *conn, int err, int8_t speed)
{
    if (err) {
        ESP_LOGE(TAG, "Read track playback speed failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Read track playback speed succeeded, speed %d", speed);

    if (is_remote_read == false) {
        err = esp_ble_audio_mcc_read_seeking_speed(conn_handle);
        if (err) {
            ESP_LOGE(TAG, "Read seeking speed failed, err %d", err);
        }
    }
}

static void set_playback_speed_cb(esp_ble_conn_t *conn, int err, int8_t speed)
{
    if (err) {
        ESP_LOGE(TAG, "Set track playback speed failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Set track playback speed succeeded, speed %d", speed);
}

static void read_seeking_speed_cb(esp_ble_conn_t *conn, int err, int8_t speed)
{
    if (err) {
        ESP_LOGE(TAG, "Read seeking playback speed failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Read seeking playback speed succeeded, speed %d", speed);

    if (is_remote_read == false) {
        err = esp_ble_audio_mcc_read_playing_order(conn_handle);
        if (err) {
            ESP_LOGE(TAG, "Read playing order failed, err %d", err);
        }
    }
}

static void read_playing_order_cb(esp_ble_conn_t *conn, int err, uint8_t order)
{
    if (err) {
        ESP_LOGE(TAG, "Read playing order failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Read playing order succeeded, order %u", order);

    if (is_remote_read == false) {
        err = esp_ble_audio_mcc_read_playing_orders_supported(conn_handle);
        if (err) {
            ESP_LOGE(TAG, "Read playing orders supported failed, err %d", err);
        }
    }
}

static void set_playing_order_cb(esp_ble_conn_t *conn, int err, uint8_t order)
{
    if (err) {
        ESP_LOGE(TAG, "Set playing order failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Set playing order succeeded, order %u", order);
}

static void read_playing_orders_supported_cb(esp_ble_conn_t *conn, int err, uint16_t orders)
{
    if (err) {
        ESP_LOGE(TAG, "Read playing orders supported failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Read playing orders supported succeeded, orders 0x%04x", orders);

    if (is_remote_read == false) {
        err = esp_ble_audio_mcc_read_media_state(conn_handle);
        if (err) {
            ESP_LOGE(TAG, "Read media state failed, err %d", err);
        }
    }
}

static void read_media_state_cb(esp_ble_conn_t *conn, int err, uint8_t state)
{
    if (err) {
        ESP_LOGE(TAG, "Read media state failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Read media state succeeded, state %u", state);

    if (is_remote_read == false) {
        err = esp_ble_audio_mcc_read_opcodes_supported(conn_handle);
        if (err) {
            ESP_LOGE(TAG, "Read opcodes supported failed, err %d", err);
        }
    }
}

static void send_cmd_cb(esp_ble_conn_t *conn, int err, const struct mpl_cmd *cmd)
{
    if (err) {
        ESP_LOGE(TAG, "Send command failed, err %d, cmd %p", err, cmd);
        return;
    }

    ESP_LOGI(TAG, "Send command succeeded, cmd %p", cmd);
}

static void cmd_ntf_cb(esp_ble_conn_t *conn, int err, const struct mpl_cmd_ntf *ntf)
{
    if (err) {
        ESP_LOGE(TAG, "Invalid command ntf received, err %d, ntf %p", err, ntf);
        return;
    }

    ESP_LOGI(TAG, "Command ntf received, ntf %p", ntf);
}

static void read_opcodes_supported_cb(esp_ble_conn_t *conn, int err, uint32_t opcodes)
{
    if (err) {
        ESP_LOGE(TAG, "Read opcodes supported failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Read opcodes supported succeeded, opcodes 0x%08lx", opcodes);

    if (is_remote_read == false) {
        err = esp_ble_audio_mcc_read_content_control_id(conn_handle);
        if (err) {
            ESP_LOGE(TAG, "Read content control id failed, err %d", err);
        }
    }
}

static void read_content_control_id_cb(esp_ble_conn_t *conn, int err, uint8_t ccid)
{
    if (err) {
        ESP_LOGE(TAG, "Read content control id failed, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Read content control id succeeded, ccid %u", ccid);

    is_remote_read = true;
}

static esp_ble_audio_mcc_cb_t mcc_cb = {
    .discover_mcs                  = discover_mcs_cb,
    .read_player_name              = read_player_name_cb,
    .track_changed_ntf             = track_changed_ntf_cb,
    .read_track_title              = read_track_title_cb,
    .read_track_duration           = read_track_duration_cb,
    .read_track_position           = read_track_position_cb,
    .set_track_position            = set_track_position_cb,
    .read_playback_speed           = read_playback_speed_cb,
    .set_playback_speed            = set_playback_speed_cb,
    .read_seeking_speed            = read_seeking_speed_cb,
    .read_playing_order            = read_playing_order_cb,
    .set_playing_order             = set_playing_order_cb,
    .read_playing_orders_supported = read_playing_orders_supported_cb,
    .read_media_state              = read_media_state_cb,
    .send_cmd                      = send_cmd_cb,
    .cmd_ntf                       = cmd_ntf_cb,
    .read_opcodes_supported        = read_opcodes_supported_cb,
    .read_content_control_id       = read_content_control_id_cb,
};

int mcp_ctlr_init(void)
{
    int err;

    err = esp_ble_audio_mcc_init(&mcc_cb);
    if (err) {
        ESP_LOGE(TAG, "Failed to init MCC, err %d", err);
        return err;
    }

    ESP_LOGI(TAG, "MCP controller initialized");

    return 0;
}

int mcp_discover_mcs(void)
{
    int err;

    conn_handle = default_conn_handle_get();
    if (conn_handle == CONN_HANDLE_INIT) {
        ESP_LOGE(TAG, "%s, not connected", __func__);
        return -ENOTCONN;
    }

    /* New session — re-arm the initial read cascade. */
    is_remote_read = false;

    err = esp_ble_audio_mcc_discover_mcs(conn_handle, true);
    if (err) {
        ESP_LOGE(TAG, "Failed to discover MCS, err %d", err);
        return err;
    }

    return 0;
}

int mcp_send_cmd(uint8_t mcp_opcode)
{
    esp_ble_audio_mpl_cmd_t cmd = {
        .opcode = mcp_opcode,
        .use_param = false,
    };
    int err;

    err = esp_ble_audio_mcc_send_cmd(conn_handle, &cmd);
    if (err) {
        ESP_LOGE(TAG, "Failed to send MCP command, err %d", err);
        return err;
    }

    return 0;
}
