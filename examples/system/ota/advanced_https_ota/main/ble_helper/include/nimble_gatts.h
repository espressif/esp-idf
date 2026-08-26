/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef H_NIMBLE_GATTS_
#define H_NIMBLE_GATTS_

#include "sdkconfig.h"
#if CONFIG_BT_NIMBLE_ENABLED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "nimble/ble.h"
#include "modlog/modlog.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"

struct ble_hs_cfg;
struct ble_gatt_register_ctxt;

int bleprph_gap_event(struct ble_gap_event *event, void *arg);

/** GATT server. */
#define GATT_SVR_SVC_ALERT_UUID               0x1811
#define GATT_SVR_CHR_SUP_NEW_ALERT_CAT_UUID   0x2A47
#define GATT_SVR_CHR_NEW_ALERT                0x2A46
#define GATT_SVR_CHR_SUP_UNR_ALERT_CAT_UUID   0x2A48
#define GATT_SVR_CHR_UNR_ALERT_STAT_UUID      0x2A45
#define GATT_SVR_CHR_ALERT_NOT_CTRL_PT        0x2A44

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
int nimble_gatt_svr_init(void);
void ble_store_config_init(void);
void print_addr(const void *addr);
void bleprph_print_conn_desc(struct ble_gap_conn_desc *desc);
void bleprph_advertise(void);
int bleprph_gap_event(struct ble_gap_event *event, void *arg);
void bleprph_host_task(void *param);
void bleprph_on_reset(int reason);
void bleprph_on_sync(void);

#ifdef __cplusplus
}
#endif

#endif

#endif
