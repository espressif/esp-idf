/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_TEST_H_
#define _BLE_MESH_TEST_H_

#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include "mesh_buf.h"
#include "sdkconfig.h"

#if defined(CONFIG_BLE_MESH_SELF_TEST)
int bt_mesh_test(void);
#else
static inline int bt_mesh_test(void)
{
    return 0;
}
#endif

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

#endif /* _BLE_MESH_TEST_H_ */
