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

#include "test.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "ble_mesh_console_lib.h"
#include "ble_mesh_adapter.h"
#include "transaction.h"
#include "esp_ble_mesh_config_model_api.h"
#include "ble_mesh_console_decl.h"
#include "ble_mesh_model.h"

/* We include the internal header file mesh_bearer_adapt.h here
   just for some specific test purpose, which is not recommended for the other applications. */
#include "mesh_bearer_adapt.h"

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
    struct arg_str *dev_uuid;
    struct arg_int *pub_config;
    struct arg_end *end;
} ble_mesh_comp_t;
static ble_mesh_comp_t component;

typedef struct {
    struct arg_int *action;
    struct arg_end *end;
} ble_mesh_deinit_t;
static ble_mesh_deinit_t deinit;

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
SemaphoreHandle_t ble_mesh_node_sema = NULL;

typedef struct {
    struct arg_str *add_del;
    struct arg_str *device_addr;
    struct arg_str *device_uuid;
    struct arg_int *addr_type;
    struct arg_int *bearer;
    struct arg_int *oob_info;
    struct arg_int *flag;
    struct arg_end *end;
} ble_mesh_provisioner_addr_t;
ble_mesh_provisioner_addr_t provisioner_addr;

typedef struct {
    struct arg_str *action_type;
    struct arg_int *net_idx;
    struct arg_int *app_idx;
    struct arg_str *key;
    struct arg_end *end;
} ble_mesh_provisioner_add_key_t;
ble_mesh_provisioner_add_key_t provisioner_add_key;

typedef struct {
    struct arg_int *unicast_addr;
    struct arg_end *end;
} ble_mesh_provisioner_get_node_t;
ble_mesh_provisioner_get_node_t provisioner_get_node;

typedef struct {
    struct arg_int *oob_info;
    struct arg_int *unicast_addr;
    struct arg_int *element_num;
    struct arg_int *net_idx;
    struct arg_str *dev_key;
    struct arg_str *uuid;
    struct arg_end *end;
} ble_mesh_provisioner_add_node_t;
ble_mesh_provisioner_add_node_t provisioner_add_node;

typedef struct {
    struct arg_int *appkey_index;
    struct arg_int *element_address;
    struct arg_int *network_index;
    struct arg_int *mod_id;
    struct arg_int *cid;
    struct arg_end *end;
} ble_mesh_provisioner_bind_model_t;
ble_mesh_provisioner_bind_model_t provisioner_local_bind;

typedef struct {
    struct arg_str *action_type;
    struct arg_int *enable;
    struct arg_int *op;
    struct arg_int *hb_src;
    struct arg_int *hb_dst;
    struct arg_int *type;
    struct arg_end *end;
} ble_mesh_provisioner_heartbeat_t;
static ble_mesh_provisioner_heartbeat_t heartbeat;

#ifdef CONFIG_BLE_MESH_USE_DUPLICATE_SCAN
typedef struct {
    struct arg_str *action_type;
    struct arg_int *type;
    struct arg_str *info;
    struct arg_end *end;
} ble_mesh_exceptional_list_t;
static ble_mesh_exceptional_list_t exceptional_list_test;
#endif

extern bool deinit_flag;
extern void ble_mesh_generic_onoff_client_model_cb(esp_ble_mesh_generic_client_cb_event_t event,
                                                   esp_ble_mesh_generic_client_cb_param_t *param);
extern void ble_mesh_configuration_client_model_cb(esp_ble_mesh_cfg_client_cb_event_t event,
                                                   esp_ble_mesh_cfg_client_cb_param_t *param);
extern void ble_mesh_vendor_model_cb(esp_ble_mesh_model_cb_event_t event, esp_ble_mesh_model_cb_param_t *param);
extern void ble_mesh_generic_server_model_cb(esp_ble_mesh_generic_server_cb_event_t event,
                                             esp_ble_mesh_generic_server_cb_param_t *param);

void ble_mesh_register_cmd(void);
// Register callback function
void ble_mesh_prov_cb(esp_ble_mesh_prov_cb_event_t event, esp_ble_mesh_prov_cb_param_t *param);

