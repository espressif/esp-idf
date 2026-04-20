/*
 * SPDX-FileCopyrightText: 2021-2022 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_system.h"

#include "host/ble_gap.h"
#include "services/gap/ble_svc_gap.h"

#include "esp_ble_iso_common_api.h"

#include "ble_iso_example_init.h"
#include "ble_iso_example_utils.h"

#define TAG "CIS_PER"

#define LOCAL_DEVICE_NAME       "CIS Peripheral"
#define LOCAL_DEVICE_NAME_LEN   (sizeof(LOCAL_DEVICE_NAME) - 1)

#define ADV_HANDLE              0
#define ADV_SID                 0
#define ADV_TX_POWER            127
#define ADV_ADDRESS             BLE_OWN_ADDR_PUBLIC
#define ADV_PRIMARY_PHY         BLE_HCI_LE_PHY_1M
#define ADV_SECONDARY_PHY       BLE_HCI_LE_PHY_2M
#define ADV_INTERVAL            BLE_GAP_ADV_ITVL_MS(200)

#define SECURITY_LEVEL          ESP_BLE_ISO_SECURITY_NO_MITM

#define CIS_SDU_SIZE            120

static uint8_t ext_adv_data[3 + 2 + LOCAL_DEVICE_NAME_LEN];

static example_iso_rx_metrics_t rx_metrics;

static void iso_connected_cb(esp_ble_iso_chan_t *chan)
{
    const esp_ble_iso_chan_path_t data_path = {
        .pid = ESP_BLE_ISO_DATA_PATH_HCI,
        .format = ESP_BLE_ISO_CODING_FORMAT_TRANSPARENT,
    };
    esp_err_t err;

    ESP_LOGI(TAG, "[CIS #0] Connected");

    err = esp_ble_iso_setup_data_path(chan, ESP_BLE_ISO_DATA_PATH_DIR_OUTPUT, &data_path);
    if (err) {
        ESP_LOGE(TAG, "[CIS #0] Failed to setup data path, err %d", err);
        return;
    }

    example_iso_rx_metrics_reset(&rx_metrics);
}

static void iso_disconnected_cb(esp_ble_iso_chan_t *chan, uint8_t reason)
{
    ESP_LOGI(TAG, "[CIS #0] Disconnected, reason 0x%02x", reason);
}

static void iso_recv_cb(esp_ble_iso_chan_t *chan,
                        const esp_ble_iso_recv_info_t *info,
                        const uint8_t *data, uint16_t len)
{
    rx_metrics.last_sdu_len = len;
    example_iso_rx_metrics_on_recv(info, &rx_metrics, TAG, "CIS #0");
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

static esp_ble_iso_chan_t iso_chan = {
    .ops = &iso_ops,
    .qos = &iso_qos,
};

static int iso_accept(const esp_ble_iso_accept_info_t *info,
                      esp_ble_iso_chan_t **chan)
{
    ESP_LOGI(TAG, "Incoming CIS request from handle %u", info->acl->handle);

    if (iso_chan.iso) {
        ESP_LOGE(TAG, "No channels available");
        return -ENOMEM;
    }

    *chan = &iso_chan;

    return 0;
}

static esp_ble_iso_server_t iso_server = {
    .sec_level = SECURITY_LEVEL,
    .accept = iso_accept,
};

static void build_adv_data(void)
{
    ext_adv_data[0] = 0x02;
    ext_adv_data[1] = EXAMPLE_AD_TYPE_FLAGS;
    ext_adv_data[2] = EXAMPLE_AD_FLAGS_GENERAL | EXAMPLE_AD_FLAGS_NO_BREDR;
    ext_adv_data[3] = (uint8_t)(LOCAL_DEVICE_NAME_LEN + 1);
    ext_adv_data[4] = EXAMPLE_AD_TYPE_NAME_COMPLETE;
    memcpy(&ext_adv_data[5], LOCAL_DEVICE_NAME, LOCAL_DEVICE_NAME_LEN);
}

static void ext_adv_start(void)
{
    struct ble_gap_ext_adv_params ext_params = {0};
    struct os_mbuf *data = NULL;
    int err;

    build_adv_data();

    ext_params.connectable = 1;
    ext_params.scannable = 0;
    ext_params.legacy_pdu = 0;
    ext_params.own_addr_type = ADV_ADDRESS;
    ext_params.primary_phy = ADV_PRIMARY_PHY;
    ext_params.secondary_phy = ADV_SECONDARY_PHY;
    ext_params.tx_power = ADV_TX_POWER;
    ext_params.sid = ADV_SID;
    ext_params.itvl_min = ADV_INTERVAL;
    ext_params.itvl_max = ADV_INTERVAL;

    err = ble_gap_ext_adv_configure(ADV_HANDLE, &ext_params, NULL,
                                    example_iso_gap_event_cb, NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to configure ext adv params, err %d", err);
        return;
    }

    data = os_msys_get_pkthdr(sizeof(ext_adv_data), 0);
    if (data == NULL) {
        ESP_LOGE(TAG, "Failed to get ext adv mbuf");
        return;
    }

    err = os_mbuf_append(data, ext_adv_data, sizeof(ext_adv_data));
    if (err) {
        ESP_LOGE(TAG, "Failed to append ext adv data, err %d", err);
        os_mbuf_free_chain(data);
        return;
    }

    err = ble_gap_ext_adv_set_data(ADV_HANDLE, data);
    if (err) {
        ESP_LOGE(TAG, "Failed to set ext adv data, err %d", err);
        return;
    }

    err = ble_gap_ext_adv_start(ADV_HANDLE, 0, 0);
    if (err) {
        ESP_LOGE(TAG, "Failed to start ext advertising, err %d", err);
        return;
    }

    ESP_LOGI(TAG, "Advertising started (handle %u)", ADV_HANDLE);
}

static void acl_connect(esp_ble_iso_gap_app_event_t *event)
{
    if (event->acl_connect.status) {
        ESP_LOGE(TAG, "Connection failed, status %d", event->acl_connect.status);
        return;
    }

    ESP_LOGI(TAG, "Connected: handle %u role %u peer %02x:%02x:%02x:%02x:%02x:%02x",
             event->acl_connect.conn_handle, event->acl_connect.role,
             event->acl_connect.dst.val[5], event->acl_connect.dst.val[4],
             event->acl_connect.dst.val[3], event->acl_connect.dst.val[2],
             event->acl_connect.dst.val[1], event->acl_connect.dst.val[0]);
}

static void acl_disconnect(esp_ble_iso_gap_app_event_t *event)
{
    ESP_LOGI(TAG, "Disconnected: handle %u reason 0x%02x",
             event->acl_disconnect.conn_handle, event->acl_disconnect.reason);

    ext_adv_start();
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

    err = esp_ble_iso_common_init(&info);
    if (err) {
        ESP_LOGE(TAG, "Failed to initialize ISO, err %d", err);
        return;
    }

    err = esp_ble_iso_server_register(&iso_server);
    if (err) {
        ESP_LOGE(TAG, "Failed to register ISO server, err %d", err);
        return;
    }

    err = ble_svc_gap_device_name_set(LOCAL_DEVICE_NAME);
    if (err) {
        ESP_LOGE(TAG, "Failed to set device name, err %d", err);
        return;
    }

    ext_adv_start();
}
