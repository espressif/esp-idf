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

LOG_MODULE_REGISTER(LEA_MCS, CONFIG_BT_ISO_LOG_LEVEL);

int bt_le_bluedroid_gmcs_init(void)
{
    struct bt_gatt_service *gmcs_svc;

    gmcs_svc = lib_mcs_svc_get();
    if (!gmcs_svc) {
        LOG_ERR("[B]GmcsSvcGetFail");
        return -ENODEV;
    }

    bt_le_bluedroid_set_svc_in_progress(GMCS_IN_PROGRESS);

    return bt_le_bluedroid_svc_init(gmcs_svc);
}

int bt_le_bluedroid_gmcs_start(void)
{
    struct bt_gatt_service *gmcs_svc;

    gmcs_svc = lib_mcs_svc_get();
    if (!gmcs_svc) {
        LOG_ERR("[B]GmcsSvcGetFail");
        return -ENODEV;
    }

    bt_le_bluedroid_set_svc_in_progress(GMCS_IN_PROGRESS);

    return bt_le_bluedroid_svc_start(gmcs_svc);
}
