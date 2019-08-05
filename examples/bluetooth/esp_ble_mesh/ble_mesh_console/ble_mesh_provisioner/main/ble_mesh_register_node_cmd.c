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

#include "soc/soc.h"
#include "esp_bt.h"
#include "esp_bt_device.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_config_model_api.h"

#include "ble_mesh_adapter.h"

typedef struct {
    struct arg_str *static_val;
    struct arg_int *static_val_len;
    struct arg_int *output_size;
    struct arg_int *output_actions;
    struct arg_int *input_size;
    struct arg_int *input_actions;
    struct arg_int *prov_start_address;
    struct arg_end *end;
} ble_mesh_prov_t;
static ble_mesh_prov_t oob;

typedef struct {
    struct arg_int *model_type;
    struct arg_int *config_index;
    struct arg_int *pub_config;
    struct arg_end *end;
} ble_mesh_comp_t;
static ble_mesh_comp_t component;

typedef struct {
    struct arg_int *bearer;
    struct arg_int *enable;
    struct arg_end *end;
} ble_mesh_bearer_t;
static ble_mesh_bearer_t bearer;

typedef struct {
    struct arg_str *action_type;
    struct arg_int *tx_sense_power;
    struct arg_end *end;
} ble_mesh_tx_sense_power;
static ble_mesh_tx_sense_power power_set;

ble_mesh_node_status node_status = {
    .previous = 0x0,
    .current = 0x0,
};

SemaphoreHandle_t ble_mesh_node_sema;

void ble_mesh_register_node_cmd(void);
// Register callback function
void ble_mesh_prov_cb(esp_ble_mesh_prov_cb_event_t event, esp_ble_mesh_prov_cb_param_t *param);
void ble_mesh_model_cb(esp_ble_mesh_model_cb_event_t event, esp_ble_mesh_model_cb_param_t *param);


void ble_mesh_register_mesh_node(void)
{
    ble_mesh_register_node_cmd();
}

int ble_mesh_register_node_cb(int argc, char** argv)
{
    ESP_LOGD(TAG, "enter %s\n", __func__);
    ble_mesh_node_init();
    esp_ble_mesh_register_prov_callback(ble_mesh_prov_cb);
    esp_ble_mesh_register_custom_model_callback(ble_mesh_model_cb);
    ESP_LOGI(TAG, "Node:Reg,OK");
    ESP_LOGD(TAG, "exit %s\n", __func__);
    return 0;
}

