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

#include <../host/conn_internal.h>

#include "host/ble_gatt.h"

#include "common/host.h"

#include "../../../lib/include/audio.h"

LOG_MODULE_REGISTER(LEA_CAS, CONFIG_BT_ISO_LOG_LEVEL);

#if CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER
extern struct ble_gatt_svc_def *cas_get_included_csis(void *csis_svc_p);

static struct bt_gatt_service *inc_csis_svc;

static struct ble_gatt_svc_def *cas_inc_svcs[] = {
    NULL,
    NULL,
};
#endif /* CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER */

static struct ble_gatt_svc_def gatt_svc_cas[] = {
    {
        /* Published Audio Capabilities Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BT_UUID_CAS_VAL),
        .includes = NULL,
        .characteristics = NULL,
    },
    {
        0, /* No more services. */
    },
};

int bt_le_nimble_cas_attr_handle_set(void)
{
    struct bt_gatt_service *cas_svc;
    uint16_t handle;
    int rc;

    cas_svc = lib_cas_svc_get();
    assert(cas_svc);

    LOG_DBG("[N]CasAttrHdlSet[%u]", cas_svc->attr_count);

    rc = ble_gatts_find_svc(BLE_UUID16_DECLARE(BT_UUID_CAS_VAL), &handle);
    if (rc) {
        LOG_ERR("[N]CasNotFound[%d]", rc);
        return rc;
    }

    LOG_DBG("[N]Hdl[%u]", handle);

    for (size_t i = 0; i < cas_svc->attr_count; i++) {
        (cas_svc->attrs + i)->handle = handle + i;
    }

    return 0;
}

int bt_le_nimble_cas_init(void *csis_svc_p)
{
    int rc;

    LOG_DBG("[N]CasInit[%p]", csis_svc_p);

#if CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER
    /* The instance of CSIS is included by CAS */
    if (csis_svc_p) {
        cas_inc_svcs[0] = cas_get_included_csis(csis_svc_p);
        if (cas_inc_svcs[0] == NULL) {
            LOG_ERR("[N]CasIncCsisNotFound");
            return -1;
        }

        /* Terminate the included service array with NULL */
        cas_inc_svcs[1] = NULL;

        gatt_svc_cas[0].includes = (const struct ble_gatt_svc_def **)cas_inc_svcs;

        inc_csis_svc = csis_svc_p;
    }
#else /* CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER */
    ARG_UNUSED(csis_svc_p);
#endif /* CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER */

    rc = ble_gatts_count_cfg(gatt_svc_cas);
    if (rc) {
        LOG_ERR("[N]CasCountCfgFail[%d]", rc);
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svc_cas);
    if (rc) {
        LOG_ERR("[N]CasAddSvcsFail[%d]", rc);
        return rc;
    }

#if !CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER
    struct bt_gatt_service *cas_svc;

    cas_svc = lib_cas_svc_get();
    assert(cas_svc);

    /* Insert CAS to the GATT db list */
    rc = bt_gatt_service_register_safe(cas_svc);
    if (rc) {
        LOG_ERR("[N]CasSvcRegFail[%d]", rc);
    }
#endif /* !CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER */

    return rc;
}
