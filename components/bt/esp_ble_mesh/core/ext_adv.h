/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _EXT_ADV_H_
#define _EXT_ADV_H_

#include "mesh/atomic.h"
#include "mesh/access.h"
#include "mesh/adapter.h"
#include "mesh/queue.h"
#include "adv_common.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void bt_mesh_adv_send(struct net_buf *buf, uint8_t xmit,
                                    const struct bt_mesh_send_cb *cb,
                                    void *cb_data)
{
    bt_mesh_generic_adv_send(buf, xmit, cb, cb_data, BLE_MESH_ADDR_UNASSIGNED, BLE_MESH_ADDR_UNASSIGNED, false);
}

void bt_mesh_adv_update(void);

void bt_mesh_adv_init(void);
void bt_mesh_adv_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _EXT_ADV_H_ */
