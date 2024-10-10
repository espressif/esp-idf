/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_v11_UTILS_H_
#define _BLE_MESH_v11_UTILS_H_

#include <stdint.h>
#include <stdbool.h>

#include "mesh/buf.h"

#include "net.h"
#include "prov_common.h"
#include "proxy_server.h"
#include "mesh/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

int bt_mesh_v11_init(const void *cfg, size_t cfg_size,
                     const void *func, size_t func_size);

int bt_mesh_v11_deinit(void);

#define BLE_MESH_TAG_SEND_SEGMENTED     BIT(0)
#define BLE_MESH_TAG_IMMUTABLE_CRED     BIT(1)
#define BLE_MESH_TAG_USE_DIRECTED       BIT(2)
#define BLE_MESH_TAG_RELAY              BIT(3)
#define BLE_MESH_TAG_FRIENDSHIP         BIT(4)

bool bt_mesh_tag_send_segmented(uint8_t tag);

bool bt_mesh_tag_immutable_cred(uint8_t tag);

bool bt_mesh_tag_use_directed(uint8_t tag);

bool bt_mesh_tag_relay(uint8_t tag);

bool bt_mesh_tag_friendship(uint8_t tag);

bool bt_mesh_uar_valid(void *uar);

bool bt_mesh_addr_in_uar(void *uar, uint16_t addr);

void bt_mesh_add_uar_be(struct net_buf_simple *buf, void *uar);

struct bt_mesh_prov_link;

void bt_mesh_prov_record_req(const uint8_t *data);

void bt_mesh_prov_record_rsp(const uint8_t *data);

void bt_mesh_prov_records_get(const uint8_t *data);

void bt_mesh_prov_records_list(const uint8_t *data);

int bt_mesh_node_cert_based_prov_init(void);

void bt_mesh_node_cert_based_prov_deinit(void);

void bt_mesh_pvnr_record_req(struct bt_mesh_prov_link *link,
                             struct net_buf_simple *buf);

void bt_mesh_pvnr_record_rsp(struct bt_mesh_prov_link *link,
                             struct net_buf_simple *buf);

void bt_mesh_pvnr_records_get(struct bt_mesh_prov_link *link,
                              struct net_buf_simple *buf);

void bt_mesh_pvnr_records_list(struct bt_mesh_prov_link *link,
                               struct net_buf_simple *buf);

int bt_mesh_prov_salt_epa(const uint8_t conf_salt[32],
                          const uint8_t prov_rand[32],
                          const uint8_t dev_rand[32],
                          uint8_t prov_salt[16]);

int bt_mesh_prov_conf_salt_epa(const uint8_t conf_inputs[145], uint8_t salt[32]);

int bt_mesh_prov_conf_key_epa(const uint8_t dhkey[32], const uint8_t auth[32],
                              const uint8_t conf_salt[32], uint8_t conf_key[32]);

int bt_mesh_prov_conf_epa(const uint8_t conf_key[32], const uint8_t rand[32],
                          uint8_t conf[32]);

uint8_t bt_mesh_comp_page_check(uint8_t page, bool largest);

int bt_mesh_get_comp_data(struct net_buf_simple *buf, uint8_t page,
                          uint16_t offset, bool full_element);

int bt_mesh_private_beacon_key(const uint8_t net_key[16], uint8_t private_beacon_key[16]);

void bt_mesh_directed_proxy_server_set_blacklist(void *client);

int bt_mesh_directed_proxy_server_directed_proxy_caps_status_send(void *conn, void *sub);

void bt_mesh_directed_proxy_server_connected(void *client);

void bt_mesh_directed_proxy_server_directed_proxy_caps_send(void *sub, bool clear);

void bt_mesh_directed_proxy_server_directed_proxy_ctrl_recv(void *client, void *rx,
                                                            struct net_buf_simple *buf);

int bt_mesh_directed_proxy_server_solicitation(void *client, void *sub);

int bt_mesh_directed_proxy_server_update_dep_node(void *sub, void *client, uint8_t type);

int bt_mesh_directed_forwarding_node_solicitation(void *model, void *sub);

int bt_mesh_directed_friend_solicitation(void *frnd, void *sub);

int bt_mesh_directed_update_dependent_node(void *sub, uint8_t type,
                                           uint16_t addr, uint8_t num_elem);

int bt_mesh_directed_forwarding_ctl_recv(uint8_t ctl_op, void *rx,
                                         struct net_buf_simple *buf);

void bt_mesh_restore_directed_forwarding_state(uint16_t net_idx,
                                               uint8_t directed_forwarding);

uint8_t bt_mesh_get_and_disable_directed_forwarding_state(void *sub);

int bt_mesh_directed_forwarding_sub_init(void *sub);

int bt_mesh_recovery_directed_forwarding_table(void *sub);

int bt_mesh_clear_directed_forwarding_table_data(uint16_t net_idx);

int bt_mesh_clear_all_directed_forwarding_table_data(void);

void bt_mesh_store_directed_forwarding_table_data(uint16_t net_idx);

int bt_mesh_power_up_create_path_origin_fsm(void *model);

int bt_mesh_private_beacon_timer_init(void);

