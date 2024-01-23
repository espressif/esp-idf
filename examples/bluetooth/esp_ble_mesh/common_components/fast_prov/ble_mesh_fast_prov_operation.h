/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_FAST_PROV_OPERATION_H_
#define _BLE_MESH_FAST_PROV_OPERATION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ble_mesh_fast_prov_common.h"

esp_err_t example_store_node_info(const uint8_t uuid[16], uint16_t node_addr,
                                  uint8_t elem_num, uint16_t net_idx,
                                  uint16_t app_idx, uint8_t onoff);

example_node_info_t *example_get_node_info(uint16_t node_addr);

bool example_is_node_exist(const uint8_t uuid[16]);

uint16_t example_get_node_address(int node_idx);

esp_ble_mesh_model_t *example_find_model(uint16_t element_addr, uint16_t model_id,
        uint16_t company_id);

esp_err_t example_handle_config_app_key_add_evt(uint16_t app_idx);

esp_err_t example_add_fast_prov_group_address(uint16_t model_id, uint16_t group_addr);

esp_err_t example_delete_fast_prov_group_address(uint16_t model_id, uint16_t group_addr);

esp_err_t example_send_config_appkey_add(esp_ble_mesh_model_t *model,
        example_msg_common_info_t *info,
        esp_ble_mesh_cfg_app_key_add_t *add_key);

esp_err_t example_send_generic_onoff_get(esp_ble_mesh_model_t *model,
        example_msg_common_info_t *info);

esp_err_t example_send_generic_onoff_set(esp_ble_mesh_model_t *model,
        example_msg_common_info_t *info,
        uint8_t onoff, uint8_t tid, bool need_ack);

esp_err_t example_send_fast_prov_info_set(esp_ble_mesh_model_t *model,
        example_msg_common_info_t *info,
        example_fast_prov_info_set_t *set);

esp_err_t example_send_fast_prov_net_key_add(esp_ble_mesh_model_t *model,
        example_msg_common_info_t *info,
        uint8_t net_key[16]);

esp_err_t example_send_fast_prov_self_prov_node_addr(esp_ble_mesh_model_t *model,
        example_msg_common_info_t *info,
        struct net_buf_simple *node_addr);

esp_err_t example_send_fast_prov_all_node_addr_get(esp_ble_mesh_model_t *model,
        example_msg_common_info_t *info);

esp_err_t example_send_fast_prov_status_msg(esp_ble_mesh_model_t *model,
        esp_ble_mesh_msg_ctx_t *ctx,
        uint32_t opcode, struct net_buf_simple *msg);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_FAST_PROV_OPERATION_H_ */
