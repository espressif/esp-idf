// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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
#include <string.h>
#include <esp_err.h>
#include <esp_log.h>

#include "wifi_constants.pb-c.h"
#include "wifi_config.pb-c.h"

#include <wifi_provisioning/wifi_config.h>

static const char* TAG = "WiFiProvConfig";

typedef struct wifi_prov_config_cmd {
    int cmd_num;
    esp_err_t (*command_handler)(WiFiConfigPayload *req,
                                 WiFiConfigPayload *resp, void *priv_data);
} wifi_prov_config_cmd_t;

static esp_err_t cmd_get_status_handler(WiFiConfigPayload *req,
                                        WiFiConfigPayload *resp, void *priv_data);

static esp_err_t cmd_set_config_handler(WiFiConfigPayload *req,
                                        WiFiConfigPayload *resp, void *priv_data);

static esp_err_t cmd_apply_config_handler(WiFiConfigPayload *req,
                                          WiFiConfigPayload *resp, void *priv_data);

static wifi_prov_config_cmd_t cmd_table[] = {
    {
        .cmd_num = WI_FI_CONFIG_MSG_TYPE__TypeCmdGetStatus,
        .command_handler = cmd_get_status_handler
    },
    {
        .cmd_num = WI_FI_CONFIG_MSG_TYPE__TypeCmdSetConfig,
        .command_handler = cmd_set_config_handler
    },
    {
        .cmd_num = WI_FI_CONFIG_MSG_TYPE__TypeCmdApplyConfig,
        .command_handler = cmd_apply_config_handler
    }
};

static esp_err_t cmd_get_status_handler(WiFiConfigPayload *req,
                                        WiFiConfigPayload *resp, void *priv_data)
{
    ESP_LOGD(TAG, "Enter cmd_get_status_handler");
    wifi_prov_config_handlers_t *h = (wifi_prov_config_handlers_t *) priv_data;
    if (!h) {
        ESP_LOGE(TAG, "Command invoked without handlers");
        return ESP_ERR_INVALID_STATE;
    }

    RespGetStatus *resp_payload = (RespGetStatus *) malloc(sizeof(RespGetStatus));
    if (!resp_payload) {
        ESP_LOGE(TAG, "Error allocating memory");
        return ESP_ERR_NO_MEM;
    }
    resp_get_status__init(resp_payload);

    wifi_prov_config_get_data_t resp_data;
    if (h->get_status_handler(&resp_data, &h->ctx) == ESP_OK) {
        if (resp_data.wifi_state == WIFI_PROV_STA_CONNECTING) {
            resp_payload->sta_state = WIFI_STATION_STATE__Connecting;
            resp_payload->state_case = RESP_GET_STATUS__STATE_CONNECTED;
        } else if (resp_data.wifi_state == WIFI_PROV_STA_CONNECTED) {
            resp_payload->sta_state  = WIFI_STATION_STATE__Connected;
            resp_payload->state_case = RESP_GET_STATUS__STATE_CONNECTED;
            WifiConnectedState *connected = (WifiConnectedState *)(
                                            malloc(sizeof(WifiConnectedState)));
            if (!connected) {
                ESP_LOGE(TAG, "Error allocating memory");
                return ESP_ERR_NO_MEM;
            }
            resp_payload->connected  = connected;
            wifi_connected_state__init(connected);

            connected->ip4_addr = strdup(resp_data.conn_info.ip_addr);
            if (connected->ip4_addr == NULL) {
                free(resp_payload);
                return ESP_ERR_NO_MEM;
            }

            connected->bssid.len  = sizeof(resp_data.conn_info.bssid);
            connected->bssid.data = (uint8_t *) strndup(resp_data.conn_info.bssid,
                                                        sizeof(resp_data.conn_info.bssid));
            if (connected->bssid.data == NULL) {
                free(connected->ip4_addr);
                free(resp_payload);
                return ESP_ERR_NO_MEM;
            }

            connected->ssid.len   = strlen(resp_data.conn_info.ssid);
            connected->ssid.data  = (uint8_t *) strdup(resp_data.conn_info.ssid);
            if (connected->ssid.data == NULL) {
                free(connected->bssid.data);
                free(connected->ip4_addr);
                free(resp_payload);
                return ESP_ERR_NO_MEM;
            }

            connected->channel    = resp_data.conn_info.channel;
            connected->auth_mode  = resp_data.conn_info.auth_mode;
        } else if (resp_data.wifi_state == WIFI_PROV_STA_DISCONNECTED) {
            resp_payload->sta_state = WIFI_STATION_STATE__ConnectionFailed;
            resp_payload->state_case = RESP_GET_STATUS__STATE_FAIL_REASON;

            if (resp_data.fail_reason == WIFI_PROV_STA_AUTH_ERROR) {
                resp_payload->fail_reason = WIFI_CONNECT_FAILED_REASON__AuthError;
            } else if (resp_data.fail_reason == WIFI_PROV_STA_AP_NOT_FOUND) {
                resp_payload->fail_reason = WIFI_CONNECT_FAILED_REASON__NetworkNotFound;
            }
        }
        resp_payload->status = STATUS__Success;
    }

    resp->payload_case = WI_FI_CONFIG_PAYLOAD__PAYLOAD_RESP_GET_STATUS;
    resp->resp_get_status = resp_payload;
    return ESP_OK;
}

