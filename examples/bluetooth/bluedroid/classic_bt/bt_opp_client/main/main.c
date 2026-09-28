/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_opp_api.h"
#include "bt_app_core_utils.h"

#define OPP_TAG "OPP_CLIENT"
#define LOCAL_DEVICE_NAME "ESP_OPP_CLIENT"

static const char local_device_name[]  = LOCAL_DEVICE_NAME;
static const char remote_device_name[] = CONFIG_EXAMPLE_PEER_DEVICE_NAME;

static const uint8_t sample_vcard_1[] =
    "BEGIN:VCARD\r\n"
    "VERSION:2.1\r\n"
    "N:ESP;OPP;;;\r\n"
    "FN:ESP OPP 1\r\n"
    "TEL:11111111\r\n"
    "END:VCARD\r\n";

static const uint8_t sample_vcard_2[] =
    "BEGIN:VCARD\r\n"
    "VERSION:2.1\r\n"
    "N:ESP;OPP;;;\r\n"
    "FN:ESP OPP 2\r\n"
    "TEL:22222222\r\n"
    "END:VCARD\r\n";

static const uint8_t sample_vcard_3[] =
    "BEGIN:VCARD\r\n"
    "VERSION:2.1\r\n"
    "N:ESP;OPP;;;\r\n"
    "FN:ESP OPP 3\r\n"
    "TEL:33333333\r\n"
    "END:VCARD\r\n";

typedef struct {
    const char *name;
    const char *type;
    const uint8_t *data;
    uint32_t len;
} opp_sample_object_t;

typedef struct {
    int fd;
    const uint8_t *data;
    uint32_t len;
} opp_tx_task_arg_t;

/* Multiple objects pushed within a single connection. */
static const opp_sample_object_t s_sample_objects[] = {
    { "esp_opp_1.vcf", "text/x-vcard", sample_vcard_1, sizeof(sample_vcard_1) - 1 },
    { "esp_opp_2.vcf", "text/x-vcard", sample_vcard_2, sizeof(sample_vcard_2) - 1 },
    { "esp_opp_3.vcf", "text/x-vcard", sample_vcard_3, sizeof(sample_vcard_3) - 1 },
};

static size_t s_sample_object_index;
static esp_opp_conn_hdl_t s_client_handle = ESP_OPP_INVALID_HANDLE;
static bool s_opp_client_ready;
static bool s_sample_connect_started;
static esp_bd_addr_t s_peer_bda;

static char *bda2str(const uint8_t *bda, char *str, size_t size)
{
    if (bda == NULL || str == NULL || size < 18) {
        return NULL;
    }

    snprintf(str, size, "%02x:%02x:%02x:%02x:%02x:%02x",
             bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
    return str;
}

static bool get_name_from_eir(uint8_t *eir, char *bdname, uint8_t *bdname_len)
{
    uint8_t *rmt_bdname = NULL;
    uint8_t rmt_bdname_len = 0;

    if (!eir) {
        return false;
    }

    rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME, &rmt_bdname_len);
    if (!rmt_bdname) {
        rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME, &rmt_bdname_len);
    }

    if (rmt_bdname) {
        if (rmt_bdname_len > ESP_BT_GAP_MAX_BDNAME_LEN) {
            rmt_bdname_len = ESP_BT_GAP_MAX_BDNAME_LEN;
        }
        if (bdname) {
            memcpy(bdname, rmt_bdname, rmt_bdname_len);
            bdname[rmt_bdname_len] = '\0';
        }
        if (bdname_len) {
            *bdname_len = rmt_bdname_len;
        }
        return true;
    }

    return false;
}

static bool peer_name_valid(void)
{
    if (remote_device_name[0] == '\0') {
        ESP_LOGE(OPP_TAG, "EXAMPLE_PEER_DEVICE_NAME is empty");
        return false;
    }
    return true;
}

static void connect_peer(const esp_bd_addr_t bda)
{
    char bda_str[18] = {0};
    esp_opp_client_connect_param_t param = {
        .sec_mask = ESP_BT_SEC_AUTHENTICATE,
    };

    memcpy(param.bd_addr, bda, sizeof(esp_bd_addr_t));
    ESP_LOGI(OPP_TAG, "Connecting to %s (%s)", remote_device_name,
             bda2str(bda, bda_str, sizeof(bda_str)));
    ESP_ERROR_CHECK(esp_opp_client_connect(&param));
}

static void start_peer_discovery(void)
{
    if (!s_opp_client_ready || s_sample_connect_started) {
        return;
    }
    if (!peer_name_valid()) {
        return;
    }
    ESP_LOGI(OPP_TAG, "Discovering peer name: %s", remote_device_name);
    ESP_ERROR_CHECK(esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 30, 0));
}

/* Send the object at the current index over the active client connection.
 * Returns true if an open was requested, false when there is nothing left. */
