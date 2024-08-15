/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 * Tests for switching between partitions: factory, OTAx, test.
 */

#include "esp_system.h"
#include "bootloader_common.h"
#include "../bootloader_flash/include/bootloader_flash_priv.h"
#include "esp_log.h"
#include "unity.h"
#include "utils_update.h"
#include "sdkconfig.h"

static const char *TAG = "ota_test";


/* @brief Checks and prepares the partition so that the factory app is launched after that.
 */
static void start_test(void)
{
    ESP_LOGI(TAG, "boot count 1 - reset");
    set_boot_count_in_nvs(1);
    erase_ota_data();
    ESP_LOGI(TAG, "ota_data erased");
    reboot_as_deep_sleep();
}

static void test_flow1(void)
{
    uint8_t boot_count = get_boot_count_from_nvs();
    boot_count++;
    set_boot_count_in_nvs(boot_count);
    ESP_LOGI(TAG, "boot count %d", boot_count);
    const esp_partition_t *cur_app = get_running_firmware();
    switch (boot_count) {
        case 2:
            ESP_LOGI(TAG, "Factory");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, cur_app->subtype);
            copy_current_app_to_next_part_and_reboot();
            break;
        case 3:
            ESP_LOGI(TAG, "OTA0");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_0, cur_app->subtype);
            mark_app_valid();
            copy_current_app_to_next_part_and_reboot();
            break;
        case 4:
            ESP_LOGI(TAG, "OTA1");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_1, cur_app->subtype);
            mark_app_valid();
            copy_current_app_to_next_part_and_reboot();
            break;
        case 5:
            ESP_LOGI(TAG, "OTA0");
            mark_app_valid();
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_0, cur_app->subtype);
            erase_ota_data();
            break;
        default:
            erase_ota_data();
            TEST_FAIL_MESSAGE("Unexpected stage");
            break;
    }
}

// 1 Stage: After POWER_RESET erase OTA_DATA for this test              -> reboot through deep sleep.
// 2 Stage: run factory -> check it -> copy factory to OTA0             -> reboot  --//--
// 3 Stage: run OTA0    -> check it -> copy OTA0 to OTA1                -> reboot  --//--
// 4 Stage: run OTA1    -> check it -> copy OTA1 to OTA0                -> reboot  --//--
// 5 Stage: run OTA0    -> check it -> erase OTA_DATA for next tests    -> PASS
TEST_CASE_MULTIPLE_STAGES("Switching between factory, OTA0, OTA1, OTA0", "[app_update][timeout=90][reset=DEEPSLEEP_RESET, DEEPSLEEP_RESET, DEEPSLEEP_RESET, DEEPSLEEP_RESET]", start_test, test_flow1, test_flow1, test_flow1, test_flow1);

static void test_flow2(void)
{
    uint8_t boot_count = get_boot_count_from_nvs();
    boot_count++;
    set_boot_count_in_nvs(boot_count);
    ESP_LOGI(TAG, "boot count %d", boot_count);
    const esp_partition_t *cur_app = get_running_firmware();
    switch (boot_count) {
        case 2:
            ESP_LOGI(TAG, "Factory");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, cur_app->subtype);
            copy_current_app_to_next_part_and_reboot();
            break;
        case 3:
            ESP_LOGI(TAG, "OTA0");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_0, cur_app->subtype);
            mark_app_valid();
            copy_current_app_to_next_part(cur_app, get_next_update_partition());
            corrupt_ota_data(CORR_CRC_1_SECTOR_OTA_DATA);
            reboot_as_deep_sleep();
            break;
        case 4:
            ESP_LOGI(TAG, "Factory");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, cur_app->subtype);
            erase_ota_data();
            break;
        default:
            erase_ota_data();
            TEST_FAIL_MESSAGE("Unexpected stage");
            break;
    }
}

