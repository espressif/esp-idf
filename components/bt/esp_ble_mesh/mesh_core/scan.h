/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2020 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SCAN_H_
#define _SCAN_H_

#include "mesh_bearer_adapt.h"

#ifdef __cplusplus
extern "C" {
#endif

const bt_mesh_addr_t *bt_mesh_get_unprov_dev_addr(void);

int bt_mesh_scan_enable(void);

int bt_mesh_scan_disable(void);

int bt_mesh_scan_with_wl_enable(void);

struct bt_mesh_ble_scan_param {
    uint32_t duration;
};

int bt_mesh_start_ble_scan(struct bt_mesh_ble_scan_param *param);

int bt_mesh_stop_ble_scan(void);

#ifdef __cplusplus
}
#endif

#endif /* _SCAN_H_ */
