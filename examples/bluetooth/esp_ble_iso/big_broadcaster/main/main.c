/*
 * SPDX-FileCopyrightText: 2021-2022 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_ble_iso_common_api.h"

#include "ble_iso_example_init.h"
#include "ble_iso_example_utils.h"

#include "adv.h"

#define LOCAL_DEVICE_NAME       "BIG Broadcaster"
#define LOCAL_DEVICE_NAME_LEN   (sizeof(LOCAL_DEVICE_NAME) - 1)
#define LOCAL_BROADCAST_CODE    "1234"  /* Maximum length is 16 */

#define BIG_SDU_INTERVAL_US     10000   /* 10ms */
#define BIG_LATENCY_MS          10      /* 10ms */
#define BIG_PHY                 ESP_BLE_ISO_PHY_2M
#define BIG_RTN                 2
#define BIG_PACKING             0       /* 0 - sequential, 1 - interleaved */
#define BIG_FRAMING             0       /* 0 - unframed, 1 - framed */
#define BIS_ISO_CHAN_COUNT      2       /* Use exactly 2 BIS channels */
#define BIS_SDU_SIZE            120
#define BIG_ENCRYPTION          true

_Static_assert(CONFIG_BT_ISO_MAX_CHAN >= BIS_ISO_CHAN_COUNT,
               "CONFIG_BT_ISO_MAX_CHAN must be >= BIS_ISO_CHAN_COUNT");

static uint8_t ext_adv_data[3 + 2 + LOCAL_DEVICE_NAME_LEN];
static uint8_t per_adv_data[LOCAL_DEVICE_NAME_LEN];

static esp_ble_iso_big_t *out_big;

static size_t connected_bis_count;

static struct iso_chan_tx {
    int chan_idx;
    uint16_t seq_num;
    example_iso_tx_count_t tx_count;
    uint8_t data[BIS_SDU_SIZE];
} chan_tx[BIS_ISO_CHAN_COUNT];

/* One scheduler for the BIG: each tick sends one SDU on every BIS. */
static example_iso_tx_scheduler_t tx_scheduler;
#if CONFIG_EXAMPLE_ISO_TX_TIMESTAMP
static example_iso_tx_sync_t tx_sync;
#endif /* CONFIG_EXAMPLE_ISO_TX_TIMESTAMP */

static int bis_chan_index_get(const esp_ble_iso_chan_t *chan);
#if !CONFIG_EXAMPLE_ISO_TX_TIMESTAMP
static int iso_chan_send(int chan_idx);
#endif /* !CONFIG_EXAMPLE_ISO_TX_TIMESTAMP */

static void iso_connected_cb(esp_ble_iso_chan_t *chan)
{
    const esp_ble_iso_chan_path_t data_path = {
        .pid = ESP_BLE_ISO_DATA_PATH_HCI,
        .format = ESP_BLE_ISO_CODING_FORMAT_TRANSPARENT,
    };
    int chan_idx = bis_chan_index_get(chan);
    esp_err_t err;

    ESP_LOGI(TAG, "[BIS #%d] Connected", chan_idx);

    err = esp_ble_iso_setup_data_path(chan, ESP_BLE_ISO_DATA_PATH_DIR_INPUT, &data_path);
    if (err) {
        ESP_LOGE(TAG, "[BIS #%d] Failed to setup data path, err %d", chan_idx, err);
        return;
    }

    chan_tx[chan_idx].tx_count = (example_iso_tx_count_t){0};

#if CONFIG_EXAMPLE_ISO_TX_TIMESTAMP
    /* Sends from the first tick, once the whole BIG is connected. */
    err = example_iso_tx_sync_start(&tx_sync, chan, BIG_SDU_INTERVAL_US);
    if (err) {
        ESP_LOGE(TAG, "[BIS #%d] Timestamp TX start failed, err %d", chan_idx, err);
    }
#endif /* CONFIG_EXAMPLE_ISO_TX_TIMESTAMP */

    connected_bis_count++;

    if (connected_bis_count < BIS_ISO_CHAN_COUNT) {
        ESP_LOGI(TAG, "Waiting for remaining BIS channels (%u/%u)",
                 connected_bis_count, BIS_ISO_CHAN_COUNT);
        return;
    }

    ESP_LOGI(TAG, "All %u BIS channels connected, starting TX %s timestamps", BIS_ISO_CHAN_COUNT,
             IS_ENABLED(CONFIG_EXAMPLE_ISO_TX_TIMESTAMP) ? "with" : "without");

    example_iso_tx_scheduler_reset(&tx_scheduler);

    /* Note: esp timer is not accurate enough */
    err = example_iso_tx_scheduler_start(&tx_scheduler, BIG_SDU_INTERVAL_US);
    if (err) {
        ESP_LOGE(TAG, "Scheduler start failed, err %d", err);
        return;
    }

#if !CONFIG_EXAMPLE_ISO_TX_TIMESTAMP
    for (size_t i = 0; i < BIS_ISO_CHAN_COUNT; i++) {
        chan_tx[i].seq_num = 0;
        iso_chan_send((int)i);
    }
#endif /* !CONFIG_EXAMPLE_ISO_TX_TIMESTAMP */
}

