/*
 * SPDX-FileCopyrightText: 2021-2022 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <assert.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_ble_iso_common_api.h"

#include "ble_iso_example_init.h"
#include "ble_iso_example_utils.h"

#include "central.h"

#define TARGET_DEVICE_NAME      "CIS Peripheral"
#define TARGET_DEVICE_NAME_LEN  (sizeof(TARGET_DEVICE_NAME) - 1)

#define SECURITY_LEVEL          ESP_BLE_ISO_SECURITY_NO_MITM

#define CIG_LATENCY_MS          10      /* 10ms */
#define CIG_SDU_INTERVAL_US     10000   /* 10ms */
#define CIG_SCA                 ESP_BLE_ISO_SCA_UNKNOWN
#define CIG_PACKING             0       /* 0 - sequential, 1 - interleaved */
#define CIG_FRAMING             0       /* 0 - unframed, 1 - framed */
#define CIS_PHY                 ESP_BLE_ISO_PHY_2M
#define CIS_RTN                 2
#define CIS_SDU_SIZE            120

static bool acl_connected;

static uint16_t iso_seq_num;
static uint8_t iso_data[CIS_SDU_SIZE];

static esp_ble_iso_cig_t *out_cig;

static example_iso_tx_scheduler_t tx_scheduler;

static void iso_chan_send(void);

static void iso_connected_cb(esp_ble_iso_chan_t *chan)
{
    const esp_ble_iso_chan_path_t data_path = {
        .pid    = ESP_BLE_ISO_DATA_PATH_HCI,
        .format = ESP_BLE_ISO_CODING_FORMAT_TRANSPARENT,
    };
    esp_err_t err;

    ESP_LOGI(TAG, "[CIS #0] Connected");

    err = esp_ble_iso_setup_data_path(chan, ESP_BLE_ISO_DATA_PATH_DIR_INPUT, &data_path);
    if (err) {
        ESP_LOGE(TAG, "[CIS #0] Failed to setup data path, err %d", err);
        return;
    }

    iso_seq_num = 0;
    example_iso_tx_scheduler_reset(&tx_scheduler);

    /* Note: esp timer is not accurate enough */
    err = example_iso_tx_scheduler_start(&tx_scheduler, CIG_SDU_INTERVAL_US);
    if (err) {
        ESP_LOGE(TAG, "[CIS #0] Scheduler start failed, err %d", err);
        return;
    }

    iso_chan_send();
}

static void iso_disconnected_cb(esp_ble_iso_chan_t *chan, uint8_t reason)
{
    esp_err_t err;

    ESP_LOGI(TAG, "[CIS #0] Disconnected, reason 0x%02x", reason);

    err = example_iso_tx_scheduler_stop(&tx_scheduler);
    if (err) {
        ESP_LOGE(TAG, "[CIS #0] Scheduler stop failed, err %d", err);
    }

    /* Per BT Core 6.0 §7.7.5: a CIS on the Central retains its handle and
     * data path across HCI_Disconnection_Complete (only the Peripheral side
     * and other connection types get auto-deleted by the controller). So on
     * the Central we must remove explicitly, otherwise the next setup on
     * reconnect returns Command Disallowed.
     */
    err = esp_ble_iso_remove_data_path(chan, ESP_BLE_ISO_DATA_PATH_DIR_INPUT);
    if (err) {
        ESP_LOGE(TAG, "[CIS #0] Failed to remove data path, err %d", err);
    }
}

static void iso_sent_cb(esp_ble_iso_chan_t *chan, void *user_data)
{
    example_iso_tx_scheduler_on_sent(&tx_scheduler, user_data, TAG, "CIS #0");
}

static esp_ble_iso_chan_ops_t iso_ops = {
    .connected    = iso_connected_cb,
    .disconnected = iso_disconnected_cb,
    .sent         = iso_sent_cb,
};

static esp_ble_iso_chan_io_qos_t iso_tx_qos = {
    .sdu = CIS_SDU_SIZE,
    .phy = CIS_PHY,
    .rtn = CIS_RTN,
};

static esp_ble_iso_chan_qos_t iso_qos = {
    .tx = &iso_tx_qos,
    .rx = NULL,
};

static esp_ble_iso_chan_t iso_chan = {
    .ops = &iso_ops,
    .qos = &iso_qos,
    .required_sec_level = SECURITY_LEVEL,
};

