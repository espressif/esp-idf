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

LOG_MODULE_REGISTER(LEA_ASCS, CONFIG_BT_ISO_LOG_LEVEL);

int bt_le_bluedroid_ascs_init(void)
{
    struct bt_gatt_service *ascs_svc;

    LOG_DBG("[B]AscsInit");

    ascs_svc = lib_ascs_svc_get();
    if (!ascs_svc) {
        LOG_ERR("[B]AscsSvcGetFail");
        return -ENODEV;
    }

    bt_le_bluedroid_set_svc_in_progress(ASCS_IN_PROGRESS);

    return bt_le_bluedroid_svc_init(ascs_svc);
}

int bt_le_bluedroid_ascs_start(void)
{
    struct bt_gatt_service *ascs_svc;

    LOG_DBG("[B]AscsStart");

    ascs_svc = lib_ascs_svc_get();
    if (!ascs_svc) {
        LOG_ERR("[B]AscsSvcGetFail");
        return -ENODEV;
    }

    bt_le_bluedroid_set_svc_in_progress(ASCS_IN_PROGRESS);

    return bt_le_bluedroid_svc_start(ascs_svc);
}
