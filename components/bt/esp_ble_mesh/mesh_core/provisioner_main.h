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

#ifndef _PROVISIONER_MAIN_H_
#define _PROVISIONER_MAIN_H_

#include "mesh_util.h"
#include "mesh_kernel.h"
#include "mesh_access.h"
#include "net.h"

#define MESH_NAME_SIZE  31

/* Each node information stored by provisioner */
struct bt_mesh_node_t {
    char  node_name[MESH_NAME_SIZE];    /* Node name */
    u8_t  dev_uuid[16];                 /* Device UUID pointer, stored in provisioner_prov.c */
    u16_t oob_info;                     /* Node OOB information */
    u16_t unicast_addr;                 /* Node unicast address */
    u8_t  element_num;                  /* Node element number */
    u16_t net_idx;                      /* Node provision net_idx */
    u8_t  flags;                        /* Node key refresh flag and iv update flag */
    u32_t iv_index;                     /* Node IV Index */
    u8_t  dev_key[16];                  /* Node device key */
} __packed;

/* The following APIs are for key init, node provision & node reset. */

int provisioner_node_provision(int node_index, const u8_t uuid[16], u16_t oob_info,
                               u16_t unicast_addr, u8_t element_num, u16_t net_idx,
                               u8_t flags, u32_t iv_index, const u8_t dev_key[16]);

int provisioner_node_reset(int node_index);

int provisioner_upper_reset_all_nodes(void);

int provisioner_upper_init(void);

/* The following APIs are for provisioner upper layers internal usage. */

const u8_t *provisioner_net_key_get(u16_t net_idx);

struct bt_mesh_subnet *provisioner_subnet_get(u16_t net_idx);

bool provisioner_check_msg_dst_addr(u16_t dst_addr);

const u8_t *provisioner_dev_key_get(u16_t dst_addr);

struct bt_mesh_app_key *provisioner_app_key_find(u16_t app_idx);

u32_t provisioner_get_prov_node_count(void);

/* The following APIs are for provisioner application use. */

int bt_mesh_provisioner_store_node_info(struct bt_mesh_node_t *node_info);

int bt_mesh_provisioner_get_all_node_unicast_addr(struct net_buf_simple *buf);

int bt_mesh_provisioner_set_node_name(int node_index, const char *name);

const char *bt_mesh_provisioner_get_node_name(int node_index);

int bt_mesh_provisioner_get_node_index(const char *name);

struct bt_mesh_node_t *bt_mesh_provisioner_get_node_info(u16_t unicast_addr);

u32_t bt_mesh_provisioner_get_net_key_count(void);

u32_t bt_mesh_provisioner_get_app_key_count(void);

int bt_mesh_provisioner_local_app_key_add(const u8_t app_key[16], u16_t net_idx, u16_t *app_idx);

const u8_t *bt_mesh_provisioner_local_app_key_get(u16_t net_idx, u16_t app_idx);

int bt_mesh_provisioner_local_app_key_delete(u16_t net_idx, u16_t app_idx);

int bt_mesh_provisioner_local_net_key_add(const u8_t net_key[16], u16_t *net_idx);

const u8_t *bt_mesh_provisioner_local_net_key_get(u16_t net_idx);

int bt_mesh_provisioner_local_net_key_delete(u16_t net_idx);

int bt_mesh_provisioner_get_own_unicast_addr(u16_t *addr, u8_t *elem_num);

/* Provisioner bind local client model with proper appkey index */
int bt_mesh_provisioner_bind_local_model_app_idx(u16_t elem_addr, u16_t mod_id,
        u16_t cid, u16_t app_idx);

/* This API can be used to change the net_idx binded with the app_idx. */
int bt_mesh_provisioner_bind_local_app_net_idx(u16_t net_idx, u16_t app_idx);

/* Provisioner print own element information */
int bt_mesh_provisioner_print_local_element_info(void);

/* The following APIs are for fast provisioning */

const u8_t *fast_prov_dev_key_get(u16_t dst_addr);

struct bt_mesh_subnet *fast_prov_subnet_get(u16_t net_idx);

struct bt_mesh_app_key *fast_prov_app_key_find(u16_t net_idx, u16_t app_idx);

u8_t bt_mesh_set_fast_prov_net_idx(u16_t net_idx);

u8_t bt_mesh_add_fast_prov_net_key(const u8_t net_key[16]);

const u8_t *bt_mesh_get_fast_prov_net_key(u16_t net_idx);

const u8_t *bt_mesh_get_fast_prov_app_key(u16_t net_idx, u16_t app_idx);

#endif /* _PROVISIONER_MAIN_H_ */
