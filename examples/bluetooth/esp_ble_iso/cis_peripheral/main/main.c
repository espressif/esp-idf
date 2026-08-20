/*
 * SPDX-FileCopyrightText: 2021-2022 Nordic Semiconductor ASA
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

#include "esp_ble_iso_common_api.h"

#include "ble_iso_example_init.h"
#include "ble_iso_example_utils.h"

#include "peripheral.h"

#define CIS_SDU_SIZE            120
#define CIS_COUNT               2

static uint8_t ext_adv_data[3 + 2 + LOCAL_DEVICE_NAME_LEN];

/* Each CIS is received and reported independently, so it keeps its own metrics. */
struct cis_ctx {
    esp_ble_iso_chan_t       chan;
    example_iso_rx_metrics_t rx_metrics;
    char                     name[8];
};

static struct cis_ctx cis_ctxs[CIS_COUNT];

static struct cis_ctx *ctx_by_chan(const esp_ble_iso_chan_t *chan)
{
    for (size_t i = 0; i < ARRAY_SIZE(cis_ctxs); i++) {
        if (&cis_ctxs[i].chan == chan) {
            return &cis_ctxs[i];
        }
    }

    return NULL;
}

static void iso_connected_cb(esp_ble_iso_chan_t *chan)
{
    const esp_ble_iso_chan_path_t data_path = {
        .pid = ESP_BLE_ISO_DATA_PATH_HCI,
        .format = ESP_BLE_ISO_CODING_FORMAT_TRANSPARENT,
    };
    struct cis_ctx *ctx = ctx_by_chan(chan);
    esp_err_t err;

    ESP_LOGI(TAG, "[%s] Connected", ctx->name);

    err = esp_ble_iso_setup_data_path(chan, ESP_BLE_ISO_DATA_PATH_DIR_OUTPUT, &data_path);
    if (err) {
        ESP_LOGE(TAG, "[%s] Failed to setup data path, err %d", ctx->name, err);
        return;
    }

    example_iso_rx_metrics_reset(&ctx->rx_metrics);
}

static void iso_disconnected_cb(esp_ble_iso_chan_t *chan, uint8_t reason)
{
    struct cis_ctx *ctx = ctx_by_chan(chan);

    ESP_LOGI(TAG, "[%s] Disconnected, reason 0x%02x", ctx->name, reason);
}

static void iso_recv_cb(esp_ble_iso_chan_t *chan,
                        const esp_ble_iso_recv_info_t *info,
                        const uint8_t *data, uint16_t len)
{
    struct cis_ctx *ctx = ctx_by_chan(chan);

    ctx->rx_metrics.last_sdu_len = len;
    example_iso_rx_metrics_on_recv(info, &ctx->rx_metrics, TAG, ctx->name);
}

static esp_ble_iso_chan_ops_t iso_ops = {
    .connected    = iso_connected_cb,
    .disconnected = iso_disconnected_cb,
    .recv         = iso_recv_cb,
};

static esp_ble_iso_chan_io_qos_t iso_rx = {
    .sdu = CIS_SDU_SIZE,
};

static esp_ble_iso_chan_qos_t iso_qos = {
    .rx = &iso_rx,
    .tx = NULL,
};

static int iso_accept(const esp_ble_iso_accept_info_t *info,
                      esp_ble_iso_chan_t **chan)
{
    ESP_LOGI(TAG, "Incoming CIS request from handle %u", info->acl->handle);

    /* The central establishes the CIS one at a time, so hand out the next
     * channel that is not already carrying one. */
    for (size_t i = 0; i < ARRAY_SIZE(cis_ctxs); i++) {
        if (cis_ctxs[i].chan.iso == NULL) {
            *chan = &cis_ctxs[i].chan;
            return 0;
        }
    }

    ESP_LOGE(TAG, "No channels available");

    return -ENOMEM;
}

static esp_ble_iso_server_t iso_server = {
    .accept = iso_accept,
};

static void acl_connect(esp_ble_iso_gap_app_event_t *event)
{
    if (event->acl_connect.status) {
        ESP_LOGE(TAG, "Connection failed, status %d", event->acl_connect.status);
        return;
    }

    ESP_LOGI(TAG, "Connected: handle %u role %u peer %02x:%02x:%02x:%02x:%02x:%02x",
             event->acl_connect.conn_handle, event->acl_connect.role,
             EXAMPLE_BT_ADDR_PRINT_ARGS(event->acl_connect.dst.val));
}

static void acl_disconnect(esp_ble_iso_gap_app_event_t *event)
{
    ESP_LOGI(TAG, "Disconnected: handle %u reason 0x%02x",
             event->acl_disconnect.conn_handle, event->acl_disconnect.reason);

    ext_adv_start(ext_adv_data, sizeof(ext_adv_data));
}

static void security_change(esp_ble_iso_gap_app_event_t *event)
{
    if (event->security_change.status) {
        ESP_LOGE(TAG, "Security change failed, status %d", event->security_change.status);
        return;
    }

    ESP_LOGI(TAG, "Security: handle %u level %u bonded %u",
             event->security_change.conn_handle,
             event->security_change.sec_level,
             event->security_change.bonded);
}

static void iso_gap_app_cb(esp_ble_iso_gap_app_event_t *event)
{
    switch (event->type) {
    case ESP_BLE_ISO_GAP_EVENT_ACL_CONNECT:
        acl_connect(event);
        break;
    case ESP_BLE_ISO_GAP_EVENT_ACL_DISCONNECT:
        acl_disconnect(event);
        break;
    case ESP_BLE_ISO_GAP_EVENT_SECURITY_CHANGE:
        security_change(event);
        break;
    default:
        break;
    }
}

static void build_ext_adv_data(void)
{
    ext_adv_data[0] = 0x02;
    ext_adv_data[1] = EXAMPLE_AD_TYPE_FLAGS;
    ext_adv_data[2] = EXAMPLE_AD_FLAGS_GENERAL | EXAMPLE_AD_FLAGS_NO_BREDR;
    ext_adv_data[3] = (uint8_t)(LOCAL_DEVICE_NAME_LEN + 1); /* AD type + name */
    ext_adv_data[4] = EXAMPLE_AD_TYPE_NAME_COMPLETE;
    memcpy(&ext_adv_data[5], LOCAL_DEVICE_NAME, LOCAL_DEVICE_NAME_LEN);
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

    /* All CIS share the same callbacks and RX QoS; they differ only by which
     * channel iso_accept() hands out and where its metrics are kept. */
    for (size_t i = 0; i < ARRAY_SIZE(cis_ctxs); i++) {
        cis_ctxs[i].chan.ops = &iso_ops;
        cis_ctxs[i].chan.qos = &iso_qos;
        snprintf(cis_ctxs[i].name, sizeof(cis_ctxs[i].name), "CIS #%zu", i);
    }

    err = esp_ble_iso_server_register(&iso_server);
    if (err) {
        ESP_LOGE(TAG, "Failed to register ISO server, err %d", err);
        return;
    }

    err = set_device_name();
    if (err) {
        ESP_LOGE(TAG, "Failed to set device name, err %d", err);
        return;
    }

    build_ext_adv_data();

    ext_adv_start(ext_adv_data, sizeof(ext_adv_data));
}
