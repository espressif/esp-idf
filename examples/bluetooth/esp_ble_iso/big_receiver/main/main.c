/*
 * SPDX-FileCopyrightText: 2021-2022 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_ble_iso_common_api.h"

#include "ble_iso_example_init.h"
#include "ble_iso_example_utils.h"

#include "scan.h"

#define TARGET_DEVICE_NAME      "BIG Broadcaster"
#define TARGET_DEVICE_NAME_LEN  (sizeof(TARGET_DEVICE_NAME) - 1)

#define TARGET_BROADCAST_CODE   "1234"

#define BIS_ISO_CHAN_COUNT      2       /* Use exactly 2 BIS channels */
#define BIG_SYNC_TIMEOUT        100     /* 100 * 10ms = 1s */

/* CONFIG_BT_ISO_MAX_CHAN must be >= 2 */
_Static_assert(CONFIG_BT_ISO_MAX_CHAN >= BIS_ISO_CHAN_COUNT,
               "CONFIG_BT_ISO_MAX_CHAN must be >= BIS_ISO_CHAN_COUNT");

static bool per_adv_synced;
static bool big_synced;

static esp_ble_iso_big_t *out_big;

/* Per-BIS counters, indexed by BIS channel. */
static example_iso_rx_metrics_t rx_metrics[BIS_ISO_CHAN_COUNT];

static int bis_chan_index_get(const esp_ble_iso_chan_t *chan);

static void iso_connected_cb(esp_ble_iso_chan_t *chan)
{
    const esp_ble_iso_chan_path_t data_path = {
        .pid    = ESP_BLE_ISO_DATA_PATH_HCI,
        .format = ESP_BLE_ISO_CODING_FORMAT_TRANSPARENT,
    };
    int chan_idx = bis_chan_index_get(chan);
    esp_err_t err;

    ESP_LOGI(TAG, "[BIS #%d] Connected", chan_idx);

    /* New BIS session — reset RX counters so milestones reflect
     * this session only. Matches the session-start reset pattern
     * used by cis_peripheral and the audio examples.
     */
    if (chan_idx >= 0) {
        example_iso_rx_metrics_reset(&rx_metrics[chan_idx]);
    }

    err = esp_ble_iso_setup_data_path(chan, ESP_BLE_ISO_DATA_PATH_DIR_OUTPUT, &data_path);
    if (err) {
        ESP_LOGE(TAG, "[BIS #%d] Failed to setup data path, err %d", chan_idx, err);
        return;
    }
}

static void iso_disconnected_cb(esp_ble_iso_chan_t *chan, uint8_t reason)
{
    /* Common BIS disconnect reasons during broadcaster restart:
     *
     *   0x08  CONN_TIMEOUT        - no subevent received within
     *                               BIG_Sync_Timeout (broadcaster is gone
     *                               or out of range). This is the common
     *                               case.
     *
     *   0x3D  TERM_DUE_TO_MIC_FAIL - packets arrived but MIC check
     *                               failed repeatedly. Only possible on
     *                               encrypted BIGs; typically means the
     *                               broadcaster restarted with a new
     *                               session before we timed out, so the
     *                               old session key no longer decrypts.
     *
     *                               !!! LOW PROBABILITY !!!
     *                               Requires the broadcaster to come
     *                               back on air within the BIG_Sync_
     *                               Timeout window — a narrow race.
     *
     * Both are normal: receiver will drop PA sync and re-discover.
     */
    ESP_LOGI(TAG, "[BIS #%d] Disconnected, reason 0x%02x",
             bis_chan_index_get(chan), reason);

    big_synced = false;
    out_big = NULL;
}

static void iso_recv_cb(esp_ble_iso_chan_t *chan,
                        const esp_ble_iso_recv_info_t *info,
                        const uint8_t *data, uint16_t len)
{
    int chan_idx = bis_chan_index_get(chan);
    char name[24];

    if (chan_idx < 0) {
        ESP_LOGW(TAG, "Unknown BIS channel");
        return;
    }

    snprintf(name, sizeof(name), "BIS #%d", chan_idx);
    rx_metrics[chan_idx].last_sdu_len = len;
    example_iso_rx_metrics_on_recv(info, &rx_metrics[chan_idx], TAG, name);
}

static esp_ble_iso_chan_ops_t iso_ops = {
    .connected    = iso_connected_cb,
    .disconnected = iso_disconnected_cb,
    .recv         = iso_recv_cb,
};

static esp_ble_iso_chan_io_qos_t iso_rx_qos[BIS_ISO_CHAN_COUNT];

static esp_ble_iso_chan_qos_t bis_iso_qos[] = {
    { .rx = &iso_rx_qos[0], },
    { .rx = &iso_rx_qos[1], },
};

static esp_ble_iso_chan_t bis_iso_chan[] = {
    { .ops = &iso_ops, .qos = &bis_iso_qos[0], },
    { .ops = &iso_ops, .qos = &bis_iso_qos[1], },
};

static esp_ble_iso_chan_t *bis[] = {
    &bis_iso_chan[0],
    &bis_iso_chan[1],
};

