/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>

#include "esp_err.h"

#include "btc_ble_mesh_prov.h"
#include "esp_ble_mesh_networking_api.h"

#define ESP_BLE_MESH_TX_SDU_MAX ((CONFIG_BLE_MESH_ADV_BUF_COUNT - 3) * 12)

static esp_err_t ble_mesh_model_send_msg(esp_ble_mesh_model_t *model,
                                         esp_ble_mesh_msg_ctx_t *ctx,
                                         uint32_t opcode,
                                         btc_ble_mesh_model_act_t act,
                                         uint16_t length, uint8_t *data,
                                         int32_t msg_timeout, bool need_rsp,
                                         esp_ble_mesh_dev_role_t device_role)
{
    btc_ble_mesh_model_args_t arg = {0};
    uint8_t op_len = 0, mic_len = 0;
    uint8_t *msg_data = NULL;
    btc_msg_t msg = {0};
    esp_err_t status = ESP_OK;

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    if (ctx && ctx->addr == ESP_BLE_MESH_ADDR_UNASSIGNED) {
        BT_ERR("Invalid destination address 0x0000");
        return ESP_ERR_INVALID_ARG;
    }

    /* When data is NULL, it is mandatory to set length to 0 to prevent users from misinterpreting parameters. */
    if (data == NULL) {
        length = 0;
    }

    if (opcode < 0x100) {
        op_len = 1;
    } else if (opcode < 0x10000) {
        op_len = 2;
    } else {
        op_len = 3;
    }

    if (act == BTC_BLE_MESH_ACT_MODEL_PUBLISH) {
        if (op_len + length > model->pub->msg->size) {
            BT_ERR("Too small publication msg size %d", model->pub->msg->size);
            return ESP_ERR_INVALID_ARG;
        }
    }

    if (act == BTC_BLE_MESH_ACT_MODEL_PUBLISH) {
        /* When "send_rel" is true and "send_szmic" is 1, 8-octets TransMIC will
         * be used, otherwise 4-octets TransMIC will be used.
         */
        mic_len = (model->pub->send_rel && model->pub->send_szmic) ?
                   ESP_BLE_MESH_MIC_LONG : ESP_BLE_MESH_MIC_SHORT;
    } else {
        /* When the message is tagged with the send-segmented tag and "send_szmic"
         * is 1, 8-octets TransMIC will be used, otherwise 4-octets TransMIC will
         * be used.
         */
        mic_len = ((ctx->send_tag & ESP_BLE_MESH_TAG_SEND_SEGMENTED) && ctx->send_szmic) ?
                   ESP_BLE_MESH_MIC_LONG : ESP_BLE_MESH_MIC_SHORT;
    }

    if (op_len + length + mic_len > MIN(ESP_BLE_MESH_SDU_MAX_LEN, ESP_BLE_MESH_TX_SDU_MAX)) {
        BT_ERR("Too large data length %d", length);
        return ESP_ERR_INVALID_ARG;
    }

    if (act == BTC_BLE_MESH_ACT_MODEL_PUBLISH) {
        bt_mesh_model_msg_init(model->pub->msg, opcode);
        net_buf_simple_add_mem(model->pub->msg, data, length);
    } else {
        msg_data = (uint8_t *)bt_mesh_malloc(op_len + length);
        if (msg_data == NULL) {
            return ESP_ERR_NO_MEM;
        }
        esp_ble_mesh_model_msg_opcode_init(msg_data, opcode);
        memcpy(msg_data + op_len, data, length);
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MODEL;
    msg.act = act;

    if (act == BTC_BLE_MESH_ACT_MODEL_PUBLISH) {
        arg.model_publish.model = model;
    } else {
        arg.model_send.model = model;
        arg.model_send.ctx = ctx;
        arg.model_send.need_rsp = need_rsp;
        arg.model_send.opcode = opcode;
        arg.model_send.length = op_len + length;
        arg.model_send.data = msg_data;
        arg.model_send.msg_timeout = msg_timeout;
    }

    status = (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_model_args_t), btc_ble_mesh_model_arg_deep_copy,
                btc_ble_mesh_model_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);

    bt_mesh_free(msg_data);

    return status;
}

