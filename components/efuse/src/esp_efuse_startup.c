/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"
#include "hal/efuse_hal.h"
#include "rom/efuse.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_check.h"
#include "esp_efuse_utility.h"
#include "esp_system.h"
#include "esp_flash_encrypt.h"
#include "esp_secure_boot.h"
#include "esp_log.h"
#include "esp_private/startup_internal.h"
#ifdef CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
#include "esp_partition.h"
#endif
#include "sdkconfig.h"

#if __has_include("esp_app_desc.h")
#include "esp_app_desc.h"
#endif

static __attribute__((unused)) const char *TAG = "efuse_init";

ESP_SYSTEM_INIT_FN(init_efuse_check, CORE, BIT(0), 1)
{
    // (Only for C3): We check if the efuse BLOCK0 has certain coding errors then reset the chip.
    if (esp_efuse_check_errors() != ESP_OK) {
        esp_restart();
    }
    return ESP_OK;
}

// It comes after init_show_app_info to print the consistent application information.
ESP_SYSTEM_INIT_FN(init_efuse_show_app_info, CORE, BIT(0), 21)
{
    if (LOG_LOCAL_LEVEL >= ESP_LOG_INFO) {
        ESP_EARLY_LOGI(TAG, "Min chip rev:     v%d.%d", CONFIG_ESP_REV_MIN_FULL / 100, CONFIG_ESP_REV_MIN_FULL % 100);
        ESP_EARLY_LOGI(TAG, "Max chip rev:     v%d.%d %s", CONFIG_ESP_REV_MAX_FULL / 100, CONFIG_ESP_REV_MAX_FULL % 100,
                        efuse_hal_get_disable_wafer_version_major() ? "(constraint ignored)" : "");
        unsigned revision = efuse_hal_chip_revision();
        ESP_EARLY_LOGI(TAG, "Chip rev:         v%d.%d", revision / 100, revision % 100);
    }
    return ESP_OK;
}

#ifdef CONFIG_EFUSE_VIRTUAL
static void init_efuse_virtual(void)
{
    ESP_LOGW(TAG, "eFuse virtual mode is enabled. If Secure boot or Flash encryption is enabled then it does not provide any security. FOR TESTING ONLY!");
#ifdef CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
    // For efuse virtual mode we need to seed virtual efuses from flash
    // esp_flash must be initialized in advance because here we read the efuse partition.
    const esp_partition_t *efuse_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_EFUSE_EM, NULL);
    if (efuse_partition) {
        esp_efuse_init_virtual_mode_in_flash(efuse_partition->address, efuse_partition->size);
    }
#else // !CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
    // For efuse virtual mode we need to seed virtual efuses from efuse_regs.
    esp_efuse_utility_update_virt_blocks();
#endif // !CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
}
#endif // CONFIG_EFUSE_VIRTUAL

static esp_err_t init_efuse_secure(void)
{
#if CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
    // For anti-rollback case, recheck security version before we boot-up the current application
    const esp_app_desc_t *desc = esp_app_get_description();
    ESP_RETURN_ON_FALSE(esp_efuse_check_secure_version(desc->secure_version), ESP_FAIL, TAG, "Incorrect secure version of app");
#endif

#ifdef CONFIG_SECURE_FLASH_ENC_ENABLED
    esp_flash_encryption_init_checks();
#endif

#if defined(CONFIG_SECURE_BOOT) || defined(CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT)
    // Note: in some configs this may read flash, so placed after flash init
    esp_secure_boot_init_checks();
#endif

#if SOC_EFUSE_ECDSA_USE_HARDWARE_K
    if (esp_efuse_find_purpose(ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY, NULL)) {
        // ECDSA key purpose block is present and hence permanently enable
        // the hardware TRNG supplied k mode (most secure mode)
        ESP_RETURN_ON_ERROR(esp_efuse_write_field_bit(ESP_EFUSE_ECDSA_FORCE_USE_HARDWARE_K), TAG, "Failed to enable hardware k mode");
    }
#endif

#if CONFIG_SECURE_DISABLE_ROM_DL_MODE
    // Permanently disable ROM download mode
    ESP_RETURN_ON_ERROR(esp_efuse_disable_rom_download_mode(), TAG, "Failed to disable ROM download mode");
#endif

#if CONFIG_SECURE_ENABLE_SECURE_ROM_DL_MODE
    // Permanently disable ROM secure download mode
    ESP_RETURN_ON_ERROR(esp_efuse_enable_rom_secure_download_mode(), TAG, "Failed to enable Secure Download mode");
#endif

#if CONFIG_ESP32_DISABLE_BASIC_ROM_CONSOLE
    // ESP32 only: Permanently disable BASIC ROM Console feature
    esp_efuse_disable_basic_rom_console();
#endif
    return ESP_OK;
}

// Set efuse ROM_LOG_MODE on first boot
// For CONFIG_BOOT_ROM_LOG_ALWAYS_ON (default) or undefined (ESP32), leave
// ROM_LOG_MODE undefined (no need to call this function during startup)
#if CONFIG_BOOT_ROM_LOG_ALWAYS_OFF
#define ROM_LOG_MODE ESP_EFUSE_ROM_LOG_ALWAYS_OFF
#elif CONFIG_BOOT_ROM_LOG_ON_GPIO_LOW
#define ROM_LOG_MODE ESP_EFUSE_ROM_LOG_ON_GPIO_LOW
#elif CONFIG_BOOT_ROM_LOG_ON_GPIO_HIGH
#define ROM_LOG_MODE ESP_EFUSE_ROM_LOG_ON_GPIO_HIGH
#endif

#ifdef ROM_LOG_MODE
static esp_err_t init_efuse_rom_log(void)
{
    // Applicable for any chips except ESP32: Permanently disable ROM startup logs
    if (ets_efuse_get_uart_print_control() != ROM_LOG_MODE) {
        esp_err_t error = esp_efuse_set_rom_log_scheme(ROM_LOG_MODE);
        error = (error == ESP_ERR_NOT_SUPPORTED) ? ESP_OK : error;
        ESP_RETURN_ON_ERROR(error, TAG, "Failed to set ROM log scheme");
    }
    return ESP_OK;
}
#endif // ROM_LOG_MODE

ESP_SYSTEM_INIT_FN(init_efuse, CORE, BIT(0), 140)
{
    esp_err_t error = ESP_OK;

#ifdef CONFIG_EFUSE_VIRTUAL
    init_efuse_virtual();
#endif

    error = init_efuse_secure();
    ESP_RETURN_ON_ERROR(error, TAG, "Failed in secure eFuse init");

#ifdef ROM_LOG_MODE
    error = init_efuse_rom_log();
#endif

    return error;
}

void esp_efuse_startup_include_func(void)
{
    // Hook to force the linker to include this file
}