// 1 Stage: After POWER_RESET erase OTA_DATA for this test              -> reboot through deep sleep.
// 2 Stage: run factory -> check it -> copy factory to OTA0             -> reboot  --//--
// 3 Stage: run OTA0    -> check it -> corrupt ota data                 -> reboot  --//--
// 4 Stage: run factory -> check it -> erase OTA_DATA for next tests    -> PASS
TEST_CASE_MULTIPLE_STAGES("Switching between factory, OTA0, corrupt ota_sec1, factory", "[app_update][timeout=90][reset=DEEPSLEEP_RESET, DEEPSLEEP_RESET, DEEPSLEEP_RESET]", start_test, test_flow2, test_flow2, test_flow2);

static void test_flow3(void)
{
    uint8_t boot_count = get_boot_count_from_nvs();
    boot_count++;
    set_boot_count_in_nvs(boot_count);
    ESP_LOGI(TAG, "boot count %d", boot_count);
    const esp_partition_t *cur_app = get_running_firmware();
    switch (boot_count) {
        case 2:
            ESP_LOGI(TAG, "Factory");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, cur_app->subtype);
            copy_current_app_to_next_part_and_reboot();
            break;
        case 3:
            ESP_LOGI(TAG, "OTA0");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_0, cur_app->subtype);
            mark_app_valid();
            copy_current_app_to_next_part_and_reboot();
            break;
        case 4:
            ESP_LOGI(TAG, "OTA1");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_1, cur_app->subtype);
            mark_app_valid();
            copy_current_app_to_next_part(cur_app, get_next_update_partition());
            corrupt_ota_data(CORR_CRC_2_SECTOR_OTA_DATA);
            reboot_as_deep_sleep();
            break;
        case 5:
            ESP_LOGI(TAG, "OTA0");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_0, cur_app->subtype);
            erase_ota_data();
            break;
        default:
            erase_ota_data();
            TEST_FAIL_MESSAGE("Unexpected stage");
            break;
    }
}

// 1 Stage: After POWER_RESET erase OTA_DATA for this test              -> reboot through deep sleep.
// 2 Stage: run factory -> check it -> copy factory to OTA0             -> reboot  --//--
// 3 Stage: run OTA0    -> check it -> copy OTA0 to OTA1                -> reboot  --//--
// 3 Stage: run OTA1    -> check it -> corrupt ota sector2              -> reboot  --//--
// 4 Stage: run OTA0    -> check it -> erase OTA_DATA for next tests    -> PASS
TEST_CASE_MULTIPLE_STAGES("Switching between factory, OTA0, OTA1, corrupt ota_sec2, OTA0", "[app_update][timeout=90][reset=DEEPSLEEP_RESET, DEEPSLEEP_RESET, DEEPSLEEP_RESET, DEEPSLEEP_RESET]", start_test, test_flow3, test_flow3, test_flow3, test_flow3);

#ifdef CONFIG_BOOTLOADER_FACTORY_RESET
static void test_flow4(void)
{
    uint8_t boot_count = get_boot_count_from_nvs();
    boot_count++;
    set_boot_count_in_nvs(boot_count);
    ESP_LOGI(TAG, "boot count %d", boot_count);
    const esp_partition_t *cur_app = get_running_firmware();
    switch (boot_count) {
        case 2:
            ESP_LOGI(TAG, "Factory");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, cur_app->subtype);
            copy_current_app_to_next_part_and_reboot();
            break;
        case 3:
            ESP_LOGI(TAG, "OTA0");
#ifdef BOOTLOADER_RESERVE_RTC_MEM
            TEST_ASSERT_FALSE(bootloader_common_get_rtc_retain_mem_factory_reset_state());
#endif
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_0, cur_app->subtype);
            mark_app_valid();
            set_output_pin(CONFIG_BOOTLOADER_NUM_PIN_FACTORY_RESET);
            esp_restart();
            break;
        case 4:
            reset_output_pin(CONFIG_BOOTLOADER_NUM_PIN_FACTORY_RESET);
            ESP_LOGI(TAG, "Factory");
#ifdef BOOTLOADER_RESERVE_RTC_MEM
            TEST_ASSERT_TRUE(bootloader_common_get_rtc_retain_mem_factory_reset_state());
            TEST_ASSERT_FALSE(bootloader_common_get_rtc_retain_mem_factory_reset_state());