void ble_mesh_register_mesh_node(void)
{
    ble_mesh_register_cmd();
}

int ble_mesh_register_cb(int argc, char** argv)
{
    esp_ble_mesh_register_prov_callback(ble_mesh_prov_cb);
    esp_ble_mesh_register_custom_model_callback(ble_mesh_vendor_model_cb);
    esp_ble_mesh_register_generic_server_callback(ble_mesh_generic_server_model_cb);
    esp_ble_mesh_register_generic_client_callback(ble_mesh_generic_onoff_client_model_cb);
    esp_ble_mesh_register_config_client_callback(ble_mesh_configuration_client_model_cb);
    ESP_LOGI(TAG, "Bm:Reg,OK");
    return 0;
}

void ble_mesh_prov_cb(esp_ble_mesh_prov_cb_event_t event, esp_ble_mesh_prov_cb_param_t *param)
{
    transaction_t *trans = NULL;
    ESP_LOGD(TAG, "enter %s, event = %d", __func__, event);

    switch (event) {
    case ESP_BLE_MESH_PROV_REGISTER_COMP_EVT:
        ble_mesh_callback_check_err_code(param->prov_register_comp.err_code, "Bm:Init");
        if (param->prov_register_comp.err_code == ESP_OK) {
            deinit_flag = false;
        }
        break;
    case ESP_BLE_MESH_DEINIT_MESH_COMP_EVT:
        ble_mesh_callback_check_err_code(param->deinit_mesh_comp.err_code, "Bm:DeInit");
        if (param->deinit_mesh_comp.err_code == ESP_OK) {
            deinit_flag = true;
            do {
                trans = transaction_get(TRANS_TYPE_MESH_PERF, TRANS_MESH_SEND_MESSAGE, trans);
                if (trans) {
                    transaction_abort(trans, ESP_ERR_INVALID_STATE);
                }
            }while(trans);
        }
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
        ESP_LOGI(TAG, "Bm:Reset");
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
        ESP_LOGI(TAG, "Provisioner:%s,"MACSTR",0x%x,0x%04x,0x%x",
            param->provisioner_recv_unprov_adv_pkt.bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT",
            MAC2STR(param->provisioner_recv_unprov_adv_pkt.addr),
            param->provisioner_recv_unprov_adv_pkt.addr_type,
            param->provisioner_recv_unprov_adv_pkt.oob_info,
            param->provisioner_recv_unprov_adv_pkt.adv_type);
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
    case ESP_BLE_MESH_PROVISIONER_ENABLE_HEARTBEAT_RECV_COMP_EVT:
        ble_mesh_callback_check_err_code(param->provisioner_enable_heartbeat_recv_comp.err_code, "Provisioner:EnHbRecv");
        break;
    case ESP_BLE_MESH_PROVISIONER_SET_HEARTBEAT_FILTER_TYPE_COMP_EVT:
        ble_mesh_callback_check_err_code(param->provisioner_set_heartbeat_filter_type_comp.err_code, "Provisioner:SetHbFilterType");
        break;
    case ESP_BLE_MESH_PROVISIONER_SET_HEARTBEAT_FILTER_INFO_COMP_EVT:
        ble_mesh_callback_check_err_code(param->provisioner_set_heartbeat_filter_info_comp.err_code, "Provisioner:SetHbFilterInfo");
        break;
    case ESP_BLE_MESH_PROVISIONER_RECV_HEARTBEAT_MESSAGE_EVT:
        ESP_LOGI(TAG, "Provisioner:HbRecv,OK,%d,%d", param->provisioner_recv_heartbeat.hb_src, param->provisioner_recv_heartbeat.hb_dst);
        break;
#endif
    default:
        break;
    }
    ESP_LOGD(TAG, "exit %s", __func__);
}

int ble_mesh_power_set(int argc, char **argv)
{
    esp_err_t result = ESP_OK;

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
        ESP_LOGI(TAG, "Node:SetPower,OK");
    }
    return result;
}