void ble_mesh_prov_cb(esp_ble_mesh_prov_cb_event_t event, esp_ble_mesh_prov_cb_param_t *param)
{
    ESP_LOGD(TAG, "enter %s, event = %d", __func__, event);
    switch (event) {
    case ESP_BLE_MESH_PROV_REGISTER_COMP_EVT:
        ble_mesh_callback_check_err_code(param->prov_register_comp.err_code, "Provisioning:Register");
        break;
    case ESP_BLE_MESH_NODE_PROV_ENABLE_COMP_EVT:
        ble_mesh_callback_check_err_code(param->node_prov_enable_comp.err_code, "Node:EnBearer");
        break;
    case ESP_BLE_MESH_NODE_PROV_DISABLE_COMP_EVT:
        ble_mesh_callback_check_err_code(param->node_prov_disable_comp.err_code, "Node:DisBearer");
        break;
    case ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT:
        ESP_LOGI(TAG, "Node:LinkOpen,OK,%d", param->node_prov_link_open.bearer);
        break;
    case ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT:
        ESP_LOGI(TAG, "Node:LinkClose,OK,%d", param->node_prov_link_close.bearer);
        break;
    case ESP_BLE_MESH_NODE_PROV_OUTPUT_NUMBER_EVT:
        ESP_LOGI(TAG, "Node:OutPut,%d,%d", param->node_prov_output_num.action, param->node_prov_output_num.number);
        break;
    case ESP_BLE_MESH_NODE_PROV_OUTPUT_STRING_EVT:
        ESP_LOGI(TAG, "Node:OutPutStr,%s", param->node_prov_output_str.string);
        break;
    case ESP_BLE_MESH_NODE_PROV_INPUT_EVT:
        ESP_LOGI(TAG, "Node:InPut,%d,%d", param->node_prov_input.action, param->node_prov_input.size);
        break;
    case ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT:
        ESP_LOGI(TAG, "Node:OK,%d,%d", param->node_prov_complete.net_idx, param->node_prov_complete.addr);
        ble_mesh_set_node_prestore_params(param->node_prov_complete.net_idx, param->node_prov_complete.addr);
        break;
    case ESP_BLE_MESH_NODE_PROV_RESET_EVT:
        ESP_LOGI(TAG, "Node:Reset");
        break;
    case ESP_BLE_MESH_NODE_PROV_INPUT_NUMBER_COMP_EVT:
        ble_mesh_callback_check_err_code(param->node_prov_input_num_comp.err_code, "Node:InputNum");
        break;
    case ESP_BLE_MESH_NODE_PROV_INPUT_STRING_COMP_EVT:
        ble_mesh_callback_check_err_code(param->node_prov_input_str_comp.err_code, "Node:InputStr");
        break;
    case ESP_BLE_MESH_NODE_SET_UNPROV_DEV_NAME_COMP_EVT:
        ble_mesh_callback_check_err_code(param->node_set_unprov_dev_name_comp.err_code, "Node:SetName");
        break;
    case ESP_BLE_MESH_NODE_PROXY_IDENTITY_ENABLE_COMP_EVT:
        ble_mesh_callback_check_err_code(param->node_proxy_identity_enable_comp.err_code, "Node:ProxyIndentity");
        break;
    case ESP_BLE_MESH_NODE_PROXY_GATT_ENABLE_COMP_EVT:
        ble_mesh_callback_check_err_code(param->node_proxy_gatt_enable_comp.err_code, "Node:EnProxyGatt");
        break;
    case ESP_BLE_MESH_NODE_PROXY_GATT_DISABLE_COMP_EVT:
        ble_mesh_callback_check_err_code(param->node_proxy_gatt_disable_comp.err_code, "Node:DisProxyGatt");
        break;
#if (CONFIG_BLE_MESH_PROVISIONER)
    case ESP_BLE_MESH_PROVISIONER_RECV_UNPROV_ADV_PKT_EVT:
        ESP_LOGI(TAG, "Provisioner recv unprovisioned device beacon:");
        ESP_LOG_BUFFER_HEX("Device UUID %s", param->provisioner_recv_unprov_adv_pkt.dev_uuid, 16);
        ESP_LOG_BUFFER_HEX("Address %s", param->provisioner_recv_unprov_adv_pkt.addr, 6);
        ESP_LOGI(TAG, "Address type 0x%x, oob_info 0x%04x, adv_type 0x%x, bearer 0x%x",
            param->provisioner_recv_unprov_adv_pkt.addr_type, param->provisioner_recv_unprov_adv_pkt.oob_info,
            param->provisioner_recv_unprov_adv_pkt.adv_type, param->provisioner_recv_unprov_adv_pkt.bearer);
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT:
        ESP_LOGI(TAG, "Provisioner:LinkOpen,OK,%d", param->provisioner_prov_link_open.bearer);
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_LINK_CLOSE_EVT:
        ESP_LOGI(TAG, "Provisioner:LinkClose,OK,%d,%d",
                 param->provisioner_prov_link_close.bearer, param->provisioner_prov_link_close.reason);
        break;
    case ESP_BLE_MESH_PROVISIONER_ADD_UNPROV_DEV_COMP_EVT:
        ble_mesh_callback_check_err_code(param->provisioner_add_unprov_dev_comp.err_code, "Provisioner:DevAdd");
        break;
    case ESP_BLE_MESH_PROVISIONER_DELETE_DEV_COMP_EVT:
        ble_mesh_callback_check_err_code(param->provisioner_delete_dev_comp.err_code, "Provisioner:DevDel");
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT:
        ESP_LOGI(TAG, "Provisioner:OK,%d,%d", param->provisioner_prov_complete.netkey_idx, param->provisioner_prov_complete.unicast_addr);
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_ENABLE_COMP_EVT:
        ble_mesh_callback_check_err_code(param->provisioner_prov_enable_comp.err_code, "Provisioner:EnBearer");
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_DISABLE_COMP_EVT:
        ble_mesh_callback_check_err_code(param->provisioner_prov_disable_comp.err_code, "Provisioner:DisBearer");
        break;
    case ESP_BLE_MESH_PROVISIONER_SET_DEV_UUID_MATCH_COMP_EVT:
        ble_mesh_callback_check_err_code(param->provisioner_set_dev_uuid_match_comp.err_code, "Provisioner:UuidMatch");
        break;
    case ESP_BLE_MESH_PROVISIONER_SET_PROV_DATA_INFO_COMP_EVT:
        ble_mesh_callback_check_err_code(param->provisioner_set_prov_data_info_comp.err_code, "Provisioner:DataInfo");
        break;
    case ESP_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT:
        ble_mesh_callback_check_err_code(param->provisioner_set_node_name_comp.err_code, "Provisioner:NodeName");
        break;
    case ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT:
        ble_mesh_callback_check_err_code(param->provisioner_add_app_key_comp.err_code, "Provisioner:AppKeyAdd");
        break;
    case ESP_BLE_MESH_PROVISIONER_BIND_APP_KEY_TO_MODEL_COMP_EVT:
        ble_mesh_callback_check_err_code(param->provisioner_bind_app_key_to_model_comp.err_code, "Provisioner:AppKeyBind");
        break;
    case ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_NET_KEY_COMP_EVT:
        ble_mesh_callback_check_err_code(param->provisioner_add_net_key_comp.err_code, "Provisioner:NetKeyAdd");
        break;
#endif
    default:
        break;
    }
    ESP_LOGD(TAG, "exit %s\n", __func__);
}

