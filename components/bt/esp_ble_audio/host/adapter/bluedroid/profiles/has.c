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

LOG_MODULE_REGISTER(LEA_HAS, CONFIG_BT_ISO_LOG_LEVEL);

int bt_le_bluedroid_has_init(void)
{
    struct bt_gatt_service *has_svc;

    LOG_DBG("[B]HasInit");

    has_svc = lib_has_svc_get();
    if (!has_svc) {
        LOG_ERR("[B]HasSvcGetFail");
        return -ENODEV;
    }

    bt_le_bluedroid_set_svc_in_progress(HAS_IN_PROGRESS);

    return bt_le_bluedroid_svc_init(has_svc);
}

int bt_le_bluedroid_has_start(void)
{
    struct bt_gatt_service *has_svc;

    LOG_DBG("[B]HasStart");

    has_svc = lib_has_svc_get();
    if (!has_svc) {
        LOG_ERR("[B]HasSvcGetFail");
        return -ENODEV;
    }

    bt_le_bluedroid_set_svc_in_progress(HAS_IN_PROGRESS);

    return bt_le_bluedroid_svc_start(has_svc);
}