static int get_dev_uuid(uint8_t uuid[16])
{
    uint8_t addr[6] = {0};

    extern int get_bd_addr(uint8_t addr[6]);
    if (get_bd_addr(addr)) {
        return -1;
    }

    memcpy(uuid, addr, BD_ADDR_LEN);
    return 0;
}

static int ble_mesh_load_oob(int argc, char **argv)
{
    uint8_t *static_val;

    int nerrors = arg_parse(argc, argv, (void **) &oob);
    if (nerrors != 0) {
        arg_print_errors(stderr, oob.end, argv[0]);
        return 1;
    }

    //parsing prov
#if CONFIG_BLE_MESH_NODE
    prov.uuid = dev_uuid;
    if (get_dev_uuid(dev_uuid)) {
        return 1;
    }
    if (oob.static_val->count != 0) {
        static_val = malloc(oob.static_val_len->ival[0] + 1);
        if (static_val == NULL) {
            ESP_LOGE(TAG, "malloc fail,%s,%d", __func__, __LINE__);
            return ESP_ERR_NO_MEM;
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
            ESP_LOGE(TAG, "malloc fail,%s,%d", __func__, __LINE__);
            return ESP_ERR_NO_MEM;
        }
        get_value_string((char *)oob.static_val->sval[0], (char *)static_val);
        prov.prov_static_oob_val = static_val;
    }
    arg_int_to_value(oob.prov_start_address, prov.prov_start_address, "provisioner start address");
    arg_int_to_value(oob.static_val_len, prov.prov_static_oob_len, "provisioner static value length");
#endif

    ESP_LOGI(TAG, "OOB:Load,OK");
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

    err = ble_mesh_init_node_prestore_params();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Bm:NodeInitPreParam,Fail");
        return err;
    }

    local_component = ble_mesh_get_component(component.model_type->ival[0]); 

    if (component.dev_uuid->count != 0) {
        device_uuid = malloc((ESP_BLE_MESH_OCTET16_LEN + 1) * sizeof(uint8_t));
        if (device_uuid == NULL) {
            ESP_LOGE(TAG, "ble mesh malloc failed, %d", __LINE__);
            return ESP_ERR_NO_MEM;
        }
        err = get_value_string((char *)component.dev_uuid->sval[0], (char *)device_uuid);
        if (err == ESP_OK) {
            memcpy(dev_uuid, device_uuid, ESP_BLE_MESH_OCTET16_LEN);
        } else {
            str_2_mac((uint8_t *)component.dev_uuid->sval[0], device_uuid);
            memcpy(dev_uuid, device_uuid, BD_ADDR_LEN);
        }
    } else {
        if (get_dev_uuid(dev_uuid)) {
            return 1;
        }
    }

    err = esp_ble_mesh_init(&prov, local_component);
    
    free(device_uuid);
    return err;
}

int ble_mesh_provisioner_heartbeat(int argc, char** argv)
{
    esp_err_t result = ESP_OK;
    bool enable = 1;
    uint8_t type = 0;
    esp_ble_mesh_heartbeat_filter_info_t info;
    uint8_t op = 0;

    int nerrors = arg_parse(argc, argv, (void **) &heartbeat);
    if (nerrors != 0) {
        arg_print_errors(stderr, heartbeat.end, argv[0]);
        return 1;
    }

    arg_int_to_value(heartbeat.enable, enable, "enable/disable receiving heartbeat");
    arg_int_to_value(heartbeat.type, type, "heartbeat filter type");
    arg_int_to_value(heartbeat.hb_dst, info.hb_dst, "destination address");
    arg_int_to_value(heartbeat.hb_src, info.hb_src, "source address");
    arg_int_to_value(heartbeat.op, op, "op");

    if (strcmp(heartbeat.action_type->sval[0], "recv") == 0){
        result = esp_ble_mesh_provisioner_recv_heartbeat(enable);
    }else if(strcmp(heartbeat.action_type->sval[0], "type") == 0){
        result = esp_ble_mesh_provisioner_set_heartbeat_filter_type(type);
    }else if(strcmp(heartbeat.action_type->sval[0], "info")== 0){
        result = esp_ble_mesh_provisioner_set_heartbeat_filter_info(op, &info);
    }

    if(result == ESP_OK){
        ESP_LOGI(TAG, "provisioner:OK");
    }else{
        ESP_LOGE(TAG, "provisioner:ERROR");
    }
    return result;
}

