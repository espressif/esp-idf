/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <sys/param.h>
#include <string.h>
#include "esp_app_desc.h"
#include "sdkconfig.h"


// Application version info
const __attribute__((weak)) __attribute__((section(".rodata_desc")))  esp_app_desc_t esp_app_desc = {
    .magic_word = ESP_APP_DESC_MAGIC_WORD,
#ifdef CONFIG_APP_EXCLUDE_PROJECT_VER_VAR
    .version = "",
#else
    .version = PROJECT_VER,
#endif

#ifdef CONFIG_APP_EXCLUDE_PROJECT_NAME_VAR
    .project_name = "",
#else
    .project_name = PROJECT_NAME,
#endif
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
};


#ifndef CONFIG_APP_EXCLUDE_PROJECT_VER_VAR
_Static_assert(sizeof(PROJECT_VER) <= sizeof(esp_app_desc.version), "PROJECT_VER is longer than version field in structure");
#endif
_Static_assert(sizeof(IDF_VER) <= sizeof(esp_app_desc.idf_ver), "IDF_VER is longer than idf_ver field in structure");
#ifndef CONFIG_APP_EXCLUDE_PROJECT_NAME_VAR
_Static_assert(sizeof(PROJECT_NAME) <= sizeof(esp_app_desc.project_name), "PROJECT_NAME is longer than project_name field in structure");
#endif

const esp_app_desc_t *esp_app_get_description(void)
{
    return &esp_app_desc;
}

char app_elf_sha256_str[CONFIG_APP_RETRIEVE_LEN_ELF_SHA + 1] = { 0 };

/* The esp_app_desc.app_elf_sha256 should be possible to print in panic handler or core dump during cache is disabled.
 * But because the cache is disabled the reading esp_app_desc.app_elf_sha256 is not right and
 * can lead to a complete lock-up of the CPU.
 * For this reason we do a reading of esp_app_desc.app_elf_sha256 and convert to string while start up in esp_system_init_app_elf_sha256()
 * and keep it in the static app_elf_sha256_str variable.
 */
__attribute__((constructor)) void esp_app_format_init_elf_sha256(void)
{
    if (*((int *)&app_elf_sha256_str) != 0) {
        // app_elf_sha256_str is already set
        return;
    }
    // At -O2 optimization level, GCC optimizes out the copying of the first byte of the app_elf_sha256,
    // because it is zero at compile time, and only modified afterwards by esptool.
    // Casting to volatile disables the optimization.
    const volatile char* src = (const volatile char*)esp_app_desc.app_elf_sha256;
    for (size_t i = 0; i < sizeof(app_elf_sha256_str) / 2; ++i) {
        char c = src[i];
        for (size_t s = 0; s < 2; ++s) {
            char val = (c >> 4) & 0xF;
            app_elf_sha256_str[2 * i + s] = (val < 10) ? ('0' + val) : ('a' + val - 10);
            c <<= 4;
        }
    }
    app_elf_sha256_str[sizeof(app_elf_sha256_str) - 1] = 0;
}

int esp_app_get_elf_sha256(char* dst, size_t size)
{
    if (dst == NULL || size < 2) {
        return 0;
    }
    esp_app_format_init_elf_sha256();
    size_t n = MIN(size, sizeof(app_elf_sha256_str));
    memcpy(dst, app_elf_sha256_str, n);
    dst[n - 1] = 0;
    return n;
}
