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

#include "esp_ble_mesh_networking_api.h"
#include "ble_mesh_adapter.h"

#if (CONFIG_BLE_MESH_CFG_CLI)
typedef struct {
    struct arg_str *action_type;
    struct arg_str *set_state;
    struct arg_int *opcode;
    struct arg_int *count;
    struct arg_int *feature;
    struct arg_int *unicast_address;
    struct arg_int *appkey_index;
    struct arg_int *mod_id;
    struct arg_int *addr;
    struct arg_int *cid;
    struct arg_int *value;
    struct arg_int *relay_statue;
    struct arg_int *relay_transmit;
    struct arg_int *net_idx;
    struct arg_end *end;
} ble_mesh_client_get_set_state_t;
ble_mesh_client_get_set_state_t configuration_client_model_operation;

void ble_mesh_register_configuration_client_model_command(void);

void ble_mesh_register_configuration_client_model(void)
{
    ble_mesh_register_configuration_client_model_command();
}

void ble_mesh_configuration_client_model_cb(esp_ble_mesh_cfg_client_cb_event_t event,
                                            esp_ble_mesh_cfg_client_cb_param_t *param)
{
    uint32_t opcode;
    ESP_LOGD(TAG, "enter %s, event = %x, error_code = %x", __func__, event, param->error_code);

    if (!param->error_code) {
        opcode = param->params->opcode;
        switch (event) {
        case ESP_BLE_MESH_CFG_CLIENT_GET_STATE_EVT:
            switch (opcode) {
            case ESP_BLE_MESH_MODEL_OP_BEACON_GET:
                ESP_LOGI(TAG, "CfgClient:beacon,0x%x", param->status_cb.beacon_status.beacon);
                break;
            case ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET:
                ESP_LOGI(TAG, "CfgClient:page,0x%x,len,0x%x", param->status_cb.comp_data_status.page, param->status_cb.comp_data_status.composition_data->len);
                break;
            case ESP_BLE_MESH_MODEL_OP_DEFAULT_TTL_GET:
                ESP_LOGI(TAG, "CfgClient:ttl,0x%x", param->status_cb.default_ttl_status.default_ttl);
                break;
            case ESP_BLE_MESH_MODEL_OP_GATT_PROXY_GET:
                ESP_LOGI(TAG, "CfgClient:proxy,0x%x", param->status_cb.gatt_proxy_status.gatt_proxy);
                break;
            case ESP_BLE_MESH_MODEL_OP_RELAY_GET:
                ESP_LOGI(TAG, "CfgClient:relay,0x%x,retransmit,0x%x", param->status_cb.relay_status.relay, param->status_cb.relay_status.retransmit);
                break;
            case ESP_BLE_MESH_MODEL_OP_MODEL_PUB_GET:
                if (param->status_cb.model_pub_status.status == ESP_OK) {
                    ESP_LOGI(TAG, "CfgClient:PublishGet,OK,0x%x", param->status_cb.model_pub_status.publish_addr);
                } else {
                    ESP_LOGI(TAG, "CfgClient:PublishGet,Fail");
                }
                
                break;
            case ESP_BLE_MESH_MODEL_OP_FRIEND_GET:
                ESP_LOGI(TAG, "CfgClient:friend,0x%x", param->status_cb.friend_status.friend_state);
                break;
            case ESP_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_GET:
                if (param->status_cb.heartbeat_pub_status.status == ESP_OK) {
                    ESP_LOGI(TAG, "CfgClient:HeartBeatPubGet,OK,destination:0x%x,countlog:0x%x,periodlog:0x%x,ttl:0x%x,features:0x%x,net_idx:0x%x",
                             param->status_cb.heartbeat_pub_status.dst, param->status_cb.heartbeat_pub_status.count,  param->status_cb.heartbeat_pub_status.period,
                             param->status_cb.heartbeat_pub_status.ttl, param->status_cb.heartbeat_pub_status.features, param->status_cb.heartbeat_pub_status.net_idx);
                } else {
                    ESP_LOGI(TAG, "CfgClient:HeartBeatGet,Fail,%d", param->status_cb.heartbeat_pub_status.status);
                }
                break;
            case ESP_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_GET:
                if (param->status_cb.heartbeat_sub_status.status == ESP_OK) {
                    ESP_LOGI(TAG, "CfgClient:HeartBeatSubGet,OK,source:0x%x,destination:0x%x, periodlog:0x%x,countlog:0x%x,minhops:0x%x,maxhops:0x%x",
                             param->status_cb.heartbeat_sub_status.src, param->status_cb.heartbeat_sub_status.dst, param->status_cb.heartbeat_sub_status.period,
                             param->status_cb.heartbeat_sub_status.count, param->status_cb.heartbeat_sub_status.min_hops, param->status_cb.heartbeat_sub_status.max_hops);
                } else {
                    ESP_LOGI(TAG, "CfgClient:HeartBeatSubGet,Fail,%d", param->status_cb.heartbeat_sub_status.status);
                }
                break;
            default:
                ESP_LOGI(TAG, "Not supported config client get message opcode");
                break;
            }
            break;
        case ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT:
            switch (opcode) {
            case ESP_BLE_MESH_MODEL_OP_BEACON_SET:
                ESP_LOGI(TAG, "CfgClient:beacon,0x%x", param->status_cb.beacon_status.beacon);
                break;
            case ESP_BLE_MESH_MODEL_OP_DEFAULT_TTL_SET:
                ESP_LOGI(TAG, "CfgClient:ttl,0x%x", param->status_cb.default_ttl_status.default_ttl);
                break;
            case ESP_BLE_MESH_MODEL_OP_GATT_PROXY_SET:
                ESP_LOGI(TAG, "CfgClient:proxy,0x%x", param->status_cb.gatt_proxy_status.gatt_proxy);
                break;
            case ESP_BLE_MESH_MODEL_OP_RELAY_SET:
                ESP_LOGI(TAG, "CfgClient:relay,0x%x, retransmit: 0x%x", param->status_cb.relay_status.relay, param->status_cb.relay_status.retransmit);
                break;
            case ESP_BLE_MESH_MODEL_OP_MODEL_PUB_SET:
                if (param->status_cb.model_pub_status.status == ESP_OK) {
                    ESP_LOGI(TAG, "CfgClient:PublishSet,OK,0x%x", param->status_cb.model_pub_status.publish_addr);
                } else {
                    ESP_LOGI(TAG, "CfgClient:PublishSet,Fail,%d", param->status_cb.model_pub_status.status);
                }                
                break;
            case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_ADD:
                if (param->status_cb.model_sub_status.status == ESP_OK) {
                    ESP_LOGI(TAG, "CnfClient:SubAdd,OK,%x,%x", param->status_cb.model_sub_status.element_addr, param->status_cb.model_sub_status.sub_addr);
                } else {
                    ESP_LOGI(TAG, "CnfClient:SubAdd,Fail,%x", param->status_cb.model_sub_status.status);
                }
                break;
            case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE:
                if (param->status_cb.model_sub_status.status == ESP_OK) {
                    ESP_LOGI(TAG, "CnfClient:SubDel,OK,%x,%x", param->status_cb.model_sub_status.element_addr, param->status_cb.model_sub_status.sub_addr);
                } else {
                    ESP_LOGI(TAG, "CnfClient:SubDel,Fail,%x", param->status_cb.model_sub_status.status);
                }
                break;
            case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_OVERWRITE:
                break;
            case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_ADD:
                break;
            case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_DELETE:
                break;
            case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_OVERWRITE:
                break;
            case ESP_BLE_MESH_MODEL_OP_NET_KEY_ADD:
                if (param->status_cb.netkey_status.status == ESP_OK) {
                    ESP_LOGI(TAG, "CfgClient:NetKeyAdd,OK");
                } else {
                    ESP_LOGI(TAG, "CfgClient:NetKeyAdd,Fail,%d", param->status_cb.netkey_status.status);
                }
                break;
            case ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD:
                if (param->status_cb.appkey_status.status == ESP_OK) {
                    ESP_LOGI(TAG, "CnfClient:AddAppkey,OK,%x,%x,%x", param->status_cb.appkey_status.net_idx, param->status_cb.appkey_status.app_idx, param->params->ctx.addr);
                } else {
                    ESP_LOGI(TAG, "CnfClient:AddAppkey,Fail,%x", param->status_cb.appkey_status.status);
                }
                break;
            case ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND:
                if (param->status_cb.model_app_status.status == ESP_OK) {
                    ESP_LOGI(TAG, "CnfClient:AppkeyBind,OK,%x,%x,%x", param->status_cb.model_app_status.app_idx, param->status_cb.model_app_status.model_id, param->params->ctx.addr);
                } else {
                    ESP_LOGI(TAG, "CnfClient:AppkeyBind,Fail,%x", param->status_cb.model_app_status.status);
                }
                break;
            case ESP_BLE_MESH_MODEL_OP_FRIEND_SET:
                ESP_LOGI(TAG, "CfgClient:friend: 0x%x", param->status_cb.friend_status.friend_state);
                break;
            case ESP_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_SET:
                if (param->status_cb.heartbeat_pub_status.status == ESP_OK) {
                    ESP_LOGI(TAG, "CfgClient:HeartBeatPubSet,OK,destination:0x%x,countlog:0x%x, periodlog:0x%x,ttl:0x%x,features:0x%x,net_idx: 0x%x",
                             param->status_cb.heartbeat_pub_status.dst, param->status_cb.heartbeat_pub_status.count, param->status_cb.heartbeat_pub_status.period,
                             param->status_cb.heartbeat_pub_status.ttl, param->status_cb.heartbeat_pub_status.features, param->status_cb.heartbeat_pub_status.net_idx);
                } else {
                    ESP_LOGI(TAG, "CfgClient:HeartBeatSet,Fail,%d", param->status_cb.heartbeat_pub_status.status);
                }
                break;
            case ESP_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_SET:
                if (param->status_cb.heartbeat_sub_status.status == ESP_OK) {
                    ESP_LOGI(TAG, "CfgClient:HeartBeatSubSet,OK,source:0x%x,destination:0x%x, periodlog:0x%x,countlog:0x%x,minhops:0x%x,maxhops:0x%x",
                             param->status_cb.heartbeat_sub_status.src, param->status_cb.heartbeat_sub_status.dst, param->status_cb.heartbeat_sub_status.period,
                             param->status_cb.heartbeat_sub_status.count, param->status_cb.heartbeat_sub_status.min_hops, param->status_cb.heartbeat_sub_status.max_hops);
                } else {
                    ESP_LOGI(TAG, "CfgClient:HeartBeatSubSet,Fail,%d", param->status_cb.heartbeat_sub_status.status);
                }
                break;
            default:
                ESP_LOGI(TAG, "Not supported config client set message opcode");
                break;
            }
            break;
        case ESP_BLE_MESH_CFG_CLIENT_PUBLISH_EVT:
            ESP_LOGI(TAG, "CnfClient:Publish,OK");
            break;
        case ESP_BLE_MESH_CFG_CLIENT_EVT_MAX:
            ESP_LOGI(TAG, "CnfClient:MaxEvt");
            break;
        case ESP_BLE_MESH_CFG_CLIENT_TIMEOUT_EVT:
            ESP_LOGI(TAG, "CfgClient:TimeOut");
            break;
        default:
            ESP_LOGI(TAG, "CfgClient:InvalidEvent");
            break;
        }
    } else {
        ESP_LOGI(TAG, "CnfClient:Fail,%d", param->error_code);
    }
    ESP_LOGD(TAG, "exit %s", __func__);
}

