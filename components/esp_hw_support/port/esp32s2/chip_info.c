/*
 * SPDX-FileCopyrightText: 2013-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_chip_info.h"
#include "hal/efuse_ll.h"
#include "hal/efuse_hal.h"

void esp_chip_info(esp_chip_info_t *out_info)
{
    uint32_t pkg_ver = efuse_ll_get_chip_ver_pkg();

    memset(out_info, 0, sizeof(*out_info));

    out_info->model = CHIP_ESP32S2;
    out_info->revision = efuse_hal_chip_revision();
    out_info->cores = 1;
    out_info->features = CHIP_FEATURE_WIFI_BGN;

    switch (pkg_ver) {
    case 0: // ESP32-S2
        break;
    case 1: // ESP32-S2FH16
        // fallthrough
    case 2: // ESP32-S2FH32
        out_info->features |= CHIP_FEATURE_EMB_FLASH;
        break;
    default: // New package, features unknown
        break;
    }
}
