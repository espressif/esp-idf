/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _MESH_H_
#define _MESH_H_

#include "net.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_MESH_KEY_PRIMARY                0x0000
#define BLE_MESH_KEY_ANY                    0xFFFF

#define BLE_MESH_ADDR_IS_UNICAST(addr)      ((addr) && (addr) < 0x8000)
#define BLE_MESH_ADDR_IS_VIRTUAL(addr)      ((addr) >= 0x8000 && (addr) < 0xC000)
#define BLE_MESH_ADDR_IS_GROUP(addr)        ((addr) >= 0xC000 && (addr) < 0xFF00)
/* Currently not including all-ipt-nodes & all-ipt-border-routers in RFU & fixed group */
#define BLE_MESH_ADDR_IS_RFU(addr)          ((addr) >= 0xFF00 && (addr) <= 0xFFFA)
#define BLE_MESH_ADDR_IS_FIXED_GROUP(addr)  ((addr) >= 0xFFFB && ((addr) < 0xFFFF || (addr) == 0xFFFF))

#define BLE_MESH_SUPPORTING_TYPE_FRIEND     1
#define BLE_MESH_SUPPORTING_TYPE_PROXY      2
#define BLE_MESH_SUPPORTING_TYPE_SBR        3

struct bt_mesh_net;

#ifdef __cplusplus
}
#endif

#endif /* _MESH_H_ */
