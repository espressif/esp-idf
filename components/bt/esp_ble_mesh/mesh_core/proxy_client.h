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

#ifndef _PROVISIONER_PROXY_H_
#define _PROVISIONER_PROXY_H_

#include "mesh_buf.h"
#include "net.h"

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
        u8_t  net_id[8];
        u16_t net_idx;
    } net_id;
    struct {
        u16_t src;
    } node_id;
} bt_mesh_proxy_adv_ctx_t;

struct bt_mesh_proxy_net_pdu {
    struct net_buf_simple *val;
};

struct bt_mesh_proxy_cfg_pdu {
    u8_t opcode;
    union {
        struct cfg_filter_set {
            u8_t filter_type;
        } set;
        struct cfg_addr_add {
            u16_t *addr;
            u16_t  addr_num;
        } add;
        struct cfg_addr_remove {
            u16_t *addr;
            u16_t  addr_num;
        } remove;
    };
};

typedef struct {
    u8_t type;
    union {
        struct bt_mesh_proxy_net_pdu net;
        struct bt_mesh_proxy_cfg_pdu cfg;
    };
} bt_mesh_proxy_client_pdu_t;

int bt_mesh_proxy_prov_client_send(struct bt_mesh_conn *conn, u8_t type, struct net_buf_simple *msg);

int provisioner_pb_gatt_enable(void);
int provisioner_pb_gatt_disable(void);

int bt_mesh_proxy_client_enable(void);
int bt_mesh_proxy_client_disable(void);

typedef void (*proxy_client_recv_adv_cb_t)(const bt_mesh_addr_t *addr, u8_t type, bt_mesh_proxy_adv_ctx_t *ctx);
typedef void (*proxy_client_connect_cb_t)(const bt_mesh_addr_t *addr, u8_t conn_handle, u16_t net_idx);
typedef void (*proxy_client_disconnect_cb_t)(const bt_mesh_addr_t *addr, u8_t conn_handle, u16_t net_idx, u8_t reason);
typedef void (*proxy_client_recv_filter_status_cb_t)(u8_t conn_handle, u16_t src, u16_t net_idx, u8_t filter_type, u16_t list_size);

void bt_mesh_proxy_client_set_adv_recv_cb(proxy_client_recv_adv_cb_t cb);
void bt_mesh_proxy_client_set_conn_cb(proxy_client_connect_cb_t cb);
void bt_mesh_proxy_client_set_disconn_cb(proxy_client_disconnect_cb_t cb);
void bt_mesh_proxy_client_set_filter_status_cb(proxy_client_recv_filter_status_cb_t cb);

void proxy_client_adv_ind_recv(struct net_buf_simple *buf, const bt_mesh_addr_t *addr);

int bt_mesh_proxy_client_connect(const u8_t addr[6], u8_t addr_type, u16_t net_idx);
int bt_mesh_proxy_client_disconnect(u8_t conn_handle);

bool bt_mesh_proxy_client_beacon_send(struct bt_mesh_subnet *sub);
bool bt_mesh_proxy_client_send(struct net_buf_simple *buf, u16_t dst);
int bt_mesh_proxy_client_send_cfg(u8_t conn_handle, u16_t net_idx, struct bt_mesh_proxy_cfg_pdu *pdu);

int bt_mesh_proxy_prov_client_init(void);

#endif /* _PROVISIONER_PROXY_H_ */
