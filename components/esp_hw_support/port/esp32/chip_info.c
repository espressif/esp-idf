/*
 * SPDX-FileCopyrightText: 2013-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_chip_info.h"
#include "soc/soc.h"
#include "soc/chip_revision.h"
#include "soc/efuse_reg.h"
#include "hal/efuse_ll.h"
#include "hal/efuse_hal.h"

void esp_chip_info(esp_chip_info_t* out_info)
{
    memset(out_info, 0, sizeof(*out_info));

    out_info->model = CHIP_ESP32;
    out_info->revision = efuse_hal_chip_revision();

    if (efuse_ll_get_disable_app_cpu() == 0) {
        out_info->cores = 2;
    } else {
        out_info->cores = 1;
    }
    out_info->features = CHIP_FEATURE_WIFI_BGN;
    if (efuse_ll_get_disable_bt() == 0) {
        out_info->features |= CHIP_FEATURE_BT | CHIP_FEATURE_BLE;
    }
    uint32_t package = efuse_ll_get_chip_ver_pkg();
    if (package == EFUSE_RD_CHIP_VER_PKG_ESP32D2WDQ5 ||
        package == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD2 ||
        package == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD4 ||
        package == EFUSE_RD_CHIP_VER_PKG_ESP32PICOV302) {
        out_info->features |= CHIP_FEATURE_EMB_FLASH;
    }
    if(package == EFUSE_RD_CHIP_VER_PKG_ESP32D0WDR2V3 ||
       package == EFUSE_RD_CHIP_VER_PKG_ESP32PICOV302) {
        out_info->features |= CHIP_FEATURE_EMB_PSRAM;
    }
}

#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
inline bool soc_has_cache_lock_bug(void)
{
    unsigned rev = efuse_hal_chip_revision();
    return ESP_CHIP_REV_ABOVE(rev, 300);
}
#endif
