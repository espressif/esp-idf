/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SCAN_H_
#define _SCAN_H_

#include "mesh/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_MESH_STORE_UNPROV_INFO_MAX_NUM              10
#define BLE_MESH_STORE_UNPROV_INFO_QUERY_TYPE_ADDR      0
#define BLE_MESH_STORE_UNPROV_INFO_QUERY_TYPE_UUID      BIT(0)
#define BLE_MESH_STORE_UNPROV_INFO_QUERY_TYPE_EXISTS    BIT(1)

const bt_mesh_addr_t *bt_mesh_get_unprov_dev_addr(void);
uint8_t bt_mesh_get_adv_type(void);

int bt_mesh_unprov_dev_fifo_enqueue(uint8_t uuid[16], const uint8_t addr[6], uint8_t adv_type);

int bt_mesh_unprov_dev_info_query(uint8_t uuid[16], uint8_t addr[6],
                                  uint8_t *adv_type, uint8_t query_type);

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
