/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _PROXY_CLIENT_H_
#define _PROXY_CLIENT_H_

#include "net.h"
#include "mesh/adapter.h"
#include "prov_common.h"

#include "mesh_v1.1/utils.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union {
    struct {
        uint8_t  net_id[8];
        uint16_t net_idx;
    } net_id;
    struct {
        uint16_t src;
    } node_id;
    struct {
        uint8_t  net_id[8];
        uint16_t net_idx;
    } private_net_id;
    struct {
        uint16_t src;
    } private_node_id;
} bt_mesh_proxy_adv_ctx_t;

struct bt_mesh_proxy_net_pdu {
    struct net_buf_simple *val;
};

struct bt_mesh_proxy_cfg_pdu {
    uint8_t opcode;
    union {
        struct cfg_filter_set {
            uint8_t filter_type;
        } set;
        struct cfg_addr_add {
            uint16_t *addr;
            uint16_t  addr_num;
        } add;
        struct cfg_addr_remove {
            uint16_t *addr;
            uint16_t  addr_num;
        } remove;
        struct cfg_direct_proxy_ctrl {
            uint8_t use_directed;
            struct {
                uint16_t len_present:1,
                         range_start:15;
                uint8_t  range_length;
            } proxy_client_uar;
        } direct_proxy_ctrl;
    };
};

typedef struct {
    uint8_t type;
    union {
        struct bt_mesh_proxy_net_pdu net;
        struct bt_mesh_proxy_cfg_pdu cfg;
    };
} bt_mesh_proxy_client_pdu_t;

int bt_mesh_proxy_client_send(struct bt_mesh_conn *conn, uint8_t type,
                              struct net_buf_simple *msg);

int bt_mesh_proxy_client_prov_enable(void);
int bt_mesh_proxy_client_prov_disable(void);

int bt_mesh_proxy_client_gatt_enable(void);
int bt_mesh_proxy_client_gatt_disable(void);

typedef void (*proxy_client_recv_adv_cb_t)(const bt_mesh_addr_t *addr, uint8_t type,
                                           bt_mesh_proxy_adv_ctx_t *ctx, int8_t rssi);
typedef void (*proxy_client_connect_cb_t)(const bt_mesh_addr_t *addr,
                                          uint8_t conn_handle, uint16_t net_idx);
typedef void (*proxy_client_disconnect_cb_t)(const bt_mesh_addr_t *addr, uint8_t conn_handle,
                                             uint16_t net_idx, uint8_t reason);
typedef void (*proxy_client_recv_filter_status_cb_t)(uint8_t conn_handle, uint16_t src, uint16_t net_idx,
                                                     uint8_t filter_type, uint16_t list_size);

void bt_mesh_proxy_client_set_adv_recv_cb(proxy_client_recv_adv_cb_t cb);
void bt_mesh_proxy_client_set_conn_cb(proxy_client_connect_cb_t cb);
void bt_mesh_proxy_client_set_disconn_cb(proxy_client_disconnect_cb_t cb);
void bt_mesh_proxy_client_set_filter_status_cb(proxy_client_recv_filter_status_cb_t cb);

void bt_mesh_proxy_client_gatt_adv_recv(struct net_buf_simple *buf,
                                        const bt_mesh_addr_t *addr, int8_t rssi);

int bt_mesh_proxy_client_connect(const uint8_t addr[6], uint8_t addr_type, uint16_t net_idx);
int bt_mesh_proxy_client_disconnect(uint8_t conn_handle);

bool bt_mesh_proxy_client_beacon_send(struct bt_mesh_subnet *sub, bool private);

bool bt_mesh_proxy_client_relay(struct net_buf_simple *buf, uint16_t dst);

int bt_mesh_proxy_client_cfg_send(uint8_t conn_handle, uint16_t net_idx,
                                  struct bt_mesh_proxy_cfg_pdu *pdu);

#if CONFIG_BLE_MESH_RPR_SRV
int bt_mesh_rpr_srv_set_waiting_prov_link(struct bt_mesh_prov_link* link,
                                          bt_mesh_addr_t *addr);
#endif

int bt_mesh_proxy_client_init(void);
int bt_mesh_proxy_client_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _PROXY_CLIENT_H_ */