#endif
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, cur_app->subtype);
            erase_ota_data();
            break;
        default:
            erase_ota_data();
            TEST_FAIL_MESSAGE("Unexpected stage");
            break;
    }
}
// 1 Stage: After POWER_RESET erase OTA_DATA for this test           -> reboot through deep sleep.
// 2 Stage: run factory -> check it -> copy factory to OTA0          -> reboot  --//--
// 3 Stage: run OTA0    -> check it -> set_pin_factory_reset         -> reboot
// 4 Stage: run factory -> check it -> erase OTA_DATA for next tests -> PASS
TEST_CASE_MULTIPLE_STAGES("Switching between factory, OTA0, sets pin_factory_reset, factory", "[app_update][timeout=90][reset=DEEPSLEEP_RESET, SW_CPU_RESET, DEEPSLEEP_RESET]", start_test, test_flow4, test_flow4, test_flow4);
#endif

#ifdef CONFIG_BOOTLOADER_APP_TEST
static void test_flow5(void)
{
    uint8_t boot_count = get_boot_count_from_nvs();
    boot_count++;
    set_boot_count_in_nvs(boot_count);
    ESP_LOGI(TAG, "boot count %d", boot_count);
    const esp_partition_t *cur_app = get_running_firmware();
    switch (boot_count) {
        case 2:
            ESP_LOGI(TAG, "Factory");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, cur_app->subtype);
            set_output_pin(CONFIG_BOOTLOADER_NUM_PIN_APP_TEST);
            esp_partition_copy(esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_TEST, NULL), 0, cur_app, 0, cur_app->size);
            esp_restart();
            break;
        case 3:
            reset_output_pin(CONFIG_BOOTLOADER_NUM_PIN_APP_TEST);
            ESP_LOGI(TAG, "Test");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_TEST, cur_app->subtype);
            esp_restart();
            break;
        case 4:
            ESP_LOGI(TAG, "Factory");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, cur_app->subtype);
            erase_ota_data();
            break;
        default:
            reset_output_pin(CONFIG_BOOTLOADER_NUM_PIN_APP_TEST);
            erase_ota_data();
            TEST_FAIL_MESSAGE("Unexpected stage");
            break;
    }
}

// 1 Stage: After POWER_RESET erase OTA_DATA for this test                          -> reboot through deep sleep.
// 2 Stage: run factory    -> check it -> copy factory to Test and set pin_test_app -> reboot
// 3 Stage: run test       -> check it -> reset pin_test_app                        -> reboot
// 4 Stage: run factory    -> check it -> erase OTA_DATA for next tests             -> PASS
TEST_CASE_MULTIPLE_STAGES("Switching between factory, test, factory", "[app_update][timeout=90][reset=SW_CPU_RESET, SW_CPU_RESET, DEEPSLEEP_RESET]", start_test, test_flow5, test_flow5, test_flow5);
#endif

static void test_rollback1(void)
{
    uint8_t boot_count = get_boot_count_from_nvs();
    boot_count++;
    set_boot_count_in_nvs(boot_count);
    ESP_LOGI(TAG, "boot count %d", boot_count);
    const esp_partition_t *cur_app = get_running_firmware();
    esp_ota_img_states_t ota_state = 0x5555AAAA;
    const esp_partition_t* update_partition = NULL;
    switch (boot_count) {
        case 2:
            ESP_LOGI(TAG, "Factory");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, cur_app->subtype);
            TEST_ASSERT_NULL(esp_ota_get_last_invalid_partition());
            TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, esp_ota_get_state_partition(cur_app, &ota_state));
            update_partition = app_update();
            TEST_ESP_OK(esp_ota_get_state_partition(update_partition, &ota_state));
#ifndef CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
            TEST_ASSERT_EQUAL(ESP_OTA_IMG_UNDEFINED, ota_state);
#else
            TEST_ASSERT_EQUAL(ESP_OTA_IMG_NEW, ota_state);
