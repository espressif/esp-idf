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
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_opp_api.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "bt_app_core_utils.h"

#define OPP_TAG "OPP_SERVER"
#define OPP_READ_BUF_SIZE 2048
#define OPP_RX_LOG_EVERY 32768

static const char local_device_name[] = CONFIG_EXAMPLE_LOCAL_DEVICE_NAME;
static esp_opp_conn_hdl_t s_pending_accept_handle = ESP_OPP_INVALID_HANDLE;
static uint32_t s_pending_accept_len;
static const uint8_t supported_formats[] = {
    ESP_OPP_FORMAT_VCARD_2_1,
    ESP_OPP_FORMAT_VCARD_3_0,
    ESP_OPP_FORMAT_ANY,
};

static char *bda2str(const uint8_t *bda, char *str, size_t size)
{
    if (bda == NULL || str == NULL || size < 18) {
        return NULL;
    }

    snprintf(str, size, "%02x:%02x:%02x:%02x:%02x:%02x",
             bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
    return str;
}

typedef struct {
    int fd;
    uint32_t expected_len;
} opp_rx_task_arg_t;

static void opp_server_read_task(void *pvParameters)
{
    opp_rx_task_arg_t *arg = (opp_rx_task_arg_t *)pvParameters;
    uint8_t *buf = (uint8_t *)malloc(OPP_READ_BUF_SIZE);
    uint32_t total = 0;
    uint32_t last_log = 0;
    int fd = arg->fd;
    uint32_t expected = arg->expected_len;
    free(arg);

    if (buf == NULL) {
        ESP_LOGE(OPP_TAG, "opp_rx: no mem for read buffer");
        close(fd);
        vTaskDelete(NULL);
        return;
    }

    while (true) {
        ssize_t n = read(fd, buf, OPP_READ_BUF_SIZE);
        if (n < 0) {
            if (errno == EAGAIN) {
                vTaskDelay(pdMS_TO_TICKS(5));
                continue;
            }
            ESP_LOGE(OPP_TAG, "read(fd=%d) failed errno=%d", fd, errno);
            break;
        }
        if (n == 0) {
            break; /* object EOF */
        }
        total += (uint32_t)n;
        if (total - last_log >= OPP_RX_LOG_EVERY || total >= expected) {
            ESP_LOGI(OPP_TAG, "RX fd=%d total=%" PRIu32 " / %" PRIu32, fd, total, expected);
            last_log = total;
        }
    }

    ESP_LOGI(OPP_TAG, "RX done fd=%d total=%" PRIu32, fd, total);
    free(buf);
    close(fd);
    vTaskDelete(NULL);
}

static void start_opp_rx_task(int fd, uint32_t expected_len)
{
    opp_rx_task_arg_t *arg;

    if (fd < 0) {
        ESP_LOGE(OPP_TAG, "invalid RX fd");
        return;
    }
    arg = (opp_rx_task_arg_t *)malloc(sizeof(*arg));
    if (arg == NULL) {
        close(fd);
        return;
    }
    arg->fd = fd;
    arg->expected_len = expected_len;
    if (xTaskCreate(opp_server_read_task, "opp_rx", 4096, arg, 5, NULL) != pdPASS) {
        close(arg->fd);
        free(arg);
    }
}

static void bt_app_opp_server_copy_incoming(void *p_dest, void *p_src, int len)
{
    esp_opp_server_param_t *dest = (esp_opp_server_param_t *)p_dest;
    const esp_opp_server_param_t *src = (const esp_opp_server_param_t *)p_src;

    if (src->incoming.name) {
        dest->incoming.name = strdup(src->incoming.name);
    }
    if (src->incoming.type) {
        dest->incoming.type = strdup(src->incoming.type);
    }
}

static void bt_app_opp_server_free_incoming(void *p_param)
{
    esp_opp_server_param_t *param = (esp_opp_server_param_t *)p_param;

    free((void *)param->incoming.name);
    free((void *)param->incoming.type);
    param->incoming.name = NULL;
    param->incoming.type = NULL;
}

static void bt_app_opp_server_evt_hdl(uint16_t event, void *param)
{
    esp_opp_server_param_t *opp_param = (esp_opp_server_param_t *)param;
    char bda_str[18] = {0};

    switch ((esp_opp_server_cb_event_t)event) {
    case ESP_OPP_SERVER_INIT_EVT: {
        ESP_LOGI(OPP_TAG, "ESP_OPP_SERVER_INIT_EVT status:%d", opp_param->init.status);
        if (opp_param->init.status == ESP_BT_STATUS_SUCCESS) {
            esp_opp_server_cfg_t cfg = {
                .sec_mask = ESP_BT_SEC_AUTHENTICATE,
                .service_name = "ESP Object Push",
                .supported_formats = supported_formats,
                .supported_formats_len = sizeof(supported_formats),
                .auto_accept = CONFIG_EXAMPLE_OPP_SRV_AUTO_ACCEPT,
            };
            ESP_ERROR_CHECK(esp_opp_server_start(&cfg));
        }
        break;
    }
    case ESP_OPP_SERVER_DEINIT_EVT:
        ESP_LOGI(OPP_TAG, "ESP_OPP_SERVER_DEINIT_EVT status:%d", opp_param->deinit.status);
        break;
    case ESP_OPP_SERVER_START_EVT:
        ESP_LOGI(OPP_TAG, "ESP_OPP_SERVER_START_EVT status:%d scn:%d",
                 opp_param->start.status, opp_param->start.scn);
        break;
    case ESP_OPP_SERVER_STOP_EVT:
        ESP_LOGI(OPP_TAG, "ESP_OPP_SERVER_STOP_EVT status:%d", opp_param->stop.status);
        break;
    case ESP_OPP_SERVER_CONNECTION_STATE_EVT:
        ESP_LOGI(OPP_TAG, "ESP_OPP_SERVER_CONNECTION_STATE_EVT handle:%d state:%d status:%d bda:%s",
                 opp_param->conn.handle, opp_param->conn.state, opp_param->conn.status,
                 bda2str(opp_param->conn.bd_addr, bda_str, sizeof(bda_str)));
        break;
    case ESP_OPP_SERVER_INCOMING_OBJECT_EVT:
        ESP_LOGI(OPP_TAG, "ESP_OPP_SERVER_INCOMING_OBJECT_EVT handle:%d name:%s type:%s len:%" PRIu32
                 " fd:%d needs_accept:%d",
                 opp_param->incoming.handle,
                 opp_param->incoming.name ? opp_param->incoming.name : "",
                 opp_param->incoming.type ? opp_param->incoming.type : "",
                 opp_param->incoming.len,
                 opp_param->incoming.fd,
                 opp_param->incoming.needs_accept);
        /* First object on a connection needs accept; later ones are auto-authorized.
         * Do not block this handler (accept timeout is ~30s). */
        if (opp_param->incoming.needs_accept) {
            s_pending_accept_handle = opp_param->incoming.handle;
            s_pending_accept_len = opp_param->incoming.len;
            ESP_ERROR_CHECK(esp_opp_server_accept(opp_param->incoming.handle));
        } else {
            start_opp_rx_task(opp_param->incoming.fd, opp_param->incoming.len);
        }
        break;
    case ESP_OPP_SERVER_ACCEPT_EVT:
        ESP_LOGI(OPP_TAG, "ESP_OPP_SERVER_ACCEPT_EVT handle:%d status:%d fd:%d",
                 opp_param->accept.handle, opp_param->accept.status, opp_param->accept.fd);
        if (opp_param->accept.status == ESP_BT_STATUS_SUCCESS) {
            uint32_t expected = 0;
            if (opp_param->accept.handle == s_pending_accept_handle) {
                expected = s_pending_accept_len;
                s_pending_accept_handle = ESP_OPP_INVALID_HANDLE;
            }
            start_opp_rx_task(opp_param->accept.fd, expected);
        }
        break;
    case ESP_OPP_SERVER_PROGRESS_EVT:
        ESP_LOGI(OPP_TAG, "ESP_OPP_SERVER_PROGRESS_EVT handle:%d transferred:%" PRIu32 " total:%" PRIu32,
                 opp_param->progress.handle, opp_param->progress.transferred, opp_param->progress.total);
        break;
    case ESP_OPP_SERVER_TRANSFER_COMPLETE_EVT:
        ESP_LOGI(OPP_TAG, "ESP_OPP_SERVER_TRANSFER_COMPLETE_EVT handle:%d status:%d transferred:%" PRIu32,
                 opp_param->complete.handle, opp_param->complete.status, opp_param->complete.transferred);
        break;
    default:
        break;
    }
}

static void esp_opp_server_cb(esp_opp_server_cb_event_t event, esp_opp_server_param_t *param)
{
    switch (event) {
    case ESP_OPP_SERVER_INCOMING_OBJECT_EVT:
        bt_app_work_dispatch(bt_app_opp_server_evt_hdl, event, param, sizeof(esp_opp_server_param_t),
                             bt_app_opp_server_copy_incoming, bt_app_opp_server_free_incoming);
        break;
    default:
        bt_app_work_dispatch(bt_app_opp_server_evt_hdl, event, param,
                             sizeof(esp_opp_server_param_t), NULL, NULL);
        break;
    }
}

static void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    char bda_str[18] = {0};

    switch (event) {
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
    ESP_ERROR_CHECK(esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE));
    ESP_ERROR_CHECK(esp_opp_server_register_callback(esp_opp_server_cb));
    ESP_ERROR_CHECK(esp_opp_server_init());

    ESP_LOGI(OPP_TAG, "Local device name: %s address: %s",
             local_device_name, bda2str(esp_bt_dev_get_address(), bda_str, sizeof(bda_str)));
}