int bt_mesh_private_beacon_timer_free(void);

int bt_mesh_private_beacon_timer_submit(int32_t delay);

int bt_mesh_private_beacon_timer_cancel(void);

void bt_mesh_private_beacon_timer_reset(void);

uint8_t bt_mesh_private_beacon_update_addr_type(const void *ad);

void bt_mesh_private_beacon_update_random(void *sub);

void bt_mesh_private_beacon_create(void *sub, struct net_buf_simple *buf);

void bt_mesh_private_beacon_recv(struct net_buf_simple *buf);

void bt_mesh_private_beacon_enable(void);

void bt_mesh_private_beacon_disable(void);

void bt_mesh_proxy_server_solic_recv(struct net_buf_simple *data,
                                     const bt_mesh_addr_t *addr,
                                     int8_t rssi);

uint16_t bt_mesh_proxy_server_get_solic_adv_net_idx(void);

int32_t bt_mesh_proxy_server_get_solic_adv_remaining(void);

void bt_mesh_proxy_server_stop_solic_adv_priv_net_id(void);

int bt_mesh_proxy_client_solic_send(uint16_t net_idx, uint16_t ssrc, uint16_t dst);

void bt_mesh_create_proxy_solic_nonce(uint8_t nonce[13], const uint8_t *pdu,
                                      uint32_t iv_index);

int bt_mesh_proxy_solic_init(void);

int bt_mesh_proxy_solic_deinit(void);

void bt_mesh_sar_init(void);

uint8_t bt_mesh_subnet_bridge_state_get(void);

void bt_mesh_delete_netkey_in_bridge_table(uint16_t net_idx);

bool bt_mesh_bridge_change_net_key(void *rx, const uint8_t **enc,
                                   const uint8_t **priv,
                                   uint8_t *nid, uint8_t cred);

int bt_mesh_print_subnet_bridge_table(void);

void bt_mesh_disable_directed_proxy_state(uint16_t net_idx);

void bt_mesh_disable_directed_friend_state(uint16_t net_idx);

uint8_t bt_mesh_direct_net_transmit_get(void);

uint8_t bt_mesh_private_beacon_state_get(void);

uint8_t bt_mesh_private_gatt_proxy_state_get(void);

void bt_mesh_rpr_cli_prov_complete(void *link, uint16_t index, uint16_t net_idx);

void bt_mesh_rpr_cli_node_addr_update(void *link);

int bt_mesh_rpr_srv_unprov_beacon_recv(struct net_buf_simple *buf,
                                      uint8_t adv_type,
                                      const bt_mesh_addr_t *addr,
                                      int8_t rssi);

int bt_mesh_rpr_srv_extended_scan(struct net_buf_simple *buf,
                                  const bt_mesh_addr_t *addr,
                                  bool *rp_adv);

int bt_mesh_rpr_srv_netkey_del(uint16_t del_idx);

int bt_mesh_rpr_srv_recv_link_ack(uint8_t *data, bool is_uuid);

int bt_mesh_rpr_srv_send_outbound_report(void *data, bool is_uuid);

int bt_mesh_rpr_srv_send_pdu_report(uint8_t uuid[16], uint8_t type,
                                   struct net_buf_simple *buf);

int bt_mesh_rpr_srv_store_nppi_data(const uint8_t uuid[16], const uint8_t net_key[16],
                                    uint16_t net_idx, uint8_t flags, uint32_t iv_index,
                                    uint16_t addr, const uint8_t dev_key[16]);

bool bt_mesh_rpr_srv_nppi_check(uint8_t nppi, uint8_t net_key[16],
                                uint16_t net_idx, uint32_t iv_index,
                                uint16_t addr, uint8_t *reason);

bool bt_mesh_valid_security_cred(void *tx);

void bt_mesh_choose_better_security_cred(void *tx);

void bt_mesh_model_pub_use_directed(void *tx, uint8_t directed_pub_policy);

void bt_mesh_is_directed_path_needed(void *tx);

void bt_mesh_net_adv_xmit_update(void *tx);

void bt_mesh_update_net_send_cred(void *tx, uint8_t *bearer);

int bt_mesh_directed_decrypt(void *sub, const uint8_t *data, size_t data_len,
                             void *rx, struct net_buf_simple *buf);

uint8_t bt_mesh_net_retrans_match(void *rx, uint8_t *cred, uint8_t *tag);

bool bt_mesh_dev_key_ca_valid(void);

uint8_t bt_mesh_get_sar_sis(void);

uint8_t bt_mesh_get_sar_urc(void);

uint8_t bt_mesh_get_sar_urwpc(void);

uint8_t bt_mesh_get_sar_uris(void);

uint8_t bt_mesh_get_sar_urii(void);

uint8_t bt_mesh_get_sar_mrc(void);

uint8_t bt_mesh_get_sar_mris(void);

uint8_t bt_mesh_get_sar_st(void);

uint8_t bt_mesh_get_sar_adi(void);

uint8_t bt_mesh_get_sar_arc(void);

uint8_t bt_mesh_get_sar_dt(void);

uint8_t bt_mesh_get_sar_rsis(void);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_v11_UTILS_H_ */
