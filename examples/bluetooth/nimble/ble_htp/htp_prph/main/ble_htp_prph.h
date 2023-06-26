/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef H_BLE_HTP_PRPH_
#define H_BLE_HTP_PRPH_

#include "nimble/ble.h"
#include "modlog/modlog.h"
#include "services/htp/ble_svc_htp.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 16 Bit Device Information Service Characteristic UUIDs */
#define GATT_DIS_DEVICE_INFO_UUID                         0x180A
#define GATT_DIS_CHR_UUID16_SYS_ID                        0x2A23
#define GATT_DIS_CHR_UUID16_MODEL_NO                      0x2A24
#define GATT_DIS_CHR_UUID16_MFC_NAME                      0x2A29

struct ble_hs_cfg;
struct ble_gatt_register_ctxt;

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
int gatt_svr_init(void);

#ifdef __cplusplus
}
#endif

#endif
