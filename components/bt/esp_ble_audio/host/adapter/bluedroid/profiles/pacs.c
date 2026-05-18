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

LOG_MODULE_REGISTER(LEA_PACS, CONFIG_BT_ISO_LOG_LEVEL);

int bt_le_bluedroid_pacs_init(void)
{
    struct bt_gatt_service *pacs_svc;

    LOG_DBG("[B]PacsInit");

    pacs_svc = lib_pacs_svc_get();
    if (!pacs_svc) {
        LOG_ERR("[B]PacsSvcGetFail");
        return -ENODEV;
    }

    bt_le_bluedroid_set_svc_in_progress(PACS_IN_PROGRESS);

    return bt_le_bluedroid_svc_init(pacs_svc);
}

int bt_le_bluedroid_pacs_start(void)
{
    struct bt_gatt_service *pacs_svc;

    LOG_DBG("[B]PacsStart");

    pacs_svc = lib_pacs_svc_get();
    if (!pacs_svc) {
        LOG_ERR("[B]PacsSvcGetFail");
        return -ENODEV;
    }

    bt_le_bluedroid_set_svc_in_progress(PACS_IN_PROGRESS);

    return bt_le_bluedroid_svc_start(pacs_svc);
}
