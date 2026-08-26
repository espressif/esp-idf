/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _HEARTBEAT_H_
#define _HEARTBEAT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void bt_mesh_set_hb_sub_dst(uint16_t addr);

uint16_t bt_mesh_get_hb_sub_dst(void);

void bt_mesh_heartbeat_recv(uint16_t src, uint16_t dst, uint8_t hops, uint16_t feat);

void bt_mesh_heartbeat_send(void);

typedef void (* bt_mesh_pvnr_hb_recv_cb_t)(uint16_t hb_src, uint16_t hb_dst,
                                           uint8_t init_ttl, uint8_t rx_ttl,
                                           uint8_t hops, uint16_t feat, int8_t rssi);

int bt_mesh_pvnr_register_hb_recv_cb(bt_mesh_pvnr_hb_recv_cb_t cb);

int bt_mesh_pvnr_set_hb_recv_filter_type(uint8_t filter_type);

int bt_mesh_pvnr_set_hb_recv_filter_info(uint8_t op, uint16_t src, uint16_t dst);

void bt_mesh_pvnr_heartbeat_recv(uint16_t hb_src, uint16_t hb_dst,
                                 uint8_t init_ttl, uint8_t rx_ttl,
                                 uint8_t hops, uint16_t feat, int8_t rssi);

#ifdef __cplusplus
}
#endif

#endif /* _HEARTBEAT_H_ */
