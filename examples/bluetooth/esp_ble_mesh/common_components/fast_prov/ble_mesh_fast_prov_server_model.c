/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_local_data_operation_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_proxy_api.h"
#include "esp_ble_mesh_config_model_api.h"

#include "ble_mesh_fast_prov_operation.h"
#include "ble_mesh_fast_prov_server_model.h"

#define TAG "FAST_PROV_SERVER"

/* Array used to store all node addresses */
static uint16_t all_node_addr[120];
static uint16_t all_node_addr_cnt;

esp_err_t example_store_remote_node_address(uint16_t node_addr)
{
    if (!ESP_BLE_MESH_ADDR_IS_UNICAST(node_addr)) {
        ESP_LOGE(TAG, "%s: Not a unicast address", __func__);
        return ESP_ERR_INVALID_ARG;
    }

    for (int i = 0; i < ARRAY_SIZE(all_node_addr); i++) {
        if (all_node_addr[i] == node_addr) {
            ESP_LOGW(TAG, "%s: Node address 0x%04x already exists", __func__, node_addr);
            return ESP_OK;
        }
    }

    for (int i = 0; i < ARRAY_SIZE(all_node_addr); i++) {
        if (all_node_addr[i] == ESP_BLE_MESH_ADDR_UNASSIGNED) {
            all_node_addr[i] = node_addr;
            all_node_addr_cnt++;
            return ESP_OK;
        }
    }

    ESP_LOGE(TAG, "%s: remote node address queue is full", __func__);
    return ESP_FAIL;
}

esp_ble_mesh_cfg_srv_t *get_cfg_srv_user_data(void)
{
    esp_ble_mesh_model_t *model = NULL;

    model = example_find_model(esp_ble_mesh_get_primary_element_address(),
                               ESP_BLE_MESH_MODEL_ID_CONFIG_SRV, ESP_BLE_MESH_CID_NVAL);
    if (!model) {
        ESP_LOGE(TAG, "%s: Failed to get config server model", __func__);
        return NULL;
    }

    return (esp_ble_mesh_cfg_srv_t *)(model->user_data);
}

/* Timeout handler for disable_fast_prov_timer */
static void disable_fast_prov_cb(struct k_work *work)
{
    example_fast_prov_server_t *srv = NULL;

    srv = CONTAINER_OF(work, example_fast_prov_server_t, disable_fast_prov_timer.work);
    if (!srv) {
        ESP_LOGE(TAG, "%s: Failed to get fast prov server model user_data", __func__);
        return;
    }

    if (esp_ble_mesh_set_fast_prov_action(FAST_PROV_ACT_SUSPEND)) {
        ESP_LOGE(TAG, "%s: Failed to disable fast provisioning", __func__);
        return;
    }
}

/* Timeout handler for gatt_proxy_enable_timer */
static void enable_gatt_proxy_cb(struct k_work *work)
{
    example_fast_prov_server_t *srv = NULL;

    srv = CONTAINER_OF(work, example_fast_prov_server_t, gatt_proxy_enable_timer.work);
    if (!srv) {
        ESP_LOGE(TAG, "%s: Failed to get fast prov server model user_data", __func__);
        return;
    }

    if (bt_mesh_atomic_test_and_clear_bit(srv->srv_flags, RELAY_PROXY_DISABLED)) {
        ESP_LOGI(TAG, "%s: Enable BLE Mesh Relay & GATT Proxy", __func__);
        /* For Primary Provisioner, Relay will not be enabled */
        esp_ble_mesh_proxy_gatt_enable();
        esp_ble_mesh_proxy_identity_enable();
    }

    return;
}

static void example_free_set_info(example_fast_prov_server_t *srv)
{
    if (srv && srv->set_info) {
        bt_mesh_free(srv->set_info);
        srv->set_info = NULL;
    }
}

