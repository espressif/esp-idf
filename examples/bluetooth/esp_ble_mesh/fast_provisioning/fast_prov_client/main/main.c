/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_generic_model_api.h"

#include "ble_mesh_fast_prov_common.h"
#include "ble_mesh_fast_prov_operation.h"
#include "ble_mesh_fast_prov_client_model.h"
#include "ble_mesh_example_init.h"

#define TAG "EXAMPLE"

#define PROV_OWN_ADDR       0x0001
#define APP_KEY_OCTET       0x12
#define GROUP_ADDRESS       0xC000

static uint8_t dev_uuid[16] = { 0xdd, 0xdd };
static uint8_t match[] = { 0xdd, 0xdd };

static const esp_ble_mesh_client_op_pair_t fast_prov_cli_op_pair[] = {
    { ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_SET,      ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS      },
    { ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_ADD,   ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_STATUS   },
    { ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_GET, ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_STATUS },
};

static esp_ble_mesh_cfg_srv_t config_server = {
    /* 3 transmissions with a 20ms interval */
    .net_transmit = ESP_BLE_MESH_TRANSMIT(2, 20),
    .relay = ESP_BLE_MESH_RELAY_DISABLED,
    .relay_retransmit = ESP_BLE_MESH_TRANSMIT(2, 20),
    .beacon = ESP_BLE_MESH_BEACON_ENABLED,
#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
    .gatt_proxy = ESP_BLE_MESH_GATT_PROXY_ENABLED,
#else
    .gatt_proxy = ESP_BLE_MESH_GATT_PROXY_NOT_SUPPORTED,
#endif
#if defined(CONFIG_BLE_MESH_FRIEND)
    .friend_state = ESP_BLE_MESH_FRIEND_ENABLED,
#else
    .friend_state = ESP_BLE_MESH_FRIEND_NOT_SUPPORTED,
#endif
    .default_ttl = 7,
};
esp_ble_mesh_client_t config_client;
esp_ble_mesh_client_t gen_onoff_client;
esp_ble_mesh_client_t fast_prov_client = {
    .op_pair_size = ARRAY_SIZE(fast_prov_cli_op_pair),
    .op_pair = fast_prov_cli_op_pair,
};

static esp_ble_mesh_model_op_t fast_prov_cli_op[] = {
    ESP_BLE_MESH_MODEL_OP(ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS,      1),
    ESP_BLE_MESH_MODEL_OP(ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_STATUS,   2),
    ESP_BLE_MESH_MODEL_OP(ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_STATUS, 2),
    ESP_BLE_MESH_MODEL_OP_END,
};

static esp_ble_mesh_model_t root_models[] = {
    ESP_BLE_MESH_MODEL_CFG_SRV(&config_server),
    ESP_BLE_MESH_MODEL_CFG_CLI(&config_client),
    ESP_BLE_MESH_MODEL_GEN_ONOFF_CLI(NULL, &gen_onoff_client),
};

static esp_ble_mesh_model_t vnd_models[] = {
    ESP_BLE_MESH_VENDOR_MODEL(CID_ESP, ESP_BLE_MESH_VND_MODEL_ID_FAST_PROV_CLI,
    fast_prov_cli_op, NULL, &fast_prov_client),
};

static esp_ble_mesh_elem_t elements[] = {
    ESP_BLE_MESH_ELEMENT(0, root_models, vnd_models),
};

static esp_ble_mesh_comp_t comp = {
    .cid           = CID_ESP,
    .element_count = ARRAY_SIZE(elements),
    .elements      = elements,
};

static esp_ble_mesh_prov_t prov = {
    .prov_uuid           = dev_uuid,
    .prov_unicast_addr   = PROV_OWN_ADDR,
    .prov_start_address  = 0x0005,
    .prov_attention      = 0x00,
    .prov_algorithm      = 0x00,
    .prov_pub_key_oob    = 0x00,
    .prov_static_oob_val = NULL,
    .prov_static_oob_len = 0x00,
    .flags               = 0x00,
    .iv_index            = 0x00,
};

example_prov_info_t prov_info = {
    .net_idx       = ESP_BLE_MESH_KEY_PRIMARY,
    .app_idx       = ESP_BLE_MESH_KEY_PRIMARY,
    .node_addr_cnt = 100,
    .unicast_max   = 0x7FFF,
    .group_addr    = GROUP_ADDRESS,
    .max_node_num  = 0x01,
};