#endif
            reboot_as_deep_sleep();
            break;
        case 3:
            ESP_LOGI(TAG, "OTA0");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_0, cur_app->subtype);
            TEST_ASSERT_NULL(esp_ota_get_last_invalid_partition());
            TEST_ESP_OK(esp_ota_get_state_partition(cur_app, &ota_state));
#ifndef CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
            TEST_ASSERT_EQUAL(ESP_OTA_IMG_UNDEFINED, ota_state);
#else
            TEST_ASSERT_EQUAL(ESP_OTA_IMG_PENDING_VERIFY, ota_state);
#endif
            TEST_ESP_OK(esp_ota_mark_app_valid_cancel_rollback());
            TEST_ESP_OK(esp_ota_get_state_partition(cur_app, &ota_state));
            TEST_ASSERT_EQUAL(ESP_OTA_IMG_VALID, ota_state);
            reboot_as_deep_sleep();
            break;
        case 4:
            ESP_LOGI(TAG, "OTA0");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_0, cur_app->subtype);
            TEST_ESP_OK(esp_ota_get_state_partition(cur_app, &ota_state));
            TEST_ASSERT_EQUAL(ESP_OTA_IMG_VALID, ota_state);
            TEST_ESP_OK(esp_ota_mark_app_invalid_rollback_and_reboot());
            break;
        default:
            erase_ota_data();
            TEST_FAIL_MESSAGE("Unexpected stage");
            break;
    }
}

static void test_rollback1_1(void)
{
    set_boot_count_in_nvs(5);
    esp_ota_img_states_t ota_state = 0x5555AAAA;
    uint8_t boot_count = get_boot_count_from_nvs();
    ESP_LOGI(TAG, "boot count %d", boot_count);
    const esp_partition_t *cur_app = get_running_firmware();
    ESP_LOGI(TAG, "Factory");
    TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, cur_app->subtype);

    const esp_partition_t *invalid_partition = esp_ota_get_last_invalid_partition();
    const esp_partition_t* next_update_partition = esp_ota_get_next_update_partition(NULL);
    TEST_ASSERT_NOT_NULL(invalid_partition);
    TEST_ASSERT_NOT_NULL(next_update_partition);
    TEST_ASSERT_EQUAL_PTR(invalid_partition, next_update_partition);
    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, esp_ota_get_state_partition(cur_app, &ota_state));
    TEST_ESP_OK(esp_ota_get_state_partition(invalid_partition, &ota_state));
    TEST_ASSERT_EQUAL(ESP_OTA_IMG_INVALID, ota_state);

    erase_ota_data();
}

// 1 Stage: After POWER_RESET erase OTA_DATA for this test                          -> reboot through deep sleep.
// 2 Stage: run factory    -> check it -> copy factory to next app slot             -> reboot  --//--
// 3 Stage: run OTA0       -> check it -> esp_ota_mark_app_valid_cancel_rollback()                 -> reboot  --//--
// 4 Stage: run OTA0       -> check it -> esp_ota_mark_app_invalid_rollback_and_reboot()         -> reboot
// 5 Stage: run factory    -> check it -> erase OTA_DATA for next tests             -> PASS
TEST_CASE_MULTIPLE_STAGES("Test rollback. factory, OTA0, OTA0, rollback -> factory", "[app_update][timeout=90][reset=DEEPSLEEP_RESET, DEEPSLEEP_RESET, DEEPSLEEP_RESET, SW_CPU_RESET]", start_test, test_rollback1, test_rollback1, test_rollback1, test_rollback1_1);

static void test_rollback2(void)
{
    uint8_t boot_count = get_boot_count_from_nvs();
    boot_count++;
    set_boot_count_in_nvs(boot_count);
    ESP_LOGI(TAG, "boot count %d", boot_count);
    const esp_partition_t *cur_app = get_running_firmware();
    esp_ota_img_states_t ota_state = 0x5555AAAA;
    const esp_partition_t* update_partition = NULL;
    switch (boot_count) {
        case 2:
            ESP_LOGI(TAG, "Factory");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, cur_app->subtype);
            TEST_ASSERT_NULL(esp_ota_get_last_invalid_partition());
            TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, esp_ota_get_state_partition(cur_app, &ota_state));
            update_partition = app_update();
            TEST_ESP_OK(esp_ota_get_state_partition(update_partition, &ota_state));
