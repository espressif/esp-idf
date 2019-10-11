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

#define BLE_MESH_PROXY_NET_PDU   0x00
#define BLE_MESH_PROXY_BEACON    0x01
#define BLE_MESH_PROXY_CONFIG    0x02
#define BLE_MESH_PROXY_PROV      0x03

/**
 * @brief This function is called to send proxy protocol messages.
 *
 * @param[in] conn: Pointer to bt_conn structure
 * @param[in] type: Proxy protocol message type
 * @param[in] msg:  Pointer to the buffer contains sending message.
 *
 * @return Zero-success, other-fail
 */
int provisioner_proxy_send(struct bt_mesh_conn *conn, u8_t type, struct net_buf_simple *msg);

/**
 * @brief This function is called to parse received node identity and net
 *        id adv pkts and create connection if deceided to.
 *
 * @param[in] buf: Pointer to the buffer contains received message.
 *
 * @return None
 */
void provisioner_proxy_srv_data_recv(struct net_buf_simple *buf);

/**
 * @brief This function is called to initialize proxy provisioner structure
 *        and register proxy connection related callbacks.
 *
 * @return Zero-success, other-fail
 */
int provisioner_proxy_init(void);

/**
 * @brief This function is called to enable dealing with proxy provisioning
 *        messages.
 *
 * @return Zero-success, other-fail
 */
int provisioner_pb_gatt_enable(void);

/**
 * @brief This function is called to disable dealing with proxy provisioning
 *        messages and if proxy provisioning connections exist, the connections
 *        will be disconnected.
 *
 * @return Zero-success, other-fail
 */
int provisioner_pb_gatt_disable(void);

/* The following APIs are for application use */
/**
 * @brief This function is called to enable receiving node identity and net
 *        id adv pkts.
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_provisioner_proxy_enable(void);

/**
 * @brief This function is called to disable receiving node identity and net
 *        id adv pkts, and if proxy connections exist, these connections will
 *        be disconnected.
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_provisioner_proxy_disable(void);

#endif /* _PROVISIONER_PROXY_H_ */
