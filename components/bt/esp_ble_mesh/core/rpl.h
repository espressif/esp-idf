/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _RPL_H_
#define _RPL_H_

#include "net.h"

#ifdef __cplusplus
extern "C" {
#endif

void bt_mesh_update_rpl(struct bt_mesh_rpl *rpl, struct bt_mesh_net_rx *rx);

bool bt_mesh_rpl_check(struct bt_mesh_net_rx *rx, struct bt_mesh_rpl **match);

void bt_mesh_rpl_update(void);

void bt_mesh_rpl_reset_single(uint16_t src, bool erase);

void bt_mesh_rpl_reset(bool erase);

#ifdef __cplusplus
}
#endif

#endif /* _RPL_H_ */
