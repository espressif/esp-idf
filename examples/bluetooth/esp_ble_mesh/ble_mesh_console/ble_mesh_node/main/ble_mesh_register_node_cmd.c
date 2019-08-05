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

#include "esp_bt.h"
#include "soc/soc.h"

#include "esp_bt_device.h"

#include "test.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_provisioning_api.h"

#include "ble_mesh_console_lib.h"
#include "ble_mesh_adapter.h"

typedef struct {
    struct arg_str *static_val;
    struct arg_int *static_val_len;
    struct arg_int *output_size;
    struct arg_int *output_actions;
    struct arg_int *input_size;
    struct arg_int *input_actions;
    struct arg_end *end;
} ble_mesh_prov_t;
static ble_mesh_prov_t oob;

typedef struct {
    struct arg_int *model_type;
    struct arg_int *config_index;
    struct arg_str *dev_uuid;
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
    struct arg_int *package_num;
    struct arg_end *end;
} ble_mesh_node_statistices_t;
ble_mesh_node_statistices_t node_statistices;

typedef struct {
    struct arg_str *action_type;
    struct arg_int *tx_sense_power;
    struct arg_end *end;
} ble_mesh_tx_sense_power;
static ble_mesh_tx_sense_power power_set;

typedef struct {
    struct arg_str *net_key;
    struct arg_int *net_idx;
    struct arg_int *unicast_addr;
    struct arg_str *dev_key;
    struct arg_str *app_key;
    struct arg_int *app_idx;
    struct arg_int *group_addr;
    struct arg_end *end;
} ble_mesh_node_network_info_t;
ble_mesh_node_network_info_t node_network_info;

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
        ESP_LOGI(TAG, "Provisioning:Success,%d", param->node_prov_complete.addr);
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
    default:
        break;
    }
    ESP_LOGD(TAG, "exit %s", __func__);
}

