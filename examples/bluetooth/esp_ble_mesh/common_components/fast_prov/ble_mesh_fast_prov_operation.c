/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "esp_ble_mesh_local_data_operation_api.h"

#include "ble_mesh_fast_prov_common.h"
#include "ble_mesh_fast_prov_operation.h"
#include "ble_mesh_fast_prov_client_model.h"
#include "ble_mesh_fast_prov_server_model.h"

#define TAG "FAST_PROV_OP"

/* Provisioned node information context */
static example_node_info_t nodes_info[CONFIG_BLE_MESH_MAX_PROV_NODES] = {
    [0 ... (CONFIG_BLE_MESH_MAX_PROV_NODES - 1)] = {
        .reprov       = false,
        .unicast_addr = ESP_BLE_MESH_ADDR_UNASSIGNED,
        .element_num  = 0x0,
        .net_idx      = ESP_BLE_MESH_KEY_UNUSED,
        .app_idx      = ESP_BLE_MESH_KEY_UNUSED,
        .onoff        = LED_OFF,
        .lack_of_addr = false,
        .unicast_min  = ESP_BLE_MESH_ADDR_UNASSIGNED,
        .unicast_max  = ESP_BLE_MESH_ADDR_UNASSIGNED,
        .flags        = 0x0,
        .iv_index     = 0x0,
        .group_addr   = ESP_BLE_MESH_ADDR_UNASSIGNED,
        .prov_addr    = ESP_BLE_MESH_ADDR_UNASSIGNED,
        .match_len    = 0,
        .action       = FAST_PROV_ACT_NONE,
    }
};

esp_err_t example_store_node_info(const uint8_t uuid[16], uint16_t node_addr,
                                  uint8_t elem_num, uint16_t net_idx,
                                  uint16_t app_idx, uint8_t onoff)
{
    example_node_info_t *node = NULL;

    if (!uuid || !ESP_BLE_MESH_ADDR_IS_UNICAST(node_addr) || !elem_num) {
        return ESP_ERR_INVALID_ARG;
    }

    for (int i = 0; i < ARRAY_SIZE(nodes_info); i++) {
        node = &nodes_info[i];
        if (!memcmp(node->uuid, uuid, 16)) {
            ESP_LOGW(TAG, "%s: reprovisioned node", __func__);
            node->reprov = true;
            node->unicast_addr = node_addr;
            node->element_num = elem_num;
            node->net_idx = net_idx;
            node->app_idx = app_idx;
            node->onoff = onoff;
            return ESP_OK;
        }
    }

    for (int i = 0; i < ARRAY_SIZE(nodes_info); i++) {
        node = &nodes_info[i];
        if (node->unicast_addr == ESP_BLE_MESH_ADDR_UNASSIGNED) {
            memcpy(node->uuid, uuid, 16);
            node->reprov = false;
            node->unicast_addr = node_addr;
            node->element_num = elem_num;
            node->net_idx = net_idx;
            node->app_idx = app_idx;
            node->onoff = onoff;
            node->lack_of_addr = false;
            return ESP_OK;
        }
    }

    ESP_LOGE(TAG, "%s: nodes_info is full", __func__);
    return ESP_FAIL;
}

example_node_info_t *example_get_node_info(uint16_t node_addr)
{
    example_node_info_t *node = NULL;

    if (!ESP_BLE_MESH_ADDR_IS_UNICAST(node_addr)) {
        return NULL;
    }

    for (int i = 0; i < ARRAY_SIZE(nodes_info); i++) {
        node = &nodes_info[i];
        if (node_addr >= node->unicast_addr &&
                node_addr < node->unicast_addr + node->element_num) {
            return node;
        }
    }

    return NULL;
}