void ble_mesh_model_cb(esp_ble_mesh_model_cb_event_t event, esp_ble_mesh_model_cb_param_t *param)
{
    esp_err_t result = ESP_OK;
    uint8_t status;

    ESP_LOGD(TAG, "enter %s, event=%x\n", __func__, event);

    switch (event) {
    case ESP_BLE_MESH_MODEL_OPERATION_EVT:
        if (param->model_operation.model != NULL && param->model_operation.model->op != NULL) {
            if (param->model_operation.opcode == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET) {
                ESP_LOGI(TAG, "Node:GetStatus,OK");
                ble_mesh_node_get_state(status);
                result = esp_ble_mesh_server_model_send_msg(param->model_operation.model, param->model_operation.ctx, ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS,
                         sizeof(status), &status);
            } else if (param->model_operation.opcode == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET) {
                ble_mesh_node_set_state(param->model_operation.msg[0]);
                ESP_LOGI(TAG, "Node:SetAck,OK,%d,%d", param->model_operation.msg[0], param->model_operation.ctx->recv_ttl);
                result = esp_ble_mesh_server_model_send_msg(param->model_operation.model, param->model_operation.ctx, ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS,
                         sizeof(status), param->model_operation.msg);
            } else if (param->model_operation.opcode == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK) {
                ble_mesh_node_set_state(param->model_operation.msg[0]);
                ESP_LOGI(TAG, "Node:SetUnAck,OK,%d,%d", param->model_operation.msg[0], param->model_operation.ctx->recv_ttl);
            } else if (param->model_operation.opcode == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS) {
                ESP_LOGI(TAG, "Node:Status,Success,%d", param->model_operation.length);
            } else if (param->model_operation.opcode == ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_SET) {
                ESP_LOGI(TAG, "VendorModel:SetAck,OK,%d", param->model_operation.ctx->recv_ttl);
            } else if (param->model_operation.opcode == ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_STATUS) {
                uint64_t current_time = esp_timer_get_time();
                result = ble_mesh_test_performance_client_model_accumulate_time(((uint32_t)(current_time - start_time) / 1000), param->model_operation.msg, param->model_operation.ctx->recv_ttl, param->model_operation.length);
                ESP_LOGI(TAG, "VendorModel:Status,OK,%d", param->model_operation.ctx->recv_ttl);
                if (ble_mesh_test_perf_send_sema != NULL && result == ESP_OK) {
                    xSemaphoreGive(ble_mesh_test_perf_send_sema);
                }
            }
        }
        break;
    case ESP_BLE_MESH_MODEL_SEND_COMP_EVT:
        if (param->model_send_comp.err_code == ESP_OK) {
            ESP_LOGI(TAG, "Node:ModelSend,OK");
        } else {
            ESP_LOGE(TAG, "Node:ModelSend,Fail,%d,0x%X,0x%04X", param->model_send_comp.err_code, param->model_send_comp.model->model_id, param->model_send_comp.model->op->opcode);
        }
        break;
    case ESP_BLE_MESH_MODEL_PUBLISH_COMP_EVT:
        ESP_LOGI(TAG, "Node:PublishSend,OK,0x%X,%d", param->model_publish_comp.model->model_id, param->model_publish_comp.model->pub->msg->len);
        break;
    case ESP_BLE_MESH_CLIENT_MODEL_RECV_PUBLISH_MSG_EVT:
        ESP_LOGI(TAG, "Node:PublishReceive,OK,0x%04X,%d,%d", param->client_recv_publish_msg.opcode, param->client_recv_publish_msg.length, param->client_recv_publish_msg.msg[1]);
        uint64_t current_time = esp_timer_get_time();
        result = ble_mesh_test_performance_client_model_accumulate_time(((uint32_t)(current_time - start_time) / 2000), param->client_recv_publish_msg.msg, param->client_recv_publish_msg.ctx->recv_ttl, param->client_recv_publish_msg.length);
        if (ble_mesh_test_perf_send_sema != NULL && param->client_recv_publish_msg.msg[2] == VENDOR_MODEL_PERF_OPERATION_TYPE_SET_UNACK && result == ESP_OK) {
            xSemaphoreGive(ble_mesh_test_perf_send_sema);
        }
        break;
    case ESP_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT:
        ESP_LOGI(TAG, "Node:PublishUpdate,OK");
        break;
    case ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT:
        ESP_LOGI(TAG, "Node:TimeOut, 0x%04X", param->client_send_timeout.opcode);
        if (ble_mesh_test_perf_send_sema != NULL) {
            xSemaphoreGive(ble_mesh_test_perf_send_sema);
        }
        break;
    case ESP_BLE_MESH_MODEL_EVT_MAX:
        ESP_LOGI(TAG, "Node:MaxEvt");
        break;
    default:
        break;
    }

    ESP_LOGD(TAG, "exit %s\n", __func__);
}

