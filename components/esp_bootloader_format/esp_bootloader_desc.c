/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <sys/param.h>
#include "esp_bootloader_desc.h"
#include "sdkconfig.h"


// Bootloader version info
const __attribute__((weak)) __attribute__((section(".data_bootloader_desc"))) esp_bootloader_desc_t esp_bootloader_desc = {
    .magic_byte = ESP_BOOTLOADER_DESC_MAGIC_BYTE,
    .reserved = { 0 },
    .version = CONFIG_BOOTLOADER_PROJECT_VER,
    .idf_ver = IDF_VER,
#ifdef CONFIG_BOOTLOADER_COMPILE_TIME_DATE
    .date_time = __DATE__ " " __TIME__,
#else
    .date_time = "",
#endif
    .reserved2 = { 0 },
};

_Static_assert(sizeof(IDF_VER) <= sizeof(esp_bootloader_desc.idf_ver), "IDF_VER is longer than idf_ver field in structure");

const esp_bootloader_desc_t *esp_bootloader_get_description(void)
{
    return &esp_bootloader_desc;
}