bool example_is_node_exist(const uint8_t uuid[16])
{
    example_node_info_t *node = NULL;

    if (!uuid) {
        return false;
    }

    for (int i = 0; i < ARRAY_SIZE(nodes_info); i++) {
        node = &nodes_info[i];
        if (ESP_BLE_MESH_ADDR_IS_UNICAST(node->unicast_addr)) {
            if (!memcmp(node->uuid, uuid, 16)) {
                return true;
            }
        }
    }

    return false;
}

uint16_t example_get_node_address(int node_idx)
{
    return nodes_info[node_idx].unicast_addr;
}

esp_ble_mesh_model_t *example_find_model(uint16_t element_addr, uint16_t model_id,
        uint16_t company_id)
{
    esp_ble_mesh_elem_t *element = NULL;

    if (!ESP_BLE_MESH_ADDR_IS_UNICAST(element_addr)) {
        return NULL;
    }

    element = esp_ble_mesh_find_element(element_addr);
    if (!element) {
        return NULL;
    }

    if (company_id == ESP_BLE_MESH_CID_NVAL) {
        return esp_ble_mesh_find_sig_model(element, model_id);
    } else {
        return esp_ble_mesh_find_vendor_model(element, company_id, model_id);
    }
}

static esp_err_t example_set_app_idx_to_user_data(uint16_t app_idx)
{
    example_fast_prov_server_t *srv_data = NULL;
    esp_ble_mesh_model_t *srv_model = NULL;

    srv_model = example_find_model(esp_ble_mesh_get_primary_element_address(),
                                   ESP_BLE_MESH_VND_MODEL_ID_FAST_PROV_SRV, CID_ESP);
    if (!srv_model) {
        return ESP_FAIL;
    }

    srv_data = (example_fast_prov_server_t *)(srv_model->user_data);
    if (!srv_data) {
        return ESP_FAIL;
    }

    srv_data->app_idx = app_idx;
    return ESP_OK;
}

esp_err_t example_handle_config_app_key_add_evt(uint16_t app_idx)
{
    const esp_ble_mesh_comp_t *comp = NULL;
    esp_ble_mesh_elem_t *element = NULL;
    esp_ble_mesh_model_t *model = NULL;
    int i, j, k;

    comp = esp_ble_mesh_get_composition_data();
    if (!comp) {
        return ESP_FAIL;
    }

    for (i = 0; i < comp->element_count; i++) {
        element = &comp->elements[i];
        /* Bind app_idx with SIG models except the Config Client & Server models */
        for (j = 0; j < element->sig_model_count; j++) {
            model = &element->sig_models[j];
            if (model->model_id == ESP_BLE_MESH_MODEL_ID_CONFIG_SRV ||
                    model->model_id == ESP_BLE_MESH_MODEL_ID_CONFIG_CLI) {
                continue;
            }
            for (k = 0; k < ARRAY_SIZE(model->keys); k++) {
                if (model->keys[k] == app_idx) {
                    break;
                }
            }
            if (k != ARRAY_SIZE(model->keys)) {
                continue;
            }
            for (k = 0; k < ARRAY_SIZE(model->keys); k++) {
                if (model->keys[k] == ESP_BLE_MESH_KEY_UNUSED) {
                    model->keys[k] = app_idx;
                    break;
                }
            }
            if (k == ARRAY_SIZE(model->keys)) {
                ESP_LOGE(TAG, "%s: SIG model (model_id 0x%04x) is full of AppKey",
                         __func__, model->model_id);
            }
        }
        /* Bind app_idx with Vendor models */
        for (j = 0; j < element->vnd_model_count; j++) {
            model = &element->vnd_models[j];
            for (k = 0; k < ARRAY_SIZE(model->keys); k++) {
                if (model->keys[k] == app_idx) {
                    break;
                }
            }
            if (k != ARRAY_SIZE(model->keys)) {
                continue;
            }
            for (k = 0; k < ARRAY_SIZE(model->keys); k++) {
                if (model->keys[k] == ESP_BLE_MESH_KEY_UNUSED) {
                    model->keys[k] = app_idx;
                    break;
                }
            }
            if (k == ARRAY_SIZE(model->keys)) {
                ESP_LOGE(TAG, "%s: Vendor model (model_id 0x%04x, cid: 0x%04x) is full of AppKey",
                         __func__, model->vnd.model_id, model->vnd.company_id);
            }
        }
    }

    return example_set_app_idx_to_user_data(app_idx);
}