#ifndef CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
            TEST_ASSERT_EQUAL(ESP_OTA_IMG_UNDEFINED, ota_state);
#else
            TEST_ASSERT_EQUAL(ESP_OTA_IMG_NEW, ota_state);
#endif
            reboot_as_deep_sleep();
            break;
        case 3:
            ESP_LOGI(TAG, "OTA0");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_0, cur_app->subtype);
            TEST_ASSERT_NULL(esp_ota_get_last_invalid_partition());
            TEST_ESP_OK(esp_ota_get_state_partition(cur_app, &ota_state));
#ifndef CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
            TEST_ASSERT_EQUAL(ESP_OTA_IMG_UNDEFINED, ota_state);
#else
            TEST_ASSERT_EQUAL(ESP_OTA_IMG_PENDING_VERIFY, ota_state);
#endif
            TEST_ESP_OK(esp_ota_mark_app_valid_cancel_rollback());
            TEST_ASSERT_NULL(esp_ota_get_last_invalid_partition());
            TEST_ESP_OK(esp_ota_get_state_partition(cur_app, &ota_state));
            TEST_ASSERT_EQUAL(ESP_OTA_IMG_VALID, ota_state);
            update_partition = app_update();
            TEST_ESP_OK(esp_ota_get_state_partition(update_partition, &ota_state));
#ifndef CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
            TEST_ASSERT_EQUAL(ESP_OTA_IMG_UNDEFINED, ota_state);
#else
            TEST_ASSERT_EQUAL(ESP_OTA_IMG_NEW, ota_state);
#endif
            reboot_as_deep_sleep();
            break;
        case 4:
            ESP_LOGI(TAG, "OTA1");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_1, cur_app->subtype);
            TEST_ASSERT_NULL(esp_ota_get_last_invalid_partition());
            TEST_ESP_OK(esp_ota_get_state_partition(cur_app, &ota_state));
#ifndef CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
            TEST_ASSERT_EQUAL(ESP_OTA_IMG_UNDEFINED, ota_state);
            TEST_ESP_OK(esp_ota_mark_app_invalid_rollback_and_reboot());
#else
            TEST_ASSERT_EQUAL(ESP_OTA_IMG_PENDING_VERIFY, ota_state);
            reboot_as_deep_sleep();
#endif
            break;
        default:
            erase_ota_data();
            TEST_FAIL_MESSAGE("Unexpected stage");
            break;
    }
}

static void test_rollback2_1(void)
{
    set_boot_count_in_nvs(5);
    uint8_t boot_count = get_boot_count_from_nvs();
    esp_ota_img_states_t ota_state = 0x5555AAAA;
    ESP_LOGI(TAG, "boot count %d", boot_count);
    const esp_partition_t *cur_app = get_running_firmware();
    ESP_LOGI(TAG, "OTA0");
    TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_0, cur_app->subtype);

    const esp_partition_t *invalid_partition = esp_ota_get_last_invalid_partition();
    TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_1, invalid_partition->subtype);
    const esp_partition_t* next_update_partition = esp_ota_get_next_update_partition(NULL);
    TEST_ASSERT_NOT_NULL(invalid_partition);
    TEST_ASSERT_NOT_NULL(next_update_partition);
    TEST_ASSERT_EQUAL_PTR(invalid_partition, next_update_partition);
    TEST_ESP_OK(esp_ota_get_state_partition(cur_app, &ota_state));
    TEST_ASSERT_EQUAL(ESP_OTA_IMG_VALID, ota_state);
    TEST_ESP_OK(esp_ota_get_state_partition(invalid_partition, &ota_state));
#ifndef CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
    TEST_ASSERT_EQUAL(ESP_OTA_IMG_INVALID, ota_state);
#else
    TEST_ASSERT_EQUAL(ESP_OTA_IMG_ABORTED, ota_state);