static void create_cig_and_cis(uint16_t acl_handle)
{
    esp_ble_iso_connect_param_t connect_param = {0};
    esp_ble_iso_cig_param_t cig_param = {0};
    esp_ble_iso_chan_t *channels[1] = {0};
    int err;

    if (out_cig) {
        goto connect;
    }

    channels[0] = &iso_chan;

    cig_param.cis_channels = channels;
    cig_param.num_cis = ARRAY_SIZE(channels);
    cig_param.sca = CIG_SCA;
    cig_param.packing = CIG_PACKING;
    cig_param.framing = CIG_FRAMING;
    cig_param.c_to_p_latency = CIG_LATENCY_MS;
    cig_param.p_to_c_latency = CIG_LATENCY_MS;
    cig_param.c_to_p_interval = CIG_SDU_INTERVAL_US;
    cig_param.p_to_c_interval = CIG_SDU_INTERVAL_US;

    err = esp_ble_iso_cig_create(&cig_param, &out_cig);
    if (err) {
        ESP_LOGE(TAG, "Failed to create CIG, err %d", err);
        return;
    }

connect:
    connect_param.iso_chan = &iso_chan;

    err = esp_ble_iso_chan_connect(&connect_param, acl_handle, 1);
    if (err) {
        ESP_LOGE(TAG, "Failed to create CIS, err %d", err);
        return;
    }
}

static void iso_chan_send(void)
{
    int err;

    memset(iso_data, (uint8_t)iso_seq_num, sizeof(iso_data));

    err = esp_ble_iso_chan_send(&iso_chan,
                                iso_data,
                                sizeof(iso_data),
                                iso_seq_num);
    if (err) {
        ESP_LOGD(TAG, "[CIS #0] Send failed, err %d", err);
        return;
    }

    iso_seq_num++;
}

static void tx_scheduler_cb(void *arg)
{
    (void)arg;
    iso_chan_send();
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

    /* Skip if target not found or ACL already connected */
    if (!target_matched || acl_connected) {
        return;
    }

    err = ext_scan_stop();
    if (err) {
        ESP_LOGE(TAG, "Failed to stop scanning, err %d", err);
        return;
    }

    err = conn_create(event->ext_scan_recv.addr.type, event->ext_scan_recv.addr.val);
    if (err) {
        ESP_LOGE(TAG, "Failed to create connection, err %d", err);
        /* Scan was stopped above. acl_disconnect (which restarts scan) only
         * fires on an established connection — resume here to avoid a
         * no-scan / no-conn dead state. */
        ext_scan_start();
        return;
    }

    acl_connected = true;
}

static void acl_connect(esp_ble_iso_gap_app_event_t *event)
{
    int err;

    if (event->acl_connect.status) {
        ESP_LOGE(TAG, "Connection failed, status %d", event->acl_connect.status);
        acl_connected = false;
        /* Async failure: acl_disconnect won't fire (never established),
         * so restart scanning here to avoid a no-scan / no-conn dead state. */
        ext_scan_start();
        return;
    }

    ESP_LOGI(TAG, "Connected: handle %u role %u peer %02x:%02x:%02x:%02x:%02x:%02x",
             event->acl_connect.conn_handle, event->acl_connect.role,
             EXAMPLE_BT_ADDR_PRINT_ARGS(event->acl_connect.dst.val));

    if (iso_chan.required_sec_level == ESP_BLE_ISO_SECURITY_NO_MITM ||
            iso_chan.required_sec_level == ESP_BLE_ISO_SECURITY_MITM) {
        err = pairing_start(event->acl_connect.conn_handle);
        if (err) {
            ESP_LOGE(TAG, "Failed to initiate security, err %d", err);
        }
        return;
    }

    create_cig_and_cis(event->acl_connect.conn_handle);
}

static void acl_disconnect(esp_ble_iso_gap_app_event_t *event)
{
    ESP_LOGI(TAG, "Disconnected: handle %u reason 0x%02x",
             event->acl_disconnect.conn_handle, event->acl_disconnect.reason);

    acl_connected = false;

    ext_scan_start();
}

static void security_change(esp_ble_iso_gap_app_event_t *event)
{
    if (event->security_change.status) {
        security_failed_recover(event->security_change.conn_handle,
                                event->security_change.status);
        return;
    }

    ESP_LOGI(TAG, "Security: handle %u level %u bonded %u",
             event->security_change.conn_handle,
             event->security_change.sec_level,
             event->security_change.bonded);

    create_cig_and_cis(event->security_change.conn_handle);
}

static void iso_gap_app_cb(esp_ble_iso_gap_app_event_t *event)
{
    switch (event->type) {
    case ESP_BLE_ISO_GAP_EVENT_EXT_SCAN_RECV:
        ext_scan_recv(event);
        break;
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

    err = example_iso_tx_scheduler_init(&tx_scheduler,
                                        tx_scheduler_cb,
                                        NULL);
    if (err) {
        ESP_LOGE(TAG, "Failed to init tx scheduler, err %d", err);
        return;
    }

    err = set_device_name();
    if (err) {
        ESP_LOGE(TAG, "Failed to set device name, err %d", err);
        return;
    }

    err = ext_scan_start();
    if (err) {
        ESP_LOGE(TAG, "Failed to start scan, err %d", err);
        return;
    }
}