esp_err_t example_add_fast_prov_group_address(uint16_t model_id, uint16_t group_addr)
{
    const esp_ble_mesh_comp_t *comp = NULL;
    esp_ble_mesh_elem_t *element = NULL;
    esp_ble_mesh_model_t *model = NULL;
    int i, j;

    if (!ESP_BLE_MESH_ADDR_IS_GROUP(group_addr)) {
        return ESP_ERR_INVALID_ARG;
    }

    comp = esp_ble_mesh_get_composition_data();
    if (!comp) {
        return ESP_FAIL;
    }

    for (i = 0; i < comp->element_count; i++) {
        element = &comp->elements[i];
        model = esp_ble_mesh_find_sig_model(element, model_id);
        if (!model) {
            continue;
        }
        for (j = 0; j < ARRAY_SIZE(model->groups); j++) {
            if (model->groups[j] == group_addr) {
                break;
            }
        }
        if (j != ARRAY_SIZE(model->groups)) {
            ESP_LOGW(TAG, "%s: Group address already exists, element index: %d", __func__, i);
            continue;
        }
        for (j = 0; j < ARRAY_SIZE(model->groups); j++) {
            if (model->groups[j] == ESP_BLE_MESH_ADDR_UNASSIGNED) {
                model->groups[j] = group_addr;
                break;
            }
        }
        if (j == ARRAY_SIZE(model->groups)) {
            ESP_LOGE(TAG, "%s: Model is full of group address, element index: %d", __func__, i);
        }
    }

    return ESP_OK;
}

esp_err_t example_delete_fast_prov_group_address(uint16_t model_id, uint16_t group_addr)
{
    const esp_ble_mesh_comp_t *comp = NULL;
    esp_ble_mesh_elem_t *element = NULL;
    esp_ble_mesh_model_t *model = NULL;
    int i, j;

    if (!ESP_BLE_MESH_ADDR_IS_GROUP(group_addr)) {
        return ESP_ERR_INVALID_ARG;
    }

    comp = esp_ble_mesh_get_composition_data();
    if (comp == NULL) {
        return ESP_FAIL;
    }

    for (i = 0; i < comp->element_count; i++) {
        element = &comp->elements[i];

        model = esp_ble_mesh_find_sig_model(element, model_id);
        if (model == NULL) {
            continue;
        }
        for (j = 0; j < ARRAY_SIZE(model->groups); j++) {
            if (model->groups[j] == group_addr) {
                model->groups[j] = ESP_BLE_MESH_ADDR_UNASSIGNED;
                break;
            }
        }
    }

    return ESP_OK;
}

esp_err_t example_send_config_appkey_add(esp_ble_mesh_model_t *model,
        example_msg_common_info_t *info,
        esp_ble_mesh_cfg_app_key_add_t *add_key)
{
    esp_ble_mesh_client_common_param_t common = {0};
    esp_ble_mesh_cfg_client_set_state_t set = {0};
    const uint8_t *key = NULL;

    if (!model || !info) {
        return ESP_ERR_INVALID_ARG;
    }

    if (add_key) {
        set.app_key_add.net_idx = add_key->net_idx;
        set.app_key_add.app_idx = add_key->app_idx;
        memcpy(set.app_key_add.app_key, add_key->app_key, 16);
    } else {
#if defined(CONFIG_BLE_MESH_FAST_PROV)
        key = esp_ble_mesh_get_fast_prov_app_key(info->net_idx, info->app_idx);
#endif
        if (!key) {
            return ESP_FAIL;
        }
        set.app_key_add.net_idx = info->net_idx;
        set.app_key_add.app_idx = info->app_idx;
        memcpy(set.app_key_add.app_key, key, 16);
    }

    common.opcode       = ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD;
    common.model        = model;
    common.ctx.net_idx  = info->net_idx;
    common.ctx.app_idx  = 0x0000; /* not used for config messages */
    common.ctx.addr     = info->dst;
    common.ctx.send_ttl = 0;
    common.msg_timeout  = info->timeout;
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 2, 0)
    common.msg_role     = info->role;
