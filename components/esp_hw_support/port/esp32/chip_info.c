/*
 * SPDX-FileCopyrightText: 2013-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_chip_info.h"
#include "soc/soc.h"
#include "soc/efuse_reg.h"
#include "esp_efuse.h"

void esp_chip_info(esp_chip_info_t* out_info)
{
    uint32_t efuse_rd3 = REG_READ(EFUSE_BLK0_RDATA3_REG);
    memset(out_info, 0, sizeof(*out_info));

    out_info->model = CHIP_ESP32;
    out_info->revision = esp_efuse_get_chip_ver();

    if ((efuse_rd3 & EFUSE_RD_CHIP_VER_DIS_APP_CPU_M) == 0) {
        out_info->cores = 2;
    } else {
        out_info->cores = 1;
    }
    out_info->features = CHIP_FEATURE_WIFI_BGN;
    if ((efuse_rd3 & EFUSE_RD_CHIP_VER_DIS_BT_M) == 0) {
        out_info->features |= CHIP_FEATURE_BT | CHIP_FEATURE_BLE;
    }
    uint32_t package = esp_efuse_get_pkg_ver();
    if (package == EFUSE_RD_CHIP_VER_PKG_ESP32D2WDQ5 ||
        package == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD2 ||
        package == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD4 ||
        package == EFUSE_RD_CHIP_VER_PKG_ESP32PICOV302) {
        out_info->features |= CHIP_FEATURE_EMB_FLASH;
    }
    if(package == EFUSE_RD_CHIP_VER_PKG_ESP32D0WDR2V3) {
        out_info->features |= CHIP_FEATURE_EMB_PSRAM;
    }
}

#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
inline bool soc_has_cache_lock_bug(void)
{
    return (esp_efuse_get_chip_ver() == 3);
}
#endif
