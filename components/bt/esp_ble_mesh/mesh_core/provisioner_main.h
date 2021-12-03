/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _PROVISIONER_MAIN_H_
#define _PROVISIONER_MAIN_H_

#include "net.h"
#include "mesh_bearer_adapt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_MESH_INVALID_NODE_INDEX     0xFFFF
#define BLE_MESH_NODE_NAME_SIZE         31

/* Each node information stored by provisioner */
struct bt_mesh_node {
    /* Device information */
    uint8_t  addr[6];      /* Node device address */
    uint8_t  addr_type;    /* Node device address type */
    uint8_t  dev_uuid[16]; /* Node Device UUID */
    uint16_t oob_info;     /* Node OOB information */

    /* Provisioning information */
    uint16_t unicast_addr; /* Node unicast address */
    uint8_t  element_num;  /* Node element number */
    uint16_t net_idx;      /* Node NetKey Index */
    uint8_t  flags;        /* Node key refresh flag and iv update flag */
    uint32_t iv_index;     /* Node IV Index */
    uint8_t  dev_key[16];  /* Node device key */

    /* Additional information */
    char  name[BLE_MESH_NODE_NAME_SIZE + 1]; /* Node name */
    uint16_t comp_length;  /* Length of Composition Data */
    uint8_t *comp_data;    /* Value of Composition Data */
} __packed;

int bt_mesh_provisioner_init(void);

int bt_mesh_provisioner_net_create(void);

void bt_mesh_provisioner_main_reset(bool erase);

int bt_mesh_provisioner_deinit(bool erase);

bool bt_mesh_provisioner_check_is_addr_dup(uint16_t addr, uint8_t elem_num, bool comp_with_own);

uint16_t bt_mesh_provisioner_get_node_count(void);

int bt_mesh_provisioner_restore_node_info(struct bt_mesh_node *node);

int bt_mesh_provisioner_provision(const bt_mesh_addr_t *addr, const uint8_t uuid[16],
                                  uint16_t oob_info, uint16_t unicast_addr,
                                  uint8_t element_num, uint16_t net_idx,
                                  uint8_t flags, uint32_t iv_index,
                                  const uint8_t dev_key[16], uint16_t *index);

int bt_mesh_provisioner_remove_node(const uint8_t uuid[16]);

int bt_mesh_provisioner_restore_node_name(uint16_t addr, const char *name);

int bt_mesh_provisioner_restore_node_comp_data(uint16_t addr, const uint8_t *data, uint16_t length);

struct bt_mesh_node *bt_mesh_provisioner_get_node_with_uuid(const uint8_t uuid[16]);

struct bt_mesh_node *bt_mesh_provisioner_get_node_with_addr(uint16_t unicast_addr);

int bt_mesh_provisioner_delete_node_with_uuid(const uint8_t uuid[16]);

int bt_mesh_provisioner_delete_node_with_node_addr(uint16_t unicast_addr);

int bt_mesh_provisioner_delete_node_with_dev_addr(const bt_mesh_addr_t *addr);

int bt_mesh_provisioner_set_node_name(uint16_t index, const char *name);

const char *bt_mesh_provisioner_get_node_name(uint16_t index);

uint16_t bt_mesh_provisioner_get_node_index(const char *name);

struct bt_mesh_node *bt_mesh_provisioner_get_node_with_name(const char *name);

const struct bt_mesh_node **bt_mesh_provisioner_get_node_table_entry(void);

int bt_mesh_provisioner_store_node_comp_data(uint16_t addr, const uint8_t *data, uint16_t length);

const uint8_t *bt_mesh_provisioner_net_key_get(uint16_t net_idx);

struct bt_mesh_subnet *bt_mesh_provisioner_subnet_get(uint16_t net_idx);

bool bt_mesh_provisioner_check_msg_dst(uint16_t dst);

const uint8_t *bt_mesh_provisioner_dev_key_get(uint16_t dst);

struct bt_mesh_app_key *bt_mesh_provisioner_app_key_find(uint16_t app_idx);

int bt_mesh_provisioner_local_app_key_add(const uint8_t app_key[16],
                                          uint16_t net_idx, uint16_t *app_idx);

int bt_mesh_provisioner_local_app_key_update(const uint8_t app_key[16],
                                             uint16_t net_idx, uint16_t app_idx);

const uint8_t *bt_mesh_provisioner_local_app_key_get(uint16_t net_idx, uint16_t app_idx);

int bt_mesh_provisioner_local_app_key_del(uint16_t net_idx, uint16_t app_idx, bool store);

int bt_mesh_provisioner_local_net_key_add(const uint8_t net_key[16], uint16_t *net_idx);

int bt_mesh_provisioner_local_net_key_update(const uint8_t net_key[16], uint16_t net_idx);

const uint8_t *bt_mesh_provisioner_local_net_key_get(uint16_t net_idx);

int bt_mesh_provisioner_local_net_key_del(uint16_t net_idx, bool store);

/* Provisioner bind local client model with proper appkey index */
int bt_mesh_provisioner_bind_local_model_app_idx(uint16_t elem_addr, uint16_t mod_id,
                                                 uint16_t cid, uint16_t app_idx);

typedef void (* bt_mesh_heartbeat_recv_cb_t)(uint16_t hb_src, uint16_t hb_dst,
                                             uint8_t init_ttl, uint8_t rx_ttl,
                                             uint8_t hops, uint16_t feat, int8_t rssi);

int bt_mesh_provisioner_recv_heartbeat(bt_mesh_heartbeat_recv_cb_t cb);

int bt_mesh_provisioner_set_heartbeat_filter_type(uint8_t filter_type);

int bt_mesh_provisioner_set_heartbeat_filter_info(uint8_t op, uint16_t src, uint16_t dst);

void bt_mesh_provisioner_heartbeat(uint16_t hb_src, uint16_t hb_dst,
                                   uint8_t init_ttl, uint8_t rx_ttl,
                                   uint8_t hops, uint16_t feat, int8_t rssi);

/* Provisioner print own element information */
int bt_mesh_print_local_composition_data(void);

int bt_mesh_provisioner_store_node_info(struct bt_mesh_node *node);

#ifdef __cplusplus
}
#endif

#endif /* _PROVISIONER_MAIN_H_ */
