/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <sys/param.h>
#include "esp_app_desc.h"
#include "esp_attr.h"
#include "sdkconfig.h"

// Application version info
const esp_app_desc_t __attribute__((section(".rodata_desc"))) esp_app_desc = {
    .magic_word = ESP_APP_DESC_MAGIC_WORD,
    .version = PROJECT_VER,
    .project_name = PROJECT_NAME,
    .idf_ver = IDF_VER,
#ifdef CONFIG_BOOTLOADER_APP_SECURE_VERSION
    .secure_version = CONFIG_BOOTLOADER_APP_SECURE_VERSION,
#else
    .secure_version = 0,
#endif
#ifdef CONFIG_APP_COMPILE_TIME_DATE
    .time = __TIME__,
    .date = __DATE__,
#else
    .time = "",
    .date = "",
#endif
    .min_efuse_blk_rev_full = CONFIG_ESP_EFUSE_BLOCK_REV_MIN_FULL,
    .max_efuse_blk_rev_full = CONFIG_ESP_EFUSE_BLOCK_REV_MAX_FULL,
    .mmu_page_size = 31 - __builtin_clz(CONFIG_MMU_PAGE_SIZE),
};

void esp_app_desc_tee_include_impl(void)
{
    // Linker hook, exists for no other purpose
}

_Static_assert(sizeof(PROJECT_VER) <= sizeof(esp_app_desc.version), "[esp_tee] PROJECT_VER is longer than version field in structure");
_Static_assert(sizeof(IDF_VER) <= sizeof(esp_app_desc.idf_ver), "[esp_tee] IDF_VER is longer than idf_ver field in structure");
_Static_assert(sizeof(PROJECT_NAME) <= sizeof(esp_app_desc.project_name), "[esp_tee] PROJECT_NAME is longer than project_name field in structure");