esp_err_t example_fast_prov_server_recv_msg(esp_ble_mesh_model_t *model,
        esp_ble_mesh_msg_ctx_t *ctx, struct net_buf_simple *buf)
{
    example_fast_prov_server_t *srv = NULL;
    struct net_buf_simple *msg = NULL;
    uint32_t opcode = 0;
    esp_err_t err;

    if (!model || !model->user_data || !ctx || !buf) {
        return ESP_ERR_INVALID_ARG;
    }

    srv = (example_fast_prov_server_t *)model->user_data;

    ESP_LOG_BUFFER_HEX("fast prov server recv", buf->data, buf->len);

    switch (ctx->recv_op) {
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_SET: {
        /* fast prov info status (maximum 9 octets):
         * status_bit_mask (2) + status_ctx_flag (1) + status_unicast (1) + status_net_idx (1) +
         * status_group (1) + status_pri_prov (1) + status_match (1) + status_action (1).
         */
        uint8_t match_len = 0, match_val[16] = {0};
        uint8_t status_unicast = 0;
        uint8_t flags = 0;

        msg = bt_mesh_alloc_buf(9);
        opcode = ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS;

        if (srv->set_info) {
            ESP_LOGW(TAG, "%s: Set fast prov info is already in progress", __func__);
            net_buf_simple_add_le16(msg, 0xFFFF);
            break;
        }

        /* If fast prov server state is pending, can not set fast prov info,
         * and send response message with all status set to 0x01 (i.e. fail).
         */
        if (srv->state == STATE_PEND) {
            uint8_t val[7] = { 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 };
            net_buf_simple_add_le16(msg, 0x7f);
            net_buf_simple_add_mem(msg, val, sizeof(val));
            break;
        }

        uint16_t ctx_flags = net_buf_simple_pull_le16(buf);
        if (ctx_flags == 0) {
            net_buf_simple_add_le16(msg, BIT(0));
            net_buf_simple_add_u8(msg, 0x01);   /* invalid ctx_flags */
            break;
        }

        uint16_t node_addr_cnt = (ctx_flags & BIT(0)) ? net_buf_simple_pull_le16(buf) : 0x0;
        uint16_t unicast_min   = (ctx_flags & BIT(1)) ? net_buf_simple_pull_le16(buf) : (esp_ble_mesh_get_primary_element_address() + esp_ble_mesh_get_element_count());
        uint16_t unicast_max   = (ctx_flags & BIT(2)) ? net_buf_simple_pull_le16(buf) : 0x7FFF;
        if (ctx_flags & BIT(3)) {
            flags = net_buf_simple_pull_u8(buf);
        } else {
            flags = (uint8_t)bt_mesh.sub[0].kr_flag;
            if (bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS)) {
                flags |= BLE_MESH_NET_FLAG_IVU;
            }
        }
        uint32_t iv_index      = (ctx_flags & BIT(4)) ? net_buf_simple_pull_le32(buf) : bt_mesh.iv_index;
        uint16_t net_idx       = (ctx_flags & BIT(5)) ? net_buf_simple_pull_le16(buf) : srv->net_idx;
        uint16_t group_addr    = (ctx_flags & BIT(6)) ? net_buf_simple_pull_le16(buf) : ESP_BLE_MESH_ADDR_UNASSIGNED;
        uint16_t pri_prov_addr = (ctx_flags & BIT(7)) ? net_buf_simple_pull_le16(buf) : ESP_BLE_MESH_ADDR_UNASSIGNED;
        if (ctx_flags & BIT(8)) {
            match_len = buf->len - ((ctx_flags & BIT(9)) ? 1 : 0);
            if (match_len > ESP_BLE_MESH_OCTET16_LEN) {
                net_buf_simple_add_le16(msg, BIT(5));
                net_buf_simple_add_u8(msg, 0x01);   /* too large match value length */
                break;
            }
            memcpy(match_val, buf->data, match_len);
            net_buf_simple_pull(buf, match_len);
        }
        uint8_t action = (ctx_flags & BIT(9)) ? net_buf_simple_pull_u8(buf) : FAST_PROV_ACT_NONE;

        /* If fast prov server state is active, the device can only suspend or exit fast provisioning */
        if (srv->state == STATE_ACTIVE) {
            net_buf_simple_add_le16(msg, BIT(6));
            switch (action & BIT_MASK(2)) {
            case FAST_PROV_ACT_SUSPEND:
            case FAST_PROV_ACT_EXIT:
                srv->pend_act = action & BIT_MASK(2);
                if (!bt_mesh_atomic_test_and_set_bit(srv->srv_flags, DISABLE_FAST_PROV_START)) {
                    k_delayed_work_submit(&srv->disable_fast_prov_timer, DISABLE_FAST_PROV_TIMEOUT);
                }
                net_buf_simple_add_u8(msg, 0x00);   /* action succeed */
                break;
            default:
                net_buf_simple_add_u8(msg, 0x04);   /* action already in progress */
                break;
            }
            break;
        }

        if ((ctx_flags & BIT(1)) || (ctx_flags & BIT(2))) {
            if (!ESP_BLE_MESH_ADDR_IS_UNICAST(unicast_min) || !ESP_BLE_MESH_ADDR_IS_UNICAST(unicast_max)) {
                status_unicast = 0x01;  /* not a unicast address */
            } else if (unicast_min > unicast_max) {
                status_unicast = 0x02;  /* min bigger than max */
            } else if (unicast_min < (esp_ble_mesh_get_primary_element_address() + esp_ble_mesh_get_element_count())) {
                status_unicast = 0x04;  /* overlap with own element address */
            }
            if (status_unicast) {
                net_buf_simple_add_le16(msg, BIT(1));
                net_buf_simple_add_u8(msg, status_unicast);
                break;
            }
        }

        if (ctx_flags & BIT(6)) {
            if (!ESP_BLE_MESH_ADDR_IS_GROUP(group_addr)) {
                net_buf_simple_add_le16(msg, BIT(3));
                net_buf_simple_add_u8(msg, 0x01);   /* not a group address */
                break;
            }
            err = example_add_fast_prov_group_address(ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV, group_addr);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%s: Failed to add group address 0x%04x", __func__, group_addr);
                net_buf_simple_add_le16(msg, BIT(3));
                net_buf_simple_add_u8(msg, 0x02);   /* add group address failed */
                break;
            }
            srv->group_addr = group_addr;
        }

        if (ctx_flags & BIT(7)) {
            if (!ESP_BLE_MESH_ADDR_IS_UNICAST(pri_prov_addr)) {
                net_buf_simple_add_le16(msg, BIT(4));
                net_buf_simple_add_u8(msg, 0x01);   /* not a unicast address */
                break;
            }
        }

        if (ctx_flags & BIT(9)) {
            if ((action & BIT_MASK(2)) != FAST_PROV_ACT_ENTER) {
                net_buf_simple_add_le16(msg, BIT(6));
                net_buf_simple_add_u8(msg, 0x01);   /* action failed */
                break;
            }
        } else {
            net_buf_simple_add_le16(msg, BIT(6));
            net_buf_simple_add_u8(msg, 0x03);   /* none action */
            break;
        }

        memcpy(&srv->ctx, ctx, sizeof(esp_ble_mesh_msg_ctx_t));
        srv->set_info = bt_mesh_calloc(sizeof(struct fast_prov_info_set));
        if (!srv->set_info) {
            ESP_LOGE(TAG, "%s: Failed to allocate memory", __func__);
            bt_mesh_free_buf(msg);
            return ESP_FAIL;
        }

        if (unicast_max < unicast_min + srv->max_node_num - 1) {
            srv->max_node_num = unicast_max - unicast_min + 1;
        }

        srv->set_info->set_succeed = false;
        srv->set_info->node_addr_cnt = node_addr_cnt;
        srv->set_info->unicast_min = unicast_min;
        srv->set_info->unicast_max = unicast_max;
        srv->set_info->flags = flags;
        srv->set_info->iv_index = iv_index;
        srv->set_info->net_idx = net_idx;
        srv->set_info->pri_prov_addr = pri_prov_addr;
        srv->set_info->match_len = match_len;
        memcpy(srv->set_info->match_val, match_val, match_len);
        srv->set_info->action = action;

        esp_ble_mesh_fast_prov_info_t info_set = {
            .unicast_min = unicast_min,
            .unicast_max = unicast_min + srv->max_node_num - 1,
            .flags = flags,
            .iv_index = iv_index,
            .net_idx = net_idx,
            .offset = 0x00,
            .match_len = match_len,
        };
        memcpy(info_set.match_val, match_val, match_len);
        err = esp_ble_mesh_set_fast_prov_info(&info_set);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to set fast prov info", __func__);
            net_buf_simple_add_le16(msg, BIT(5) | BIT(2) | BIT(1));
            net_buf_simple_add_u8(msg, 0x01);   /* set unicast failed */
            net_buf_simple_add_u8(msg, 0x01);   /* set net_idx failed */
            net_buf_simple_add_u8(msg, 0x01);   /* set UUID match failed */
            break;
        }
        /* If setting fast prov info successfully, wait for the event callback */
        bt_mesh_free_buf(msg);
        return ESP_OK;
    }
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_ADD: {
        uint8_t status_net_key, status_action;

        msg = bt_mesh_alloc_buf(2);

        if (srv->state == STATE_PEND) {
            /* Add fast prov net_key, wait for event callback and call esp_ble_mesh_set_fast_prov_act() to set action */
        } else {
            /* If state is not pending, can not add net_key */
            status_net_key = 0x01;  /* status_net_key: fail */
            status_action  = 0x01;  /* status_action: fail */
        }

        opcode = ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_STATUS;
        net_buf_simple_add_u8(msg, status_net_key);
        net_buf_simple_add_u8(msg, status_action);
        break;
    }
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR: {
        if (buf->len % 2) {
            ESP_LOGE(TAG, "%s: Invalid Fast Prov Node Addr message length", __func__);
            return ESP_FAIL;
        }

        if (bt_mesh_atomic_test_and_clear_bit(srv->srv_flags, GATT_PROXY_ENABLE_START)) {
            k_delayed_work_cancel(&srv->gatt_proxy_enable_timer);
        }

        for (; buf->len; ) {
            uint16_t node_addr = net_buf_simple_pull_le16(buf);
            ESP_LOGI(TAG, "Node address: 0x%04x", node_addr);
            err = example_store_remote_node_address(node_addr);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%s: Failed to store node address 0x%04x", __func__, node_addr);
            }
        }

        opcode = ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_ACK;
        break;
    }
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_GET: {
        /* Top device (e.g. phone) tries to get all node addresses */
        msg = bt_mesh_alloc_buf(all_node_addr_cnt * 2);
        if (!msg) {
            ESP_LOGE(TAG, "%s: Failed to allocate memory", __func__);
            return ESP_FAIL;
        }

        for (int i = 0; i < all_node_addr_cnt; i++) {
            net_buf_simple_add_le16(msg, all_node_addr[i]);
        }
        ESP_LOG_BUFFER_HEX("All node address", msg->data, msg->len);

        opcode = ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_STATUS;
        break;
    }
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_GROUP_ADD: {
        uint16_t own_addr = esp_ble_mesh_get_primary_element_address();
        uint16_t group_addr = net_buf_simple_pull_le16(buf);
        ESP_LOGI(TAG, "%s, group address 0x%04x", __func__, group_addr);
        if (!ESP_BLE_MESH_ADDR_IS_GROUP(group_addr)) {
            return ESP_FAIL;
        }
        for (; buf->len; ) {
            uint16_t dst = net_buf_simple_pull_le16(buf);
            ESP_LOGI(TAG, "%s, dst 0x%04x, own address 0x%04x", __func__, dst, own_addr);
            if (dst == own_addr) {
                err = example_add_fast_prov_group_address(ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV, group_addr);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "%s: Failed to add group address 0x%04x", __func__, group_addr);
                }
                return err;
            }
        }
        return ESP_OK;
    }
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_GROUP_DELETE: {
        uint16_t own_addr = esp_ble_mesh_get_primary_element_address();
        uint16_t group_addr = net_buf_simple_pull_le16(buf);
        ESP_LOGI(TAG, "%s, group address 0x%04x", __func__, group_addr);
        if (!ESP_BLE_MESH_ADDR_IS_GROUP(group_addr)) {
            return ESP_FAIL;
        }
        for (; buf->len; ) {
            uint16_t dst = net_buf_simple_pull_le16(buf);
            ESP_LOGI(TAG, "%s, dst 0x%04x, own address 0x%04x", __func__, dst, own_addr);
            if (dst == own_addr) {
                err = example_delete_fast_prov_group_address(ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV, group_addr);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "%s: Failed to delete group address 0x%04x", __func__, group_addr);
                }
                return err;
            }
        }
        return ESP_OK;
    }
    default:
        ESP_LOGW(TAG, "%s: Not a Fast Prov Client message opcode", __func__);
        return ESP_FAIL;
    }

    err = example_send_fast_prov_status_msg(model, ctx, opcode, msg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to send Fast Prov Status message", __func__);
        example_free_set_info(srv);
    }

    bt_mesh_free_buf(msg);
    return err;
}

