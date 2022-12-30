// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "esp_timer.h"
#include "ble_mesh_adapter.h"
#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_networking_api.h"

#if (CONFIG_BLE_MESH_GENERIC_ONOFF_CLI)
typedef struct {
    struct arg_str *action_type;
    struct arg_int *op_en;
    struct arg_int *unicast_address;
    struct arg_int *onoff_state;
    struct arg_int *trans_id;
    struct arg_int *trans_time;
    struct arg_int *delay;
    struct arg_int *opcode;
    struct arg_int *appkey_idx;
    struct arg_int *role;
    struct arg_int *net_idx;
    struct arg_end *end;
} ble_mesh_gen_onoff_state_t;
static ble_mesh_gen_onoff_state_t gen_onoff_state;

void ble_mesh_register_gen_onoff_client_command(void);

void ble_mesh_register_gen_onoff_client(void)
{
    ble_mesh_register_gen_onoff_client_command();
}

void ble_mesh_generic_onoff_client_model_cb(esp_ble_mesh_generic_client_cb_event_t event,
                                            esp_ble_mesh_generic_client_cb_param_t *param)
{
    uint32_t opcode = param->params->opcode;

    ESP_LOGD(TAG, "enter %s: event is %d, error code is %d, opcode is 0x%x\n",
             __func__, event, param->error_code, opcode);

    switch (event) {
    case ESP_BLE_MESH_GENERIC_CLIENT_GET_STATE_EVT: {
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET:
            if (param->error_code == ESP_OK) {
                ESP_LOGI(TAG, "GenOnOffClient:GetStatus,OK,%d", param->status_cb.onoff_status.present_onoff);
            } else {
                ESP_LOGE(TAG, "GenOnOffClient:GetStatus,Fail,%d", param->error_code);
            }
            break;
        default:
            break;
        }
        break;
    }
    case ESP_BLE_MESH_GENERIC_CLIENT_SET_STATE_EVT: {
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET:
            if (param->error_code == ESP_OK) {
                ESP_LOGI(TAG, "GenOnOffClient:SetStatus,OK,%d", param->status_cb.onoff_status.present_onoff);
            } else {
                ESP_LOGE(TAG, "GenOnOffClient:SetStatus,Fail,%d", param->error_code);
            }
            break;
        case ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK:
            if (param->error_code == ESP_OK) {
                ESP_LOGI(TAG, "GenOnOffClient:SetUNACK,OK");
            } else {
                ESP_LOGE(TAG, "GenOnOffClient:SetUNACK,Fail,%d", param->error_code);
            }
            break;
        default:
            break;
        }
        break;
    }
    case ESP_BLE_MESH_GENERIC_CLIENT_PUBLISH_EVT: {
        if (param->error_code == ESP_OK) {
            ESP_LOGI(TAG, "GenOnOffClient:Publish,OK");
        } else {
            ESP_LOGE(TAG, "GenOnOffClient:Publish,Fail,%d", param->error_code);
        }
        break;
    }
    case ESP_BLE_MESH_GENERIC_CLIENT_TIMEOUT_EVT:
        ESP_LOGE(TAG, "GenOnOffClient:TimeOut,%d", param->error_code);
        break;
    case ESP_BLE_MESH_GENERIC_CLIENT_EVT_MAX:
        ESP_LOGE(TAG, "GenONOFFClient:InvalidEvt,%d", param->error_code);
        break;
    default:
        break;
    }
    ESP_LOGD(TAG, "exit %s", __func__);
}

void ble_mesh_generic_server_model_cb(esp_ble_mesh_generic_server_cb_event_t event,
                                      esp_ble_mesh_generic_server_cb_param_t *param)
{
    uint32_t opcode = param->ctx.recv_op;
    uint8_t status;

    ESP_LOGD(TAG, "enter %s: event is %d, opcode is 0x%04x", __func__, event, opcode);

    switch (event) {
        case ESP_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT:
            if (opcode == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET) {
                ESP_LOGI(TAG, "GenOnOffServer:Set,OK,%d", param->value.state_change.onoff_set.onoff);
                ble_mesh_node_set_state(param->value.state_change.onoff_set.onoff);
            } else if (opcode == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK) {
                ESP_LOGI(TAG, "GenOnOffServer:SetUnAck,OK,%d", param->value.state_change.onoff_set.onoff);
                ble_mesh_node_set_state(param->value.state_change.onoff_set.onoff);
            }
            break;
    case ESP_BLE_MESH_GENERIC_SERVER_RECV_GET_MSG_EVT: {
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET:
            ESP_LOGI(TAG, "GenOnOffServer:Get,OK");
            ble_mesh_node_get_state(status);
            esp_ble_mesh_server_model_send_msg(param->model, &param->ctx, ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS,
                                                   sizeof(status), &status);
            break;
        default:
            break;
        }
        break;
    }
    case ESP_BLE_MESH_GENERIC_SERVER_RECV_SET_MSG_EVT: {
        if (opcode == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET || opcode == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK) {
            esp_ble_mesh_gen_onoff_srv_t *srv = param->model->user_data;
            if (param->value.set.onoff.op_en == false) {
                srv->state.onoff = param->value.set.onoff.onoff;
            } else {
                /* TODO: Delay and state transition */
               srv->state.onoff = param->value.set.onoff.onoff;
            }
        }

        switch (opcode) {
            case ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET:
                ESP_LOGI(TAG, "GenOnOffServer:Set,OK,%d", param->value.set.onoff.onoff);
                ble_mesh_node_set_state(param->value.set.onoff.onoff);
                ble_mesh_node_get_state(status);
                esp_ble_mesh_server_model_send_msg(param->model, &param->ctx, ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS,
                                                   sizeof(status), &status);
                break;
            case ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK:
                ble_mesh_node_set_state(param->value.set.onoff.onoff);
                ESP_LOGI(TAG, "GenOnOffServer:SetUnAck,OK,%d", param->value.set.onoff.onoff);
                break;
            default:
                break;
        }
        break;
    }
    default:
        break;
    }
    ESP_LOGD(TAG, "exit %s", __func__);
}

