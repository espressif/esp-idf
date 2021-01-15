// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <esp_log.h>
#include <string.h>
#include <esp_err.h>
#include <esp_wifi.h>

#include "wifi_scan.pb-c.h"

#include <wifi_provisioning/wifi_scan.h>

static const char *TAG = "proto_wifi_scan";

typedef struct wifi_prov_scan_cmd {
    int cmd_num;
    esp_err_t (*command_handler)(WiFiScanPayload *req,
                                 WiFiScanPayload *resp, void *priv_data);
} wifi_prov_scan_cmd_t;

static esp_err_t cmd_scan_start_handler(WiFiScanPayload *req,
                                        WiFiScanPayload *resp,
                                        void *priv_data);

static esp_err_t cmd_scan_status_handler(WiFiScanPayload *req,
                                         WiFiScanPayload *resp,
                                         void *priv_data);

static esp_err_t cmd_scan_result_handler(WiFiScanPayload *req,
                                         WiFiScanPayload *resp,
                                         void *priv_data);

static wifi_prov_scan_cmd_t cmd_table[] = {
    {
        .cmd_num = WI_FI_SCAN_MSG_TYPE__TypeCmdScanStart,
        .command_handler = cmd_scan_start_handler
    },
    {
        .cmd_num = WI_FI_SCAN_MSG_TYPE__TypeCmdScanStatus,
        .command_handler = cmd_scan_status_handler
    },
    {
        .cmd_num = WI_FI_SCAN_MSG_TYPE__TypeCmdScanResult,
        .command_handler = cmd_scan_result_handler
    }
};

static esp_err_t cmd_scan_start_handler(WiFiScanPayload *req,
                                        WiFiScanPayload *resp, void *priv_data)
{
    wifi_prov_scan_handlers_t *h = (wifi_prov_scan_handlers_t *) priv_data;
    if (!h) {
        ESP_LOGE(TAG, "Command invoked without handlers");
        return ESP_ERR_INVALID_STATE;
    }

    RespScanStart *resp_payload = (RespScanStart *) malloc(sizeof(RespScanStart));
    if (!resp_payload) {
        ESP_LOGE(TAG, "Error allocating memory");
        return ESP_ERR_NO_MEM;
    }

    resp_scan_start__init(resp_payload);
    resp->status = (h->scan_start(req->cmd_scan_start->blocking,
                                  req->cmd_scan_start->passive,
                                  req->cmd_scan_start->group_channels,
                                  req->cmd_scan_start->period_ms,
                                  &h->ctx) == ESP_OK ?
                            STATUS__Success : STATUS__InternalError);
    resp->payload_case = WI_FI_SCAN_PAYLOAD__PAYLOAD_RESP_SCAN_START;
    resp->resp_scan_start = resp_payload;
    return ESP_OK;
}

static esp_err_t cmd_scan_status_handler(WiFiScanPayload *req,
                                         WiFiScanPayload *resp, void *priv_data)
{
    bool scan_finished = false;
    uint16_t result_count = 0;

    wifi_prov_scan_handlers_t *h = (wifi_prov_scan_handlers_t *) priv_data;
    if (!h) {
        ESP_LOGE(TAG, "Command invoked without handlers");
        return ESP_ERR_INVALID_STATE;
    }

    RespScanStatus *resp_payload = (RespScanStatus *) malloc(sizeof(RespScanStatus));
    if (!resp_payload) {
        ESP_LOGE(TAG, "Error allocating memory");
        return ESP_ERR_NO_MEM;
    }

    resp_scan_status__init(resp_payload);
    resp->status = (h->scan_status(&scan_finished, &result_count, &h->ctx) == ESP_OK ?
                            STATUS__Success : STATUS__InternalError);
    resp_payload->scan_finished = scan_finished;
    resp_payload->result_count = result_count;
    resp->payload_case = WI_FI_SCAN_PAYLOAD__PAYLOAD_RESP_SCAN_STATUS;
    resp->resp_scan_status = resp_payload;
    return ESP_OK;
}