static int bis_chan_index_get(const esp_ble_iso_chan_t *chan)
{
    for (size_t i = 0; i < BIS_ISO_CHAN_COUNT; i++) {
        if (chan == &bis_iso_chan[i]) {
            return (int)i;
        }
    }

    return -1;
}

static bool data_cb(uint8_t type, const uint8_t *data,
                    uint8_t data_len, void *user_data)
{
    bool *target_matched = user_data;

    assert(target_matched);

    switch (type) {
    case EXAMPLE_AD_TYPE_NAME_COMPLETE:
        *target_matched = (data_len == TARGET_DEVICE_NAME_LEN) &&
                          !memcmp(data, TARGET_DEVICE_NAME, TARGET_DEVICE_NAME_LEN);
        return false;
    default:
        return true;
    }
}

static void ext_scan_recv(esp_ble_iso_gap_app_event_t *event)
{
    bool target_matched = false;
    int err;

    esp_ble_iso_data_parse(event->ext_scan_recv.data,
                           event->ext_scan_recv.data_len,
                           data_cb, &target_matched);

    /* Periodic advertising interval. 0 if no periodic advertising. */
    if (!target_matched || event->ext_scan_recv.per_adv_itvl == 0) {
        return;
    }

    if (per_adv_synced == false) {
        err = pa_sync_create(event->ext_scan_recv.addr.type,
                             event->ext_scan_recv.addr.val,
                             event->ext_scan_recv.sid);
        if (err) {
            ESP_LOGE(TAG, "Failed to create PA sync, err %d", err);
            return;
        }

        per_adv_synced = true;
    }
}

static void pa_sync(esp_ble_iso_gap_app_event_t *event)
{
    int err;

    if (event->pa_sync.status) {
        ESP_LOGE(TAG, "PA sync failed, status %d", event->pa_sync.status);
        per_adv_synced = false;
        return;
    }

    ESP_LOGI(TAG, "PA synced: handle %u sid %u phy %u peer %02x:%02x:%02x:%02x:%02x:%02x",
             event->pa_sync.sync_handle, event->pa_sync.sid, event->pa_sync.adv_phy,
             EXAMPLE_BT_ADDR_PRINT_ARGS(event->pa_sync.addr.val));

    /* PA sync is established; the BIGInfo report will arrive via the
     * PA sync channel, so the extended scanner is no longer needed.
     * Stop it now — pa_sync_lost() will restart it on loss.
     */
    err = ext_scan_stop();
    if (err) {
        ESP_LOGW(TAG, "Failed to stop scanning, err %d", err);
    }
}

static void pa_sync_lost(esp_ble_iso_gap_app_event_t *event)
{
    ESP_LOGI(TAG, "PA sync lost: sync_handle %u reason 0x%02x",
             event->pa_sync_lost.sync_handle, event->pa_sync_lost.reason);

    per_adv_synced = false;
    big_synced = false;
    out_big = NULL;

    ext_scan_start();
}

static void biginfo_recv(esp_ble_iso_gap_app_event_t *event)
{
    esp_ble_iso_big_sync_param_t param = {0};
    int err;

    if (big_synced == false) {
        param.bis_channels = bis;
        param.num_bis = BIS_ISO_CHAN_COUNT;
        param.bis_bitfield = BIT_MASK(BIS_ISO_CHAN_COUNT);
        param.mse = event->biginfo_recv.nse;
        param.sync_timeout = BIG_SYNC_TIMEOUT;
        param.encryption = event->biginfo_recv.encryption;
        if (param.encryption) {
            memcpy(param.bcode, TARGET_BROADCAST_CODE,
                   MIN(ESP_BLE_ISO_BROADCAST_CODE_SIZE, strlen(TARGET_BROADCAST_CODE)));
        }

        err = esp_ble_iso_big_sync(event->biginfo_recv.sync_handle, &param, &out_big);
        if (err) {
            ESP_LOGE(TAG, "Failed to create BIG sync, err %d", err);
            return;
        }

        big_synced = true;
    }
}

static void iso_gap_app_cb(esp_ble_iso_gap_app_event_t *event)
{
    switch (event->type) {
    case ESP_BLE_ISO_GAP_EVENT_EXT_SCAN_RECV:
        ext_scan_recv(event);
        break;
    case ESP_BLE_ISO_GAP_EVENT_PA_SYNC:
        pa_sync(event);
        break;
    case ESP_BLE_ISO_GAP_EVENT_PA_SYNC_LOST:
        pa_sync_lost(event);
        break;
    case ESP_BLE_ISO_GAP_EVENT_BIGINFO_RECV:
        biginfo_recv(event);
        break;
    default:
        break;
    }
}

void app_main(void)
{
    esp_ble_iso_init_info_t info = {
        .gap_cb = iso_gap_app_cb,
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

    err = esp_ble_iso_common_init(&info);
    if (err) {
        ESP_LOGE(TAG, "Failed to initialize ISO, err %d", err);
        return;
    }

    err = ext_scan_start();
    if (err) {
        ESP_LOGE(TAG, "Failed to start scan, err %d", err);
        return;
    }
}
