/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _PROXY_CLIENT_H_
#define _PROXY_CLIENT_H_

#include "net.h"
#include "mesh_bearer_adapt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_MESH_PROXY_ADV_NET_ID           0x00
#define BLE_MESH_PROXY_ADV_NODE_ID          0x01

#define BLE_MESH_PROXY_NET_PDU              0x00
#define BLE_MESH_PROXY_BEACON               0x01
#define BLE_MESH_PROXY_CONFIG               0x02
#define BLE_MESH_PROXY_PROV                 0x03

#define BLE_MESH_PROXY_CFG_FILTER_SET       0x00
#define BLE_MESH_PROXY_CFG_FILTER_ADD       0x01
#define BLE_MESH_PROXY_CFG_FILTER_REMOVE    0x02
#define BLE_MESH_PROXY_CFG_FILTER_STATUS    0x03

typedef union {
    struct {
        uint8_t  net_id[8];
        uint16_t net_idx;
    } net_id;
    struct {
        uint16_t src;
    } node_id;
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

bool bt_mesh_proxy_client_beacon_send(struct bt_mesh_subnet *sub);

bool bt_mesh_proxy_client_relay(struct net_buf_simple *buf, uint16_t dst);

int bt_mesh_proxy_client_cfg_send(uint8_t conn_handle, uint16_t net_idx,
                                  struct bt_mesh_proxy_cfg_pdu *pdu);

int bt_mesh_proxy_client_init(void);
int bt_mesh_proxy_client_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _PROXY_CLIENT_H_ */
