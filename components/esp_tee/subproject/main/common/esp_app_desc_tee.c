/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <sys/param.h>
#include "esp_app_desc.h"
#include "esp_attr.h"
#include "esp_tee_version.h"
#include "sdkconfig.h"

// Application version info
const esp_app_desc_t __attribute__((section(".rodata_desc"))) esp_app_desc = {
    .magic_word = ESP_APP_DESC_MAGIC_WORD,
    .version = ESP_TEE_VERSION_STR,
    .project_name = PROJECT_NAME,
    .idf_ver = IDF_VER_DEF,
#ifdef CONFIG_SECURE_TEE_ANTI_ROLLBACK
    .secure_version = CONFIG_SECURE_TEE_SECURE_VERSION,
#else
    .secure_version = 0,
#endif
    /* NOTE: Kept empty unconditionally - the TEE binary must be reproducible */
    .time = "",
    .date = "",
    .min_efuse_blk_rev_full = CONFIG_ESP_EFUSE_BLOCK_REV_MIN_FULL,
    .max_efuse_blk_rev_full = CONFIG_ESP_EFUSE_BLOCK_REV_MAX_FULL,
    .mmu_page_size = 31 - __builtin_clz(CONFIG_MMU_PAGE_SIZE),
};

void esp_app_desc_tee_include_impl(void)
{
    // Linker hook, exists for no other purpose
}

_Static_assert(sizeof(ESP_TEE_VERSION_STR) <= sizeof(esp_app_desc.version), "[esp_tee] ESP_TEE_VERSION_STR is longer than version field in structure");
_Static_assert(__builtin_strcmp(PROJECT_VER, ESP_TEE_VERSION_STR) == 0, "[esp_tee] version.cmake does not match esp_tee_version.h");
_Static_assert(ESP_TEE_VERSION_MINOR < 256 && ESP_TEE_VERSION_PATCH < 256, "[esp_tee] TEE minor/patch version must fit ESP_TEE_VERSION_VAL");
_Static_assert(sizeof(IDF_VER_DEF) <= sizeof(esp_app_desc.idf_ver), "[esp_tee] IDF_VER_DEF is longer than idf_ver field in structure");
_Static_assert(sizeof(PROJECT_NAME) <= sizeof(esp_app_desc.project_name), "[esp_tee] PROJECT_NAME is longer than project_name field in structure");