static void iso_disconnected_cb(esp_ble_iso_chan_t *chan, uint8_t reason)
{
    ESP_LOGI(TAG, "[BIS #%d] Disconnected, reason 0x%02x",
             bis_chan_index_get(chan), reason);

    if (connected_bis_count > 0) {
        connected_bis_count--;
    }

#if CONFIG_EXAMPLE_ISO_TX_TIMESTAMP
    example_iso_tx_sync_stop(&tx_sync, chan);
#endif /* CONFIG_EXAMPLE_ISO_TX_TIMESTAMP */

    if (connected_bis_count == 0) {
        esp_err_t err;

        ESP_LOGI(TAG, "All BIS channels disconnected, TX stopped");

        err = example_iso_tx_scheduler_stop(&tx_scheduler);
        if (err) {
            ESP_LOGE(TAG, "Scheduler stop failed, err %d", err);
        }

        out_big = NULL;
    }
}

static void iso_sent_cb(esp_ble_iso_chan_t *chan, void *user_data)
{
    int chan_idx = bis_chan_index_get(chan);
    char name[24];

    if (chan_idx < 0) {
        ESP_LOGW(TAG, "Unknown BIS channel");
        return;
    }

    snprintf(name, sizeof(name), "BIS #%d", chan_idx);
    example_iso_tx_scheduler_on_sent(&tx_scheduler, user_data, &chan_tx[chan_idx].tx_count,
                                     TAG, name);
}

static esp_ble_iso_chan_ops_t iso_ops = {
    .connected    = iso_connected_cb,
    .disconnected = iso_disconnected_cb,
    .sent         = iso_sent_cb,
};

static esp_ble_iso_chan_io_qos_t iso_tx_qos = {
    .sdu = BIS_SDU_SIZE,
    .rtn = BIG_RTN,
    .phy = BIG_PHY,
};

static esp_ble_iso_chan_qos_t bis_iso_qos = {
    .tx = &iso_tx_qos,
};