#endif
    erase_ota_data();
}

// 1 Stage: After POWER_RESET erase OTA_DATA for this test                                     -> reboot through deep sleep.
// 2 Stage: run factory        -> check it -> copy factory to next app slot                    -> reboot  --//--
// 3 Stage: run OTA0           -> check it -> esp_ota_mark_app_valid_cancel_rollback(), copy to next app slot -> reboot  --//--
// 4 Stage: run OTA1           -> check it -> PENDING_VERIFY/esp_ota_mark_app_invalid_rollback_and_reboot() -> reboot
// 5 Stage: run OTA0(rollback) -> check it -> erase OTA_DATA for next tests                    -> PASS
TEST_CASE_MULTIPLE_STAGES("Test rollback. factory, OTA0, OTA1, rollback -> OTA0", "[app_update][timeout=90][reset=DEEPSLEEP_RESET, DEEPSLEEP_RESET, DEEPSLEEP_RESET, SW_CPU_RESET]", start_test, test_rollback2, test_rollback2, test_rollback2, test_rollback2_1);

static void test_erase_last_app_flow(void)
{
    uint8_t boot_count = get_boot_count_from_nvs();
    boot_count++;
    set_boot_count_in_nvs(boot_count);
    ESP_LOGI(TAG, "boot count %d", boot_count);
    const esp_partition_t *cur_app = get_running_firmware();
    switch (boot_count) {
        case 2:
            ESP_LOGI(TAG, "Factory");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, cur_app->subtype);
            app_update();
            reboot_as_deep_sleep();
            break;
        case 3:
            ESP_LOGI(TAG, "OTA0");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_0, cur_app->subtype);
            mark_app_valid();
            app_update();
            reboot_as_deep_sleep();
            break;
        case 4:
            ESP_LOGI(TAG, "OTA1");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_1, cur_app->subtype);
            TEST_ESP_OK(esp_ota_erase_last_boot_app_partition());
            TEST_ESP_OK(esp_ota_mark_app_invalid_rollback_and_reboot());
            reboot_as_deep_sleep();
            break;
        default:
            erase_ota_data();
            TEST_FAIL_MESSAGE("Unexpected stage");
            break;
    }
}

static void test_erase_last_app_rollback(void)
{
    set_boot_count_in_nvs(5);
    uint8_t boot_count = get_boot_count_from_nvs();
    ESP_LOGI(TAG, "boot count %d", boot_count);
    const esp_partition_t *cur_app = get_running_firmware();
    ESP_LOGI(TAG, "erase_last_app");
    TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, cur_app->subtype);
    TEST_ESP_ERR(ESP_FAIL, esp_ota_erase_last_boot_app_partition());
    erase_ota_data();
}

// 1 Stage: After POWER_RESET erase OTA_DATA for this test                                     -> reboot through deep sleep.
// 2 Stage: run factory        -> check it -> copy factory to OTA0                             -> reboot  --//--
// 3 Stage: run OTA0           -> check it -> copy factory to OTA1                             -> reboot  --//--
// 4 Stage: run OTA1           -> check it -> erase OTA0 and rollback                          -> reboot
// 5 Stage: run factory        -> check it -> erase OTA_DATA for next tests                    -> PASS
TEST_CASE_MULTIPLE_STAGES("Test erase_last_boot_app_partition. factory, OTA1, OTA0, factory", "[app_update][timeout=90][reset=DEEPSLEEP_RESET, DEEPSLEEP_RESET, DEEPSLEEP_RESET, SW_CPU_RESET]", start_test, test_erase_last_app_flow, test_erase_last_app_flow, test_erase_last_app_flow, test_erase_last_app_rollback);

static void test_flow6(void)
{
    uint8_t boot_count = get_boot_count_from_nvs();
    boot_count++;
    set_boot_count_in_nvs(boot_count);
    ESP_LOGI(TAG, "boot count %d", boot_count);
    const esp_partition_t *cur_app = get_running_firmware();
    switch (boot_count) {
        case 2:
            ESP_LOGI(TAG, "Factory");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, cur_app->subtype);
            copy_current_app_to_next_part_with_offset_and_reboot();
            break;
        case 3:
            ESP_LOGI(TAG, "OTA0");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_0, cur_app->subtype);
            mark_app_valid();
            erase_ota_data();
            break;
        default:
            erase_ota_data();
            TEST_FAIL_MESSAGE("Unexpected stage");
            break;
    }
}