void ble_mesh_model_cb(esp_ble_mesh_model_cb_event_t event, esp_ble_mesh_model_cb_param_t *param)
{
    uint8_t status;
    uint16_t result;
    uint8_t data[4];
    
    ESP_LOGD(TAG, "enter %s, event=%x\n", __func__, event);
    printf("enter %s, event=%x\n", __func__, event);
    switch (event) {
    case ESP_BLE_MESH_MODEL_OPERATION_EVT:
        if (param->model_operation.model != NULL && param->model_operation.model->op != NULL) {
            if (param->model_operation.opcode == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET) {
                ESP_LOGI(TAG, "Node:GetStatus,Success");
                ble_mesh_node_get_state(status);
                esp_ble_mesh_server_model_send_msg(param->model_operation.model, param->model_operation.ctx, ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS,
                                                   sizeof(status), &status);
            } else if (param->model_operation.opcode == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET) {
                ESP_LOGI(TAG, "Node:SetAck,Success,%d,%d,%d", param->model_operation.msg[0], param->model_operation.ctx->recv_ttl, param->model_operation.length);
                ble_mesh_node_set_state(param->model_operation.msg[0]);
                ble_mesh_node_get_state(status);
                esp_ble_mesh_server_model_send_msg(param->model_operation.model, param->model_operation.ctx, ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS,
                                                   sizeof(status), &status);
            } else if (param->model_operation.opcode == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK) {
                ble_mesh_node_set_state(param->model_operation.msg[0]);
                ESP_LOGI(TAG, "Node:SetUnAck,Success,%d,%d", param->model_operation.msg[0], param->model_operation.ctx->recv_ttl);
            } else if (param->model_operation.opcode == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS) {
                ESP_LOGI(TAG, "Node:Status,Success,%d", param->model_operation.length);
            } else if (param->model_operation.opcode == ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_SET) {
                ESP_LOGI(TAG, "VendorModel:SetAck,Success,%d", param->model_operation.ctx->recv_ttl);
                data[0] = param->model_operation.msg[0];
                data[1] = param->model_operation.msg[1];
                data[2] = param->model_operation.msg[2];
                data[3] = param->model_operation.ctx->recv_ttl;
                result = ble_mesh_node_statistics_accumultate(param->model_operation.msg, param->model_operation.length, VENDOR_MODEL_PERF_OPERATION_TYPE_SET);
                if (result == 0) {
                    esp_ble_mesh_server_model_send_msg(param->model_operation.model, param->model_operation.ctx,
                                                       ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_STATUS, sizeof(data), data);
                }
            } else if (param->model_operation.opcode == ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_SET_UNACK) {
                ESP_LOGI(TAG, "VendorModel:SetUnAck,Success,%d,%d", param->model_operation.ctx->recv_ttl, param->model_operation.length);
                result = ble_mesh_node_statistics_accumultate(param->model_operation.msg, param->model_operation.length, VENDOR_MODEL_PERF_OPERATION_TYPE_SET_UNACK);
                if (result == 0) {
                    esp_ble_mesh_server_model_send_msg(param->model_operation.model, param->model_operation.ctx,
                                                       ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_STATUS, param->model_operation.length, param->model_operation.msg);
                }
            }
        }
        break;
    case ESP_BLE_MESH_MODEL_SEND_COMP_EVT:
        if (param->model_send_comp.err_code == ESP_OK) {
            ESP_LOGI(TAG, "Node:ModelSend,OK");
        } else {
            ESP_LOGE(TAG, "Node:ModelSend,Fail");
        }
        break;
    case ESP_BLE_MESH_MODEL_PUBLISH_COMP_EVT:
        ESP_LOGI(TAG, "PublishSend,OK,0x%x,%d,", param->model_publish_comp.model->model_id, param->model_publish_comp.model->pub->msg->len);
        break;
    case ESP_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT:
        ESP_LOGI(TAG, "PublishUpdate,OK");
        break;
    case ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT:
        ESP_LOGI(TAG, "Node:TimeOut");
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
    ESP_LOGD(TAG, "enter %s\n", __func__);
    int nerrors = arg_parse(argc, argv, (void **) &power_set);
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
    int nerrors = arg_parse(argc, argv, (void **) &oob);

    ESP_LOGD(TAG, "enter %s \n", __func__);

    if (nerrors != 0) {
        arg_print_errors(stderr, oob.end, argv[0]);
        return 1;
    }

    //parsing prov
    if (oob.static_val->count != 0) {
        static_val = malloc(oob.static_val_len->ival[0] + 1);
        get_value_string((char *)oob.static_val->sval[0], (char *)static_val);
        prov.static_val = static_val;
    }

    arg_int_to_value(oob.static_val_len, prov.static_val_len, "static_val_len");
    arg_int_to_value(oob.output_size, prov.output_size, "output_size");
    arg_int_to_value(oob.output_actions, prov.output_actions, "output_actions");
    arg_int_to_value(oob.input_size, prov.input_size, "input_size");
    arg_int_to_value(oob.input_actions, prov.input_actions, "input_action");

    ESP_LOGI(TAG, "OOB:Load,OK\n");
    ESP_LOGD(TAG, "exit %s\n", __func__);
    return 0;
}

int ble_mesh_init(int argc, char **argv)
{
    int err;
    esp_ble_mesh_comp_t *local_component = NULL;
    uint8_t *device_uuid =NULL;

    int nerrors = arg_parse(argc, argv, (void **) &component);
    if (nerrors != 0) {
        arg_print_errors(stderr, component.end, argv[0]);
        return 1;
    }

    ESP_LOGD(TAG, "enter %s, module %x\n", __func__, component.model_type->ival[0]);
    local_component = ble_mesh_get_component(component.model_type->ival[0]);

    if (component.dev_uuid->count != 0) {
        device_uuid = malloc((16 + 1) * sizeof(uint8_t));
        if (device_uuid == NULL) {
            ESP_LOGE(TAG, "ble mesh malloc failed, %d\n", __LINE__);
        }
        get_value_string((char *)component.dev_uuid->sval[0], (char *) device_uuid);
        memcpy(dev_uuid, device_uuid, 16);
    } else {
        memcpy(dev_uuid, esp_bt_dev_get_address(), 6);
    }

    err = esp_ble_mesh_init(&prov, local_component);
    if (err) {
        ESP_LOGE(TAG, "Initializing mesh failed (err %d)\n", err);
        return err;
    }
    
    free(device_uuid);
    ESP_LOGD(TAG, "exit %s\n", __func__);
    return err;
}

int ble_mesh_node_enable_bearer(int argc, char **argv)
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
            //err = esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_N12);
            err = esp_ble_mesh_node_prov_enable(bearer.bearer->ival[0]);
        } else {
            err = esp_ble_mesh_node_prov_disable(bearer.bearer->ival[0]);
        }
    } else {
        return 1;
    }

    ESP_LOGD(TAG, "exit %s\n", __func__);
    return err;
}

int ble_mesh_node_reset(int argc, char** argv)
{
    esp_err_t err;
    ESP_LOGD(TAG, "enter %s\n", __func__);

    err = esp_ble_mesh_node_local_reset();

    ESP_LOGD(TAG, "exit %s\n", __func__);
    return err;
}