int ble_mesh_node_enable_bearer(int argc, char **argv)
{
    esp_err_t  err = 0;

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
    return err;
}

#ifdef CONFIG_BLE_MESH_USE_DUPLICATE_SCAN
int ble_mesh_exceptional_list_test(int argc, char **argv)
{
    esp_err_t err = ESP_FAIL;
    uint32_t type = BLE_MESH_EXCEP_LIST_TYPE_MESH_BEACON;
    uint8_t *info = NULL;

    int nerrors = arg_parse(argc, argv, (void **) &exceptional_list_test);
    if (nerrors != 0) {
        arg_print_errors(stderr, exceptional_list_test.end, argv[0]);
        return 1;
    }

    arg_int_to_value(exceptional_list_test.type, type, "device info type");

    if (exceptional_list_test.info->count != 0) {
        info = malloc((BD_ADDR_LEN + 1) * sizeof(uint8_t));
        if (info == NULL) {
            ESP_LOGE(TAG, "ble mesh malloc failed, %d", __LINE__);
            return ESP_ERR_NO_MEM;
        } else {
            get_value_string((char *)exceptional_list_test.info->sval[0], (char *)info);
        }
    }

    if (strcmp(exceptional_list_test.action_type->sval[0], "add") == 0) {
        err = bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_SUB_CODE_ADD, type, info);
    } else if (strcmp(exceptional_list_test.action_type->sval[0], "remove") == 0) {
        err = bt_mesh_update_exceptional_list( BLE_MESH_EXCEP_LIST_SUB_CODE_REMOVE, type, info);
    } else if (strcmp(exceptional_list_test.action_type->sval[0], "clean") == 0) {
        err = bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_SUB_CODE_CLEAN, type, NULL);
    }

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Bm:UpdateExcepList,OK");
    } else {
        ESP_LOGE(TAG, "Bm:UpdateExcepList,Fail");
    }

    if (info != NULL) {
        free(info);
    }

    return err;
}
#endif

int ble_mesh_deinit(int argc, char **argv)
{
    int err;
    esp_ble_mesh_deinit_param_t param = {};

    int nerrors = arg_parse(argc, argv, (void **) &deinit);
    if (nerrors != 0) {
        arg_print_errors(stderr, deinit.end, argv[0]);
        return 1;
    }

    ble_mesh_deinit_node_prestore_params();

    if (deinit.action->count != 0) {
        param.erase_flash = deinit.action->ival[0];
        err = esp_ble_mesh_deinit(&param);
    } else {
        return 1;
    }
    return err;
}

int ble_mesh_provisioner_enable_bearer(int argc, char **argv)
{
    esp_err_t  err = 0;

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
    return err;
}

int ble_mesh_node_reset(int argc, char** argv)
{
    esp_err_t err;

    err = esp_ble_mesh_node_local_reset();
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

    if (strcmp(node_statistices.action_type->sval[0], "init") == 0) {
        result = ble_mesh_node_statistics_init(node_statistices.package_num->ival[0]);
        ESP_LOGI(TAG, "Node:InitStatistics,OK");
    } else if (strcmp(node_statistices.action_type->sval[0], "get") == 0) {
        ble_mesh_node_statistics_get();
        ESP_LOGI(TAG, "Node:GetStatistics,OK");
    } else if (strcmp(node_statistices.action_type->sval[0], "destroy") == 0) {
        ble_mesh_node_statistics_destroy();
        ESP_LOGI(TAG, "Node:DestroyStatistics");
    }

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

    err = get_value_string((char *)node_network_info.net_key->sval[0], (char *)info.net_key);
    err = get_value_string((char *)node_network_info.dev_key->sval[0], (char *)info.dev_key);
    err = get_value_string((char *)node_network_info.app_key->sval[0], (char *)info.app_key);
    arg_int_to_value(node_network_info.net_idx, info.net_idx, "network key index");
    arg_int_to_value(node_network_info.unicast_addr, info.unicast_addr, "unicast address");
    arg_int_to_value(node_network_info.app_idx, info.app_idx, "appkey index");
    arg_int_to_value(node_network_info.group_addr, info.group_addr, "group address");

    err = bt_mesh_device_auto_enter_network(&info);
    if (err == ESP_OK) {
        ESP_LOGD(TAG, "NODE:EnNetwork,OK");
    } else {
        ESP_LOGE(TAG, "NODE:EnNetwork,FAIL,%d", err);
    }
    return err;
}