esp_err_t example_handle_fast_prov_info_set_comp_evt(esp_ble_mesh_model_t *model, uint8_t status_unicast,
        uint8_t status_net_idx, uint8_t status_match)
{
    example_fast_prov_server_t *srv = NULL;
    struct net_buf_simple *msg = NULL;
    esp_err_t err;

    if (!model || !model->user_data) {
        return ESP_ERR_INVALID_ARG;
    }

    srv = (example_fast_prov_server_t *)model->user_data;
    if (!srv->set_info) {
        return ESP_FAIL;
    }

    msg = bt_mesh_alloc_buf(9);

    if (status_unicast || status_match) {
        net_buf_simple_add_le16(msg, BIT(5) | BIT(1));
        net_buf_simple_add_u8(msg, status_unicast);
        net_buf_simple_add_u8(msg, status_match);
        goto send;
    }

    /* Update Fast Prov Server Model user_data */
    srv->unicast_min = srv->set_info->unicast_min + srv->max_node_num;
    srv->unicast_max = srv->set_info->unicast_max;
    srv->unicast_cur = srv->set_info->unicast_min + srv->max_node_num;
    if (srv->unicast_max <= srv->unicast_min) {
        srv->unicast_step = 0;
    } else {
        srv->unicast_step = (srv->unicast_max - srv->unicast_min) / srv->max_node_num;
    }
    srv->flags = srv->set_info->flags;
    srv->iv_index = srv->set_info->iv_index;
    srv->net_idx = srv->set_info->net_idx;
    if (srv->set_info->action & BIT(7)) {
        srv->primary_role = true;
        srv->node_addr_cnt = srv->set_info->node_addr_cnt;
        srv->prim_prov_addr = esp_ble_mesh_get_primary_element_address();
        srv->top_address = srv->ctx.addr;
    } else {
        srv->primary_role = false;
        srv->prim_prov_addr = srv->set_info->pri_prov_addr;
    }
    srv->match_len = srv->set_info->match_len;
    memcpy(srv->match_val, srv->set_info->match_val, srv->set_info->match_len);

    if (status_net_idx) {
        ESP_LOGW(TAG, "%s: Wait for fast prov netkey to be added", __func__);
        srv->pend_act = FAST_PROV_ACT_ENTER;
        srv->state    = STATE_PEND;
        net_buf_simple_add_le16(msg, BIT(6) | BIT(2));
        net_buf_simple_add_u8(msg, status_net_idx); /* wait for net_key */
        net_buf_simple_add_u8(msg, 0x02);           /* pending action */
        goto send;
    }

    /* Sets fast prov action */
    err = esp_ble_mesh_set_fast_prov_action(FAST_PROV_ACT_ENTER);
    if (err == ESP_OK) {
        bt_mesh_free_buf(msg);
        return ESP_OK;
    }

    ESP_LOGE(TAG, "%s: Failed to set fast prov action", __func__);
    net_buf_simple_add_le16(msg, BIT(6));
    net_buf_simple_add_u8(msg, 0x01);   /* action failed */

send:
    err = example_send_fast_prov_status_msg(model, &srv->ctx, ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS, msg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to send Fast Prov Status message", __func__);
        example_free_set_info(srv);
    }

    bt_mesh_free_buf(msg);
    return err;
}

