/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "sdkconfig.h"
#include "bootloader_common.h"
#include "hal/efuse_ll.h"
#include "hal/efuse_hal.h"
#include "esp_attr.h"

IRAM_ATTR uint32_t bootloader_common_get_chip_ver_pkg(void)
{
    return efuse_ll_get_chip_ver_pkg();
}

int bootloader_clock_get_rated_freq_mhz(void)
{
#ifdef CONFIG_IDF_TARGET_ESP32
    return efuse_hal_get_rated_freq_mhz();

#elif CONFIG_IDF_TARGET_ESP32C2
    return 120;

#elif CONFIG_IDF_TARGET_ESP32C3
    return 160;

#elif CONFIG_IDF_TARGET_ESP32C6
    return 160;

#elif CONFIG_IDF_TARGET_ESP32H2
    //IDF-6570
    return 96;

#elif CONFIG_IDF_TARGET_ESP32P4
    return 400;

#elif CONFIG_IDF_TARGET_ESP32S2
    return 240;

#elif CONFIG_IDF_TARGET_ESP32S3
    return 240;

#endif
}
