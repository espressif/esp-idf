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

#include <stdint.h>
#include <string.h>

#include "esp_err.h"

#include "btc_ble_mesh_prov.h"
#include "esp_ble_mesh_provisioning_api.h"

#define MAX_PROV_LINK_IDX   (CONFIG_BLE_MESH_PBA_SAME_TIME + CONFIG_BLE_MESH_PBG_SAME_TIME)
#define MAX_OOB_INPUT_NUM   0x5F5E0FF   /* Decimal: 99999999 */

esp_err_t esp_ble_mesh_register_prov_callback(esp_ble_mesh_prov_cb_t callback)
{
    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    return (btc_profile_cb_set(BTC_PID_PROV, callback) == 0 ? ESP_OK : ESP_FAIL);
}

bool esp_ble_mesh_node_is_provisioned(void)
{
    return bt_mesh_is_provisioned();
}

static bool prov_bearers_valid(esp_ble_mesh_prov_bearer_t bearers)
{
    if ((!(bearers & (ESP_BLE_MESH_PROV_ADV | ESP_BLE_MESH_PROV_GATT))) ||
        (IS_ENABLED(CONFIG_BLE_MESH_PB_ADV) &&
            !IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) &&
            !(bearers & ESP_BLE_MESH_PROV_ADV)) ||
        (!IS_ENABLED(CONFIG_BLE_MESH_PB_ADV) &&
            IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) &&
            !(bearers & ESP_BLE_MESH_PROV_GATT))) {
        return false;
    }
    return true;
}