static void provisioner_prov_link_open(esp_ble_mesh_prov_bearer_t bearer)
{
    ESP_LOGI(TAG, "%s link open", bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT");
}

static void provisioner_prov_link_close(esp_ble_mesh_prov_bearer_t bearer, uint8_t reason)
{
    ESP_LOGI(TAG, "%s link close, reason 0x%02x",
             bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT", reason);

    if (bearer == ESP_BLE_MESH_PROV_ADV && reason != 0x00) {
        prov_info.max_node_num++;
    }
}

static void provisioner_prov_complete(int node_index, const uint8_t uuid[16], uint16_t unicast_addr,
                                      uint8_t elem_num, uint16_t net_idx)
{
    example_node_info_t *node = NULL;
    char name[11] = {0};
    esp_err_t err;

    ESP_LOGI(TAG, "Node index: 0x%x, unicast address: 0x%02x, element num: %d, netkey index: 0x%02x",
             node_index, unicast_addr, elem_num, net_idx);
    ESP_LOGI(TAG, "Node uuid:  %s", bt_hex(uuid, 16));

    sprintf(name, "%s%d", "NODE-", node_index);
    if (esp_ble_mesh_provisioner_set_node_name(node_index, name)) {
        ESP_LOGE(TAG, "%s: Failed to set node name", __func__);
        return;
    }

    /* Sets node info */
    err = example_store_node_info(uuid, unicast_addr, elem_num, prov_info.net_idx,
                                  prov_info.app_idx, LED_OFF);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to set node info", __func__);
        return;
    }

    /* Gets node info */
    node = example_get_node_info(unicast_addr);
    if (!node) {
        ESP_LOGE(TAG, "%s: Failed to get node info", __func__);
        return;
    }

    /* The Provisioner will send Config AppKey Add to the node. */
    example_msg_common_info_t info = {
        .net_idx = node->net_idx,
        .app_idx = node->app_idx,
        .dst = node->unicast_addr,
        .timeout = 0,
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 2, 0)
        .role = ROLE_PROVISIONER,
#endif
    };
    esp_ble_mesh_cfg_app_key_add_t add_key = {
        .net_idx = prov_info.net_idx,
        .app_idx = prov_info.app_idx,
    };
    memcpy(add_key.app_key, prov_info.app_key, 16);
    err = example_send_config_appkey_add(config_client.model, &info, &add_key);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to send Config AppKey Add message", __func__);
        return;
    }
}

static void example_recv_unprov_adv_pkt(uint8_t dev_uuid[16], uint8_t addr[BLE_MESH_ADDR_LEN],
                                        esp_ble_mesh_addr_type_t addr_type, uint16_t oob_info,
                                        uint8_t adv_type, esp_ble_mesh_prov_bearer_t bearer)
{
    esp_ble_mesh_unprov_dev_add_t add_dev = {0};
    esp_ble_mesh_dev_add_flag_t flag;
    esp_err_t err;
    bool reprov;

    if (bearer & ESP_BLE_MESH_PROV_ADV) {
        /* Checks if the device has been provisioned previously. If the device
         * is a re-provisioned one, we will ignore the 'max_node_num' count and
         * start to provision it directly.
         */
        reprov = example_is_node_exist(dev_uuid);
        if (reprov) {
            goto add;
        }

        if (prov_info.max_node_num == 0) {
            return;
        }

        ESP_LOGI(TAG, "address:  %s, address type: %d, adv type: %d", bt_hex(addr, 6), addr_type, adv_type);
        ESP_LOGI(TAG, "dev uuid: %s", bt_hex(dev_uuid, 16));
        ESP_LOGI(TAG, "oob info: %d, bearer: %s", oob_info, (bearer & ESP_BLE_MESH_PROV_ADV) ? "PB-ADV" : "PB-GATT");

add:
        memcpy(add_dev.addr, addr, 6);
        add_dev.addr_type = (esp_ble_mesh_addr_type_t)addr_type;
        memcpy(add_dev.uuid, dev_uuid, 16);
        add_dev.oob_info = oob_info;
        add_dev.bearer = (esp_ble_mesh_prov_bearer_t)bearer;
        flag = ADD_DEV_RM_AFTER_PROV_FLAG | ADD_DEV_START_PROV_NOW_FLAG | ADD_DEV_FLUSHABLE_DEV_FLAG;
        err = esp_ble_mesh_provisioner_add_unprov_dev(&add_dev, flag);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to start provisioning a device", __func__);
            return;
        }

        if (!reprov) {
            if (prov_info.max_node_num) {
                prov_info.max_node_num--;
            }
        }
    }
}

