/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_ADV_H_
#define _BLE_ADV_H_

#include "mesh/atomic.h"
#include "mesh/access.h"
#include "mesh/adapter.h"
#include "mesh/utils.h"
#include "adv_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV

enum {
    TIMER_INIT,  /* Resend timer is initialized */
    NUM_FLAGS,
};

struct bt_mesh_ble_adv_tx {
    struct bt_mesh_ble_adv_param param;
    struct net_buf              *buf;
    struct k_delayed_work        resend;
    BLE_MESH_ATOMIC_DEFINE(flags, NUM_FLAGS);
};

int bt_mesh_start_ble_advertising(const struct bt_mesh_ble_adv_param *param,
                                  const struct bt_mesh_ble_adv_data *data, uint8_t *index);

int bt_mesh_stop_ble_advertising(uint8_t index);

void bt_mesh_ble_adv_init(void);

#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_ble_adv_deinit(void);
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif /* _BLE_ADV_H_ */
