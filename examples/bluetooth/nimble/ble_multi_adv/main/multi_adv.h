/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef H_BLE_MULTI_ADV_
#define H_BLE_MULTI_ADV_

#include <stdbool.h>
#include "nimble/ble.h"
#include "modlog/modlog.h"
#include "esp_peripheral.h"
#ifdef __cplusplus
extern "C" {
#endif

struct ble_hs_cfg;
struct ble_gatt_register_ctxt;

typedef int ble_instance_cb_fn(uint16_t instance);

struct ble_instance_cb_register {
    ble_addr_t addr;
    ble_instance_cb_fn *cb;
};

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
int gatt_svr_init(void);

#ifdef __cplusplus
}
#endif

#endif
