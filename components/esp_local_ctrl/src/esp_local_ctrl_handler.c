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
#include <string.h>
#include <esp_err.h>
#include <esp_log.h>

#include "esp_local_ctrl.h"
#include "esp_local_ctrl_priv.h"
#include "esp_local_ctrl.pb-c.h"

#define SAFE_ALLOCATION(type, var)                  \
    type *var = (type *) malloc(sizeof(type));      \
    if (!var) {                                     \
        ESP_LOGE(TAG, "Error allocating memory");   \
        return ESP_ERR_NO_MEM;                      \
    }

static const char* TAG = "esp_local_ctrl_handler";

typedef struct esp_local_ctrl_cmd {
    int cmd_num;
    esp_err_t (*command_handler)(LocalCtrlMessage *req,
                                 LocalCtrlMessage *resp, void **ctx);
} esp_local_ctrl_cmd_t;

static esp_err_t cmd_get_prop_count_handler(LocalCtrlMessage *req,
                                            LocalCtrlMessage *resp, void **ctx);

static esp_err_t cmd_get_prop_vals_handler(LocalCtrlMessage *req,
                                           LocalCtrlMessage *resp, void **ctx);

static esp_err_t cmd_set_prop_vals_handler(LocalCtrlMessage *req,
                                           LocalCtrlMessage *resp, void **ctx);

static esp_local_ctrl_cmd_t cmd_table[] = {
    {
        .cmd_num = LOCAL_CTRL_MSG_TYPE__TypeCmdGetPropertyCount,
        .command_handler = cmd_get_prop_count_handler
    },
    {
        .cmd_num = LOCAL_CTRL_MSG_TYPE__TypeCmdGetPropertyValues,
        .command_handler = cmd_get_prop_vals_handler
    },
    {
        .cmd_num = LOCAL_CTRL_MSG_TYPE__TypeCmdSetPropertyValues,
        .command_handler = cmd_set_prop_vals_handler
    }
};

static uint16_t err_to_status(esp_err_t err)
{
    uint16_t status;
    switch (err) {
        case ESP_OK:
            status = STATUS__Success;
            break;
        case ESP_ERR_INVALID_ARG:
            status = STATUS__InvalidArgument;
            break;
        case ESP_ERR_INVALID_STATE:
            status = STATUS__InvalidProto;
            break;
        default:
            status = STATUS__InternalError;
    }
    return status;
}

static esp_err_t cmd_get_prop_count_handler(LocalCtrlMessage *req,
                                            LocalCtrlMessage *resp, void **ctx)
{
    SAFE_ALLOCATION(RespGetPropertyCount, resp_payload);
    resp_get_property_count__init(resp_payload);

    size_t prop_count = 0;
    resp_payload->status = err_to_status(esp_local_ctrl_get_prop_count(&prop_count));
    resp_payload->count = prop_count;
    resp->payload_case = LOCAL_CTRL_MESSAGE__PAYLOAD_RESP_GET_PROP_COUNT;
    resp->resp_get_prop_count = resp_payload;
    ESP_LOGD(TAG, "Got properties count %d", prop_count);
    return ESP_OK;
}

typedef void (*prop_val_free_fn_t)(void *val);

static esp_err_t cmd_get_prop_vals_handler(LocalCtrlMessage *req,
                                           LocalCtrlMessage *resp, void **ctx)
{
    SAFE_ALLOCATION(RespGetPropertyValues, resp_payload);
    resp_get_property_values__init(resp_payload);

    esp_local_ctrl_prop_val_t *vals = calloc(req->cmd_get_prop_vals->n_indices,
                                             sizeof(esp_local_ctrl_prop_val_t));
    esp_local_ctrl_prop_t *descs = calloc(req->cmd_get_prop_vals->n_indices,
                                          sizeof(esp_local_ctrl_prop_t));
    prop_val_free_fn_t *free_fns = calloc(req->cmd_get_prop_vals->n_indices,
                                          sizeof(prop_val_free_fn_t));
    resp_payload->props = calloc(req->cmd_get_prop_vals->n_indices,
                                 sizeof(PropertyInfo *));
    if (!vals || !descs || !free_fns || !resp_payload->props) {
        ESP_LOGE(TAG, "Failed to allocate memory for getting values");
        free(vals);
        free(descs);
        free(free_fns);
        free(resp_payload->props);
        free(resp_payload);
        return ESP_ERR_NO_MEM;
    }

    esp_err_t ret = esp_local_ctrl_get_prop_values(req->cmd_get_prop_vals->n_indices,
                                                   req->cmd_get_prop_vals->indices,
                                                   descs, vals);
    resp_payload->status = err_to_status(ret);
    if (ret == ESP_OK) {
        resp_payload->n_props = 0;
        for (size_t i = 0; i < req->cmd_get_prop_vals->n_indices; i++) {
            resp_payload->props[i] = malloc(sizeof(PropertyInfo));
            if (!resp_payload->props[i]) {
                resp_payload->status = STATUS__InternalError;
                break;
            }
            resp_payload->n_props++;
            property_info__init(resp_payload->props[i]);
            resp_payload->props[i]->name  = descs[i].name;
            resp_payload->props[i]->type  = descs[i].type;
            resp_payload->props[i]->flags = descs[i].flags;
            resp_payload->props[i]->value.data = vals[i].data;
            resp_payload->props[i]->value.len  = vals[i].size;
            free_fns[i] = vals[i].free_fn;
        }
    }
    resp->payload_case = LOCAL_CTRL_MESSAGE__PAYLOAD_RESP_GET_PROP_VALS;
    resp->resp_get_prop_vals = resp_payload;
    (*ctx) = (void *)free_fns;
    free(vals);
    free(descs);

    /* Unless it's a fatal error, always return ESP_OK, otherwise
     * the underlying connection will be closed by protocomm */
    return ESP_OK;
}