int ble_mesh_configuration_client_model_operation(int argc, char **argv)
{
    int err = ESP_OK;
    esp_ble_mesh_elem_t *element = NULL;
    const uint8_t *app_key = NULL;
    esp_ble_mesh_cfg_default_ttl_set_t ttl_set;
    esp_ble_mesh_cfg_gatt_proxy_set_t proxy_set;
    esp_ble_mesh_cfg_app_key_add_t app_key_add;
    esp_ble_mesh_cfg_heartbeat_pub_set_t heartbeat_pub_set;
    esp_ble_mesh_cfg_model_pub_set_t mod_pub_set = {
        .company_id = 0xFFFF,
        .cred_flag = false,
        .publish_period = 0,
        .publish_retransmit = 0,
    };
    esp_ble_mesh_cfg_model_sub_add_t mod_sub_add = {
        .company_id = 0xFFFF,
    };
    esp_ble_mesh_cfg_model_sub_delete_t mod_sub_del = {
        .company_id = 0xFFFF,
    };
    esp_ble_mesh_cfg_relay_set_t relay_set;
    esp_ble_mesh_client_common_param_t client_common = {
        .msg_role = ROLE_PROVISIONER,
        .msg_timeout = 0,
        .ctx.send_ttl = 7,
    };
    esp_ble_mesh_cfg_client_get_state_t get_state = {
        .comp_data_get.page = 0,
        .model_pub_get.company_id = 0xFFFF,
    };
    esp_ble_mesh_cfg_model_app_bind_t mod_app_bind = {
        .company_id = 0xFFFF,
    };

    element = esp_ble_mesh_find_element(esp_ble_mesh_get_primary_element_address());
    if (!element) {
        ESP_LOGE(TAG, "Element 0x%04x not exists", esp_ble_mesh_get_primary_element_address());
        return ESP_FAIL;
    }

    client_common.model = esp_ble_mesh_find_sig_model(element, ESP_BLE_MESH_MODEL_ID_CONFIG_CLI);
    if (!client_common.model) {
        ESP_LOGE(TAG, "CfgClient:LoadModel,Fail");
        return ESP_FAIL;
    }

    int nerrors = arg_parse(argc, argv, (void **) &configuration_client_model_operation);
    if (nerrors != 0) {
        arg_print_errors(stderr, configuration_client_model_operation.end, argv[0]);
        return 1;
    }

    if (configuration_client_model_operation.opcode->count != 0) {
        client_common.opcode = configuration_client_model_operation.opcode->ival[0];
    }

    if (configuration_client_model_operation.net_idx->count != 0) {
        client_common.ctx.net_idx = configuration_client_model_operation.net_idx->ival[0];
        app_key_add.net_idx = configuration_client_model_operation.net_idx->ival[0];
        heartbeat_pub_set.net_idx = configuration_client_model_operation.net_idx->ival[0];
    }

    if (configuration_client_model_operation.unicast_address->count != 0) {
        client_common.ctx.addr = configuration_client_model_operation.unicast_address->ival[0];
        get_state.model_pub_get.element_addr = configuration_client_model_operation.unicast_address->ival[0];
        mod_app_bind.element_addr = configuration_client_model_operation.unicast_address->ival[0];
        mod_sub_add.element_addr = configuration_client_model_operation.unicast_address->ival[0];
        mod_sub_del.element_addr = configuration_client_model_operation.unicast_address->ival[0];
        mod_pub_set.element_addr = configuration_client_model_operation.unicast_address->ival[0];
    }

    if (configuration_client_model_operation.appkey_index->count != 0) {
        client_common.ctx.app_idx = configuration_client_model_operation.appkey_index->ival[0];
        mod_app_bind.model_app_idx = configuration_client_model_operation.appkey_index->ival[0];
        app_key_add.app_idx = configuration_client_model_operation.appkey_index->ival[0];
        mod_pub_set.publish_app_idx = configuration_client_model_operation.appkey_index->ival[0];
    }

    if (configuration_client_model_operation.value->count != 0) {
        ttl_set.ttl = configuration_client_model_operation.value->ival[0];
        proxy_set.gatt_proxy = configuration_client_model_operation.value->ival[0];
        mod_pub_set.publish_ttl = configuration_client_model_operation.value->ival[0];
        heartbeat_pub_set.ttl = configuration_client_model_operation.value->ival[0];
    }

    if (configuration_client_model_operation.addr->count != 0) {
        mod_sub_del.sub_addr = configuration_client_model_operation.addr->ival[0];
        mod_sub_add.sub_addr = configuration_client_model_operation.addr->ival[0];
        mod_pub_set.publish_addr = configuration_client_model_operation.addr->ival[0];
        heartbeat_pub_set.dst = configuration_client_model_operation.addr->ival[0];
    }

    if (configuration_client_model_operation.mod_id->count != 0) {
        mod_app_bind.model_id = configuration_client_model_operation.mod_id->ival[0];
        mod_sub_add.model_id = configuration_client_model_operation.mod_id->ival[0];
        mod_sub_del.model_id = configuration_client_model_operation.mod_id->ival[0];
        get_state.model_pub_get.model_id = configuration_client_model_operation.mod_id->ival[0];;
        mod_pub_set.model_id = configuration_client_model_operation.mod_id->ival[0];
    }

    if (configuration_client_model_operation.relay_statue->count != 0) {
        relay_set.relay = configuration_client_model_operation.relay_statue->ival[0];
        mod_pub_set.publish_period = configuration_client_model_operation.relay_statue->ival[0];
        heartbeat_pub_set.period = configuration_client_model_operation.relay_statue->ival[0];
    }

    if (configuration_client_model_operation.relay_transmit->count != 0) {
        relay_set.relay_retransmit = configuration_client_model_operation.relay_transmit->ival[0];
        mod_pub_set.publish_retransmit = configuration_client_model_operation.relay_transmit->ival[0];
    }

    if (configuration_client_model_operation.cid->count != 0) {
        mod_app_bind.company_id = configuration_client_model_operation.cid->ival[0];
        mod_sub_del.company_id = configuration_client_model_operation.cid->ival[0];
        mod_sub_add.company_id = configuration_client_model_operation.cid->ival[0];
        mod_pub_set.company_id = configuration_client_model_operation.cid->ival[0];
    }

    if (configuration_client_model_operation.count->count != 0) {
        heartbeat_pub_set.count = configuration_client_model_operation.count->ival[0];
    }

    if (configuration_client_model_operation.feature->count != 0) {
        heartbeat_pub_set.feature = configuration_client_model_operation.feature->ival[0];
    }

    if (configuration_client_model_operation.action_type->count != 0) {
        if (strcmp(configuration_client_model_operation.action_type->sval[0], "get") == 0) {
            err = esp_ble_mesh_config_client_get_state(&client_common, &get_state);
        } else if (strcmp(configuration_client_model_operation.action_type->sval[0], "set") == 0) {
            if (configuration_client_model_operation.set_state->count != 0) {
                if (strcmp(configuration_client_model_operation.set_state->sval[0], "appkey") == 0) {
                    app_key = esp_ble_mesh_provisioner_get_local_app_key(app_key_add.net_idx, app_key_add.app_idx);
                    if (app_key == NULL) {
                        ESP_LOGE(TAG, "CnfClient:AddAppkey,Fail,app key or network key NULL");
                        return ESP_FAIL;
                    } else {
                        memcpy(app_key_add.app_key, app_key, 16);
                    }
                    err = esp_ble_mesh_config_client_set_state(&client_common, (esp_ble_mesh_cfg_client_set_state_t *)&app_key_add);
                } else if (strcmp(configuration_client_model_operation.set_state->sval[0], "appbind") == 0) {
                    err = esp_ble_mesh_config_client_set_state(&client_common, (esp_ble_mesh_cfg_client_set_state_t *)&mod_app_bind);
                } else if (strcmp(configuration_client_model_operation.set_state->sval[0], "ttl") == 0) {
                    err = esp_ble_mesh_config_client_set_state(&client_common, (esp_ble_mesh_cfg_client_set_state_t *)&ttl_set);
                } else if (strcmp(configuration_client_model_operation.set_state->sval[0], "proxy") == 0) {
                    err = esp_ble_mesh_config_client_set_state(&client_common, (esp_ble_mesh_cfg_client_set_state_t *)&proxy_set);
                } else if (strcmp(configuration_client_model_operation.set_state->sval[0], "subadd") == 0) {
                    err = esp_ble_mesh_config_client_set_state(&client_common, (esp_ble_mesh_cfg_client_set_state_t *)&mod_sub_add);
                } else if (strcmp(configuration_client_model_operation.set_state->sval[0], "subdel") == 0) {
                    err = esp_ble_mesh_config_client_set_state(&client_common, (esp_ble_mesh_cfg_client_set_state_t *)&mod_sub_del);
                } else if (strcmp(configuration_client_model_operation.set_state->sval[0], "relay") == 0) {
                    err = esp_ble_mesh_config_client_set_state(&client_common, (esp_ble_mesh_cfg_client_set_state_t *)&relay_set);
                } else if (strcmp(configuration_client_model_operation.set_state->sval[0], "pubset") == 0) {
                    err = esp_ble_mesh_config_client_set_state(&client_common, (esp_ble_mesh_cfg_client_set_state_t *)&mod_pub_set);
                } else if (strcmp(configuration_client_model_operation.set_state->sval[0], "reset") == 0) {
                    err = esp_ble_mesh_config_client_set_state(&client_common, NULL);
                }else if(strcmp(configuration_client_model_operation.set_state->sval[0], "hbpub") == 0){
                    err = esp_ble_mesh_config_client_set_state(&client_common, (esp_ble_mesh_cfg_client_set_state_t *)&heartbeat_pub_set);
                }
            }
        }
    }

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "ConfigClient:OK");
    } else {
        ESP_LOGI(TAG, "ConfigClient:Fail");
    }
    return err;
}

