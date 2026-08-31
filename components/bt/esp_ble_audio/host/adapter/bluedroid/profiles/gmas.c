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

LOG_MODULE_REGISTER(LEA_GMAS, CONFIG_BT_ISO_LOG_LEVEL);

int bt_le_bluedroid_gmas_init(void)
{
    struct bt_gatt_service *gmas_svc;

    LOG_DBG("[B]GmasInit");

    gmas_svc = lib_gmas_svc_get();
    if (!gmas_svc) {
        LOG_ERR("[B]GmasSvcGetFail");
        return -ENODEV;
    }

    bt_le_bluedroid_set_svc_in_progress(GMAS_IN_PROGRESS);

    return bt_le_bluedroid_svc_init(gmas_svc);
}

int bt_le_bluedroid_gmas_start(void)
{
    struct bt_gatt_service *gmas_svc;

    LOG_DBG("[B]GmasStart");

    gmas_svc = lib_gmas_svc_get();
    if (!gmas_svc) {
        LOG_ERR("[B]GmasSvcGetFail");
        return -ENODEV;
    }

    bt_le_bluedroid_set_svc_in_progress(GMAS_IN_PROGRESS);

    return bt_le_bluedroid_svc_start(gmas_svc);
}
