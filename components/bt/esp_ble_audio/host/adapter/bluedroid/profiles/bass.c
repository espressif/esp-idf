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

LOG_MODULE_REGISTER(LEA_BASS, CONFIG_BT_ISO_LOG_LEVEL);

int bt_le_bluedroid_bass_init(void)
{
    struct bt_gatt_service *bass_svc;

    LOG_DBG("[B]BassInit");

    bass_svc = lib_bap_bass_svc_get();
    if (!bass_svc) {
        LOG_ERR("[B]BassSvcGetFail");
        return -ENODEV;
    }

    bt_le_bluedroid_set_svc_in_progress(BASS_IN_PROGRESS);

    return bt_le_bluedroid_svc_init(bass_svc);
}

int bt_le_bluedroid_bass_start(void)
{
    struct bt_gatt_service *bass_svc;

    LOG_DBG("[B]BassStart");

    bass_svc = lib_bap_bass_svc_get();
    if (!bass_svc) {
        LOG_ERR("[B]BassSvcGetFail");
        return -ENODEV;
    }

    bt_le_bluedroid_set_svc_in_progress(BASS_IN_PROGRESS);

    return bt_le_bluedroid_svc_start(bass_svc);
}