static void example_provisioning_callback(esp_ble_mesh_prov_cb_event_t event,
        esp_ble_mesh_prov_cb_param_t *param)
{
    switch (event) {
    case ESP_BLE_MESH_PROV_REGISTER_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROV_REGISTER_COMP_EVT, err_code: %d",
                 param->prov_register_comp.err_code);
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_ENABLE_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_PROV_ENABLE_COMP_EVT");
        break;
    case ESP_BLE_MESH_PROVISIONER_RECV_UNPROV_ADV_PKT_EVT:
        example_recv_unprov_adv_pkt(param->provisioner_recv_unprov_adv_pkt.dev_uuid, param->provisioner_recv_unprov_adv_pkt.addr,
                                    param->provisioner_recv_unprov_adv_pkt.addr_type, param->provisioner_recv_unprov_adv_pkt.oob_info,
                                    param->provisioner_recv_unprov_adv_pkt.adv_type, param->provisioner_recv_unprov_adv_pkt.bearer);
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT, bearer %s",
                 param->provisioner_prov_link_open.bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT");
        provisioner_prov_link_open(param->provisioner_prov_link_open.bearer);
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_LINK_CLOSE_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_PROV_LINK_CLOSE_EVT, bearer %s reason 0x%02x",
                 param->provisioner_prov_link_close.bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT",
                 param->provisioner_prov_link_close.reason);
        provisioner_prov_link_close(param->provisioner_prov_link_close.bearer,
                                    param->provisioner_prov_link_close.reason);
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT");
        provisioner_prov_complete(param->provisioner_prov_complete.node_idx,
                                  param->provisioner_prov_complete.device_uuid,
                                  param->provisioner_prov_complete.unicast_addr,
                                  param->provisioner_prov_complete.element_num,
                                  param->provisioner_prov_complete.netkey_idx);
        break;
    case ESP_BLE_MESH_PROVISIONER_ADD_UNPROV_DEV_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_ADD_UNPROV_DEV_COMP_EVT, err_code: %d",
                 param->provisioner_add_unprov_dev_comp.err_code);
        break;
    case ESP_BLE_MESH_PROVISIONER_SET_DEV_UUID_MATCH_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_SET_DEV_UUID_MATCH_COMP_EVT, err_code: %d",
                 param->provisioner_set_dev_uuid_match_comp.err_code);
        break;
    case ESP_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT, err_code: %d",
                 param->provisioner_set_node_name_comp.err_code);
        break;
    case ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT: {
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT, err_code %d", param->provisioner_add_app_key_comp.err_code);
        if (param->provisioner_add_app_key_comp.err_code == ESP_OK) {
            esp_err_t err;
            prov_info.app_idx = param->provisioner_add_app_key_comp.app_idx;
            err = esp_ble_mesh_provisioner_bind_app_key_to_local_model(PROV_OWN_ADDR, prov_info.app_idx,
                    ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_CLI, ESP_BLE_MESH_CID_NVAL);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%s: Failed to bind AppKey with OnOff Client Model", __func__);
                return;
            }
            err = esp_ble_mesh_provisioner_bind_app_key_to_local_model(PROV_OWN_ADDR, prov_info.app_idx,
                    ESP_BLE_MESH_VND_MODEL_ID_FAST_PROV_CLI, CID_ESP);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%s: Failed to bind AppKey with Fast Prov Client Model", __func__);
                return;
            }
        }
        break;
    }
    case ESP_BLE_MESH_PROVISIONER_BIND_APP_KEY_TO_MODEL_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_BIND_APP_KEY_TO_MODEL_COMP_EVT, err_code %d", param->provisioner_bind_app_key_to_model_comp.err_code);
        break;
    default:
        break;
    }
    return;
}