int ble_mesh_power_set(int argc, char **argv)
{
    esp_err_t result = ESP_OK;
    int nerrors = arg_parse(argc, argv, (void **) &power_set);

    ESP_LOGD(TAG, "enter %s\n", __func__);

    if (nerrors != 0) {
        arg_print_errors(stderr, power_set.end, argv[0]);
        return 1;
    }

    if (strcmp(power_set.action_type->sval[0], "tx") == 0) {
        result = esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, power_set.tx_sense_power->ival[0]);
    } else if (strcmp(power_set.action_type->sval[0], "sense") == 0) {
        uint32_t *reg = (uint32_t *)(0x6001c07c);
        int reg_addr = 0x6001c07c;
        uint32_t flag = 0x00FF0000;
        uint32_t sense_new = power_set.tx_sense_power->ival[0];
        uint32_t reg_to_write = ((*reg) &= ~flag) | ((256 - sense_new) << 16);
        REG_WRITE(reg_addr, reg_to_write);

    }

    if (result == ESP_OK) {
        ESP_LOGI(TAG, "Node:SetPower,OK\n");
    }

    ESP_LOGD(TAG, "exit %s\n", __func__);
    return result;
}

static int ble_mesh_load_oob(int argc, char **argv)
{
    uint8_t *static_val;

    ESP_LOGD(TAG, "enter %s \n", __func__);

    int nerrors = arg_parse(argc, argv, (void **) &oob);
    if (nerrors != 0) {
        arg_print_errors(stderr, oob.end, argv[0]);
        return 1;
    }

    //parsing prov
#if CONFIG_BLE_MESH_NODE
    prov.uuid = dev_uuid;
    memcpy(dev_uuid, esp_bt_dev_get_address(), 6);
    if (oob.static_val->count != 0) {
        static_val = malloc(oob.static_val_len->ival[0] + 1);
        if (static_val == NULL) {
            ESP_LOGE(TAG, "malloc fail,%s,%d\n", __func__, __LINE__);
        }
        get_value_string((char *)oob.static_val->sval[0], (char *)static_val);
        prov.static_val = static_val;
    }

    arg_int_to_value(oob.static_val_len, prov.static_val_len, "static value length");
    arg_int_to_value(oob.output_size, prov.output_size, "output size");
    arg_int_to_value(oob.output_actions, prov.output_actions, "output actions");
    arg_int_to_value(oob.input_size, prov.input_size, "input size");
    arg_int_to_value(oob.input_actions, prov.input_actions, "input actions");
#endif

#if CONFIG_BLE_MESH_PROVISIONER
    if (oob.static_val->count != 0) {
        static_val = malloc(oob.static_val_len->ival[0] + 1);
        if (static_val == NULL) {
            ESP_LOGE(TAG, "malloc fail,%s,%d\n", __func__, __LINE__);
        }
        get_value_string((char *)oob.static_val->sval[0], (char *)static_val);
        prov.prov_static_oob_val = static_val;
    }
    arg_int_to_value(oob.prov_start_address, prov.prov_start_address, "provisioner start address");
    arg_int_to_value(oob.static_val_len, prov.prov_static_oob_len, "provisioner static value length");
#endif

    ESP_LOGI(TAG, "OOB:Load,OK\n");

    ESP_LOGD(TAG, "exit %s\n", __func__);
    return 0;
}


