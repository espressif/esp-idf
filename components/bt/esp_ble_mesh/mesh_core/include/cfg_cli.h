/** @file
 *  @brief Bluetooth Mesh Configuration Client Model APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _BLE_MESH_CFG_CLI_H_
#define _BLE_MESH_CFG_CLI_H_

#include "client_common.h"

/**
 * @brief Bluetooth Mesh
 * @defgroup bt_mesh_cfg_cli Bluetooth Mesh Configuration Client Model
 * @ingroup bt_mesh
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Config client model common structure */
typedef bt_mesh_client_user_data_t      bt_mesh_config_client_t;
typedef bt_mesh_client_internal_data_t  config_internal_data_t;

extern const struct bt_mesh_model_op bt_mesh_cfg_cli_op[];
extern const struct bt_mesh_model_cb bt_mesh_cfg_cli_cb;

#define BLE_MESH_MODEL_CFG_CLI(cli_data)            \
        BLE_MESH_MODEL_CB(BLE_MESH_MODEL_ID_CFG_CLI,   \
            bt_mesh_cfg_cli_op, NULL, cli_data, &bt_mesh_cfg_cli_cb)

int bt_mesh_cfg_comp_data_get(bt_mesh_client_common_param_t *param, uint8_t page);

int bt_mesh_cfg_beacon_get(bt_mesh_client_common_param_t *param);

int bt_mesh_cfg_beacon_set(bt_mesh_client_common_param_t *param, uint8_t val);

int bt_mesh_cfg_ttl_get(bt_mesh_client_common_param_t *param);

int bt_mesh_cfg_ttl_set(bt_mesh_client_common_param_t *param, uint8_t val);

int bt_mesh_cfg_friend_get(bt_mesh_client_common_param_t *param);

int bt_mesh_cfg_friend_set(bt_mesh_client_common_param_t *param, uint8_t val);

int bt_mesh_cfg_gatt_proxy_get(bt_mesh_client_common_param_t *param);

int bt_mesh_cfg_gatt_proxy_set(bt_mesh_client_common_param_t *param, uint8_t val);

int bt_mesh_cfg_relay_get(bt_mesh_client_common_param_t *param);

int bt_mesh_cfg_relay_set(bt_mesh_client_common_param_t *param,
                          uint8_t relay, uint8_t retransmit);

int bt_mesh_cfg_net_key_add(bt_mesh_client_common_param_t *param,
                            uint16_t net_idx, const uint8_t net_key[16]);

int bt_mesh_cfg_app_key_add(bt_mesh_client_common_param_t *param,
                            uint16_t net_idx, uint16_t app_idx,
                            const uint8_t app_key[16]);

int bt_mesh_cfg_mod_app_bind(bt_mesh_client_common_param_t *param,
                             uint16_t elem_addr, uint16_t app_idx,
                             uint16_t mod_id, uint16_t cid);

struct bt_mesh_cfg_mod_pub {
    uint16_t addr;
    uint16_t app_idx;
    bool     cred_flag;
    uint8_t  ttl;
    uint8_t  period;
    uint8_t  transmit;
};

int bt_mesh_cfg_mod_pub_get(bt_mesh_client_common_param_t *param,
                            uint16_t elem_addr, uint16_t mod_id, uint16_t cid);

int bt_mesh_cfg_mod_pub_set(bt_mesh_client_common_param_t *param,
                            uint16_t elem_addr, uint16_t mod_id, uint16_t cid,
                            struct bt_mesh_cfg_mod_pub *pub);

int bt_mesh_cfg_mod_sub_add(bt_mesh_client_common_param_t *param,
                            uint16_t elem_addr, uint16_t sub_addr,
                            uint16_t mod_id, uint16_t cid);

int bt_mesh_cfg_mod_sub_del(bt_mesh_client_common_param_t *param,
                            uint16_t elem_addr, uint16_t sub_addr,
                            uint16_t mod_id, uint16_t cid);

