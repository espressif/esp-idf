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
#include <zephyr/bluetooth/audio/vcp.h>

#include "bluedroid/server.h"

#include "common/host.h"

#include "../../../lib/include/audio.h"

LOG_MODULE_REGISTER(LEA_VCS, CONFIG_BT_ISO_LOG_LEVEL);

#define VOCS_INST_COUNT     CONFIG_BT_VCP_VOL_REND_VOCS_INSTANCE_COUNT
#define AICS_INST_COUNT     CONFIG_BT_VCP_VOL_REND_AICS_INSTANCE_COUNT

static uint8_t inc_vocs_svc_count;
static uint8_t inc_aics_svc_count;

static struct inc_svc_inst inc_vocs_insts[VOCS_INST_COUNT];
static struct inc_svc_inst inc_aics_insts[AICS_INST_COUNT];

struct inc_svc_inst *vcs_not_included_inst(void)
{
    for (size_t i = 0; i < inc_vocs_svc_count; i++) {
        if (inc_vocs_insts[i].included == false) {
            return &inc_vocs_insts[i];
        }
    }

    for (size_t i = 0; i < inc_aics_svc_count; i++) {
        if (inc_aics_insts[i].included == false) {
            return &inc_aics_insts[i];
        }
    }

    return NULL;
}

int bt_le_bluedroid_vcs_init(void *vcp_inc)
{
    struct bt_vcp_included *vcp_included;
    struct bt_gatt_service *vcs_svc;
    int err;

    LOG_DBG("[B]VcsInit");

    vcs_svc = lib_vcs_svc_get();
    if (!vcs_svc) {
        LOG_ERR("[B]VcsSvcGetFail");
        return -ENODEV;
    }

    /* Reset before the NULL check so a re-init with vcp_included == NULL
     * doesn't iterate over stale entries from a previous non-NULL call. */
    inc_vocs_svc_count = 0;
    inc_aics_svc_count = 0;

    vcp_included = vcp_inc;

    if (vcp_included) {
        if (vcp_included->vocs_cnt > VOCS_INST_COUNT ||
                vcp_included->aics_cnt > AICS_INST_COUNT) {
            return -EINVAL;
        }

        inc_vocs_svc_count = vcp_included->vocs_cnt;
        inc_aics_svc_count = vcp_included->aics_cnt;

        bt_le_bluedroid_set_svc_in_progress(VOCS_IN_PROGRESS);

        /* VCS may include zero or more instances of VOCS */
        for (size_t i = 0; i < inc_vocs_svc_count; i++) {
            inc_vocs_insts[i].svc_p = lib_vocs_svc_get(vcp_included->vocs[i]);
            if (!inc_vocs_insts[i].svc_p) {
                LOG_ERR("[B]VocsSvcGetFail[%u]", i);
                return -ENODEV;
            }

            /* Reset included before svc_init; see mics.c for rationale. */
            inc_vocs_insts[i].included = false;

            err = bt_le_bluedroid_svc_init(inc_vocs_insts[i].svc_p);
            if (err) {
                return err;
            }
        }

        bt_le_bluedroid_set_svc_in_progress(AICS_IN_PROGRESS);

        /* VCS may include zero or more instances of AICS */
        for (size_t i = 0; i < inc_aics_svc_count; i++) {
            inc_aics_insts[i].svc_p = lib_aics_svc_get(vcp_included->aics[i]);
            if (!inc_aics_insts[i].svc_p) {
                LOG_ERR("[B]AicsSvcGetFail[%u]", i);
                return -ENODEV;
            }

            /* Reset included before svc_init; see mics.c for rationale. */
            inc_aics_insts[i].included = false;

            err = bt_le_bluedroid_svc_init(inc_aics_insts[i].svc_p);
            if (err) {
                return err;
            }
        }
    }

    bt_le_bluedroid_set_svc_in_progress(VCS_IN_PROGRESS);

    err = bt_le_bluedroid_svc_init(vcs_svc);
    if (err) {
        return err;
    }

    if (vcs_not_included_inst()) {
        LOG_ERR("[B]VcsVocsAicsNotAllInc");
        return -EIO;
    }

    return 0;
}

int bt_le_bluedroid_vcs_start(void)
{
    struct bt_gatt_service *vcs_svc;
    int err;

    LOG_DBG("[B]VcsStart");

    vcs_svc = lib_vcs_svc_get();
    if (!vcs_svc) {
        LOG_ERR("[B]VcsSvcGetFail");
        return -ENODEV;
    }

    bt_le_bluedroid_set_svc_in_progress(VOCS_IN_PROGRESS);

    for (size_t i = 0; i < inc_vocs_svc_count; i++) {
        err = bt_le_bluedroid_svc_start(inc_vocs_insts[i].svc_p);
        if (err) {
            return err;
        }
    }

    bt_le_bluedroid_set_svc_in_progress(AICS_IN_PROGRESS);

    for (size_t i = 0; i < inc_aics_svc_count; i++) {
        err = bt_le_bluedroid_svc_start(inc_aics_insts[i].svc_p);
        if (err) {
            return err;
        }
    }

    bt_le_bluedroid_set_svc_in_progress(VCS_IN_PROGRESS);

    return bt_le_bluedroid_svc_start(vcs_svc);
}