int ble_mesh_provision_address(int argc, char **argv)
{
    esp_err_t err = ESP_OK;
    esp_ble_mesh_unprov_dev_add_t device_addr = {0};
    uint8_t  preset_addr_uuid[16] = {0x01, 0x02};
    esp_ble_mesh_device_delete_t del_dev = {
        .flag = BIT(0),
    };

    int nerrors = arg_parse(argc, argv, (void **) &provisioner_addr);
    if (nerrors != 0) {
        arg_print_errors(stderr, provisioner_addr.end, argv[0]);
        return 1;
    }

    memcpy(device_addr.uuid, preset_addr_uuid, BD_ADDR_LEN);
    if (provisioner_addr.device_addr->count != 0) {
        str_2_mac((uint8_t *)provisioner_addr.device_addr->sval[0], device_addr.addr);
        str_2_mac((uint8_t *)provisioner_addr.device_addr->sval[0], del_dev.addr);
        arg_int_to_value(provisioner_addr.addr_type, device_addr.addr_type, "address type");
        arg_int_to_value(provisioner_addr.addr_type, del_dev.addr_type, "address type");
    }
    if (provisioner_addr.device_uuid->count != 0) {
        uint8_t tmp_uuid[16] = {0};
        err = get_value_string((char *)provisioner_addr.device_uuid->sval[0], (char *)tmp_uuid);
        if (err != ESP_OK) {
            str_2_mac((uint8_t *)provisioner_addr.device_uuid->sval[0], tmp_uuid);
        }
        memcpy(device_addr.uuid, tmp_uuid, 16);
        memcpy(del_dev.uuid, tmp_uuid, 16);
        del_dev.flag = BIT(1);
    }

#if CONFIG_BLE_MESH_PROVISIONER
    if (strcmp(provisioner_addr.add_del->sval[0], "add") == 0) {
        arg_int_to_value(provisioner_addr.bearer, device_addr.bearer, "bearer");
        arg_int_to_value(provisioner_addr.oob_info, device_addr.oob_info, "oob information");
        err = esp_ble_mesh_provisioner_add_unprov_dev(&device_addr, provisioner_addr.flag->ival[0]);
    } else if (strcmp(provisioner_addr.add_del->sval[0], "del") == 0) {
        err = esp_ble_mesh_provisioner_delete_dev(&del_dev);
    }
#endif
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "Provisioner:AddDelAddr,Fail,%d", err);
    } else {
        ESP_LOGI(TAG, "Provisioner:AddDelAddr,OK");
    }
    return err;
}

int ble_mesh_provisioner_add_key(int argc, char **argv)
{
    esp_err_t err = ESP_OK;
    uint8_t key[16] = {0};
    esp_ble_mesh_prov_data_info_t info = {
        .net_idx = 1,
        .flag = PROV_DATA_NET_IDX_FLAG,
    };

    int nerrors = arg_parse(argc, argv, (void **) &provisioner_add_key);
    if (nerrors != 0) {
        arg_print_errors(stderr, provisioner_add_key.end, argv[0]);
        return 1;
    }

    err = get_value_string((char *)provisioner_add_key.key->sval[0], (char *) key);
    if (strcmp(provisioner_add_key.action_type->sval[0], "appkey") == 0) {
        err = esp_ble_mesh_provisioner_add_local_app_key(key, provisioner_add_key.net_idx->ival[0], provisioner_add_key.app_idx->ival[0]);
    } else if (strcmp(provisioner_add_key.action_type->sval[0], "netkey") == 0) {
        // choose network key
        info.net_idx = provisioner_add_key.net_idx->ival[0];
        err = esp_ble_mesh_provisioner_add_local_net_key(key, provisioner_add_key.net_idx->ival[0]);
        err = err | esp_ble_mesh_provisioner_set_prov_data_info(&info);
    }

    if (err != ESP_OK) {
        ESP_LOGI(TAG, "Provisioner:KeyAction,Fail,%d", err);
    } else {
        ESP_LOGI(TAG, "Provisioner:KeyAction,OK");
    }
    return err;
}