#endif

    return esp_ble_mesh_config_client_set_state(&common, &set);
}

esp_err_t example_send_generic_onoff_get(esp_ble_mesh_model_t *model,
        example_msg_common_info_t *info)
{
    esp_ble_mesh_generic_client_get_state_t get = {0};
    esp_ble_mesh_client_common_param_t common = {0};

    if (!model || !info) {
        return ESP_ERR_INVALID_ARG;
    }

    common.opcode       = ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET;
    common.model        = model;
    common.ctx.net_idx  = info->net_idx;
    common.ctx.app_idx  = info->app_idx;
    common.ctx.addr     = info->dst;
    common.ctx.send_ttl = 0;
    common.msg_timeout  = info->timeout;
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 2, 0)
    common.msg_role     = info->role;
#endif

    return esp_ble_mesh_generic_client_get_state(&common, &get);
}

#if CONFIG_BLE_MESH_GENERIC_ONOFF_CLI
esp_err_t example_send_generic_onoff_set(esp_ble_mesh_model_t *model,
        example_msg_common_info_t *info,
        uint8_t onoff, uint8_t tid, bool need_ack)
{
    esp_ble_mesh_generic_client_set_state_t set = {0};
    esp_ble_mesh_client_common_param_t common = {0};

    if (!model || !info) {
        return ESP_ERR_INVALID_ARG;
    }

    set.onoff_set.onoff = onoff;
    set.onoff_set.tid   = tid;
    set.onoff_set.op_en = false;

    if (need_ack) {
        common.opcode = ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET;
    } else {
        common.opcode = ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK;
    }
    common.model        = model;
    common.ctx.net_idx  = info->net_idx;
    common.ctx.app_idx  = info->app_idx;
    common.ctx.addr     = info->dst;
    common.ctx.send_ttl = 0;
    common.msg_timeout  = info->timeout;
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 2, 0)
    common.msg_role     = info->role;
#endif

    return esp_ble_mesh_generic_client_set_state(&common, &set);
}
#endif /* CONFIG_BLE_MESH_GENERIC_ONOFF_CLI */