int ble_mesh_node_statistics_regist(int argc, char **argv)
{
    int result = ESP_OK;

    int nerrors = arg_parse(argc, argv, (void **) &node_statistices);
    if (nerrors != 0) {
        arg_print_errors(stderr, node_statistices.end, argv[0]);
        return 1;
    }

    ESP_LOGD(TAG, "enter %s\n", __func__);

    if (strcmp(node_statistices.action_type->sval[0], "init") == 0) {
        result = ble_mesh_node_statistics_init(node_statistices.package_num->ival[0]);
        ESP_LOGI(TAG, "Node:InitStatistics,OK\n");
    } else if (strcmp(node_statistices.action_type->sval[0], "get") == 0) {
        ble_mesh_node_statistics_get();
        ESP_LOGI(TAG, "Node:GetStatistics,OK\n");
    } else if (strcmp(node_statistices.action_type->sval[0], "destroy") == 0) {
        ble_mesh_node_statistics_destroy();
        ESP_LOGI(TAG, "Node:DestroyStatistics\n");
    }

    ESP_LOGD(TAG, "exit %s\n", __func__);
    return result;
}

int ble_mesh_node_enter_network_auto(int argc, char **argv)
{
    esp_err_t err = ESP_OK;
    struct bt_mesh_device_network_info info = {
        .flags = 0,
        .iv_index = 0,
    };

    int nerrors = arg_parse(argc, argv, (void **) &node_network_info);
    if (nerrors != 0) {
        arg_print_errors(stderr, node_network_info.end, argv[0]);
        return 1;
    }

    ESP_LOGD(TAG, "enter %s\n", __func__);

    arg_int_to_value(node_network_info.net_idx, info.net_idx, "network key index");
    arg_int_to_value(node_network_info.unicast_addr, info.unicast_addr, "unicast address");
    arg_int_to_value(node_network_info.app_idx, info.app_idx, "appkey index");
    arg_int_to_value(node_network_info.group_addr, info.group_addr, "group address");
    err = get_value_string((char *)node_network_info.net_key->sval[0], (char *)info.net_key);
    err = get_value_string((char *)node_network_info.dev_key->sval[0], (char *)info.dev_key);
    err = get_value_string((char *)node_network_info.app_key->sval[0], (char *)info.app_key);

    err = bt_mesh_device_auto_enter_network(&info);
    if (err == ESP_OK) {
        ESP_LOGD(TAG, "NODE:EnNetwork,OK");
    } else {
        ESP_LOGE(TAG, "NODE:EnNetwork,FAIL,%d", err);
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
    component.dev_uuid = arg_str0("d", NULL, "<uuid>", "device uuid");
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
        .command = "bmnbearer",
        .help = "ble mesh node: enable/disable different bearer",
        .hint = NULL,
        .func = &ble_mesh_node_enable_bearer,
        .argtable = &bearer,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&bearer_cmd));

    const esp_console_cmd_t reset_cmd = {
        .command = "bmnreset",
        .help = "ble mesh node: reset",
        .hint = NULL,
        .func = &ble_mesh_node_reset,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&reset_cmd));

    node_statistices.action_type = arg_str1("z", NULL, "<action>", "action type");
    node_statistices.package_num = arg_int0("p", NULL, "<package>", "package number");
    node_statistices.end = arg_end(1);

    const esp_console_cmd_t node_statistices_cmd = {
        .command = "bmsperf",
        .help = "ble mesh server: performance statistics",
        .hint = NULL,
        .func = &ble_mesh_node_statistics_regist,
        .argtable = &node_statistices,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&node_statistices_cmd));

    power_set.action_type = arg_str1("z", NULL, "<action>", "action type");
    power_set.tx_sense_power = arg_int0("t", NULL, "<power>", "tx power or sense");
    power_set.end = arg_end(1);

    const esp_console_cmd_t power_set_cmd = {
        .command = "bmtxpower",
        .help = "ble mesh: set tx power or sense",
        .hint = NULL,
        .func = &ble_mesh_power_set,
        .argtable = &power_set,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&power_set_cmd));

    node_network_info.net_key = arg_str1("k", NULL, "<net key>", "network key");
    node_network_info.net_idx = arg_int1("n", NULL, "<net index>", "network key index");
    node_network_info.unicast_addr = arg_int1("u", NULL, "<unicast address>", "unicast address");
    node_network_info.dev_key = arg_str1("d", NULL, "<device key>", "device key");
    node_network_info.app_key = arg_str1("a", NULL, "<appkey>", "app key");
    node_network_info.app_idx = arg_int1("i", NULL, "<app index>", "appkey index");
    node_network_info.group_addr = arg_int1("g", NULL, "<group address>", "group address");
    node_network_info.end = arg_end(1);

    const esp_console_cmd_t node_network_info_cmd = {
        .command = "bmnnwk",
        .help = "ble mesh node: auto join network",
        .hint = NULL,
        .func = &ble_mesh_node_enter_network_auto,
        .argtable = &node_network_info,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&node_network_info_cmd));
}