// 1 Stage: After POWER_RESET erase OTA_DATA for this test              -> reboot through deep sleep.
// 2 Stage: run factory -> check it -> copy factory to OTA0             -> reboot  --//--
// 3 Stage: run OTA0    -> check it -> erase OTA_DATA for next tests    -> PASS
TEST_CASE_MULTIPLE_STAGES("Switching between factory, OTA0 using esp_ota_write_with_offset", "[app_update][timeout=90][reset=DEEPSLEEP_RESET, DEEPSLEEP_RESET]", start_test, test_flow6, test_flow6);

TEST_CASE("Test bootloader_common_get_sha256_of_partition returns ESP_ERR_IMAGE_INVALID when image is invalid", "[partitions]")
{
    const esp_partition_t *cur_app = esp_ota_get_running_partition();
    ESP_LOGI(TAG, "copy current app to next part");
    const esp_partition_t *other_app = get_next_update_partition();
    copy_current_app_to_next_part(cur_app, other_app);
    erase_ota_data();

    uint8_t sha_256_cur_app[32];
    uint8_t sha_256_other_app[32];
    TEST_ESP_OK(bootloader_common_get_sha256_of_partition(cur_app->address, cur_app->size, cur_app->type, sha_256_cur_app));
    TEST_ESP_OK(bootloader_common_get_sha256_of_partition(other_app->address, other_app->size, other_app->type, sha_256_other_app));

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha_256_cur_app, sha_256_other_app, sizeof(sha_256_cur_app), "must be the same");

    uint32_t data = 0;
    bootloader_flash_write(other_app->address + 0x50, &data, sizeof(data), false);

    TEST_ESP_ERR(ESP_ERR_IMAGE_INVALID, bootloader_common_get_sha256_of_partition(other_app->address, other_app->size, other_app->type, sha_256_other_app));
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(sha_256_cur_app, sha_256_other_app, sizeof(sha_256_cur_app), "must be the same");
}

static void test_rollback3(void)
{
    uint8_t boot_count = get_boot_count_from_nvs();
    boot_count++;
    set_boot_count_in_nvs(boot_count);
    ESP_LOGI(TAG, "boot count %d", boot_count);
    const esp_partition_t *cur_app = get_running_firmware();
    const esp_partition_t* update_partition = NULL;
    switch (boot_count) {
        case 2:
            ESP_LOGI(TAG, "Factory");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, cur_app->subtype);
            update_partition = app_update();
            reboot_as_deep_sleep();
            break;
        case 3:
            ESP_LOGI(TAG, "OTA0");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_0, cur_app->subtype);
            TEST_ESP_OK(esp_ota_mark_app_valid_cancel_rollback());
            TEST_ASSERT_NULL(esp_ota_get_last_invalid_partition());
            update_partition = app_update();
            reboot_as_deep_sleep();
            break;
        case 4:
            ESP_LOGI(TAG, "OTA1");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_1, cur_app->subtype);
            TEST_ESP_OK(esp_ota_mark_app_valid_cancel_rollback());

            update_partition = esp_ota_get_next_update_partition(NULL);
#ifdef CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
            // two partitions are valid
            TEST_ASSERT_NULL(esp_ota_get_last_invalid_partition());
            esp_ota_img_states_t ota_state;
            TEST_ESP_OK(esp_ota_get_state_partition(update_partition, &ota_state));
            TEST_ASSERT_EQUAL(ESP_OTA_IMG_VALID, ota_state);
#endif

            esp_ota_handle_t update_handle = 0;
            TEST_ESP_OK(esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle));