int bt_mesh_cfg_mod_sub_overwrite(bt_mesh_client_common_param_t *param,
                                  uint16_t elem_addr, uint16_t sub_addr,
                                  uint16_t mod_id, uint16_t cid);

int bt_mesh_cfg_mod_sub_va_add(bt_mesh_client_common_param_t *param,
                               uint16_t elem_addr, const uint8_t label[16],
                               uint16_t mod_id, uint16_t cid);

int bt_mesh_cfg_mod_sub_va_del(bt_mesh_client_common_param_t *param,
                               uint16_t elem_addr, const uint8_t label[16],
                               uint16_t mod_id, uint16_t cid);

int bt_mesh_cfg_mod_sub_va_overwrite(bt_mesh_client_common_param_t *param,
                                     uint16_t elem_addr, const uint8_t label[16],
                                     uint16_t mod_id, uint16_t cid);

struct bt_mesh_cfg_hb_sub {
    uint16_t src;
    uint16_t dst;
    uint8_t  period;
};

int bt_mesh_cfg_hb_sub_set(bt_mesh_client_common_param_t *param,
                           struct bt_mesh_cfg_hb_sub *sub);

int bt_mesh_cfg_hb_sub_get(bt_mesh_client_common_param_t *param);

struct bt_mesh_cfg_hb_pub {
    uint16_t dst;
    uint8_t  count;
    uint8_t  period;
    uint8_t  ttl;
    uint16_t feat;
    uint16_t net_idx;
};

int bt_mesh_cfg_hb_pub_set(bt_mesh_client_common_param_t *param,
                           struct bt_mesh_cfg_hb_pub *pub);

int bt_mesh_cfg_hb_pub_get(bt_mesh_client_common_param_t *param);

int bt_mesh_cfg_node_reset(bt_mesh_client_common_param_t *param);

/* Configuration Client Status Message Context */

struct bt_mesh_cfg_comp_data_status {
    uint8_t page;
    struct net_buf_simple *comp_data;
};

struct bt_mesh_cfg_relay_status {
    uint8_t relay;
    uint8_t retransmit;
};

struct bt_mesh_cfg_netkey_status {
    uint8_t  status;
    uint16_t net_idx;
};

struct bt_mesh_cfg_appkey_status {
    uint8_t  status;
    uint16_t net_idx;
    uint16_t app_idx;
};

struct bt_mesh_cfg_mod_app_status {
    uint8_t  status;
    uint16_t elem_addr;
    uint16_t app_idx;
    uint16_t cid;
    uint16_t mod_id;
};

struct bt_mesh_cfg_mod_pub_status {
    uint8_t  status;
    uint16_t elem_addr;
    uint16_t addr;
    uint16_t app_idx;
    bool     cred_flag;
    uint8_t  ttl;
    uint8_t  period;
    uint8_t  transmit;
    uint16_t cid;
    uint16_t mod_id;
};

struct bt_mesh_cfg_mod_sub_status {
    uint8_t  status;
    uint16_t elem_addr;
    uint16_t sub_addr;
    uint16_t cid;
    uint16_t mod_id;
};

struct bt_mesh_cfg_hb_sub_status {
    uint8_t  status;
    uint16_t src;
    uint16_t dst;
    uint8_t  period;
    uint8_t  count;
    uint8_t  min;
    uint8_t  max;
};

struct bt_mesh_cfg_hb_pub_status {
    uint8_t  status;
    uint16_t dst;
    uint8_t  count;
    uint8_t  period;
    uint8_t  ttl;
    uint16_t feat;
    uint16_t net_idx;
};

struct bt_mesh_cfg_mod_sub_list {
    uint8_t  status;
    uint16_t elem_addr;
    uint16_t cid;
    uint16_t mod_id;
    struct net_buf_simple *addr;
};

struct bt_mesh_cfg_net_key_list {
    struct net_buf_simple *net_idx;
};

