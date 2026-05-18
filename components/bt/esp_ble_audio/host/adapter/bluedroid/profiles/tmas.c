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

LOG_MODULE_REGISTER(LEA_TMAS, CONFIG_BT_ISO_LOG_LEVEL);

int bt_le_bluedroid_tmas_init(void)
{
    struct bt_gatt_service *tmas_svc;

    LOG_DBG("[B]TmasInit");

    tmas_svc = lib_tmas_svc_get();
    if (!tmas_svc) {
        LOG_ERR("[B]TmasSvcGetFail");
        return -ENODEV;
    }

    bt_le_bluedroid_set_svc_in_progress(TMAS_IN_PROGRESS);

    return bt_le_bluedroid_svc_init(tmas_svc);
}

int bt_le_bluedroid_tmas_start(void)
{
    struct bt_gatt_service *tmas_svc;

    LOG_DBG("[B]TmasStart");

    tmas_svc = lib_tmas_svc_get();
    if (!tmas_svc) {
        LOG_ERR("[B]TmasSvcGetFail");
        return -ENODEV;
    }

    bt_le_bluedroid_set_svc_in_progress(TMAS_IN_PROGRESS);

    return bt_le_bluedroid_svc_start(tmas_svc);
}