#ifdef CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
            // After esp_ota_begin, the only one partition is valid
            // ota data slots do not have an entry about the update_partition.
            TEST_ESP_ERR(ESP_ERR_NOT_FOUND, esp_ota_get_state_partition(update_partition, &ota_state));
#endif
            copy_app_partition(update_handle, get_running_firmware());
            TEST_ESP_OK(esp_ota_end(update_handle));
            // esp_ota_set_boot_partition is not called, so the running app will not be changed after reboot
            reboot_as_deep_sleep();
            break;
        default:
            erase_ota_data();
            TEST_FAIL_MESSAGE("Unexpected stage");
            break;
    }
}

static void test_rollback3_1(void)
{
    set_boot_count_in_nvs(5);
    uint8_t boot_count = get_boot_count_from_nvs();
    esp_ota_img_states_t ota_state = 0x5555AAAA;
    ESP_LOGI(TAG, "boot count %d", boot_count);
    const esp_partition_t *cur_app = get_running_firmware();
    ESP_LOGI(TAG, "OTA1");
    TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_1, cur_app->subtype);
    TEST_ESP_OK(esp_ota_get_state_partition(cur_app, &ota_state));
    TEST_ASSERT_EQUAL(ESP_OTA_IMG_VALID, ota_state);

    TEST_ASSERT_NULL(esp_ota_get_last_invalid_partition());
    const esp_partition_t* next_update_partition = esp_ota_get_next_update_partition(NULL);
    TEST_ASSERT_NOT_NULL(next_update_partition);
#ifdef CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
    // ota data slots do not have an entry about the next_update_partition.
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, esp_ota_get_state_partition(next_update_partition, &ota_state));
#endif
    erase_ota_data();
}

TEST_CASE_MULTIPLE_STAGES("Test rollback. Updated partition invalidated after esp_ota_begin", "[app_update][timeout=90][reset=DEEPSLEEP_RESET, DEEPSLEEP_RESET, DEEPSLEEP_RESET, SW_CPU_RESET]", start_test, test_rollback3, test_rollback3, test_rollback3, test_rollback3_1);

static void test_rollback4(void)
{
    uint8_t boot_count = get_boot_count_from_nvs();
    boot_count++;
    set_boot_count_in_nvs(boot_count);
    ESP_LOGI(TAG, "boot count %d", boot_count);
    const esp_partition_t *cur_app = get_running_firmware();
    switch (boot_count) {
        case 2:
            ESP_LOGI(TAG, "Factory");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, cur_app->subtype);
            app_update();
            reboot_as_deep_sleep();
            break;
        case 3:
            ESP_LOGI(TAG, "OTA0");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_0, cur_app->subtype);
            TEST_ESP_OK(esp_ota_mark_app_valid_cancel_rollback());
            app_update();

            // Do not reboot and call app_update again.
            // This will not change the running partition since we haven't rebooted.
            // The esp_rewrite_otadata() will update the otadata for the non-running partition only.
            app_update();
#ifdef CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
            // The last call to esp_rewrite_otadata should have updated the otadata for the non-running partition only.
            // Therefore, calling esp_ota_get_state_partition on the running partition should succeed and not return ESP_ERR_NOT_FOUND
            const esp_partition_t* running_partition;
            running_partition = esp_ota_get_running_partition();
            esp_ota_img_states_t ota_state;
            TEST_ESP_OK(esp_ota_get_state_partition(running_partition, &ota_state));
#endif
            reboot_as_deep_sleep();
            break;
        case 4:
            ESP_LOGI(TAG, "OTA1");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_1, cur_app->subtype);
            TEST_ESP_OK(esp_ota_mark_app_valid_cancel_rollback());
            break;
        default:
            erase_ota_data();
            TEST_FAIL_MESSAGE("Unexpected stage");
            break;
    }
}

TEST_CASE_MULTIPLE_STAGES("Test esp_rewrite_otadata. Updated sequence number for non-running partition always", "[app_update][timeout=90][reset=DEEPSLEEP_RESET, DEEPSLEEP_RESET, DEEPSLEEP_RESET, SW_CPU_RESET]", start_test, test_rollback4, test_rollback4, test_rollback4);
