/*
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "net.h"
#include "provisioner_main.h"

int settings_core_init(void);
int settings_core_load(void);
int settings_core_commit(void);
int settings_core_deinit(bool erase);

void bt_mesh_store_role(void);
void bt_mesh_store_net(void);
void bt_mesh_store_iv(bool only_duration);
void bt_mesh_clear_iv(void);
void bt_mesh_store_seq(void);
void bt_mesh_clear_seq(void);
void bt_mesh_store_rpl(struct bt_mesh_rpl *rpl);
void bt_mesh_store_subnet(struct bt_mesh_subnet *sub);
void bt_mesh_store_app_key(struct bt_mesh_app_key *key);
void bt_mesh_store_hb_pub(void);
void bt_mesh_store_cfg(void);
void bt_mesh_store_mod_bind(struct bt_mesh_model *mod);
void bt_mesh_store_mod_sub(struct bt_mesh_model *mod);
void bt_mesh_store_mod_pub(struct bt_mesh_model *mod);
void bt_mesh_clear_mod_bind(void);
void bt_mesh_clear_mod_sub(void);
void bt_mesh_clear_mod_pub(void);
void bt_mesh_reset_mod_bind(void);
void bt_mesh_reset_mod_sub(void);
void bt_mesh_reset_mod_pub(void);
void bt_mesh_store_label(void);

void bt_mesh_clear_role(void);
void bt_mesh_clear_net(void);
void bt_mesh_clear_subnet(struct bt_mesh_subnet *sub);
void bt_mesh_clear_app_key(struct bt_mesh_app_key *key);
void bt_mesh_clear_rpl(void);

#if CONFIG_BLE_MESH_PROVISIONER
void bt_mesh_store_prov_info(u16_t primary_addr, u16_t alloc_addr);
void bt_mesh_clear_prov_info(void);
void bt_mesh_store_p_net_idx(void);
void bt_mesh_clear_p_net_idx(void);
void bt_mesh_store_p_app_idx(void);
void bt_mesh_clear_p_app_idx(void);
void bt_mesh_store_p_subnet(struct bt_mesh_subnet *sub);
void bt_mesh_store_p_app_key(struct bt_mesh_app_key *key);
void bt_mesh_clear_p_subnet(struct bt_mesh_subnet *sub);
void bt_mesh_clear_p_app_key(struct bt_mesh_app_key *key);
void bt_mesh_clear_rpl_single(u16_t src);
void bt_mesh_store_node_info(struct bt_mesh_node *node, bool prov);
void bt_mesh_clear_node_info(u16_t unicast_addr, bool prov);
void bt_mesh_store_node_name(struct bt_mesh_node *node, bool prov);
void bt_mesh_store_node_comp_data(struct bt_mesh_node *node, bool prov);
#if CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE
int settings_user_id_init(void);
int settings_user_id_load(void);
int settings_user_id_deinit(bool erase);
u32_t bt_mesh_provisioner_get_settings_handle(void);
int bt_mesh_provisioner_open_settings_with_index(u8_t index);
int bt_mesh_provisioner_open_settings_with_user_id(const char *user_id, u8_t *index);
int bt_mesh_provisioner_close_settings_with_index(u8_t index);
int bt_mesh_provisioner_close_settings_with_user_id(const char *user_id, u8_t *index);
int bt_mesh_provisioner_restore_settings_with_index(u8_t index);
int bt_mesh_provisioner_restore_settings_with_user_id(const char *user_id, u8_t *index);
int bt_mesh_provisioner_release_settings_with_index(u8_t index, bool erase);
int bt_mesh_provisioner_release_settings_with_user_id(const char *user_id, bool erase, u8_t *index);
int bt_mesh_provisioner_delete_settings_with_index(u8_t index);
int bt_mesh_provisioner_delete_settings_with_user_id(const char *user_id, u8_t *index);
const char *bt_mesh_provisioner_get_settings_user_id(u8_t index);
u8_t bt_mesh_provisioner_get_settings_index(const char *user_id);
u8_t bt_mesh_provisioner_get_free_settings_user_id_count(void);
#endif /* CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE */
int bt_mesh_provisioner_direct_erase_settings(void);
#endif /* CONFIG_BLE_MESH_PROVISIONER */

void bt_mesh_settings_lock(void);
void bt_mesh_settings_unlock(void);

int bt_mesh_settings_init(void);
int bt_mesh_settings_deinit(bool erase);

#endif /* _SETTINGS_H_ */
