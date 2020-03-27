/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_TEST_H_
#define _BLE_MESH_TEST_H_

#include "mesh_bearer_adapt.h"

int bt_mesh_test(void);

struct bt_mesh_device_network_info {
    u8_t  net_key[16];
    u16_t net_idx;
    u8_t  flags;
    u32_t iv_index;
    u16_t unicast_addr;
    u8_t  dev_key[16];
    u8_t  app_key[16];
    u16_t app_idx;
    u16_t group_addr;
};

int bt_mesh_device_auto_enter_network(struct bt_mesh_device_network_info *info);

/* Before trying to update the white list, users need to make sure that
 * one of the following conditions is satisfied:
 * 1. BLE scanning is disabled;
 * 2. BLE scanning is enabled with scan filter policy disabled;
 * If BLE scanning is enabled with scan filter policy enabled, users need
 * to stop BLE scanning firstly, then the white list can be updated.
 */
int bt_mesh_test_update_white_list(struct bt_mesh_white_list *wl);

int bt_mesh_test_start_scanning(bool wl_en);

int bt_mesh_test_stop_scanning(void);

#endif /* _BLE_MESH_TEST_H_ */