static void example_custom_model_callback(esp_ble_mesh_model_cb_event_t event,
        esp_ble_mesh_model_cb_param_t *param)
{
    uint32_t opcode;
    esp_err_t err;

    switch (event) {
    case ESP_BLE_MESH_MODEL_OPERATION_EVT: {
        if (!param->model_operation.model || !param->model_operation.model->op ||
                !param->model_operation.ctx) {
            ESP_LOGE(TAG, "%s: model_operation parameter is NULL", __func__);
            return;
        }
        opcode = param->model_operation.opcode;
        switch (opcode) {
        case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS:
        case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_STATUS:
        case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_STATUS: {
            ESP_LOGI(TAG, "%s: Fast Prov Client Model receives status, opcode 0x%04" PRIx32, __func__, opcode);
            err = example_fast_prov_client_recv_status(param->model_operation.model,
                    param->model_operation.ctx,
                    param->model_operation.length,
                    param->model_operation.msg);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%s: Failed to handle fast prov status message", __func__);
                return;
            }
            break;
        }
        default:
            ESP_LOGI(TAG, "%s: opcode 0x%04" PRIx32, __func__, param->model_operation.opcode);
            break;
        }
        break;
    }
    case ESP_BLE_MESH_MODEL_SEND_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_SEND_COMP_EVT, err_code %d",
                 param->model_send_comp.err_code);
        break;
    case ESP_BLE_MESH_MODEL_PUBLISH_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_PUBLISH_COMP_EVT, err_code %d",
                 param->model_publish_comp.err_code);
        break;
    case ESP_BLE_MESH_CLIENT_MODEL_RECV_PUBLISH_MSG_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_CLIENT_RECV_PUBLISH_MSG_EVT, opcode 0x%04" PRIx32,
                 param->client_recv_publish_msg.opcode);
        break;
    case ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT, opcode 0x%04" PRIx32 ", dst 0x%04x",
                 param->client_send_timeout.opcode, param->client_send_timeout.ctx->addr);
        err = example_fast_prov_client_recv_timeout(param->client_send_timeout.opcode,
                param->client_send_timeout.model,
                param->client_send_timeout.ctx);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to resend fast prov client message", __func__);
            return;
        }
        break;
    default:
        break;
    }
}

static void example_config_client_callback(esp_ble_mesh_cfg_client_cb_event_t event,
        esp_ble_mesh_cfg_client_cb_param_t *param)
{
    example_node_info_t *node = NULL;
    uint32_t opcode;
    uint16_t address;
    esp_err_t err;

    ESP_LOGI(TAG, "%s, error_code = 0x%02x, event = 0x%02x, addr: 0x%04x",
             __func__, param->error_code, event, param->params->ctx.addr);

    opcode  = param->params->opcode;
    address = param->params->ctx.addr;

    node = example_get_node_info(address);
    if (!node) {
        ESP_LOGE(TAG, "%s: Failed to get node info", __func__);
        return;
    }

    if (param->error_code) {
        ESP_LOGE(TAG, "Failed to send config client message, opcode: 0x%04" PRIx32, opcode);
        return;
    }

    switch (event) {
    case ESP_BLE_MESH_CFG_CLIENT_GET_STATE_EVT:
        break;
    case ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT:
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD: {
            example_fast_prov_info_set_t set = {0};
            if (!node->reprov || !ESP_BLE_MESH_ADDR_IS_UNICAST(node->unicast_min)) {
                /* If the node is a new one or the node is re-provisioned but the information of the node
                 * has not been set before, here we will set the Fast Prov Info Set info to the node.
                 */
                node->node_addr_cnt = prov_info.node_addr_cnt;
                node->unicast_min   = prov_info.unicast_min;
                node->unicast_max   = prov_info.unicast_max;
                node->flags         = prov.flags;
                node->iv_index      = prov.iv_index;
                node->fp_net_idx    = prov_info.net_idx;
                node->group_addr    = prov_info.group_addr;
                node->match_len     = prov_info.match_len;
                memcpy(node->match_val, prov_info.match_val, prov_info.match_len);
                node->action        = 0x81;
            }
            set.ctx_flags = 0x037F;
            memcpy(&set.node_addr_cnt, &node->node_addr_cnt,
                   sizeof(example_node_info_t) - offsetof(example_node_info_t, node_addr_cnt));
            example_msg_common_info_t info = {
                .net_idx = node->net_idx,
                .app_idx = node->app_idx,
                .dst = node->unicast_addr,
                .timeout = 0,
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 2, 0)
                .role = ROLE_PROVISIONER,
#endif
            };
            err = example_send_fast_prov_info_set(fast_prov_client.model, &info, &set);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%s: Failed to set Fast Prov Info Set message", __func__);
                return;
            }
            break;
        }
        default:
            break;
        }
        break;
    case ESP_BLE_MESH_CFG_CLIENT_PUBLISH_EVT:
        break;
    case ESP_BLE_MESH_CFG_CLIENT_TIMEOUT_EVT:
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD: {
            example_msg_common_info_t info = {
                .net_idx = node->net_idx,
                .app_idx = node->app_idx,
                .dst = node->unicast_addr,
                .timeout = 0,
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 2, 0)
                .role = ROLE_PROVISIONER,
#endif
            };
            esp_ble_mesh_cfg_app_key_add_t add_key = {
                .net_idx = prov_info.net_idx,
                .app_idx = prov_info.app_idx,
            };
            memcpy(add_key.app_key, prov_info.app_key, 16);
            err = example_send_config_appkey_add(config_client.model, &info, &add_key);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%s: Failed to send Config AppKey Add message", __func__);
                return;
            }
            break;
        }
        default:
            break;
        }
        break;
    default:
        ESP_LOGE(TAG, "Not a config client status message event");
        break;
    }
}