struct bt_mesh_cfg_app_key_list {
    uint8_t  status;
    uint16_t net_idx;
    struct net_buf_simple *app_idx;
};

struct bt_mesh_cfg_node_id_status {
    uint8_t  status;
    uint16_t net_idx;
    uint8_t  identity;
};

struct bt_mesh_cfg_mod_app_list {
    uint8_t  status;
    uint16_t elem_addr;
    uint16_t cid;
    uint16_t mod_id;
    struct net_buf_simple *app_idx;
};

struct bt_mesh_cfg_key_refresh_status {
    uint8_t  status;
    uint16_t net_idx;
    uint8_t  phase;
};

struct bt_mesh_cfg_lpn_pollto_status {
    uint16_t lpn_addr;
    int32_t  timeout;
};

int bt_mesh_cfg_mod_pub_va_set(bt_mesh_client_common_param_t *param,
                               uint16_t elem_addr, uint16_t mod_id,
                               uint16_t cid, const uint8_t label[16],
                               struct bt_mesh_cfg_mod_pub *pub);

int bt_mesh_cfg_mod_sub_del_all(bt_mesh_client_common_param_t *param,
                                uint16_t elem_addr, uint16_t mod_id, uint16_t cid);

int bt_mesh_cfg_mod_sub_get(bt_mesh_client_common_param_t *param,
                            uint16_t elem_addr, uint16_t mod_id);

int bt_mesh_cfg_mod_sub_get_vnd(bt_mesh_client_common_param_t *param,
                                uint16_t elem_addr, uint16_t mod_id, uint16_t cid);

int bt_mesh_cfg_net_key_update(bt_mesh_client_common_param_t *param,
                               uint16_t net_idx, const uint8_t net_key[16]);

int bt_mesh_cfg_net_key_delete(bt_mesh_client_common_param_t *param, uint16_t net_idx);

int bt_mesh_cfg_net_key_get(bt_mesh_client_common_param_t *param);

int bt_mesh_cfg_app_key_update(bt_mesh_client_common_param_t *param,
                               uint16_t net_idx, uint16_t app_idx,
                               const uint8_t app_key[16]);

int bt_mesh_cfg_app_key_delete(bt_mesh_client_common_param_t *param,
                               uint16_t net_idx, uint16_t app_idx);

int bt_mesh_cfg_app_key_get(bt_mesh_client_common_param_t *param, uint16_t net_idx);

int bt_mesh_cfg_node_identity_get(bt_mesh_client_common_param_t *param, uint16_t net_idx);

int bt_mesh_cfg_node_identity_set(bt_mesh_client_common_param_t *param,
                                  uint16_t net_idx, uint8_t identity);

int bt_mesh_cfg_mod_app_unbind(bt_mesh_client_common_param_t *param,
                               uint16_t elem_addr, uint16_t app_idx,
                               uint16_t mod_id, uint16_t cid);

int bt_mesh_cfg_mod_app_get(bt_mesh_client_common_param_t *param,
                            uint16_t elem_addr, uint16_t mod_id);

int bt_mesh_cfg_mod_app_get_vnd(bt_mesh_client_common_param_t *param,
                                uint16_t elem_addr, uint16_t mod_id, uint16_t cid);

int bt_mesh_cfg_kr_phase_get(bt_mesh_client_common_param_t *param, uint16_t net_idx);

int bt_mesh_cfg_kr_phase_set(bt_mesh_client_common_param_t *param,
                             uint16_t net_idx, uint8_t transition);

int bt_mesh_cfg_lpn_timeout_get(bt_mesh_client_common_param_t *param, uint16_t lpn_addr);

int bt_mesh_cfg_net_transmit_get(bt_mesh_client_common_param_t *param);

int bt_mesh_cfg_net_transmit_set(bt_mesh_client_common_param_t *param, uint8_t transmit);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __BLE_MESH_CFG_CLI_H */