static esp_err_t cmd_set_config_handler(WiFiConfigPayload *req,
                                        WiFiConfigPayload *resp, void  *priv_data)
{
    ESP_LOGD(TAG, "Enter cmd_set_config_handler");
    wifi_prov_config_handlers_t *h = (wifi_prov_config_handlers_t *) priv_data;
    if (!h) {
        ESP_LOGE(TAG, "Command invoked without handlers");
        return ESP_ERR_INVALID_STATE;
    }

    RespSetConfig *resp_payload = (RespSetConfig *) malloc(sizeof(RespSetConfig));
    if (resp_payload == NULL) {
        ESP_LOGE(TAG, "Error allocating memory");
        return ESP_ERR_NO_MEM;
    }
    resp_set_config__init(resp_payload);

    wifi_prov_config_set_data_t req_data;
    memset(&req_data, 0, sizeof(req_data));
    memcpy(req_data.ssid, req->cmd_set_config->ssid.data,
           req->cmd_set_config->ssid.len);
    memcpy(req_data.password, req->cmd_set_config->passphrase.data,
           req->cmd_set_config->passphrase.len);
    memcpy(req_data.bssid, req->cmd_set_config->bssid.data,
           req->cmd_set_config->bssid.len);
    req_data.channel = req->cmd_set_config->channel;
    if (h->set_config_handler(&req_data, &h->ctx) == ESP_OK) {
        resp_payload->status = STATUS__Success;
    }

    resp->payload_case = WI_FI_CONFIG_PAYLOAD__PAYLOAD_RESP_SET_CONFIG;
    resp->resp_set_config = resp_payload;
    return ESP_OK;
}

static esp_err_t cmd_apply_config_handler(WiFiConfigPayload *req,
                                          WiFiConfigPayload *resp, void  *priv_data)
{
    ESP_LOGD(TAG, "Enter cmd_apply_config_handler");
    wifi_prov_config_handlers_t *h = (wifi_prov_config_handlers_t *) priv_data;
    if (!h) {
        ESP_LOGE(TAG, "Command invoked without handlers");
        return ESP_ERR_INVALID_STATE;
    }

    RespApplyConfig *resp_payload = (RespApplyConfig *) malloc(sizeof(RespApplyConfig));
    if (!resp_payload) {
        ESP_LOGE(TAG, "Error allocating memory");
        return ESP_ERR_NO_MEM;
    }

    resp_apply_config__init(resp_payload);

    if (h->apply_config_handler(&h->ctx) == ESP_OK) {
        resp_payload->status = STATUS__Success;
    } else {
        resp_payload->status = STATUS__InvalidArgument;
    }

    resp->payload_case = WI_FI_CONFIG_PAYLOAD__PAYLOAD_RESP_APPLY_CONFIG;
    resp->resp_apply_config = resp_payload;
    return ESP_OK;
}

