// Copyright 2013-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
}

#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
inline bool soc_has_cache_lock_bug(void)
{
    return (esp_efuse_get_chip_ver() == 3);
}
#endif