static bool send_next_sample_object(void)
{
    if (s_client_handle == ESP_OPP_INVALID_HANDLE) {
        return false;
    }
    if (s_sample_object_index >= sizeof(s_sample_objects) / sizeof(s_sample_objects[0])) {
        return false;
    }

    const opp_sample_object_t *obj = &s_sample_objects[s_sample_object_index];
    esp_opp_client_object_cfg_t cfg = {
        .handle = s_client_handle,
        .name = obj->name,
        .type = obj->type,
        .len = obj->len,
    };

    ESP_LOGI(OPP_TAG, "Sending object[%d/%d] name:%s len:%" PRIu32,
             (int)s_sample_object_index + 1,
             (int)(sizeof(s_sample_objects) / sizeof(s_sample_objects[0])),
             obj->name, cfg.len);
    ESP_ERROR_CHECK(esp_opp_client_open_object(&cfg));
    return true;
}

static void opp_client_write_task(void *pvParameters)
{
    opp_tx_task_arg_t *arg = (opp_tx_task_arg_t *)pvParameters;
    size_t remaining = arg->len;
    const uint8_t *p = arg->data;
    int fd = arg->fd;
    free(arg);

    while (remaining > 0) {
        ssize_t n = write(fd, p, remaining);
        if (n <= 0) {
            ESP_LOGE(OPP_TAG, "write(fd=%d) failed errno=%d", fd, errno);
            close(fd);
            vTaskDelete(NULL);
            return;
        }
        p += n;
        remaining -= (size_t)n;
    }
    close(fd);
    vTaskDelete(NULL);
}

static void start_opp_tx_task(int fd)
{
    const opp_sample_object_t *obj;
    opp_tx_task_arg_t *arg;

    if (fd < 0 || s_sample_object_index >= sizeof(s_sample_objects) / sizeof(s_sample_objects[0])) {
        if (fd >= 0) {
            close(fd);
        }
        return;
    }

    obj = &s_sample_objects[s_sample_object_index];
    arg = (opp_tx_task_arg_t *)malloc(sizeof(*arg));

    if (arg == NULL) {
        close(fd);
        return;
    }

    arg->fd = fd;
    arg->data = obj->data;
    arg->len = obj->len;

    if (xTaskCreate(opp_client_write_task, "opp_tx", 4096, arg, 5, NULL) != pdPASS) {
        close(arg->fd);
        free(arg);
        return;
    }
    s_sample_object_index++;
}

static void bt_app_opp_client_evt_hdl(uint16_t event, void *param)
{
    esp_opp_client_param_t *opp_param = (esp_opp_client_param_t *)param;
    char bda_str[18] = {0};

    switch ((esp_opp_client_cb_event_t)event) {
    case ESP_OPP_CLIENT_INIT_EVT:
        ESP_LOGI(OPP_TAG, "ESP_OPP_CLIENT_INIT_EVT status:%d", opp_param->init.status);
        if (opp_param->init.status == ESP_BT_STATUS_SUCCESS) {
            s_opp_client_ready = true;
            start_peer_discovery();
        }
        break;
    case ESP_OPP_CLIENT_DEINIT_EVT:
        ESP_LOGI(OPP_TAG, "ESP_OPP_CLIENT_DEINIT_EVT status:%d", opp_param->deinit.status);
        break;
    case ESP_OPP_CLIENT_CONNECTION_STATE_EVT:
        ESP_LOGI(OPP_TAG, "ESP_OPP_CLIENT_CONNECTION_STATE_EVT handle:%d state:%d status:%d bda:%s",
                 opp_param->conn.handle, opp_param->conn.state, opp_param->conn.status,
                 bda2str(opp_param->conn.bd_addr, bda_str, sizeof(bda_str)));
        if (opp_param->conn.state == ESP_OPP_CONNECTED && opp_param->conn.status == ESP_BT_STATUS_SUCCESS) {
            s_client_handle = opp_param->conn.handle;
            s_sample_object_index = 0;
            send_next_sample_object();
        } else if (opp_param->conn.state == ESP_OPP_DISCONNECTED) {
            s_client_handle = ESP_OPP_INVALID_HANDLE;
        }
        break;
    case ESP_OPP_CLIENT_OPEN_EVT:
        ESP_LOGI(OPP_TAG, "ESP_OPP_CLIENT_OPEN_EVT handle:%d status:%d fd:%d",
                 opp_param->open.handle, opp_param->open.status, opp_param->open.fd);
        if (opp_param->open.status == ESP_BT_STATUS_SUCCESS) {
            start_opp_tx_task(opp_param->open.fd);
        }
        break;
    case ESP_OPP_CLIENT_PROGRESS_EVT:
        ESP_LOGI(OPP_TAG, "ESP_OPP_CLIENT_PROGRESS_EVT handle:%d transferred:%" PRIu32 " total:%" PRIu32,
                 opp_param->progress.handle, opp_param->progress.transferred, opp_param->progress.total);
        break;
    case ESP_OPP_CLIENT_TRANSFER_COMPLETE_EVT:
        ESP_LOGI(OPP_TAG, "ESP_OPP_CLIENT_TRANSFER_COMPLETE_EVT handle:%d status:%d transferred:%" PRIu32,
                 opp_param->complete.handle, opp_param->complete.status, opp_param->complete.transferred);
        if (opp_param->complete.status == ESP_BT_STATUS_SUCCESS) {
            if (!send_next_sample_object()) {
                ESP_ERROR_CHECK(esp_opp_client_disconnect(opp_param->complete.handle));
            }
        }
        break;
    default:
        break;
    }
}

