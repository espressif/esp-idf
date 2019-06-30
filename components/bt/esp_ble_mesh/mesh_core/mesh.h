/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _MESH_H_
#define _MESH_H_

#define BLE_MESH_KEY_PRIMARY 0x0000
#define BLE_MESH_KEY_ANY     0xffff

#define BLE_MESH_ADDR_IS_UNICAST(addr) ((addr) && (addr) < 0x8000)
#define BLE_MESH_ADDR_IS_GROUP(addr) ((addr) >= 0xc000 && (addr) <= 0xff00)
#define BLE_MESH_ADDR_IS_VIRTUAL(addr) ((addr) >= 0x8000 && (addr) < 0xc000)
#define BLE_MESH_ADDR_IS_RFU(addr) ((addr) >= 0xff00 && (addr) <= 0xfffb)

struct bt_mesh_net;

#endif /* _MESH_H_ */