int ble_mesh_init(int argc, char **argv)
{
    int err;
    esp_ble_mesh_comp_t *local_component = NULL;

    int nerrors = arg_parse(argc, argv, (void **) &component);
    if (nerrors != 0) {
        arg_print_errors(stderr, component.end, argv[0]);
        return 1;
    }

    ESP_LOGD(TAG, "enter %s, module %x\n", __func__, component.model_type->ival[0]);
    local_component = ble_mesh_get_component(component.model_type->ival[0]);


    err = esp_ble_mesh_init(&prov, local_component);
    if (err) {
        ESP_LOGE(TAG, "Initializing mesh failed (err %d)\n", err);
        return err;
    }

    ESP_LOGD(TAG, "exit %s\n", __func__);
    return err;
}

int ble_mesh_provisioner_enable_bearer(int argc, char **argv)
{
    esp_err_t  err = 0;

    ESP_LOGD(TAG, "enter %s \n", __func__);

    int nerrors = arg_parse(argc, argv, (void **) &bearer);
    if (nerrors != 0) {
        arg_print_errors(stderr, bearer.end, argv[0]);
        return 1;
    }

    if (bearer.enable->count != 0) {
        if (bearer.enable->ival[0]) {
            err = esp_ble_mesh_provisioner_prov_enable(bearer.bearer->ival[0]);
        } else {
            err = esp_ble_mesh_provisioner_prov_disable(bearer.bearer->ival[0]);
        }
    } else {
        return 1;
    }

    ESP_LOGD(TAG, "exit %s\n", __func__);
    return err;
}

void ble_mesh_register_node_cmd(void)
{
    const esp_console_cmd_t register_cmd = {
        .command  = "bmreg",
        .help = "ble mesh: provisioner/node register callback",
        .hint = NULL,
        .func = &ble_mesh_register_node_cb,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&register_cmd));
    oob.static_val = arg_str0("s", NULL, "<value>", "Static OOB value");
    oob.static_val_len = arg_int0("l", NULL, "<length>", "Static OOB value length");
    oob.output_size = arg_int0("x", NULL, "<size>", "Maximum size of Output OOB");
    oob.output_actions = arg_int0("o", NULL, "<actions>", "Supported Output OOB Actions");
    oob.input_size = arg_int0("y", NULL, "<size>", "Maximum size of Input OOB");
    oob.input_actions = arg_int0("i", NULL, "<actions>", "Supported Input OOB Actions");
    oob.prov_start_address = arg_int0("p", NULL, "<address>", "start address assigned by provisioner");
    oob.prov_start_address->ival[0] = 0x0005;
    oob.end = arg_end(1);

    const esp_console_cmd_t oob_cmd = {
        .command = "bmoob",
        .help = "ble mesh: provisioner/node config OOB parameters",
        .hint = NULL,
        .func = &ble_mesh_load_oob,
        .argtable = &oob,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&oob_cmd) );

    component.model_type = arg_int0("m", NULL, "<model>", "mesh model");
    component.config_index = arg_int0("c", NULL, "<index>", "mesh model op");
    component.config_index->ival[0] = 0; // set default value
    component.pub_config = arg_int0("p", NULL, "<publish>", "publish message buffer");
    component.end = arg_end(1);

    const esp_console_cmd_t model_cmd = {
        .command = "bminit",
        .help = "ble mesh: provisioner/node init",
        .hint = NULL,
        .func = &ble_mesh_init,
        .argtable = &component,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&model_cmd) );

    bearer.bearer = arg_int0("b", NULL, "<bearer>", "supported bearer");
    bearer.enable = arg_int0("e", NULL, "<enable/disable>", "bearers node supported");
    bearer.end = arg_end(1);

    const esp_console_cmd_t bearer_cmd = {
        .command = "bmpbearer",
        .help = "ble mesh provisioner: enable/disable different bearers",
        .hint = NULL,
        .func = &ble_mesh_provisioner_enable_bearer,
        .argtable = &bearer,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&bearer_cmd));

    power_set.tx_sense_power = arg_int0("t", NULL, "<power>", "tx power or sense");
    power_set.action_type = arg_str1("z", NULL, "<action>", "action type");
    power_set.end = arg_end(1);

    const esp_console_cmd_t power_set_cmd = {
        .command = "bmtxpower",
        .help = "ble mesh: set tx power or sense",
        .hint = NULL,
        .func = &ble_mesh_power_set,
        .argtable = &power_set,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&power_set_cmd));
}