esp_err_t esp_ble_mesh_node_prov_enable(esp_ble_mesh_prov_bearer_t bearers)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (prov_bearers_valid(bearers) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROV_ENABLE;
    arg.node_prov_enable.bearers = bearers;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_node_prov_disable(esp_ble_mesh_prov_bearer_t bearers)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (prov_bearers_valid(bearers) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROV_DISABLE;
    arg.node_prov_disable.bearers = bearers;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_node_set_oob_pub_key(uint8_t pub_key_x[32], uint8_t pub_key_y[32],
                                            uint8_t private_key[32])
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (!pub_key_x || !pub_key_y || !private_key) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_SET_OOB_PUB_KEY;

    memcpy(arg.set_oob_pub_key.pub_key_x, pub_key_x, 32);
    memcpy(arg.set_oob_pub_key.pub_key_y, pub_key_y, 32);
    memcpy(arg.set_oob_pub_key.private_key, private_key, 32);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_node_input_number(uint32_t number)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (number > MAX_OOB_INPUT_NUM) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_INPUT_NUMBER;
    arg.input_number.number = number;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_node_input_string(const char *string)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (!string || strlen(string) > ESP_BLE_MESH_PROV_INPUT_OOB_MAX_LEN) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_INPUT_STRING;
    memset(arg.input_string.string, 0, sizeof(arg.input_string.string));
    strncpy(arg.input_string.string, string,
        MIN(strlen(string), sizeof(arg.input_string.string)));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_set_unprovisioned_device_name(const char *name)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (!name || strlen(name) > ESP_BLE_MESH_DEVICE_NAME_MAX_LEN) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_SET_DEVICE_NAME;

    memset(arg.set_device_name.name, 0, sizeof(arg.set_device_name.name));
    strncpy(arg.set_device_name.name, name, ESP_BLE_MESH_DEVICE_NAME_MAX_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

#if (CONFIG_BLE_MESH_PROVISIONER)
esp_err_t esp_ble_mesh_provisioner_read_oob_pub_key(uint8_t link_idx, uint8_t pub_key_x[32],
                                                    uint8_t pub_key_y[32])
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (!pub_key_x || !pub_key_y || link_idx >= MAX_PROV_LINK_IDX) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_READ_OOB_PUB_KEY;

    arg.provisioner_read_oob_pub_key.link_idx = link_idx;
    memcpy(arg.provisioner_read_oob_pub_key.pub_key_x, pub_key_x, 32);
    memcpy(arg.provisioner_read_oob_pub_key.pub_key_y, pub_key_y, 32);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_input_string(const char *string, uint8_t link_idx)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (!string || strlen(string) > ESP_BLE_MESH_PROV_OUTPUT_OOB_MAX_LEN ||
        link_idx >= MAX_PROV_LINK_IDX) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_INPUT_STR;

    memset(arg.provisioner_input_str.string, 0, sizeof(arg.provisioner_input_str.string));
    strncpy(arg.provisioner_input_str.string, string,
        MIN(strlen(string), sizeof(arg.provisioner_input_str.string)));
    arg.provisioner_input_str.link_idx = link_idx;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_input_number(uint32_t number, uint8_t link_idx)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (number > MAX_OOB_INPUT_NUM || link_idx >= MAX_PROV_LINK_IDX) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_INPUT_NUM;

    arg.provisioner_input_num.number = number;
    arg.provisioner_input_num.link_idx = link_idx;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_prov_enable(esp_ble_mesh_prov_bearer_t bearers)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (prov_bearers_valid(bearers) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_ENABLE;

    arg.provisioner_enable.bearers = bearers;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_prov_disable(esp_ble_mesh_prov_bearer_t bearers)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (prov_bearers_valid(bearers) == false) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_DISABLE;

    arg.provisioner_disable.bearers = bearers;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_add_unprov_dev(esp_ble_mesh_unprov_dev_add_t *add_dev,
                                                  esp_ble_mesh_dev_add_flag_t flags)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (add_dev == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_DEV_ADD;

    arg.provisioner_dev_add.add_dev.addr_type = add_dev->addr_type;
    arg.provisioner_dev_add.add_dev.oob_info = add_dev->oob_info;
    arg.provisioner_dev_add.add_dev.bearer = add_dev->bearer;
    memcpy(arg.provisioner_dev_add.add_dev.addr, add_dev->addr, sizeof(esp_ble_mesh_bd_addr_t));
    memcpy(arg.provisioner_dev_add.add_dev.uuid, add_dev->uuid, 16);
    arg.provisioner_dev_add.flags = flags;
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_prov_device_with_addr(const uint8_t uuid[16],
                                                         esp_ble_mesh_bd_addr_t addr,
                                                         esp_ble_mesh_addr_type_t addr_type,
                                                         esp_ble_mesh_prov_bearer_t bearer,
                                                         uint16_t oob_info, uint16_t unicast_addr)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (uuid == NULL || (bearer == ESP_BLE_MESH_PROV_GATT && (addr == NULL ||
        addr_type > ESP_BLE_MESH_ADDR_TYPE_RANDOM)) ||
        (bearer != ESP_BLE_MESH_PROV_ADV && bearer != ESP_BLE_MESH_PROV_GATT) ||
        !ESP_BLE_MESH_ADDR_IS_UNICAST(unicast_addr)) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_PROV_DEV_WITH_ADDR;

    memcpy(arg.provisioner_prov_dev_with_addr.uuid, uuid, 16);
    if (addr) {
        memcpy(arg.provisioner_prov_dev_with_addr.addr, addr, BD_ADDR_LEN);
        arg.provisioner_prov_dev_with_addr.addr_type = addr_type;
    }
    arg.provisioner_prov_dev_with_addr.bearer = bearer;
    arg.provisioner_prov_dev_with_addr.oob_info = oob_info;
    arg.provisioner_prov_dev_with_addr.unicast_addr = unicast_addr;
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_delete_dev(esp_ble_mesh_device_delete_t *del_dev)
{
    uint8_t val = DEL_DEV_ADDR_FLAG | DEL_DEV_UUID_FLAG;
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (del_dev == NULL || (__builtin_popcount(del_dev->flag & val) != 1)) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_DEV_DEL;

    arg.provisioner_dev_del.del_dev.flag = del_dev->flag;
    if (del_dev->flag & DEL_DEV_ADDR_FLAG) {
        arg.provisioner_dev_del.del_dev.addr_type = del_dev->addr_type;
        memcpy(arg.provisioner_dev_del.del_dev.addr, del_dev->addr, sizeof(esp_ble_mesh_bd_addr_t));
    } else if (del_dev->flag & DEL_DEV_UUID_FLAG) {
        memcpy(arg.provisioner_dev_del.del_dev.uuid, del_dev->uuid, 16);
    }
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_set_dev_uuid_match(const uint8_t *match_val, uint8_t match_len,
                                                      uint8_t offset, bool prov_after_match)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (match_len + offset > ESP_BLE_MESH_OCTET16_LEN) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_SET_DEV_UUID_MATCH;

    if (match_len && match_val) {
        memcpy(arg.set_dev_uuid_match.match_val, match_val, match_len);
    }
    arg.set_dev_uuid_match.match_len = match_len;
    arg.set_dev_uuid_match.offset = offset;
    arg.set_dev_uuid_match.prov_after_match = prov_after_match;
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_set_prov_data_info(esp_ble_mesh_prov_data_info_t *prov_data_info)
{
    uint8_t val = PROV_DATA_NET_IDX_FLAG | PROV_DATA_FLAGS_FLAG | PROV_DATA_IV_INDEX_FLAG;
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (prov_data_info == NULL || (__builtin_popcount(prov_data_info->flag & val) != 1)) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_SET_PROV_DATA_INFO;

    arg.set_prov_data_info.prov_data.flag = prov_data_info->flag;
    if (prov_data_info->flag & PROV_DATA_NET_IDX_FLAG) {
        arg.set_prov_data_info.prov_data.net_idx = prov_data_info->net_idx;
    } else if (prov_data_info->flag & PROV_DATA_FLAGS_FLAG) {
        arg.set_prov_data_info.prov_data.flags = prov_data_info->flags;
    } else if (prov_data_info->flag & PROV_DATA_IV_INDEX_FLAG) {
        arg.set_prov_data_info.prov_data.iv_index = prov_data_info->iv_index;
    }
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_set_static_oob_value(const uint8_t *value, uint8_t length)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (value == NULL || length == 0 || length > 16) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_SET_STATIC_OOB_VAL;

    arg.set_static_oob_val.length = length;
    memcpy(arg.set_static_oob_val.value, value, length);
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_provisioner_set_primary_elem_addr(uint16_t addr)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (!ESP_BLE_MESH_ADDR_IS_UNICAST(addr)) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_PROVISIONER_SET_PRIMARY_ELEM_ADDR;

    arg.set_primary_elem_addr.addr = addr;
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

#endif /* CONFIG_BLE_MESH_PROVISIONER */

/* The following APIs are for fast provisioning */

#if (CONFIG_BLE_MESH_FAST_PROV)

esp_err_t esp_ble_mesh_set_fast_prov_info(esp_ble_mesh_fast_prov_info_t *fast_prov_info)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (fast_prov_info == NULL || (fast_prov_info->offset +
            fast_prov_info->match_len > ESP_BLE_MESH_OCTET16_LEN)) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_SET_FAST_PROV_INFO;

    arg.set_fast_prov_info.unicast_min = fast_prov_info->unicast_min;
    arg.set_fast_prov_info.unicast_max = fast_prov_info->unicast_max;
    arg.set_fast_prov_info.net_idx = fast_prov_info->net_idx;
    arg.set_fast_prov_info.flags = fast_prov_info->flags;
    arg.set_fast_prov_info.iv_index = fast_prov_info->iv_index;
    arg.set_fast_prov_info.offset = fast_prov_info->offset;
    arg.set_fast_prov_info.match_len = fast_prov_info->match_len;
    if (fast_prov_info->match_len && fast_prov_info->match_val) {
        memcpy(arg.set_fast_prov_info.match_val, fast_prov_info->match_val, fast_prov_info->match_len);
    }
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_mesh_set_fast_prov_action(esp_ble_mesh_fast_prov_action_t action)
{
    btc_ble_mesh_prov_args_t arg = {0};
    btc_msg_t msg = {0};

    if (action >= FAST_PROV_ACT_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_SET_FAST_PROV_ACTION;

    arg.set_fast_prov_action.action = action;
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

#endif /* CONFIG_BLE_MESH_FAST_PROV */