esp_err_t example_handle_fast_prov_action_set_comp_evt(esp_ble_mesh_model_t *model, uint8_t status_action)
{
    example_fast_prov_server_t *srv = NULL;
    struct net_buf_simple *msg = NULL;
    uint32_t opcode;
    esp_err_t err;

    if (!model || !model->user_data) {
        return ESP_ERR_INVALID_ARG;
    }

    srv = (example_fast_prov_server_t *)model->user_data;

    msg = bt_mesh_alloc_buf(9);

    switch (srv->state) {
    case STATE_IDLE: { /* fast prov info set (enter) */
        const uint8_t zero[6] = {0};
        net_buf_simple_add_le16(msg, 0x7f);
        net_buf_simple_add_mem(msg, zero, 6);
        net_buf_simple_add_u8(msg, status_action);
        opcode = ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS;
        /**
         * Disable relay should not have a impact on Mesh Proxy PDU, and
         * we can also move "disabling relay" in the event of "disabling
         * gatt proxy" here.
         */
        if (srv->node_addr_cnt == FAST_PROV_NODE_COUNT_MIN) {
            if (bt_mesh_atomic_test_and_clear_bit(srv->srv_flags, GATT_PROXY_ENABLE_START)) {
                k_delayed_work_cancel(&srv->gatt_proxy_enable_timer);
            }
            if (!bt_mesh_atomic_test_and_set_bit(srv->srv_flags, GATT_PROXY_ENABLE_START)) {
                k_delayed_work_submit(&srv->gatt_proxy_enable_timer, K_SECONDS(3));
            }
        }
        break;
    }
    case STATE_ACTIVE: /* fast prov info set (suspend/exit) */
    /* Currently we only support suspend/exit fast prov after Generic
     * OnOff Set/Set Unack is received. So no fast prov status message
     * will be sent.
     */
    case STATE_PEND: /* fast prov net_key add */
    /* In this case, we should send fast prov net_key status */
    default:
        bt_mesh_free_buf(msg);
        return ESP_OK;
    }

    err = example_send_fast_prov_status_msg(model, &srv->ctx, opcode, msg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to send Fast Prov Status message", __func__);
        example_free_set_info(srv);
        bt_mesh_free_buf(msg);
        return ESP_FAIL;
    }

    if (status_action == 0x00) {
        if (srv->state == STATE_IDLE || srv->state == STATE_PEND) {
            srv->state = STATE_ACTIVE;
        } else if (srv->state == STATE_ACTIVE) {
            srv->state = STATE_IDLE;
        }
        if (srv->set_info) {
            srv->set_info->set_succeed = true;
        }
    }

    bt_mesh_free_buf(msg);
    return ESP_OK;
}