static void esp_opp_client_cb(esp_opp_client_cb_event_t event, esp_opp_client_param_t *param)
{
    bt_app_work_dispatch(bt_app_opp_client_evt_hdl, event, param,
                         sizeof(esp_opp_client_param_t), NULL, NULL);
}

static void try_connect_discovered_peer(const esp_bd_addr_t bda)
{
    if (s_sample_connect_started || !s_opp_client_ready) {
        return;
    }
    s_sample_connect_started = true;
    memcpy(s_peer_bda, bda, sizeof(esp_bd_addr_t));
    esp_bt_gap_cancel_discovery();
    connect_peer(s_peer_bda);
}

static void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    char bda_str[18] = {0};
    char peer_bdname[ESP_BT_GAP_MAX_BDNAME_LEN + 1] = {0};
    uint8_t peer_bdname_len = 0;

    switch (event) {
    case ESP_BT_GAP_DISC_RES_EVT:
        for (int i = 0; i < param->disc_res.num_prop; i++) {
            bool got_name = false;

            if (param->disc_res.prop[i].type == ESP_BT_GAP_DEV_PROP_EIR) {
                got_name = get_name_from_eir(param->disc_res.prop[i].val, peer_bdname, &peer_bdname_len);
            } else if (param->disc_res.prop[i].type == ESP_BT_GAP_DEV_PROP_BDNAME) {
                peer_bdname_len = param->disc_res.prop[i].len;
                if (peer_bdname_len > ESP_BT_GAP_MAX_BDNAME_LEN) {
                    peer_bdname_len = ESP_BT_GAP_MAX_BDNAME_LEN;
                }
                memcpy(peer_bdname, param->disc_res.prop[i].val, peer_bdname_len);
                peer_bdname[peer_bdname_len] = '\0';
                got_name = true;
            }

            if (got_name && strlen(remote_device_name) == peer_bdname_len &&
                    strncmp(peer_bdname, remote_device_name, peer_bdname_len) == 0) {
                try_connect_discovered_peer(param->disc_res.bda);
                break;
            }
        }
        break;
    case ESP_BT_GAP_DISC_STATE_CHANGED_EVT:
        ESP_LOGI(OPP_TAG, "ESP_BT_GAP_DISC_STATE_CHANGED_EVT");
        break;
    case ESP_BT_GAP_ACL_CONN_CMPL_STAT_EVT:
        ESP_LOGI(OPP_TAG, "ACL connected bda:%s status:%d",
                 bda2str(param->acl_conn_cmpl_stat.bda, bda_str, sizeof(bda_str)),
                 param->acl_conn_cmpl_stat.stat);
        break;
    case ESP_BT_GAP_ACL_DISCONN_CMPL_STAT_EVT:
        ESP_LOGI(OPP_TAG, "ACL disconnected bda:%s reason:0x%02x",
                 bda2str(param->acl_disconn_cmpl_stat.bda, bda_str, sizeof(bda_str)),
                 param->acl_disconn_cmpl_stat.reason);
        break;
    case ESP_BT_GAP_AUTH_CMPL_EVT:
        ESP_LOGI(OPP_TAG, "AUTH complete bda:%s success:%d",
                 bda2str(param->auth_cmpl.bda, bda_str, sizeof(bda_str)),
                 param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS);
        break;
    case ESP_BT_GAP_PIN_REQ_EVT: {
        esp_bt_pin_code_t pin_code = {'1', '2', '3', '4'};
        esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        break;
    }
    case ESP_BT_GAP_CFM_REQ_EVT:
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    default:
        break;
    }
}

void app_main(void)
{
    char bda_str[18] = {0};
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());
    bt_app_task_start_up();
    ESP_ERROR_CHECK(esp_bt_gap_register_callback(esp_bt_gap_cb));
    ESP_ERROR_CHECK(esp_bt_gap_set_device_name(local_device_name));
    ESP_ERROR_CHECK(esp_bt_gap_set_scan_mode(ESP_BT_NON_CONNECTABLE, ESP_BT_NON_DISCOVERABLE));
    ESP_ERROR_CHECK(esp_opp_client_register_callback(esp_opp_client_cb));
    ESP_ERROR_CHECK(esp_opp_client_init());

    ESP_LOGI(OPP_TAG, "Local device name: %s address: %s",
             local_device_name, bda2str(esp_bt_dev_get_address(), bda_str, sizeof(bda_str)));
}
