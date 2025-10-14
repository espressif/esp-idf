/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ADV_H_
#define _ADV_H_

#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
#include "ext_adv.h"
#else /* CONFIG_BLE_MESH_SUPPORT_MULTI_ADV */
#include "mesh/atomic.h"
#include "mesh/access.h"
#include "mesh/adapter.h"
#include "mesh/utils.h"
#include "adv_common.h"

#ifdef __cplusplus
extern "C" {
#endif

void bt_mesh_adv_send(struct net_buf *buf, uint8_t xmit,
                      const struct bt_mesh_send_cb *cb,
                      void *cb_data);

void bt_mesh_adv_update(void);

void bt_mesh_adv_init(void);
void bt_mesh_adv_deinit(void);

#ifdef __cplusplus
}
#endif
#endif /* CONFIG_BLE_MESH_SUPPORT_MULTI_ADV */
#endif /* _ADV_H_ */
