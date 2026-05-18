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
#include <zephyr/bluetooth/audio/micp.h>

#include "bluedroid/server.h"

#include "common/host.h"

#include "../../../lib/include/audio.h"

LOG_MODULE_REGISTER(LEA_MICS, CONFIG_BT_ISO_LOG_LEVEL);

#define AICS_INST_COUNT     CONFIG_BT_MICP_MIC_DEV_AICS_INSTANCE_COUNT

static uint8_t inc_aics_svc_count;

static struct inc_svc_inst inc_aics_insts[AICS_INST_COUNT];

struct inc_svc_inst *mics_not_included_inst(void)
{
    for (size_t i = 0; i < inc_aics_svc_count; i++) {
        if (inc_aics_insts[i].included == false) {
            return &inc_aics_insts[i];
        }
    }

    return NULL;
}

int bt_le_bluedroid_mics_init(void *micp_inc)
{
    struct bt_micp_included *micp_included;
    struct bt_gatt_service *mics_svc;
    int err;

    LOG_DBG("[B]MicsInit");

    mics_svc = lib_mics_svc_get();
    if (!mics_svc) {
        LOG_ERR("[B]MicsSvcGetFail");
        return -ENODEV;
    }

    /* Reset before the NULL check so a re-init with micp_included == NULL
     * doesn't iterate over stale entries from a previous non-NULL call. */
    inc_aics_svc_count = 0;

    micp_included = micp_inc;

    if (micp_included) {
        if (micp_included->aics_cnt > AICS_INST_COUNT) {
            return -EINVAL;
        }

        bt_le_bluedroid_set_svc_in_progress(AICS_IN_PROGRESS);

        /* MICS may include zero or more instances of AICS */
        for (size_t i = 0; i < micp_included->aics_cnt; i++) {
            inc_aics_insts[i].svc_p = lib_aics_svc_get(micp_included->aics[i]);
            if (!inc_aics_insts[i].svc_p) {
                LOG_ERR("[B]AicsSvcGetFail[%u]", i);
                return -ENODEV;
            }

            /* Reset included before svc_init: server.c sets it to true when
             * the include attribute is processed. Without this reset, a
             * re-init where the include attribute is missing would leave
             * the stale true from a previous init and mics_not_included_inst()
             * would falsely report all instances as included. */
            inc_aics_insts[i].included = false;

            err = bt_le_bluedroid_svc_init(inc_aics_insts[i].svc_p);
            if (err) {
                return err;
            }
        }

        /* Commit the count only after every instance initialized — a mid-loop
         * failure above leaves it at 0 (reset on entry) so mics_start() never
         * iterates partially-initialized entries. */
        inc_aics_svc_count = micp_included->aics_cnt;
    }

    bt_le_bluedroid_set_svc_in_progress(MICS_IN_PROGRESS);

    err = bt_le_bluedroid_svc_init(mics_svc);
    if (err) {
        return err;
    }

    if (mics_not_included_inst()) {
        LOG_ERR("[B]MicsAicsNotAllInc");
        return -EIO;
    }

    return 0;
}

int bt_le_bluedroid_mics_start(void)
{
    struct bt_gatt_service *mics_svc;
    int err;

    LOG_DBG("[B]MicsStart");

    mics_svc = lib_mics_svc_get();
    if (!mics_svc) {
        LOG_ERR("[B]MicsSvcGetFail");
        return -ENODEV;
    }

    bt_le_bluedroid_set_svc_in_progress(AICS_IN_PROGRESS);

    for (size_t i = 0; i < inc_aics_svc_count; i++) {
        err = bt_le_bluedroid_svc_start(inc_aics_insts[i].svc_p);
        if (err) {
            return err;
        }
    }

    bt_le_bluedroid_set_svc_in_progress(MICS_IN_PROGRESS);

    return bt_le_bluedroid_svc_start(mics_svc);
}