int ble_mesh_provisioner_get_node(int argc, char **argv)
{
    uint16_t unicast_addr = 0;
    uint16_t i = 0;
    esp_ble_mesh_node_t *node_info;

    int nerrors = arg_parse(argc, argv, (void **) &provisioner_get_node);
    if (nerrors != 0) {
        arg_print_errors(stderr, provisioner_get_node.end, argv[0]);
        return 1;
    }

    arg_int_to_value(provisioner_get_node.unicast_addr, unicast_addr, "unicast address");
    node_info = esp_ble_mesh_provisioner_get_node_with_addr(unicast_addr);

    if (node_info == NULL) {
        return ESP_FAIL;
    } else {
        printf("OobInfo:0x%x,Address:0x%x,EleNum:0x%x,NetIdx:0x%x,DevKey:",
               node_info->oob_info, node_info->unicast_addr, node_info->element_num, node_info->net_idx);
        for (i = 0; i < 16; i++) {
            printf("%02x", node_info->dev_key[i]);
        }
        printf(",DevUuid:");
        for (i = 0; i < 16; i++) {
            printf("%02x", node_info->dev_uuid[i]);
        }
        printf("\n");
    }
    return ESP_OK;
}

int ble_mesh_provisioner_add_node(int argc, char **argv)
{
    struct bt_mesh_node node_info;
    esp_err_t result;

    int nerrors = arg_parse(argc, argv, (void **) &provisioner_add_node);
    if (nerrors != 0) {
        arg_print_errors(stderr, provisioner_add_node.end, argv[0]);
        return 1;
    }

    arg_int_to_value(provisioner_add_node.oob_info, node_info.oob_info, "oob information");
    arg_int_to_value(provisioner_add_node.unicast_addr, node_info.unicast_addr, "unicast address");
    arg_int_to_value(provisioner_add_node.element_num, node_info.element_num, "element number");
    arg_int_to_value(provisioner_add_node.net_idx, node_info.net_idx, "network index");
    if (provisioner_add_node.dev_key->count != 0) {
        get_value_string((char *)provisioner_add_node.dev_key->sval[0], (char *)node_info.dev_key);
    }
    if (provisioner_add_node.uuid->count != 0) {
        get_value_string((char *)provisioner_add_node.uuid->sval[0], (char *)node_info.dev_uuid); 
        get_value_string((char *)provisioner_add_node.uuid->sval[0], (char *)node_info.dev_uuid);
    }

    result = bt_mesh_provisioner_store_node_info(&node_info);
    if (result == ESP_OK) {
        ESP_LOGI(TAG, "Provisioner:AddNodeInfo,OK");
    } else {
        ESP_LOGI(TAG, "Provisioner:AddNodeInfo,ERROR,%d", result);
    }
    return result;
}

int ble_mesh_provision_bind_local_model(int argc, char **argv)
{
    esp_err_t err;
    uint16_t element_addr = 0;
    uint16_t app_idx = 0;
    uint16_t model_id = 0;
    uint16_t company_id = 0xFFFF;

    int nerrors = arg_parse(argc, argv, (void **) &provisioner_local_bind);
    if (nerrors != 0) {
        arg_print_errors(stderr, provisioner_local_bind.end, argv[0]);
        return 1;
    }

    arg_int_to_value(provisioner_local_bind.element_address, element_addr, "element address");
    arg_int_to_value(provisioner_local_bind.appkey_index, app_idx, "appkey index");
    arg_int_to_value(provisioner_local_bind.mod_id, model_id, "model id");
    arg_int_to_value(provisioner_local_bind.cid, company_id, "company id");
    err = esp_ble_mesh_provisioner_bind_app_key_to_local_model(element_addr, app_idx, model_id, company_id);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Provisioner:BindModel,Fail,%d", err);
    } else {
        ESP_LOGI(TAG, "Provisioner:BindModel,OK");
    }
    return err;
}