esp_err_t esp_ble_mesh_register_custom_model_callback(esp_ble_mesh_model_cb_t callback)
{
    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    return (btc_profile_cb_set(BTC_PID_MODEL, callback) == 0 ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_model_msg_opcode_init(uint8_t *data, uint32_t opcode)
{
    uint16_t val = 0;

    if (data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (opcode < 0x100) {
        /* 1-byte OpCode */
        data[0] = opcode & 0xff;
        return ESP_OK;
    }

    if (opcode < 0x10000) {
        /* 2-byte OpCode, big endian */
        val = sys_cpu_to_be16 (opcode);
        memcpy(data, &val, 2);
        return ESP_OK;
    }

    /* 3-byte OpCode, note that little endian for the least 2 bytes(Company ID) of opcode */
    data[0] = (opcode >> 16) & 0xff;
    val = sys_cpu_to_le16(opcode & 0xffff);
    memcpy(&data[1], &val, 2);

    return ESP_OK;
}

esp_err_t esp_ble_mesh_client_model_init(esp_ble_mesh_model_t *model)
{
    if (model == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    return btc_ble_mesh_client_model_init(model);
}

#if CONFIG_BLE_MESH_DEINIT
esp_err_t esp_ble_mesh_client_model_deinit(esp_ble_mesh_model_t *model)
{
    if (model == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    return btc_ble_mesh_client_model_deinit(model);
}
#endif /* CONFIG_BLE_MESH_DEINIT */

esp_err_t esp_ble_mesh_server_model_send_msg(esp_ble_mesh_model_t *model,
                                             esp_ble_mesh_msg_ctx_t *ctx,
                                             uint32_t opcode,
                                             uint16_t length, uint8_t *data)
{
    if (model == NULL || ctx == NULL ||
        ctx->net_idx == ESP_BLE_MESH_KEY_UNUSED ||
        ctx->app_idx == ESP_BLE_MESH_KEY_UNUSED) {
        return ESP_ERR_INVALID_ARG;
    }

    return ble_mesh_model_send_msg(model, ctx, opcode, BTC_BLE_MESH_ACT_SERVER_MODEL_SEND,
                                   length, data, 0, false, ROLE_NODE);
}

esp_err_t esp_ble_mesh_client_model_send_msg(esp_ble_mesh_model_t *model,
                                             esp_ble_mesh_msg_ctx_t *ctx,
                                             uint32_t opcode,
                                             uint16_t length, uint8_t *data,
                                             int32_t msg_timeout, bool need_rsp,
                                             esp_ble_mesh_dev_role_t device_role)
{
    if (model == NULL || ctx == NULL ||
        ctx->net_idx == ESP_BLE_MESH_KEY_UNUSED ||
        ctx->app_idx == ESP_BLE_MESH_KEY_UNUSED) {
        return ESP_ERR_INVALID_ARG;
    }

    return ble_mesh_model_send_msg(model, ctx, opcode, BTC_BLE_MESH_ACT_CLIENT_MODEL_SEND,
                                   length, data, msg_timeout, need_rsp, device_role);
}

esp_err_t esp_ble_mesh_model_publish(esp_ble_mesh_model_t *model, uint32_t opcode,
                                     uint16_t length, uint8_t *data,
                                     esp_ble_mesh_dev_role_t device_role)
{
    if (model == NULL || model->pub == NULL || model->pub->msg == NULL ||
        model->pub->publish_addr == ESP_BLE_MESH_ADDR_UNASSIGNED) {
        return ESP_ERR_INVALID_ARG;
    }

    return ble_mesh_model_send_msg(model, NULL, opcode, BTC_BLE_MESH_ACT_MODEL_PUBLISH,
                                   length, data, 0, false, device_role);
}

#if CONFIG_BLE_MESH_SERVER_MODEL
esp_err_t esp_ble_mesh_server_model_update_state(esp_ble_mesh_model_t *model,
                                                 esp_ble_mesh_server_state_type_t type,
                                                 esp_ble_mesh_server_state_value_t *value)
{
    btc_ble_mesh_model_args_t arg = {0};
    btc_msg_t msg = {0};

    if (!model || !value || type >= ESP_BLE_MESH_SERVER_MODEL_STATE_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    arg.model_update_state.model = model;
    arg.model_update_state.type = type;
    arg.model_update_state.value = value;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_MODEL;
    msg.act = BTC_BLE_MESH_ACT_SERVER_MODEL_UPDATE_STATE;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_model_args_t), btc_ble_mesh_model_arg_deep_copy,
                btc_ble_mesh_model_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif /* CONFIG_BLE_MESH_SERVER_MODEL */

esp_err_t esp_ble_mesh_node_local_reset(void)
{
    btc_msg_t msg = {0};

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_NODE_RESET;

    return (btc_transfer_context(&msg, NULL, 0, NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

#if CONFIG_BLE_MESH_PROVISIONER
esp_err_t esp_ble_mesh_provisioner_set_node_name(uint16_t index, const char *name)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (!name || (strlen(name) > ESP_BLE_MESH_NODE_NAME_MAX_LEN)) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_SET_NODE_NAME;

    arg.set_node_name.index = index;
    memset(arg.set_node_name.name, 0, sizeof(arg.set_node_name.name));
    strncpy(arg.set_node_name.name, name, ESP_BLE_MESH_NODE_NAME_MAX_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

const char *esp_ble_mesh_provisioner_get_node_name(uint16_t index)
{
    return bt_mesh_provisioner_get_node_name(index);
}

uint16_t esp_ble_mesh_provisioner_get_node_index(const char *name)
{
    if (!name || (strlen(name) > ESP_BLE_MESH_NODE_NAME_MAX_LEN)) {
        return ESP_BLE_MESH_INVALID_NODE_INDEX;
    }

    return bt_mesh_provisioner_get_node_index(name);
}

esp_err_t esp_ble_mesh_provisioner_store_node_comp_data(uint16_t unicast_addr,
                                                        uint8_t *data, uint16_t length)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (!ESP_BLE_MESH_ADDR_IS_UNICAST(unicast_addr) || !data || length <= 14) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_STORE_NODE_COMP_DATA;

    arg.store_node_comp_data.unicast_addr = unicast_addr;
    arg.store_node_comp_data.length = length;
    arg.store_node_comp_data.data = data;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), btc_ble_mesh_prov_arg_deep_copy,
                btc_ble_mesh_prov_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_ble_mesh_node_t *esp_ble_mesh_provisioner_get_node_with_uuid(const uint8_t uuid[16])
{
    if (!uuid) {
        return NULL;
    }

    return btc_ble_mesh_provisioner_get_node_with_uuid(uuid);
}

esp_ble_mesh_node_t *esp_ble_mesh_provisioner_get_node_with_addr(uint16_t unicast_addr)
{
    if (!ESP_BLE_MESH_ADDR_IS_UNICAST(unicast_addr)) {
        return NULL;
    }

    return btc_ble_mesh_provisioner_get_node_with_addr(unicast_addr);
}

esp_ble_mesh_node_t *esp_ble_mesh_provisioner_get_node_with_name(const char *name)
{
    if (!name || (strlen(name) > ESP_BLE_MESH_NODE_NAME_MAX_LEN)) {
        return NULL;
    }

    return btc_ble_mesh_provisioner_get_node_with_name(name);
}

uint16_t esp_ble_mesh_provisioner_get_prov_node_count(void)
{
    return btc_ble_mesh_provisioner_get_prov_node_count();
}

const esp_ble_mesh_node_t **esp_ble_mesh_provisioner_get_node_table_entry(void)
{
    return btc_ble_mesh_provisioner_get_node_table_entry();
}

esp_err_t esp_ble_mesh_provisioner_delete_node_with_uuid(const uint8_t uuid[16])
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (!uuid) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_DELETE_NODE_WITH_UUID;

    memcpy(arg.delete_node_with_uuid.uuid, uuid, 16);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_delete_node_with_addr(uint16_t unicast_addr)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (!ESP_BLE_MESH_ADDR_IS_UNICAST(unicast_addr)) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_DELETE_NODE_WITH_ADDR;

    arg.delete_node_with_addr.unicast_addr = unicast_addr;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_add_local_app_key(const uint8_t app_key[16],
                                                     uint16_t net_idx, uint16_t app_idx)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_ADD_LOCAL_APP_KEY;

    arg.add_local_app_key.net_idx = net_idx;
    arg.add_local_app_key.app_idx = app_idx;
    if (app_key) {
        memcpy(arg.add_local_app_key.app_key, app_key, 16);
    } else {
        bzero(arg.add_local_app_key.app_key, 16);
    }

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_update_local_app_key(const uint8_t app_key[16],
                                                        uint16_t net_idx, uint16_t app_idx)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (app_key == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_UPDATE_LOCAL_APP_KEY;

    memcpy(arg.update_local_app_key.app_key, app_key, 16);
    arg.update_local_app_key.net_idx = net_idx;
    arg.update_local_app_key.app_idx = app_idx;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

const uint8_t *esp_ble_mesh_provisioner_get_local_app_key(uint16_t net_idx, uint16_t app_idx)
{
    return bt_mesh_provisioner_local_app_key_get(net_idx, app_idx);
}

esp_err_t esp_ble_mesh_provisioner_bind_app_key_to_local_model(uint16_t element_addr, uint16_t app_idx,
                                                               uint16_t model_id, uint16_t company_id)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (!ESP_BLE_MESH_ADDR_IS_UNICAST(element_addr)) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_BIND_LOCAL_MOD_APP;

    arg.local_mod_app_bind.elem_addr = element_addr;
    arg.local_mod_app_bind.app_idx = app_idx;
    arg.local_mod_app_bind.model_id = model_id;
    arg.local_mod_app_bind.cid = company_id;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_add_local_net_key(const uint8_t net_key[16], uint16_t net_idx)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (net_idx == ESP_BLE_MESH_KEY_PRIMARY) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_ADD_LOCAL_NET_KEY;

    arg.add_local_net_key.net_idx = net_idx;
    if (net_key) {
        memcpy(arg.add_local_net_key.net_key, net_key, 16);
    } else {
        bzero(arg.add_local_net_key.net_key, 16);
    }

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_update_local_net_key(const uint8_t net_key[16], uint16_t net_idx)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (net_key == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_UPDATE_LOCAL_NET_KEY;

    memcpy(arg.update_local_net_key.net_key, net_key, 16);
    arg.update_local_net_key.net_idx = net_idx;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

const uint8_t *esp_ble_mesh_provisioner_get_local_net_key(uint16_t net_idx)
{
    return bt_mesh_provisioner_net_key_get(net_idx);
}

#if CONFIG_BLE_MESH_PROVISIONER_RECV_HB
esp_err_t esp_ble_mesh_provisioner_recv_heartbeat(bool enable)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_ENABLE_HEARTBEAT_RECV;

    arg.enable_heartbeat_recv.enable = enable;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_set_heartbeat_filter_type(uint8_t type)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (type > ESP_BLE_MESH_HEARTBEAT_FILTER_REJECTLIST) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_SET_HEARTBEAT_FILTER_TYPE;

    arg.set_heartbeat_filter_type.type = type;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_set_heartbeat_filter_info(uint8_t op, esp_ble_mesh_heartbeat_filter_info_t *info)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (op > ESP_BLE_MESH_HEARTBEAT_FILTER_REMOVE || info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!(ESP_BLE_MESH_ADDR_IS_UNICAST(info->hb_src) &&
        (ESP_BLE_MESH_ADDR_IS_UNICAST(info->hb_dst) ||
        ESP_BLE_MESH_ADDR_IS_GROUP(info->hb_dst)))) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_SET_HEARTBEAT_FILTER_INFO;

    arg.set_heartbeat_filter_info.op = op;
    arg.set_heartbeat_filter_info.hb_src = info->hb_src;
    arg.set_heartbeat_filter_info.hb_dst = info->hb_dst;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif /* CONFIG_BLE_MESH_PROVISIONER_RECV_HB */

#if CONFIG_BLE_MESH_SETTINGS
esp_err_t esp_ble_mesh_provisioner_direct_erase_settings(void)
{
    btc_msg_t msg = {0};

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;

    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_DIRECT_ERASE_SETTINGS;

    return (btc_transfer_context(&msg, NULL, 0, NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif /* CONFIG_BLE_MESH_SETTINGS */

#if CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE
esp_err_t esp_ble_mesh_provisioner_open_settings_with_index(uint8_t index)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (index >= CONFIG_BLE_MESH_MAX_NVS_NAMESPACE) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_OPEN_SETTINGS_WITH_INDEX;

    arg.open_settings_with_index.index = index;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_open_settings_with_uid(const char *uid)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (!uid || strlen(uid) > ESP_BLE_MESH_SETTINGS_UID_SIZE) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_OPEN_SETTINGS_WITH_UID;

    strncpy(arg.open_settings_with_uid.uid, uid, ESP_BLE_MESH_SETTINGS_UID_SIZE);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_close_settings_with_index(uint8_t index, bool erase)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (index >= CONFIG_BLE_MESH_MAX_NVS_NAMESPACE) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_CLOSE_SETTINGS_WITH_INDEX;

    arg.close_settings_with_index.index = index;
    arg.close_settings_with_index.erase = erase;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_close_settings_with_uid(const char *uid, bool erase)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (!uid || strlen(uid) > ESP_BLE_MESH_SETTINGS_UID_SIZE) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_CLOSE_SETTINGS_WITH_UID;

    strncpy(arg.close_settings_with_uid.uid, uid, ESP_BLE_MESH_SETTINGS_UID_SIZE);
    arg.close_settings_with_uid.erase = erase;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_delete_settings_with_index(uint8_t index)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (index >= CONFIG_BLE_MESH_MAX_NVS_NAMESPACE) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_DELETE_SETTINGS_WITH_INDEX;

    arg.delete_settings_with_index.index = index;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_delete_settings_with_uid(const char *uid)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (!uid || strlen(uid) > ESP_BLE_MESH_SETTINGS_UID_SIZE) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_DELETE_SETTINGS_WITH_UID;

    strncpy(arg.delete_settings_with_uid.uid, uid, ESP_BLE_MESH_SETTINGS_UID_SIZE);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

const char *esp_ble_mesh_provisioner_get_settings_uid(uint8_t index)
{
    if (index >= CONFIG_BLE_MESH_MAX_NVS_NAMESPACE) {
        return NULL;
    }

    return btc_ble_mesh_provisioner_get_settings_uid(index);
}

uint8_t esp_ble_mesh_provisioner_get_settings_index(const char *uid)
{
    if (!uid || strlen(uid) > ESP_BLE_MESH_SETTINGS_UID_SIZE) {
        return ESP_BLE_MESH_INVALID_SETTINGS_IDX;
    }

    return btc_ble_mesh_provisioner_get_settings_index(uid);
}

uint8_t esp_ble_mesh_provisioner_get_free_settings_count(void)
{
    return btc_ble_mesh_provisioner_get_free_settings_count();
}
#endif /* CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE */

#if CONFIG_BLE_MESH_CERT_BASED_PROV
esp_err_t esp_ble_mesh_provisioner_send_prov_records_get(uint16_t link_idx)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_SEND_PROV_RECORDS_GET;

    arg.send_prov_records_get.link_idx = link_idx;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_send_prov_record_req(uint16_t link_idx, uint16_t record_id,
                                                        uint16_t frag_offset, uint16_t max_size)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (record_id >= ESP_BLE_MESH_PROV_RECORD_MAX_ID) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_SEND_PROV_RECORD_REQUEST;

    arg.send_prov_record_req.link_idx = link_idx;
    arg.send_prov_record_req.record_id = record_id;
    arg.send_prov_record_req.frag_offset = frag_offset;
    arg.send_prov_record_req.max_size = max_size;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_send_prov_invite(uint16_t link_idx)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_SEND_PROV_INVITE;

    arg.send_prov_invite.link_idx = link_idx;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_send_link_close(uint16_t link_idx)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_SEND_LINK_CLOSE;

    arg.send_link_close.link_idx = link_idx;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif /* CONFIG_BLE_MESH_CERT_BASED_PROV */
#endif /* CONFIG_BLE_MESH_PROVISIONER */

#if CONFIG_BLE_MESH_FAST_PROV
const uint8_t *esp_ble_mesh_get_fast_prov_app_key(uint16_t net_idx, uint16_t app_idx)
{
    return bt_mesh_get_fast_prov_app_key(net_idx, app_idx);
}
#endif  /* CONFIG_BLE_MESH_FAST_PROV */