static esp_ble_iso_chan_t bis_iso_chan[] = {
    { .ops = &iso_ops, .qos = &bis_iso_qos, },
    { .ops = &iso_ops, .qos = &bis_iso_qos, },
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

static void big_create(void)
{
    esp_ble_iso_big_create_param_t param = {0};
    esp_ble_iso_ext_adv_info_t info = {
        .adv_handle = ADV_HANDLE,
    };
    size_t bcode_len = 0;
    int err;

    err = esp_ble_iso_big_ext_adv_add(&info);
    if (err) {
        ESP_LOGE(TAG, "Failed to add ext adv for BIG, err %d", err);
        return;
    }

    param.bis_channels = bis;
    param.num_bis = BIS_ISO_CHAN_COUNT;
    param.interval = BIG_SDU_INTERVAL_US;
    param.latency = BIG_LATENCY_MS;
    param.packing = BIG_PACKING;
    param.framing = BIG_FRAMING;
    param.encryption = BIG_ENCRYPTION;
    if (param.encryption) {
        memset(param.bcode, 0, ESP_BLE_ISO_BROADCAST_CODE_SIZE);

        bcode_len = strlen(LOCAL_BROADCAST_CODE);
        if (bcode_len > 0) {
            memcpy(param.bcode, LOCAL_BROADCAST_CODE,
                   MIN((size_t)ESP_BLE_ISO_BROADCAST_CODE_SIZE, bcode_len));
        }
    }

    err = esp_ble_iso_big_create(ADV_HANDLE, &param, &out_big);
    if (err) {
        ESP_LOGE(TAG, "Failed to create BIG, err %d", err);
        return;
    }
}

#if !CONFIG_EXAMPLE_ISO_TX_TIMESTAMP
static int iso_chan_send(int chan_idx)
{
    int err;

    if (chan_idx < 0 || chan_idx >= BIS_ISO_CHAN_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }

    if (bis_iso_chan[chan_idx].iso == NULL) {
        ESP_LOGW(TAG, "[BIS #%d] No channel to transmit", chan_idx);
        return ESP_ERR_INVALID_STATE;
    }

    memset(chan_tx[chan_idx].data,
           (int)chan_tx[chan_idx].seq_num,
           sizeof(chan_tx[chan_idx].data));

    err = esp_ble_iso_chan_send(&bis_iso_chan[chan_idx],
                                chan_tx[chan_idx].data,
                                sizeof(chan_tx[chan_idx].data),
                                chan_tx[chan_idx].seq_num);
    if (err) {
        ESP_LOGD(TAG, "[BIS #%d] send failed, err %d", chan_idx, err);
        return err;
    }

    chan_tx[chan_idx].seq_num++;

    return 0;
}
#else /* !CONFIG_EXAMPLE_ISO_TX_TIMESTAMP */
static int tx_send_cb(esp_ble_iso_chan_t *chan, uint16_t seq_num,
                      uint32_t timestamp, bool ts_valid)
{
    struct iso_chan_tx *tx = &chan_tx[bis_chan_index_get(chan)];
    int err;

    memset(tx->data, (uint8_t)seq_num, sizeof(tx->data));
    /* The timestamp, 0 without one, tells the receiver which SDUs of the
     * channels belong together, e.g. the left and right audio of one frame.
     */
    if (!ts_valid) {
        timestamp = 0;
    }
    memcpy(tx->data, &timestamp, sizeof(timestamp));

    if (ts_valid) {
        err = esp_ble_iso_chan_send_ts(chan, tx->data, sizeof(tx->data), seq_num, timestamp);
    } else {
        err = esp_ble_iso_chan_send(chan, tx->data, sizeof(tx->data), seq_num);
    }
    if (err) {
        ESP_LOGW(TAG, "[BIS #%d] Send failed, err %d", tx->chan_idx, err);
    }

    return err;
}
#endif /* !CONFIG_EXAMPLE_ISO_TX_TIMESTAMP */

static void tx_scheduler_cb(void *arg)
{
#if CONFIG_EXAMPLE_ISO_TX_TIMESTAMP
    example_iso_tx_sync_send_next(&tx_sync);
#else /* CONFIG_EXAMPLE_ISO_TX_TIMESTAMP */
    for (size_t i = 0; i < BIS_ISO_CHAN_COUNT; i++) {
        iso_chan_send((int)i);
    }
#endif /* CONFIG_EXAMPLE_ISO_TX_TIMESTAMP */
}

static void build_ext_adv_data(void)
{
    ext_adv_data[0] = 0x02;
    ext_adv_data[1] = EXAMPLE_AD_TYPE_FLAGS;
    ext_adv_data[2] = EXAMPLE_AD_FLAGS_GENERAL | EXAMPLE_AD_FLAGS_NO_BREDR;
    ext_adv_data[3] = (uint8_t)(LOCAL_DEVICE_NAME_LEN + 1); /* AD type + name */
    ext_adv_data[4] = EXAMPLE_AD_TYPE_NAME_COMPLETE;
    memcpy(&ext_adv_data[5], LOCAL_DEVICE_NAME, LOCAL_DEVICE_NAME_LEN);

    memcpy(per_adv_data, LOCAL_DEVICE_NAME, LOCAL_DEVICE_NAME_LEN);
}

void app_main(void)
{
    esp_ble_iso_init_info_t info = {0};
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

    for (size_t i = 0; i < BIS_ISO_CHAN_COUNT; i++) {
        chan_tx[i].chan_idx = (int)i;
    }

#if CONFIG_EXAMPLE_ISO_TX_TIMESTAMP
    tx_sync.send = tx_send_cb;
#endif /* CONFIG_EXAMPLE_ISO_TX_TIMESTAMP */
    err = example_iso_tx_scheduler_init(&tx_scheduler, tx_scheduler_cb, NULL);
    if (err) {
        ESP_LOGE(TAG, "Scheduler init failed, err %d", err);
        return;
    }

    build_ext_adv_data();

    err = ext_adv_start(ext_adv_data, sizeof(ext_adv_data),
                        per_adv_data, sizeof(per_adv_data));
    if (err) {
        return;
    }

    big_create();
}