int ble_mesh_generic_onoff_client_model(int argc, char **argv)
{
    int err = ESP_OK;
    esp_ble_mesh_elem_t *element = NULL;
    esp_ble_mesh_generic_client_set_state_t gen_client_set;
    esp_ble_mesh_generic_client_get_state_t gen_client_get;
    esp_ble_mesh_client_common_param_t onoff_common = {
        .msg_timeout = 0,
        .ctx.send_ttl = 7,
    };

    int nerrors = arg_parse(argc, argv, (void **) &gen_onoff_state);
    if (nerrors != 0) {
        arg_print_errors(stderr, gen_onoff_state.end, argv[0]);
        return 1;
    }

    element = esp_ble_mesh_find_element(esp_ble_mesh_get_primary_element_address());
    if (!element) {
        ESP_LOGE(TAG, "Element 0x%04x not exists", esp_ble_mesh_get_primary_element_address());
        return ESP_FAIL;
    }

    onoff_common.model = esp_ble_mesh_find_sig_model(element, ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_CLI);
    if (!onoff_common.model) {
        ESP_LOGI(TAG, "GenONOFFClient:LoadModel,Fail");
        return ESP_FAIL;
    }

    arg_int_to_value(gen_onoff_state.appkey_idx, onoff_common.ctx.app_idx, "appkey_index");
    arg_int_to_value(gen_onoff_state.opcode, onoff_common.opcode, "opcode");
    arg_int_to_value(gen_onoff_state.role, onoff_common.msg_role, "role");
    arg_int_to_value(gen_onoff_state.unicast_address, onoff_common.ctx.addr, "address");
    arg_int_to_value(gen_onoff_state.net_idx, onoff_common.ctx.net_idx, "network key index");
    arg_int_to_value(gen_onoff_state.op_en, gen_client_set.onoff_set.op_en, "op_en");
    arg_int_to_value(gen_onoff_state.onoff_state, gen_client_set.onoff_set.onoff, "onoff");
    arg_int_to_value(gen_onoff_state.trans_id, gen_client_set.onoff_set.tid, "tid");
    arg_int_to_value(gen_onoff_state.trans_time, gen_client_set.onoff_set.trans_time, "trans_time");
    arg_int_to_value(gen_onoff_state.delay, gen_client_set.onoff_set.delay, "delay");

    if (gen_onoff_state.action_type->count != 0) {
        if (strcmp(gen_onoff_state.action_type->sval[0], "get") == 0) {
            err = esp_ble_mesh_generic_client_get_state(&onoff_common, &gen_client_get);
        } 
        else if (strcmp(gen_onoff_state.action_type->sval[0], "set") == 0) {
            err = esp_ble_mesh_generic_client_set_state(&onoff_common, &gen_client_set);
        }
    }
    return err;
}

void ble_mesh_register_gen_onoff_client_command(void)
{
    gen_onoff_state.action_type = arg_str1("z", NULL, "<action>", "action type");
    gen_onoff_state.opcode = arg_int0("o", NULL, "<opcode>", "message opcode");
    gen_onoff_state.appkey_idx = arg_int0("a", NULL, "<appkey>", "appkey index");
    gen_onoff_state.role = arg_int0("r", NULL, "<role>", "role");
    gen_onoff_state.unicast_address = arg_int0("u", NULL, "<address>", "unicast address");
    gen_onoff_state.net_idx = arg_int0("n", NULL, "<netkey index>", "network key index");
    gen_onoff_state.op_en = arg_int0("e", NULL, "<optional>", "whether optional parameters included");
    gen_onoff_state.onoff_state = arg_int0("s", NULL, "<state>", "present onoff state");
    gen_onoff_state.trans_id = arg_int0("i", NULL, "<identifier>", "transaction identifier");
    gen_onoff_state.trans_time = arg_int0("t", NULL, "<time>", "time to complete state transition");
    gen_onoff_state.delay = arg_int0("d", NULL, "<delay>", "indicate message execution delay");
    gen_onoff_state.end = arg_end(1);

    const esp_console_cmd_t gen_onoff_state_cmd = {
        .command = "bmgocm",
        .help = "ble mesh generic onoff client model",
        .hint = NULL,
        .func = &ble_mesh_generic_onoff_client_model,
        .argtable = &gen_onoff_state,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&gen_onoff_state_cmd));
}

#endif