void ble_mesh_register_configuration_client_model_command(void)
{
    configuration_client_model_operation.action_type = arg_str1("z", NULL, "<action>", "action type");
    configuration_client_model_operation.set_state = arg_str0("x", NULL, "<state>", "set state");
    configuration_client_model_operation.opcode = arg_int0("o", NULL, "<opcode>", "message opcode");
    configuration_client_model_operation.unicast_address = arg_int0("u", NULL, "<address>", "unicast address");
    configuration_client_model_operation.net_idx = arg_int0("n", NULL, "<network>", "net work index");
    configuration_client_model_operation.appkey_index = arg_int0("i", NULL, "<index>", "appkey index");
    configuration_client_model_operation.relay_statue = arg_int0("r", NULL, "<relay>", "relay statue");
    configuration_client_model_operation.relay_transmit = arg_int0("t", NULL, "<transmit>", "relay transmit");
    configuration_client_model_operation.cid = arg_int0("c", NULL, "<cid>", "company id");
    configuration_client_model_operation.value = arg_int0("v", NULL, "<value>", "value");
    configuration_client_model_operation.addr = arg_int0("a", NULL, "<address>", "address");
    configuration_client_model_operation.mod_id = arg_int0("m", NULL, "<mod id>", "model id");
    configuration_client_model_operation.count = arg_int0("b", NULL, "<heartbeat count>", "heartbeat count");
    configuration_client_model_operation.feature = arg_int0("f", NULL, "<features>", "features");
    configuration_client_model_operation.end = arg_end(1);

    const esp_console_cmd_t client_stconfiguration_client_model_operationate_cmd = {
        .command = "bmccm",
        .help = "ble mesh configuration client model",
        .hint = NULL,
        .func = &ble_mesh_configuration_client_model_operation,
        .argtable = &configuration_client_model_operation,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&client_stconfiguration_client_model_operationate_cmd));
}
#endif