static void example_generic_client_callback(esp_ble_mesh_generic_client_cb_event_t event,
        esp_ble_mesh_generic_client_cb_param_t *param)
{
    example_node_info_t *node = NULL;
    uint32_t opcode;
    uint16_t address;

    ESP_LOGI(TAG, "%s, error_code = 0x%02x, event = 0x%02x, addr: 0x%04x",
             __func__, param->error_code, event, param->params->ctx.addr);

    opcode  = param->params->opcode;
    address = param->params->ctx.addr;

    node = example_get_node_info(address);
    if (!node) {
        ESP_LOGE(TAG, "%s: Failed to get node info", __func__);
        return;
    }

    if (param->error_code) {
        ESP_LOGE(TAG, "Failed to send generic client message, opcode: 0x%04" PRIx32, opcode);
        return;
    }

    switch (event) {
    case ESP_BLE_MESH_GENERIC_CLIENT_GET_STATE_EVT:
        break;
    case ESP_BLE_MESH_GENERIC_CLIENT_SET_STATE_EVT:
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET:
            node->onoff = param->status_cb.onoff_status.present_onoff;
            ESP_LOGI(TAG, "node->onoff: 0x%02x", node->onoff);
            break;
        default:
            break;
        }
        break;
    case ESP_BLE_MESH_GENERIC_CLIENT_PUBLISH_EVT:
        break;
    case ESP_BLE_MESH_GENERIC_CLIENT_TIMEOUT_EVT:
        break;
    default:
        ESP_LOGE(TAG, "Not a generic client status message event");
        break;
    }
}

static esp_err_t ble_mesh_init(void)
{
    esp_err_t err;

    prov_info.unicast_min = prov.prov_start_address + prov_info.max_node_num;
    prov_info.match_len   = sizeof(match);
    memcpy(prov_info.match_val, match, sizeof(match));
    memset(prov_info.app_key, APP_KEY_OCTET, sizeof(prov_info.app_key));

    esp_ble_mesh_register_prov_callback(example_provisioning_callback);
    esp_ble_mesh_register_custom_model_callback(example_custom_model_callback);
    esp_ble_mesh_register_config_client_callback(example_config_client_callback);
    esp_ble_mesh_register_generic_client_callback(example_generic_client_callback);

    err = esp_ble_mesh_init(&prov, &comp);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to initialize BLE Mesh", __func__);
        return ESP_FAIL;
    }

    err = esp_ble_mesh_provisioner_set_dev_uuid_match(match, 0x02, 0x00, false);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to set matching device UUID", __func__);
        return ESP_FAIL;
    }

    err = esp_ble_mesh_client_model_init(&vnd_models[0]);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to initialize fast prov client model", __func__);
        return ESP_FAIL;
    }

    err = esp_ble_mesh_provisioner_prov_enable((esp_ble_mesh_prov_bearer_t)(ESP_BLE_MESH_PROV_ADV | ESP_BLE_MESH_PROV_GATT));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to enable provisioning", __func__);
        return ESP_FAIL;
    }

    err = esp_ble_mesh_provisioner_add_local_app_key(prov_info.app_key, prov_info.net_idx, prov_info.app_idx);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to add local application key", __func__);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "BLE Mesh Provisioner initialized");

    return err;
}

void app_main(void)
{
    esp_err_t err;

    ESP_LOGI(TAG, "Initializing...");

    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    err = bluetooth_init();
    if (err) {
        ESP_LOGE(TAG, "esp32_bluetooth_init failed (err %d)", err);
        return;
    }

    ble_mesh_get_dev_uuid(dev_uuid);

    /* Initialize the Bluetooth Mesh Subsystem */
    err = ble_mesh_init();
    if (err) {
        ESP_LOGE(TAG, "Failed to initialize BLE Mesh (err %d)", err);
    }
}