esp_err_t example_send_fast_prov_info_set(esp_ble_mesh_model_t *model,
        example_msg_common_info_t *info,
        example_fast_prov_info_set_t *set)
{
    struct net_buf_simple *msg = NULL;
    esp_err_t err;

    if (!model || !set || !set->ctx_flags || !info) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(TAG, "min:       0x%04x, max:        0x%04x", set->unicast_min, set->unicast_max);
    ESP_LOGI(TAG, "flags:     0x%02x,   iv_index:   0x%08" PRIx32, set->flags, set->iv_index);
    ESP_LOGI(TAG, "net_idx:   0x%04x, group_addr: 0x%04x", set->net_idx, set->group_addr);
    ESP_LOGI(TAG, "action:    0x%02x", set->action);
    ESP_LOG_BUFFER_HEX("FAST_PROV_OP: match_val", set->match_val, set->match_len);

    msg = bt_mesh_alloc_buf(18 + set->match_len);
    if (!msg) {
        return ESP_FAIL;
    }

    net_buf_simple_add_le16(msg, set->ctx_flags);
    if (set->ctx_flags & BIT(0)) {
        net_buf_simple_add_le16(msg, set->node_addr_cnt);
    }
    if (set->ctx_flags & BIT(1)) {
        net_buf_simple_add_le16(msg, set->unicast_min);
    }
    if (set->ctx_flags & BIT(2)) {
        net_buf_simple_add_le16(msg, set->unicast_max);
    }
    if (set->ctx_flags & BIT(3)) {
        net_buf_simple_add_u8(msg, set->flags);
    }
    if (set->ctx_flags & BIT(4)) {
        net_buf_simple_add_le32(msg, set->iv_index);
    }
    if (set->ctx_flags & BIT(5)) {
        net_buf_simple_add_le16(msg, set->net_idx);
    }
    if (set->ctx_flags & BIT(6)) {
        net_buf_simple_add_le16(msg, set->group_addr);
    }
    if (set->ctx_flags & BIT(7)) {
        net_buf_simple_add_le16(msg, set->prov_addr);
    }
    if (set->ctx_flags & BIT(8)) {
        net_buf_simple_add_mem(msg, set->match_val, set->match_len);
    }
    if (set->ctx_flags & BIT(9)) {
        net_buf_simple_add_u8(msg, set->action);
    }

    esp_ble_mesh_msg_ctx_t ctx = {
        .net_idx  = info->net_idx,
        .app_idx  = info->app_idx,
        .addr     = info->dst,
        .send_ttl = 0,
    };
    err = esp_ble_mesh_client_model_send_msg(model, &ctx,
            ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_SET,
            msg->len, msg->data, info->timeout, true, info->role);

    bt_mesh_free_buf(msg);
    return err;
}

esp_err_t example_send_fast_prov_net_key_add(esp_ble_mesh_model_t *model,
        example_msg_common_info_t *info,
        uint8_t net_key[16])
{
    if (!model || !info || !net_key) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_ble_mesh_msg_ctx_t ctx = {
        .net_idx  = info->net_idx,
        .app_idx  = info->app_idx,
        .addr     = info->dst,
        .send_ttl = 0,
    };

    return esp_ble_mesh_client_model_send_msg(model, &ctx,
            ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_ADD,
            16, net_key, info->timeout, true, info->role);
}

esp_err_t example_send_fast_prov_self_prov_node_addr(esp_ble_mesh_model_t *model,
        example_msg_common_info_t *info,
        struct net_buf_simple *node_addr)
{
    if (!model || !info || !node_addr || (node_addr->len % 2)) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOG_BUFFER_HEX("Send node address", node_addr->data, node_addr->len);

    esp_ble_mesh_msg_ctx_t ctx = {
        .net_idx  = info->net_idx,
        .app_idx  = info->app_idx,
        .addr     = info->dst,
        .send_ttl = 0,
    };

    return esp_ble_mesh_client_model_send_msg(model, &ctx,
            ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR,
            node_addr->len, node_addr->data, info->timeout, true, info->role);
}

esp_err_t example_send_fast_prov_all_node_addr_get(esp_ble_mesh_model_t *model,
        example_msg_common_info_t *info)
{
    if (!model || !info) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_ble_mesh_msg_ctx_t ctx = {
        .net_idx  = info->net_idx,
        .app_idx  = info->app_idx,
        .addr     = info->dst,
        .send_ttl = 0,
    };

    return esp_ble_mesh_client_model_send_msg(model, &ctx,
            ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_GET,
            0, NULL, info->timeout, true, info->role);
}

esp_err_t example_send_fast_prov_status_msg(esp_ble_mesh_model_t *model,
        esp_ble_mesh_msg_ctx_t *ctx,
        uint32_t opcode, struct net_buf_simple *msg)
{
    if (!model || !ctx) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (opcode) {
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS:
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_STATUS:
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_ACK:
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_STATUS:
        ctx->send_ttl = 0;
        break;
    default:
        ESP_LOGW(TAG, "%s: Invalid fast prov status opcode 0x%04" PRIx32, __func__, opcode);
        return ESP_FAIL;
    }

    return esp_ble_mesh_server_model_send_msg(model, ctx, opcode, msg ? msg->len : 0, msg ? msg->data : NULL);
}