void ble_mesh_register_cmd(void)
{
    const esp_console_cmd_t register_cmd = {
        .command  = "bmreg",
        .help = "ble mesh: provisioner/node register callback",
        .hint = NULL,
        .func = &ble_mesh_register_cb,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&register_cmd));

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
    ESP_ERROR_CHECK( esp_console_cmd_register(&model_cmd));

    deinit.action = arg_int0("o", NULL, "<action>", "deinit action");
    deinit.end = arg_end(1);

    const esp_console_cmd_t deinit_cmd = {
        .command = "bmdeinit",
        .help = "ble mesh: provisioner/node deinit",
        .hint = NULL,
        .func = &ble_mesh_deinit,
        .argtable = &deinit,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&deinit_cmd));

    provisioner_addr.add_del = arg_str1("z", NULL, "<add/delete>", "action type");
    provisioner_addr.device_addr = arg_str0("d", NULL, "<address>", "device address");
    provisioner_addr.device_uuid = arg_str0("u", NULL, "<uuid>", "device uuid");
    provisioner_addr.addr_type = arg_int0("a", NULL, "<type>", "address type");
    provisioner_addr.flag = arg_int0("f", NULL, "<flag>", "address flag");
    provisioner_addr.flag->ival[0] = ADD_DEV_RM_AFTER_PROV_FLAG | ADD_DEV_FLUSHABLE_DEV_FLAG;
    provisioner_addr.bearer = arg_int0("b", NULL, "<bearer>", "used bearer");
    provisioner_addr.oob_info = arg_int0("o", NULL, "<oob info>", "oob information");
    provisioner_addr.end = arg_end(1);

    const esp_console_cmd_t provisioner_addr_cmd = {
        .command = "bmpdev",
        .help = "ble mesh provisioner: add/delete unprovisioned device",
        .hint = NULL,
        .func = &ble_mesh_provision_address,
        .argtable = &provisioner_addr,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&provisioner_addr_cmd));

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
    ESP_ERROR_CHECK( esp_console_cmd_register(&oob_cmd));

    bearer.bearer = arg_int0("b", NULL, "<bearer>", "supported bearer");
    bearer.enable = arg_int0("e", NULL, "<enable/disable>", "bearers node supported");
    bearer.end = arg_end(1);

    const esp_console_cmd_t bearer_node_cmd = {
        .command = "bmnbearer",
        .help = "ble mesh node/porvisioner: enable/disable different bearer",
        .hint = NULL,
        .func = &ble_mesh_node_enable_bearer,
        .argtable = &bearer,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&bearer_node_cmd));

    const esp_console_cmd_t bearer_provisioner_cmd = {
        .command = "bmpbearer",
        .help = "ble mesh provisioner: enable/disable different bearers",
        .hint = NULL,
        .func = &ble_mesh_provisioner_enable_bearer,
        .argtable = &bearer,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&bearer_provisioner_cmd));

    provisioner_add_key.action_type = arg_str1("z", NULL, "<action type>", "add appkey or network key");
    provisioner_add_key.net_idx = arg_int1("n", NULL, "<net key index>", "network key index");
    provisioner_add_key.key = arg_str1("k", NULL, "<key>", "appkey or network");
    provisioner_add_key.app_idx = arg_int0("a", NULL, "<app key index>", "appkey index");
    provisioner_add_key.end = arg_end(1);

    const esp_console_cmd_t provisioner_add_key_cmd = {
        .command = "bmpkey",
        .help = "ble mesh provisioner: key",
        .func = &ble_mesh_provisioner_add_key,
        .argtable = &provisioner_add_key,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&provisioner_add_key_cmd));

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

    provisioner_get_node.unicast_addr = arg_int1("u", NULL, "<address>", "get node by unicast address");
    provisioner_get_node.end = arg_end(1);

    const esp_console_cmd_t provisioner_get_node_cmd = {
        .command = "bmpgetn",
        .help = "ble mesh provisioner: get node",
        .func = &ble_mesh_provisioner_get_node,
        .argtable = &provisioner_get_node,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&provisioner_get_node_cmd));

    provisioner_add_node.oob_info = arg_int0("o", NULL, "<oob info>", "oob information");
    provisioner_add_node.unicast_addr = arg_int0("a", NULL, "<unicast address>", "unicast address");
    provisioner_add_node.element_num = arg_int0("e", NULL, "<element num>", "element num");
    provisioner_add_node.net_idx = arg_int0("n", NULL, "<net index>", "net index");
    provisioner_add_node.dev_key = arg_str0("d", NULL, "<device key>", "device key");
    provisioner_add_node.uuid = arg_str0("u", NULL, "<device uuid>", "device uuid");
    provisioner_add_node.end = arg_end(1);

    const esp_console_cmd_t provisioner_add_node_cmd = {
        .command = "bmpaddn",
        .help = "ble mesh provisioner: add node",
        .func = &ble_mesh_provisioner_add_node,
        .argtable = &provisioner_add_node,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&provisioner_add_node_cmd));

    provisioner_local_bind.appkey_index = arg_int1("a", NULL, "<appkey index>", "appkey index");
    provisioner_local_bind.element_address = arg_int1("e", NULL, "<element address>", "element address");
    provisioner_local_bind.network_index = arg_int1("n", NULL, "<network index>", "network index");
    provisioner_local_bind.mod_id = arg_int1("m", NULL, "<model id>", "model id");
    provisioner_local_bind.cid = arg_int0("c", NULL, "<model id>", "company id");
    provisioner_local_bind.end = arg_end(1);

    const esp_console_cmd_t provisioner_local_bind_cmd = {
        .command = "bmpbind",
        .help = "ble mesh provisioner: bind local model",
        .func = &ble_mesh_provision_bind_local_model,
        .argtable = &provisioner_local_bind,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&provisioner_local_bind_cmd));

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

    heartbeat.action_type = arg_str0("z", NULL, "<action type>", "action type");
    heartbeat.op = arg_int0("o", NULL, "<heartbeat filter>", "add or remove a heartbeat filter entry");
    heartbeat.hb_src = arg_int0("s", NULL, "<source address>", "Heartbeat source address");
    heartbeat.hb_dst = arg_int0("d", NULL, "<destination address>", "Heartbeat destination address");
    heartbeat.type = arg_int0("t", NULL, "<heartbeat filter>", "set the heartbeat filter type");
    heartbeat.enable = arg_int0("e", NULL, "<enable/disable>", "enable or disable receiving heartbeat messages");
    heartbeat.end = arg_end(1);

    const esp_console_cmd_t provisioner_heartbeat_cmd = {
        .command = "bmphb",
        .help = "ble mesh provisioner: support recv heartbeat",
        .hint = NULL,
        .func = &ble_mesh_provisioner_heartbeat,
        .argtable = &heartbeat,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&provisioner_heartbeat_cmd));

#ifdef CONFIG_BLE_MESH_USE_DUPLICATE_SCAN
    exceptional_list_test.action_type = arg_str1("z", NULL, "<action type>", "action type");
    exceptional_list_test.type = arg_int1("t", NULL, "<type>", "device info type");
    exceptional_list_test.info = arg_str0("a", NULL, "<info>", "info");
    exceptional_list_test.end = arg_end(1);

    const esp_console_cmd_t ble_mesh_exceptional_list_test_cmd = {
        .command = "bmel",
        .help = "ble mesh: duplicate scan exceptional list test",
        .hint = NULL,
        .func = &ble_mesh_exceptional_list_test,
        .argtable = &exceptional_list_test,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&ble_mesh_exceptional_list_test_cmd));
#endif

    init_transactions();
}
