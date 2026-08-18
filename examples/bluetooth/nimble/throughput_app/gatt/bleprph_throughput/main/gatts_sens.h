/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef H_GATTS_SENS_
#define H_GATTS_SENS_

#include "nimble/ble.h"
#include "modlog/modlog.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uint16_t notify_handle;

/* One byte command written by the central to THRPT_CHR_READ_WRITE. Values
 * 0-3 select the PHY, this one asks the notify test to stop sending. */
#define THRPT_CMD_STOP_NOTIFY   0xF0

struct ble_hs_cfg;
struct ble_gatt_register_ctxt;

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
int gatt_svr_init(void);
void bleprph_notify_stop_req(void);

#ifdef __cplusplus
}
#endif

#endif
