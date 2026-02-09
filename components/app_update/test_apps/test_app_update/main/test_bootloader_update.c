/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 * Tests bootloader update.
 */

#include "unity.h"
#include "esp_log.h"
#include "esp_efuse.h"
#include "esp_private/esp_flash_internal.h" //For dangerous write protection
#include "esp_rom_sys.h"
#include "utils_update.h"
#include "sdkconfig.h"

#define BOOT_COUNT_NAMESPACE "boot_count"

static __attribute__((unused)) const char *TAG = "btldr_update";

#if CONFIG_BOOTLOADER_RECOVERY_ENABLE

/* @brief Checks and prepares the partition so that the factory app is launched after that.
 */
static void start_test(void)
{
    ESP_LOGI(TAG, "boot count 1 - reset");
    set_boot_count_in_nvs(1);
    erase_ota_data();
    ESP_LOGI(TAG, "ota_data erased");
    ESP_LOGI(TAG, "Bootloader offset: 0x%x", esp_rom_get_bootloader_offset());
    reboot_as_deep_sleep();
}

static void test_flow1(void)
{
    uint8_t boot_count = get_boot_count_from_nvs();
    boot_count++;
    set_boot_count_in_nvs(boot_count);
    ESP_LOGI(TAG, "boot count %d", boot_count);

    ESP_LOGI(TAG, "Bootloader offset: 0x%x", esp_rom_get_bootloader_offset());

    const esp_partition_t *primary_bootloader;
    TEST_ESP_OK(esp_partition_register_external(NULL, ESP_PRIMARY_BOOTLOADER_OFFSET, ESP_BOOTLOADER_SIZE, "PrimaryBTLDR", ESP_PARTITION_TYPE_BOOTLOADER, ESP_PARTITION_SUBTYPE_BOOTLOADER_PRIMARY, &primary_bootloader));
    const esp_partition_t *recovery_bootloader;
    TEST_ESP_OK(esp_partition_register_external(NULL, CONFIG_BOOTLOADER_RECOVERY_OFFSET, ESP_BOOTLOADER_SIZE, "RecoveryBTLDR", ESP_PARTITION_TYPE_BOOTLOADER, ESP_PARTITION_SUBTYPE_BOOTLOADER_RECOVERY, &recovery_bootloader));
    ESP_LOGI(TAG, "Bootloaders are registered");

    // Remove write protection for the bootloader
    esp_flash_set_dangerous_write_protection(esp_flash_default_chip, false);
    switch (boot_count) {
        case 2:
            TEST_ASSERT_EQUAL_HEX32(ESP_PRIMARY_BOOTLOADER_OFFSET, esp_rom_get_bootloader_offset());

            TEST_ESP_OK(esp_partition_erase_range(recovery_bootloader, 0, recovery_bootloader->size));
            ESP_LOGI(TAG, "Erase recovery bootloader");

            TEST_ESP_OK(esp_efuse_set_recovery_bootloader_offset(CONFIG_BOOTLOADER_RECOVERY_OFFSET));

            ESP_LOGI(TAG, "Backup, copy <%s> -> <%s>", primary_bootloader->label, recovery_bootloader->label);
            TEST_ESP_OK(esp_partition_copy(recovery_bootloader, 0, primary_bootloader, 0, primary_bootloader->size));

            TEST_ESP_OK(esp_partition_erase_range(primary_bootloader, 0, primary_bootloader->size));
            ESP_LOGI(TAG, "Erase primary bootloader");
            reboot_as_deep_sleep();
            break;
        case 3:
            TEST_ASSERT_EQUAL_HEX32(CONFIG_BOOTLOADER_RECOVERY_OFFSET, esp_rom_get_bootloader_offset());

            ESP_LOGI(TAG, "Return to primary bootloader...");
            ESP_LOGI(TAG, "Copy <%s> -> <%s>", recovery_bootloader->label, primary_bootloader->label);
            TEST_ESP_OK(esp_partition_copy(primary_bootloader, 0, recovery_bootloader, 0, primary_bootloader->size));

            TEST_ESP_OK(esp_partition_erase_range(recovery_bootloader, 0, recovery_bootloader->size));
            ESP_LOGI(TAG, "Erase recovery bootloader");
            break;
        default:
            TEST_FAIL_MESSAGE("Unexpected stage");
            break;
    }
}

TEST_CASE_MULTIPLE_STAGES("Recovery bootloader feature", "[recovery_bootloader][timeout=90][reset=DEEPSLEEP_RESET, DEEPSLEEP_RESET]", start_test, test_flow1, test_flow1);

#endif // CONFIG_BOOTLOADER_RECOVERY_ENABLE
