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

LOG_MODULE_REGISTER(LEA_CSIS, CONFIG_BT_ISO_LOG_LEVEL);

#define CSIS_SVC_COUNT      CONFIG_BT_CSIP_SET_MEMBER_MAX_INSTANCE_COUNT

static struct csis_inst {
    struct bt_gatt_service *svc_p;
} csis_insts[CSIS_SVC_COUNT];

static uint8_t csis_svc_count;

int bt_le_bluedroid_csis_init(void *svc, uint8_t count)
{
    int err;

    LOG_DBG("[B]CsisInit");

    if (count > CSIS_SVC_COUNT) {
        return -1;
    }

    bt_le_bluedroid_set_svc_in_progress(CSIS_IN_PROGRESS);

    for (size_t i = 0; i < count; i++) {
        csis_insts[i].svc_p = ((struct bt_gatt_service **)svc)[i];

        err = bt_le_bluedroid_svc_init(csis_insts[i].svc_p);
        if (err) {
            return err;
        }
    }

    /* Commit the count only after the loop succeeds, so a mid-loop failure
     * doesn't leave csis_start() iterating uninitialized entries. */
    csis_svc_count = count;

    return 0;
}

int bt_le_bluedroid_csis_start(void)
{
    int err;

    LOG_DBG("[B]CsisStart");

    bt_le_bluedroid_set_svc_in_progress(CSIS_IN_PROGRESS);

    for (size_t i = 0; i < csis_svc_count; i++) {
        err = bt_le_bluedroid_svc_start(csis_insts[i].svc_p);
        if (err) {
            return err;
        }
    }

    return 0;
}