static esp_err_t cmd_set_prop_vals_handler(LocalCtrlMessage *req,
                                           LocalCtrlMessage *resp, void **ctx)
{
    SAFE_ALLOCATION(RespSetPropertyValues, resp_payload);
    resp_set_property_values__init(resp_payload);

    uint32_t *idxs = calloc(req->cmd_set_prop_vals->n_props, sizeof(uint32_t));
    esp_local_ctrl_prop_val_t *vals = calloc(req->cmd_set_prop_vals->n_props,
                                             sizeof(esp_local_ctrl_prop_val_t));
    if (!idxs || !vals) {
        ESP_LOGE(TAG, "Failed to allocate memory for setting values");
        free(idxs);
        free(vals);
        free(resp_payload);
        return ESP_ERR_NO_MEM;
    }
    for (size_t i = 0; i < req->cmd_set_prop_vals->n_props; i++) {
        idxs[i]      = req->cmd_set_prop_vals->props[i]->index;
        vals[i].data = req->cmd_set_prop_vals->props[i]->value.data;
        vals[i].size = req->cmd_set_prop_vals->props[i]->value.len;
    }

    esp_err_t ret = esp_local_ctrl_set_prop_values(req->cmd_set_prop_vals->n_props,
                                                   idxs, vals);
    resp_payload->status = err_to_status(ret);
    resp->payload_case = LOCAL_CTRL_MESSAGE__PAYLOAD_RESP_SET_PROP_VALS;
    resp->resp_set_prop_vals = resp_payload;
    free(idxs);
    free(vals);

    /* Unless it's a fatal error, always return ESP_OK, otherwise
     * the underlying connection will be closed by protocomm */
    return ESP_OK;
}

static int lookup_cmd_handler(int cmd_id)
{
    for (size_t i = 0; i < sizeof(cmd_table)/sizeof(esp_local_ctrl_cmd_t); i++) {
        if (cmd_table[i].cmd_num == cmd_id) {
            return i;
        }
    }

    return -1;
}

static void esp_local_ctrl_command_cleanup(LocalCtrlMessage *resp, void **ctx)
{
    if (!resp) {
        return;
    }

    switch (resp->msg) {
        case LOCAL_CTRL_MSG_TYPE__TypeRespGetPropertyCount:
            free(resp->resp_get_prop_count);
            break;
        case LOCAL_CTRL_MSG_TYPE__TypeRespGetPropertyValues: {
                if (resp->resp_get_prop_vals) {
                    prop_val_free_fn_t *free_fns = (prop_val_free_fn_t *)(*ctx);
                    for (size_t i = 0; i < resp->resp_get_prop_vals->n_props; i++) {
                        if (free_fns && free_fns[i]) {
                            free_fns[i](resp->resp_get_prop_vals->props[i]->value.data);
                        }
                        free(resp->resp_get_prop_vals->props[i]);
                    }
                    free(free_fns);
                    free(resp->resp_get_prop_vals->props);
                    free(resp->resp_get_prop_vals);
                }
            }
            break;
        case LOCAL_CTRL_MSG_TYPE__TypeRespSetPropertyValues:
            free(resp->resp_set_prop_vals);
            break;
        default:
            ESP_LOGE(TAG, "Unsupported response type in cleanup_handler");
    }
    return;
}

static esp_err_t esp_local_ctrl_command_dispatcher(LocalCtrlMessage *req,
                                                   LocalCtrlMessage *resp,
                                                   void **ctx)
{
    int cmd_index = lookup_cmd_handler(req->msg);
    if (cmd_index < 0) {
        ESP_LOGE(TAG, "Invalid command handler lookup");
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = cmd_table[cmd_index].command_handler(req, resp, ctx);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error executing command handler");
        return ret;
    }

    return ESP_OK;
}

esp_err_t esp_local_ctrl_data_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen,
                                      uint8_t **outbuf, ssize_t *outlen, void *priv_data)
{
    void *temp_ctx = NULL;
    LocalCtrlMessage *req = local_ctrl_message__unpack(NULL, inlen, inbuf);
    if (!req) {
        ESP_LOGE(TAG, "Unable to unpack payload data");
        return ESP_ERR_INVALID_ARG;
    }

    LocalCtrlMessage resp;
    local_ctrl_message__init(&resp);
    resp.msg = req->msg + 1; /* Response is request + 1 */

    esp_err_t ret = esp_local_ctrl_command_dispatcher(req, &resp, &temp_ctx);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "command dispatcher failed");
        esp_local_ctrl_command_cleanup(&resp, &temp_ctx);
        local_ctrl_message__free_unpacked(req, NULL);
        return ESP_FAIL;
    }

    local_ctrl_message__free_unpacked(req, NULL);

    *outlen = local_ctrl_message__get_packed_size(&resp);
    if (*outlen <= 0) {
        ESP_LOGE(TAG, "Invalid encoding for response");
        esp_local_ctrl_command_cleanup(&resp, &temp_ctx);
        return ESP_FAIL;
    }

    *outbuf = (uint8_t *) malloc(*outlen);
    if (!*outbuf) {
        ESP_LOGE(TAG, "System out of memory");
        esp_local_ctrl_command_cleanup(&resp, &temp_ctx);
        return ESP_ERR_NO_MEM;
    }

    local_ctrl_message__pack(&resp, *outbuf);
    esp_local_ctrl_command_cleanup(&resp, &temp_ctx);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, *outbuf, *outlen, ESP_LOG_DEBUG);
    return ESP_OK;
}