static esp_err_t cmd_scan_result_handler(WiFiScanPayload *req,
                                         WiFiScanPayload *resp, void *priv_data)
{
    esp_err_t err;
    wifi_prov_scan_result_t scan_result = {{0}, {0}, 0, 0, 0};
    WiFiScanResult **results = NULL;
    wifi_prov_scan_handlers_t *h = (wifi_prov_scan_handlers_t *) priv_data;
    if (!h) {
        ESP_LOGE(TAG, "Command invoked without handlers");
        return ESP_ERR_INVALID_STATE;
    }

    RespScanResult *resp_payload = (RespScanResult *) malloc(sizeof(RespScanResult));
    if (!resp_payload) {
        ESP_LOGE(TAG, "Error allocating memory");
        return ESP_ERR_NO_MEM;
    }
    resp_scan_result__init(resp_payload);

    resp->status = STATUS__Success;
    resp->payload_case = WI_FI_SCAN_PAYLOAD__PAYLOAD_RESP_SCAN_RESULT;
    resp->resp_scan_result = resp_payload;
    /* Allocate memory only if there are non-zero scan results */
    if (req->cmd_scan_result->count) {
        results = (WiFiScanResult **) calloc(req->cmd_scan_result->count,
                sizeof(WiFiScanResult *));
        if (!results) {
            ESP_LOGE(TAG, "Failed to allocate memory for results array");
            return ESP_ERR_NO_MEM;
        }
    }
    resp_payload->entries = results;
    resp_payload->n_entries = req->cmd_scan_result->count;

    /* If req->cmd_scan_result->count is 0, the below loop will
     * be skipped.
     */
    for (uint16_t i = 0; i < req->cmd_scan_result->count; i++) {
        err = h->scan_result(i + req->cmd_scan_result->start_index,
                             &scan_result, &h->ctx);
        if (err != ESP_OK) {
            resp->status = STATUS__InternalError;
            break;
        }

        results[i] = (WiFiScanResult *) malloc(sizeof(WiFiScanResult));
        if (!results[i]) {
            ESP_LOGE(TAG, "Failed to allocate memory for result entry");
            return ESP_ERR_NO_MEM;
        }
        wi_fi_scan_result__init(results[i]);

        results[i]->ssid.len = strnlen(scan_result.ssid, 32);
        results[i]->ssid.data = (uint8_t *) strndup(scan_result.ssid, 32);
        if (!results[i]->ssid.data) {
            ESP_LOGE(TAG, "Failed to allocate memory for scan result entry SSID");
            return ESP_ERR_NO_MEM;
        }

        results[i]->channel = scan_result.channel;
        results[i]->rssi = scan_result.rssi;
        results[i]->auth = scan_result.auth;

        results[i]->bssid.len = sizeof(scan_result.bssid);
        results[i]->bssid.data = malloc(results[i]->bssid.len);
        if (!results[i]->bssid.data) {
            ESP_LOGE(TAG, "Failed to allocate memory for scan result entry BSSID");
            return ESP_ERR_NO_MEM;
        }
        memcpy(results[i]->bssid.data, scan_result.bssid, results[i]->bssid.len);
    }
    return ESP_OK;
}


static int lookup_cmd_handler(int cmd_id)
{
    for (size_t i = 0; i < sizeof(cmd_table)/sizeof(wifi_prov_scan_cmd_t); i++) {
        if (cmd_table[i].cmd_num == cmd_id) {
            return i;
        }
    }

    return -1;
}

static void wifi_prov_scan_cmd_cleanup(WiFiScanPayload *resp, void *priv_data)
{
    switch (resp->msg) {
        case WI_FI_SCAN_MSG_TYPE__TypeRespScanStart:
            {
                free(resp->resp_scan_start);
            }
            break;
        case WI_FI_SCAN_MSG_TYPE__TypeRespScanStatus:
            {
                free(resp->resp_scan_status);
            }
            break;
        case WI_FI_SCAN_MSG_TYPE__TypeRespScanResult:
            {
                if (!resp->resp_scan_result) return;
                if (resp->resp_scan_result->entries) {
                    for (uint16_t i = 0; i < resp->resp_scan_result->n_entries; i++) {
                        if (!resp->resp_scan_result->entries[i]) continue;
                        free(resp->resp_scan_result->entries[i]->ssid.data);
                        free(resp->resp_scan_result->entries[i]->bssid.data);
                        free(resp->resp_scan_result->entries[i]);
                    }
                    free(resp->resp_scan_result->entries);
                }
                free(resp->resp_scan_result);
            }
            break;
        default:
            ESP_LOGE(TAG, "Unsupported response type in cleanup_handler");
            break;
    }
    return;
}

static esp_err_t wifi_prov_scan_cmd_dispatcher(WiFiScanPayload *req,
                                               WiFiScanPayload *resp, void *priv_data)
{
    esp_err_t ret;

    ESP_LOGD(TAG, "In wifi_prov_scan_cmd_dispatcher Cmd=%d", req->msg);

    int cmd_index = lookup_cmd_handler(req->msg);
    if (cmd_index < 0) {
        ESP_LOGE(TAG, "Invalid command handler lookup");
        return ESP_FAIL;
    }

    ret = cmd_table[cmd_index].command_handler(req, resp, priv_data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error executing command handler");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t wifi_prov_scan_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen,
                                 uint8_t **outbuf, ssize_t *outlen, void *priv_data)
{
    WiFiScanPayload *req;
    WiFiScanPayload resp;
    esp_err_t ret = ESP_OK;

    req = wi_fi_scan_payload__unpack(NULL, inlen, inbuf);
    if (!req) {
        ESP_LOGE(TAG, "Unable to unpack scan message");
        return ESP_ERR_INVALID_ARG;
    }

    wi_fi_scan_payload__init(&resp);
    ret = wifi_prov_scan_cmd_dispatcher(req, &resp, priv_data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Command dispatcher error %d", ret);
        ret = ESP_FAIL;
        goto exit;
    }

    resp.msg = req->msg + 1; /* Response is request + 1 */
    *outlen = wi_fi_scan_payload__get_packed_size(&resp);
    if (*outlen <= 0) {
        ESP_LOGE(TAG, "Invalid encoding for response");
        ret = ESP_FAIL;
        goto exit;
    }

    *outbuf = (uint8_t *) malloc(*outlen);
    if (!*outbuf) {
        ESP_LOGE(TAG, "System out of memory");
        ret = ESP_ERR_NO_MEM;
        goto exit;
    }
    wi_fi_scan_payload__pack(&resp, *outbuf);
    ESP_LOGD(TAG, "Response packet size : %d", *outlen);
    exit:

    wi_fi_scan_payload__free_unpacked(req, NULL);
    wifi_prov_scan_cmd_cleanup(&resp, priv_data);
    return ret;
}
