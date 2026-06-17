/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <sys/param.h>
#include <string.h>
#include "esp_app_desc.h"
#include "sdkconfig.h"

#include "esp_log.h"

// startup_internal.h is necessary for startup function definition, which does not exist on Linux (TODO: IDF-9950)
#if !CONFIG_IDF_TARGET_LINUX && !ESP_TEE_BUILD
#include "esp_private/startup_internal.h"

ESP_LOG_ATTR_TAG(TAG, "app_init");
#endif

// Application version info
#if defined(__APPLE__) && CONFIG_IDF_TARGET_LINUX
const __attribute__((weak)) __attribute__((section("__RODATA_DESC,.rodata_desc")))  esp_app_desc_t esp_app_desc = {
#else
const __attribute__((weak)) __attribute__((section(".rodata_desc")))  esp_app_desc_t esp_app_desc = {
#endif /* #if defined(__APPLE__) && CONFIG_IDF_TARGET_LINUX */
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

// On Linux we just initialize the hash to some known value for testing
#if CONFIG_IDF_TARGET_LINUX
    .app_elf_sha256 = { 0xDE, 0xAD, 0xBE, 0xEF, 0x47, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B},
#endif

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
#if !(CONFIG_IDF_TARGET_LINUX || CONFIG_APP_BUILD_TYPE_PURE_RAM_APP)
    .spi_flash_mode = CONFIG_ESPTOOLPY_FLASHMODE_VAL,
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
static void esp_app_format_init_elf_sha256(void)
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

// startup function definition and execution does not exist on the Linux target
// (TODO: IDF-9950)
#if !CONFIG_IDF_TARGET_LINUX && !ESP_TEE_BUILD
ESP_SYSTEM_INIT_FN(init_show_app_info, CORE, BIT(0), 20)
{
    // Load the current ELF SHA256
    esp_app_format_init_elf_sha256();

    // Display information about the current running image.
    if (LOG_LOCAL_LEVEL >= ESP_LOG_INFO) {
        ESP_EARLY_LOGI(TAG, "Application information:");
#ifndef CONFIG_APP_EXCLUDE_PROJECT_NAME_VAR
        ESP_EARLY_LOGI(TAG, "Project name:     %s", esp_app_desc.project_name);
#endif
#ifndef CONFIG_APP_EXCLUDE_PROJECT_VER_VAR
        ESP_EARLY_LOGI(TAG, "App version:      %s", esp_app_desc.version);
#endif
#ifdef CONFIG_BOOTLOADER_APP_SECURE_VERSION
        ESP_EARLY_LOGI(TAG, "Secure version:   %" PRIu32, esp_app_desc.secure_version);
#endif
#ifdef CONFIG_APP_COMPILE_TIME_DATE
        ESP_EARLY_LOGI(TAG, "Compile time:     %s %s", esp_app_desc.date, esp_app_desc.time);
#endif
        char buf[17];
        esp_app_get_elf_sha256(buf, sizeof(buf));
        ESP_EARLY_LOGI(TAG, "ELF file SHA256:  %s...", buf);
        ESP_EARLY_LOGI(TAG, "ESP-IDF:          %s", esp_app_desc.idf_ver);
    }
    return ESP_OK;
}
#endif
