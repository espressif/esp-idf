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
    uint8_t data[BIS_SDU_SIZE];
    example_iso_tx_scheduler_t scheduler;
} chan_tx[BIS_ISO_CHAN_COUNT];

static void iso_chan_send(int chan_idx);
static int bis_chan_index_get(const esp_ble_iso_chan_t *chan);

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

    connected_bis_count++;

    if (connected_bis_count < BIS_ISO_CHAN_COUNT) {
        ESP_LOGI(TAG, "Waiting for remaining BIS channels (%u/%u)",
                 connected_bis_count, BIS_ISO_CHAN_COUNT);
        return;
    }

    ESP_LOGI(TAG, "All %u BIS channels connected, starting TX", BIS_ISO_CHAN_COUNT);

    /* Note: esp timer is not accurate enough */
    for (size_t i = 0; i < BIS_ISO_CHAN_COUNT; i++) {
        chan_tx[i].seq_num = 0;
        example_iso_tx_scheduler_reset(&chan_tx[i].scheduler);

        err = example_iso_tx_scheduler_start(&chan_tx[i].scheduler, BIG_SDU_INTERVAL_US);
        if (err) {
            ESP_LOGE(TAG, "[BIS #%zu] Scheduler start failed, err %d", i, err);
            continue;
        }

        iso_chan_send((int)i);
    }
}

static void iso_disconnected_cb(esp_ble_iso_chan_t *chan, uint8_t reason)
{
    ESP_LOGI(TAG, "[BIS #%d] Disconnected, reason 0x%02x",
             bis_chan_index_get(chan), reason);

    if (connected_bis_count > 0) {
        connected_bis_count--;
    }

    if (connected_bis_count == 0) {
        esp_err_t err;

        ESP_LOGI(TAG, "All BIS channels disconnected, TX stopped");

        for (size_t i = 0; i < BIS_ISO_CHAN_COUNT; i++) {
            err = example_iso_tx_scheduler_stop(&chan_tx[i].scheduler);
            if (err) {
                ESP_LOGE(TAG, "[BIS #%zu] Scheduler stop failed, err %d", i, err);
            }
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
    example_iso_tx_scheduler_on_sent(&chan_tx[chan_idx].scheduler,
                                     user_data, TAG, name);
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

static void iso_chan_send(int chan_idx)
{
    int err;

    if (chan_idx < 0 || chan_idx >= BIS_ISO_CHAN_COUNT) {
        return;
    }

    if (bis_iso_chan[chan_idx].iso == NULL) {
        ESP_LOGW(TAG, "[BIS #%d] No channel to transmit", chan_idx);
        return;
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
        return;
    }

    chan_tx[chan_idx].seq_num++;
}

static void tx_scheduler_cb(void *arg)
{
    struct iso_chan_tx *tx = arg;

    if (tx == NULL) {
        return;
    }

    iso_chan_send(tx->chan_idx);
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
        err = example_iso_tx_scheduler_init(&chan_tx[i].scheduler,
                                            tx_scheduler_cb,
                                            &chan_tx[i]);
        if (err) {
            ESP_LOGE(TAG, "[BIS #%zu] Scheduler init failed, err %d", i, err);
            return;
        }
    }

    build_ext_adv_data();

    err = ext_adv_start(ext_adv_data, sizeof(ext_adv_data),
                        per_adv_data, sizeof(per_adv_data));
    if (err) {
        return;
    }

    big_create();
}
