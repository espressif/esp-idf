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

LOG_MODULE_REGISTER(LEA_CAS, CONFIG_BT_ISO_LOG_LEVEL);

#if CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER
static struct inc_svc_inst inc_csis_inst;

struct inc_svc_inst *cas_not_included_inst(void)
{
    if (inc_csis_inst.included == false) {
        return &inc_csis_inst;
    }

    return NULL;
}
#endif /* CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER */

int bt_le_bluedroid_cas_init(void *csis_svc_p)
{
    struct bt_gatt_service *cas_svc;
    int err = 0;

    LOG_DBG("[B]CasInit");

#if CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER
    /* csis_svc_p is non-NULL in practice (SET_MEMBER depends on CSIP_SET_MEMBER,
     * which populates it), but guard for parity with the nimble adapter. With
     * no CSIS, mark included=true so cas_not_included_inst() skips it and
     * svc_init never feeds a NULL svc_p into BTA_GATTS_AddIncludeService. */
    if (csis_svc_p) {
        /* The instance of CSIS is included by CAS */
        inc_csis_inst.svc_p = csis_svc_p;
        /* Reset included before svc_init; see mics.c for rationale. */
        inc_csis_inst.included = false;
    } else {
        inc_csis_inst.included = true;
    }
#else /* CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER */
    ARG_UNUSED(csis_svc_p);
#endif /* CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER */

    cas_svc = lib_cas_svc_get();
    if (!cas_svc) {
        LOG_ERR("[B]CasSvcGetFail");
        return -ENODEV;
    }

    bt_le_bluedroid_set_svc_in_progress(CAS_IN_PROGRESS);

    err = bt_le_bluedroid_svc_init(cas_svc);
    if (err) {
        return err;
    }

#if CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER
    if (cas_not_included_inst()) {
        LOG_ERR("[B]CasCsisNotInc");
        return -EIO;
    }
#else /* CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER */
    /* Insert CAS to the GATT db list */
    err = bt_gatt_service_register_safe(cas_svc);
    if (err) {
        LOG_ERR("[B]CasSvcRegFail[%d]", err);
    }
#endif /* CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER */

    return err;
}

int bt_le_bluedroid_cas_start(void)
{
    struct bt_gatt_service *cas_svc;

    LOG_DBG("[B]CasStart");

    cas_svc = lib_cas_svc_get();
    if (!cas_svc) {
        LOG_ERR("[B]CasSvcGetFail");
        return -ENODEV;
    }

    bt_le_bluedroid_set_svc_in_progress(CAS_IN_PROGRESS);

    return bt_le_bluedroid_svc_start(cas_svc);
}
