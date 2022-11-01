/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <esp_log.h>
#include <string.h>
#include <esp_err.h>

#include "wifi_ctrl.pb-c.h"

#include "wifi_ctrl.h"

static const char *TAG = "proto_wifi_ctrl";

typedef struct wifi_ctrl_cmd {
    int cmd_id;
    esp_err_t (*command_handler)(WiFiCtrlPayload *req,
                                 WiFiCtrlPayload *resp, void *priv_data);
} wifi_ctrl_cmd_t;

static esp_err_t cmd_ctrl_reset_handler(WiFiCtrlPayload *req,
                                        WiFiCtrlPayload *resp,
                                        void *priv_data);

static esp_err_t cmd_ctrl_reprov_handler(WiFiCtrlPayload *req,
                                        WiFiCtrlPayload *resp,
                                        void *priv_data);

static wifi_ctrl_cmd_t cmd_table[] = {
    {
        .cmd_id = WI_FI_CTRL_MSG_TYPE__TypeCmdCtrlReset,
        .command_handler = cmd_ctrl_reset_handler
    },
    {
        .cmd_id = WI_FI_CTRL_MSG_TYPE__TypeCmdCtrlReprov,
        .command_handler = cmd_ctrl_reprov_handler
    },
};

static esp_err_t cmd_ctrl_reset_handler(WiFiCtrlPayload *req,
                                        WiFiCtrlPayload *resp, void *priv_data)
{
    wifi_ctrl_handlers_t *h = (wifi_ctrl_handlers_t *) priv_data;
    if (!h) {
        ESP_LOGE(TAG, "Command invoked without handlers");
        return ESP_ERR_INVALID_STATE;
    }

    RespCtrlReset *resp_payload = (RespCtrlReset *) malloc(sizeof(RespCtrlReset));
    if (!resp_payload) {
        ESP_LOGE(TAG, "Error allocating memory");
        return ESP_ERR_NO_MEM;
    }

    resp_ctrl_reset__init(resp_payload);
    resp->status = (h->ctrl_reset() == ESP_OK ?
                            STATUS__Success : STATUS__InternalError);
    resp->payload_case = WI_FI_CTRL_PAYLOAD__PAYLOAD_RESP_CTRL_RESET;
    resp->resp_ctrl_reset = resp_payload;
    return ESP_OK;
}

static esp_err_t cmd_ctrl_reprov_handler(WiFiCtrlPayload *req,
                                        WiFiCtrlPayload *resp, void *priv_data)
{
    wifi_ctrl_handlers_t *h = (wifi_ctrl_handlers_t *) priv_data;
    if (!h) {
        ESP_LOGE(TAG, "Command invoked without handlers");
        return ESP_ERR_INVALID_STATE;
    }

    RespCtrlReprov *resp_payload = (RespCtrlReprov *) malloc(sizeof(RespCtrlReprov));
    if (!resp_payload) {
        ESP_LOGE(TAG, "Error allocating memory");
        return ESP_ERR_NO_MEM;
    }

    resp_ctrl_reprov__init(resp_payload);
    resp->status = (h->ctrl_reprov() == ESP_OK ?
                            STATUS__Success : STATUS__InternalError);
    resp->payload_case = WI_FI_CTRL_PAYLOAD__PAYLOAD_RESP_CTRL_REPROV;
    resp->resp_ctrl_reprov = resp_payload;
    return ESP_OK;
}

static int lookup_cmd_handler(int cmd_id)
{
    for (size_t i = 0; i < sizeof(cmd_table)/sizeof(wifi_ctrl_cmd_t); i++) {
        if (cmd_table[i].cmd_id == cmd_id) {
            return i;
        }
    }

    return -1;
}

static void wifi_ctrl_cmd_cleanup(WiFiCtrlPayload *resp, void *priv_data)
{
    switch (resp->msg) {
        case WI_FI_CTRL_MSG_TYPE__TypeRespCtrlReset:
            {
                free(resp->resp_ctrl_reset);
            }
            break;
        case WI_FI_CTRL_MSG_TYPE__TypeRespCtrlReprov:
            {
                free(resp->resp_ctrl_reprov);
            }
            break;
        default:
            ESP_LOGE(TAG, "Unsupported response type in cleanup_handler");
            break;
    }
    return;
}

static esp_err_t wifi_ctrl_cmd_dispatcher(WiFiCtrlPayload *req,
                                               WiFiCtrlPayload *resp, void *priv_data)
{
    esp_err_t ret;

    ESP_LOGD(TAG, "In wifi_ctrl_cmd_dispatcher Cmd=%d", req->msg);

    int cmd_index = lookup_cmd_handler(req->msg);
    if (cmd_index < 0) {
        ESP_LOGE(TAG, "Failed to find cmd with ID = %d in the command table", req->msg);
        return ESP_FAIL;
    }

    ret = cmd_table[cmd_index].command_handler(req, resp, priv_data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error executing command handler");
    }

    return ret;
}

esp_err_t wifi_ctrl_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen,
                                 uint8_t **outbuf, ssize_t *outlen, void *priv_data)
{
    WiFiCtrlPayload *req;
    WiFiCtrlPayload resp;
    esp_err_t ret = ESP_OK;

    req = wi_fi_ctrl_payload__unpack(NULL, inlen, inbuf);
    if (!req) {
        ESP_LOGE(TAG, "Unable to unpack ctrl message");
        return ESP_ERR_INVALID_ARG;
    }

    wi_fi_ctrl_payload__init(&resp);
    ret = wifi_ctrl_cmd_dispatcher(req, &resp, priv_data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Command dispatcher error %02X", ret);
        ret = ESP_FAIL;
        goto exit;
    }

    resp.msg = req->msg + 1; /* Response is request + 1 */
    *outlen = wi_fi_ctrl_payload__get_packed_size(&resp);
    if (*outlen <= 0) {
        ESP_LOGE(TAG, "Invalid encoding for response");
        ret = ESP_FAIL;
        goto exit;
    }

    *outbuf = (uint8_t *) malloc(*outlen);
    if (!*outbuf) {
        ESP_LOGE(TAG, "Failed to allocate memory for the output buffer");
        ret = ESP_ERR_NO_MEM;
        goto exit;
    }
    wi_fi_ctrl_payload__pack(&resp, *outbuf);
    ESP_LOGD(TAG, "Response packet size : %d", *outlen);
    exit:

    wi_fi_ctrl_payload__free_unpacked(req, NULL);
    wifi_ctrl_cmd_cleanup(&resp, priv_data);
    return ret;
}
