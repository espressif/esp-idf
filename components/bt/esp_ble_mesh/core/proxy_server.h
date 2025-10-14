/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _PROXY_SERVER_H_
#define _PROXY_SERVER_H_

#include "net.h"
#include "mesh/adapter.h"
#include "adv.h"

#ifdef __cplusplus
extern "C" {
#endif

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

struct bt_mesh_proxy_client {
    struct bt_mesh_conn *conn;

    enum __attribute__((packed)) {
        SRV_NONE,
        SRV_WHITELIST,
        SRV_BLACKLIST,
        SRV_PROV,
    } filter_type;

    uint8_t msg_type;

#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    struct {
        uint16_t addr;
        bool     proxy_client; /* Indicate if the address is the element address of Proxy Client. */
    } filter[CONFIG_BLE_MESH_PROXY_FILTER_SIZE];
    struct k_delayed_work send_beacons;

    uint8_t proxy_client_type;
    uint8_t proxy_msg_recv : 1;     /* Indicate if proxy server has received a message from proxy client */

#if CONFIG_BLE_MESH_PROXY_PRIVACY
    uint8_t proxy_privacy;
#endif
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */

    struct k_delayed_work sar_timer;

    struct net_buf_simple buf;
};

typedef void (*proxy_server_connect_cb_t)(uint8_t conn_handle);
typedef void (*proxy_server_disconnect_cb_t)(uint8_t conn_handle, uint8_t reason);

int bt_mesh_set_device_name(const char *name);

const char *bt_mesh_get_device_name(void);

int bt_mesh_proxy_server_segment_send(struct bt_mesh_conn *conn, uint8_t type,
                                      struct net_buf_simple *msg);

int bt_mesh_proxy_server_send(struct bt_mesh_conn *conn, uint8_t type,
                              struct net_buf_simple *msg);

int bt_mesh_proxy_server_prov_enable(void);
int bt_mesh_proxy_server_prov_disable(bool disconnect);

void bt_mesh_proxy_server_set_conn_cb(proxy_server_connect_cb_t cb);
void bt_mesh_proxy_server_set_disconn_cb(proxy_server_disconnect_cb_t cb);

int bt_mesh_proxy_server_gatt_enable(void);
int bt_mesh_proxy_server_gatt_disable(void);

void bt_mesh_proxy_server_gatt_disconnect(void);

void bt_mesh_proxy_server_beacon_send(struct bt_mesh_subnet *sub);

struct net_buf_simple *bt_mesh_proxy_server_get_buf(void);

int32_t bt_mesh_proxy_server_adv_start(void);
int bt_mesh_proxy_server_adv_stop(void);

void bt_mesh_proxy_server_update_net_id_rand(void);
void bt_mesh_proxy_server_update_net_id_rand_stop(void);

#if CONFIG_BLE_MESH_PRB_SRV
void bt_mesh_proxy_server_private_identity_start(struct bt_mesh_subnet *sub);
void bt_mesh_proxy_server_private_identity_stop(struct bt_mesh_subnet *sub);

void bt_mesh_disable_private_gatt_proxy(void);

bool bt_mesh_proxy_server_is_node_id_enable(void);

int bt_mesh_proxy_private_identity_disable(void);

int bt_mesh_proxy_private_identity_enable(void);
#endif /* CONFIG_BLE_MESH_PRB_SRV */

void bt_mesh_proxy_server_identity_start(struct bt_mesh_subnet *sub);
void bt_mesh_proxy_server_identity_stop(struct bt_mesh_subnet *sub);

bool bt_mesh_proxy_server_relay(struct net_buf_simple *buf, uint16_t dst);
void bt_mesh_proxy_server_addr_add(struct net_buf_simple *buf, uint16_t addr);
void bt_mesh_proxy_server_adv_flag_set(bool enable);

int bt_mesh_proxy_server_init(void);
int bt_mesh_proxy_server_deinit(void);

bool bt_mesh_proxy_server_find_client_by_addr(uint16_t addr);
uint8_t bt_mesh_proxy_server_get_all_client_type(void);

#ifdef __cplusplus
}
#endif

#endif /* _PROXY_SERVER_H_ */
