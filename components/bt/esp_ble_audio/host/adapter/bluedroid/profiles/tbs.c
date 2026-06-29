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

LOG_MODULE_REGISTER(LEA_TBS, CONFIG_BT_ISO_LOG_LEVEL);

int bt_le_bluedroid_gtbs_init(void)
{
    struct bt_gatt_service *gtbs_svc;

    gtbs_svc = lib_gtbs_svc_get();
    if (!gtbs_svc) {
        LOG_ERR("[B]GtbsSvcGetFail");
        return -ENODEV;
    }

    bt_le_bluedroid_set_svc_in_progress(GTBS_IN_PROGRESS);

    return bt_le_bluedroid_svc_init(gtbs_svc);
}

int bt_le_bluedroid_gtbs_start(void)
{
    struct bt_gatt_service *gtbs_svc;

    gtbs_svc = lib_gtbs_svc_get();
    if (!gtbs_svc) {
        LOG_ERR("[B]GtbsSvcGetFail");
        return -ENODEV;
    }

    bt_le_bluedroid_set_svc_in_progress(GTBS_IN_PROGRESS);

    return bt_le_bluedroid_svc_start(gtbs_svc);
}

int bt_le_bluedroid_tbs_init(void)
{
    struct bt_gatt_service *tbs_list;
    int err;

    /* NULL when BEARER_COUNT==0 (GTBS-only build): nothing discrete to init. */
    tbs_list = lib_tbs_server_list_get();
    if (!tbs_list) {
        return 0;
    }

    bt_le_bluedroid_set_svc_in_progress(TBS_IN_PROGRESS);

    /* Signed index: BEARER_COUNT can be 0 (GTBS-only), where an unsigned loop var
     * would trip -Werror=type-limits. */
    for (int i = 0; i < CONFIG_BT_TBS_BEARER_COUNT; i++) {
        /* Called once per bearer registration; svc_init assigns attrs[0].handle.
           Skip empty (unregistered) and already-added slots so a later
           registration doesn't re-create earlier bearers' services. */
        if (tbs_list[i].attr_count == 0 || tbs_list[i].attrs[0].handle != 0) {
            continue;
        }

        err = bt_le_bluedroid_svc_init(&tbs_list[i]);
        if (err) {
            return err;
        }
    }

    return 0;
}

int bt_le_bluedroid_tbs_start(void)
{
    struct bt_gatt_service *tbs_list;
    int err;

    /* NULL when BEARER_COUNT==0 (GTBS-only build): nothing discrete to start. */
    tbs_list = lib_tbs_server_list_get();
    if (!tbs_list) {
        return 0;
    }

    bt_le_bluedroid_set_svc_in_progress(TBS_IN_PROGRESS);

    for (int i = 0; i < CONFIG_BT_TBS_BEARER_COUNT; i++) {
        err = bt_le_bluedroid_svc_start(&tbs_list[i]);
        if (err) {
            return err;
        }
    }

    return 0;
}