static int lookup_cmd_handler(int cmd_id)
{
    int i;

    for (i = 0; i < sizeof(cmd_table)/sizeof(wifi_prov_config_cmd_t); i++) {
        if (cmd_table[i].cmd_num == cmd_id) {
            return i;
        }
    }

    return -1;
}
static void wifi_prov_config_command_cleanup(WiFiConfigPayload *resp, void *priv_data)
{
    if (!resp) {
        return;
    }

    switch (resp->msg) {
        case WI_FI_CONFIG_MSG_TYPE__TypeRespGetStatus:
            {
                switch (resp->resp_get_status->sta_state) {
                    case WIFI_STATION_STATE__Connecting:
                        break;
                    case WIFI_STATION_STATE__Connected:
                        if (resp->resp_get_status->connected) {
                            if (resp->resp_get_status->connected->ip4_addr) {
                                free(resp->resp_get_status->connected->ip4_addr);
                            }
                            if (resp->resp_get_status->connected->bssid.data) {
                                free(resp->resp_get_status->connected->bssid.data);
                            }
                            if (resp->resp_get_status->connected->ssid.data) {
                                free(resp->resp_get_status->connected->ssid.data);
                            }
                            free(resp->resp_get_status->connected);
                        }
                        break;
                    case WIFI_STATION_STATE__ConnectionFailed:
                        break;
                    default:
                        break;
                }
                free(resp->resp_get_status);
            }
            break;
        case WI_FI_CONFIG_MSG_TYPE__TypeRespSetConfig:
            {
                free(resp->resp_set_config);
            }
            break;
        case WI_FI_CONFIG_MSG_TYPE__TypeRespApplyConfig:
            {
                free(resp->resp_apply_config);
            }
            break;
        default:
            ESP_LOGE(TAG, "Unsupported response type in cleanup_handler");
            break;
    }
    return;
}

static esp_err_t wifi_prov_config_command_dispatcher(WiFiConfigPayload *req,
                                                     WiFiConfigPayload *resp, void *priv_data)
{
    esp_err_t ret;

    ESP_LOGD(TAG, "In wifi_prov_config_command_dispatcher Cmd=%d", req->msg);

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

esp_err_t wifi_prov_config_data_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen,
                                        uint8_t **outbuf, ssize_t *outlen, void *priv_data)
{
    WiFiConfigPayload *req;
    WiFiConfigPayload resp;
    esp_err_t ret;

    req = wi_fi_config_payload__unpack(NULL, inlen, inbuf);
    if (!req) {
        ESP_LOGE(TAG, "Unable to unpack config data");
        return ESP_ERR_INVALID_ARG;
    }

    wi_fi_config_payload__init(&resp);
    ret = wifi_prov_config_command_dispatcher(req, &resp, priv_data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Proto command dispatcher error %d", ret);
        return ESP_FAIL;
    }

    resp.msg = req->msg + 1; /* Response is request + 1 */
    wi_fi_config_payload__free_unpacked(req, NULL);

    *outlen = wi_fi_config_payload__get_packed_size(&resp);
    if (*outlen <= 0) {
        ESP_LOGE(TAG, "Invalid encoding for response");
        return ESP_FAIL;
    }

    *outbuf = (uint8_t *) malloc(*outlen);
    if (!*outbuf) {
        ESP_LOGE(TAG, "System out of memory");
        return ESP_ERR_NO_MEM;
    }
    wi_fi_config_payload__pack(&resp, *outbuf);
    wifi_prov_config_command_cleanup(&resp, priv_data);

    return ESP_OK;
}
