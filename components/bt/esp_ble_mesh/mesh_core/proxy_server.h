/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _PROXY_H_
#define _PROXY_H_

#include "net.h"
#include "mesh_bearer_adapt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_MESH_PROXY_NET_PDU   0x00
#define BLE_MESH_PROXY_BEACON    0x01
#define BLE_MESH_PROXY_CONFIG    0x02
#define BLE_MESH_PROXY_PROV      0x03

#if CONFIG_BLE_MESH_PROXY
/**
 * Device Name Characteristic:
 * 1. For iOS, when it tries to get the value of Device Name Characteristic, the PDU
 *    "Read By Type Request" will be used, and the valid length of corresponding
 *    response is 19 (23 - 1 - 1 - 2).
 * 2. For Android, when it tries to get the value of Device Name Characteristic, the
 *    PDU "Read Request" will be used, and the valid length of corresponding response
 *    is 22 (23 - 1).
 */
#define DEVICE_NAME_SIZE    MIN((BLE_MESH_GATT_DEF_MTU_SIZE - 4), (BLE_MESH_GAP_ADV_MAX_LEN - 2))
#else
/* For Scan Response Data, the maximum length is 29 (31 - 1 - 1) currently. */
#define DEVICE_NAME_SIZE    (BLE_MESH_GAP_ADV_MAX_LEN - 2)
#endif

int bt_mesh_set_device_name(const char *name);

int bt_mesh_proxy_server_send(struct bt_mesh_conn *conn, uint8_t type,
                              struct net_buf_simple *msg);

int bt_mesh_proxy_server_prov_enable(void);
int bt_mesh_proxy_server_prov_disable(bool disconnect);

int bt_mesh_proxy_server_gatt_enable(void);
int bt_mesh_proxy_server_gatt_disable(void);

void bt_mesh_proxy_server_gatt_disconnect(void);

void bt_mesh_proxy_server_beacon_send(struct bt_mesh_subnet *sub);

struct net_buf_simple *bt_mesh_proxy_server_get_buf(void);

int32_t bt_mesh_proxy_server_adv_start(void);
void bt_mesh_proxy_server_adv_stop(void);

void bt_mesh_proxy_server_identity_start(struct bt_mesh_subnet *sub);
void bt_mesh_proxy_server_identity_stop(struct bt_mesh_subnet *sub);

bool bt_mesh_proxy_server_relay(struct net_buf_simple *buf, uint16_t dst);
void bt_mesh_proxy_server_addr_add(struct net_buf_simple *buf, uint16_t addr);

int bt_mesh_proxy_server_init(void);
int bt_mesh_proxy_server_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _PROXY_H_ */