esp_err_t example_handle_fast_prov_status_send_comp_evt(int err_code, uint32_t opcode,
        esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx)
{
    example_fast_prov_server_t *srv = NULL;

    if (!model || !model->user_data) {
        return ESP_ERR_INVALID_ARG;
    }

    srv = (example_fast_prov_server_t *)model->user_data;

    ESP_LOGI(TAG, "%s: opcode 0x%06" PRIx32, __func__, opcode);

    switch (opcode) {
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS:
        if (err_code == 0 && srv->set_info && srv->set_info->set_succeed == true) {
            if (!bt_mesh_atomic_test_and_set_bit(srv->srv_flags, RELAY_PROXY_DISABLED)) {
                /* For Primary Provisioner: disable Relay and GATT Proxy;
                 * For other Provisioners: only disable GATT Proxy
                 */
                ESP_LOGW(TAG, "%s: Disable BLE Mesh Relay & GATT Proxy", __func__);
                esp_ble_mesh_proxy_gatt_disable();
            }
        }
        example_free_set_info(srv);
        break;
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_STATUS:
        break;
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_ACK:
        if (!bt_mesh_atomic_test_and_set_bit(srv->srv_flags, GATT_PROXY_ENABLE_START)) {
            k_delayed_work_submit(&srv->gatt_proxy_enable_timer, GATT_PROXY_ENABLE_TIMEOUT);
        }
        break;
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_STATUS:
        break;
    default:
        break;
    }

    return ESP_OK;
}

esp_err_t example_fast_prov_server_init(esp_ble_mesh_model_t *model)
{
    example_fast_prov_server_t *srv = NULL;

    if (!model || !model->user_data) {
        return ESP_ERR_INVALID_ARG;
    }

    srv = (example_fast_prov_server_t *)model->user_data;
    srv->model = model;

    k_delayed_work_init(&srv->disable_fast_prov_timer, disable_fast_prov_cb);
    k_delayed_work_init(&srv->gatt_proxy_enable_timer, enable_gatt_proxy_cb);

    return ESP_OK;
}
