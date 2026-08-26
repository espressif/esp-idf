/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef H_PHY_PRPH_
#define H_PHY_PRPH_

#include <stdbool.h>
#include "nimble/ble.h"
#include "modlog/modlog.h"
#include "esp_peripheral.h"
#ifdef __cplusplus
extern "C" {
#endif

struct ble_hs_cfg;
struct ble_gatt_register_ctxt;

/** Making sure client connects to server having LE PHY UUID */
#define LE_PHY_UUID16               0xABF2
#define LE_PHY_CHR_UUID16           0xABF3

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
int gatt_svr_init_le_phy(void);

#ifdef __cplusplus
}
#endif

#endif
