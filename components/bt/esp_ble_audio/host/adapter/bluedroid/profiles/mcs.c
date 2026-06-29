/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <zephyr/autoconf.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include "bluedroid/server.h"

#include "common/host.h"

#include "../../../lib/include/audio.h"

#if CONFIG_BT_OTS
#include "ots_internal.h"
#endif /* CONFIG_BT_OTS */

LOG_MODULE_REGISTER(LEA_MCS, CONFIG_BT_ISO_LOG_LEVEL);

#if CONFIG_BT_OTS
/* GMCS includes a single secondary OTS instance. Mirrors the CAS->CSIS included-
 * secondary pattern: create the secondary before the primary so the include
 * declaration in the GMCS table resolves to OTS's live handle. */
static struct inc_svc_inst inc_ots_inst;

struct inc_svc_inst *gmcs_not_included_inst(void)
{
    if (inc_ots_inst.included == false) {
        return &inc_ots_inst;
    }

    return NULL;
}

static int bluedroid_gmcs_ots_init(void)
{
    struct bt_ots *ots;

    ots = lib_mcs_get_ots();
    if (!ots || !ots->service) {
        LOG_ERR("[B]GmcsOtsGetFail");
        return -ENODEV;
    }

    inc_ots_inst.svc_p = ots->service;
    /* Reset included before svc_init; see mics.c for rationale. */
    inc_ots_inst.included = false;

    bt_le_bluedroid_set_svc_in_progress(OTS_IN_PROGRESS);

    return bt_le_bluedroid_svc_init(ots->service);
}

static int bluedroid_gmcs_ots_start(void)
{
    bt_le_bluedroid_set_svc_in_progress(OTS_IN_PROGRESS);

    return bt_le_bluedroid_svc_start(inc_ots_inst.svc_p);
}
#endif /* CONFIG_BT_OTS */

int bt_le_bluedroid_gmcs_init(void)
{
    struct bt_gatt_service *gmcs_svc;
    int err;

    gmcs_svc = lib_gmcs_svc_get();
    if (!gmcs_svc) {
        LOG_ERR("[B]GmcsSvcGetFail");
        return -ENODEV;
    }

#if CONFIG_BT_OTS
    /* Register the included OTS secondary first so the GMCS include resolves. */
    err = bluedroid_gmcs_ots_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_OTS */

    bt_le_bluedroid_set_svc_in_progress(GMCS_IN_PROGRESS);

    err = bt_le_bluedroid_svc_init(gmcs_svc);
    if (err) {
        return err;
    }

#if CONFIG_BT_OTS
    if (gmcs_not_included_inst()) {
        LOG_ERR("[B]GmcsOtsNotInc");
        return -EIO;
    }
#endif /* CONFIG_BT_OTS */

    return 0;
}

int bt_le_bluedroid_gmcs_start(void)
{
    struct bt_gatt_service *gmcs_svc;
    int err;

    gmcs_svc = lib_gmcs_svc_get();
    if (!gmcs_svc) {
        LOG_ERR("[B]GmcsSvcGetFail");
        return -ENODEV;
    }

#if CONFIG_BT_OTS
    /* Start the included OTS secondary before GMCS (matches init order). */
    err = bluedroid_gmcs_ots_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_OTS */

    bt_le_bluedroid_set_svc_in_progress(GMCS_IN_PROGRESS);

    err = bt_le_bluedroid_svc_start(gmcs_svc);

    return err;
}

int bt_le_bluedroid_mcs_init(void)
{
    struct bt_gatt_service *list;
    int err;

    /* NULL when CONFIG_BT_MCS_INSTANCE_COUNT == 0: nothing discrete to add. */
    list = lib_mcs_server_list_get();
    if (!list) {
        LOG_DBG("[B]McsDiscreteOff");
        return 0;
    }

    bt_le_bluedroid_set_svc_in_progress(MCS_IN_PROGRESS);

    /* Signed index: count can be 0 (avoid unsigned `< 0` -Werror=type-limits). */
    for (int i = 0; i < CONFIG_BT_MCS_INSTANCE_COUNT; i++) {
#if CONFIG_BT_OTS
        /* Each discrete MCS instance also includes the single shared OTS secondary
         * (created during gmcs_init). Reset the flag so it re-resolves per instance. */
        inc_ots_inst.included = false;
#endif /* CONFIG_BT_OTS */
        err = bt_le_bluedroid_svc_init(&list[i]);
        if (err) {
            return err;
        }
    }

    return 0;
}

int bt_le_bluedroid_mcs_start(void)
{
    struct bt_gatt_service *list;
    int err;

    list = lib_mcs_server_list_get();
    if (!list) {
        return 0;
    }

    bt_le_bluedroid_set_svc_in_progress(MCS_IN_PROGRESS);

    for (int i = 0; i < CONFIG_BT_MCS_INSTANCE_COUNT; i++) {
        err = bt_le_bluedroid_svc_start(&list[i]);
        if (err) {
            return err;
        }
    }

    return 0;
}
