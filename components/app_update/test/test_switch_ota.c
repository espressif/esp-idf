/*
 * Tests for switching between partitions: factory, OTAx, test.
 */

#include <esp_types.h>
#include <stdio.h>
#include "string.h"
#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/spi_flash.h"
#include "esp32/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/spi_flash.h"
#include "esp32s2/rom/rtc.h"
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"

#include "bootloader_common.h"
#include "../include_bootloader/bootloader_flash_priv.h"

#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "esp_flash_partitions.h"
#include "esp_image_format.h"
#include "nvs_flash.h"

#include "driver/gpio.h"
#include "esp_sleep.h"


RTC_DATA_ATTR static int boot_count = 0;
static const char *TAG = "ota_test";

/* @brief Copies a current app to next partition using handle.
 *
 * @param[in] update_handle - Handle of API ota.
 * @param[in] cur_app - Current app.
 */
static void copy_app_partition(esp_ota_handle_t update_handle, const esp_partition_t *curr_app)
{
    const void *partition_bin = NULL;
    spi_flash_mmap_handle_t  data_map;
    ESP_LOGI(TAG, "start the copy process");
    TEST_ESP_OK(esp_partition_mmap(curr_app, 0, curr_app->size, SPI_FLASH_MMAP_DATA, &partition_bin, &data_map));
    TEST_ESP_OK(esp_ota_write(update_handle, (const void *)partition_bin, curr_app->size));
    spi_flash_munmap(data_map);
    ESP_LOGI(TAG, "finish the copy process");
}

/* @brief Copies a current app to next partition using handle.
 *
 * @param[in] update_handle - Handle of API ota.
 * @param[in] cur_app - Current app.
 */
static void copy_app_partition_with_offset(esp_ota_handle_t update_handle, const esp_partition_t *curr_app)
{
    const void *partition_bin = NULL;
    spi_flash_mmap_handle_t  data_map;
    ESP_LOGI(TAG, "start the copy process");
    uint32_t offset = 0, bytes_to_write = curr_app->size;
    uint32_t write_bytes;
    while (bytes_to_write > 0) {
        write_bytes = (bytes_to_write > (4 * 1024)) ? (4 * 1024) : bytes_to_write;
        TEST_ESP_OK(esp_partition_mmap(curr_app, offset, write_bytes, SPI_FLASH_MMAP_DATA, &partition_bin, &data_map));
        TEST_ESP_OK(esp_ota_write_with_offset(update_handle, (const void *)partition_bin, write_bytes, offset));
        spi_flash_munmap(data_map);
        bytes_to_write -= write_bytes;
        offset += write_bytes;
    }
    ESP_LOGI(TAG, "finish the copy process");
}

#if defined(CONFIG_BOOTLOADER_FACTORY_RESET) || defined(CONFIG_BOOTLOADER_APP_TEST)
/* @brief Copies partition from source partition to destination partition.
 *
 * Partitions can be of any types and subtypes.
 * @param[in] dst_partition - Destination partition
 * @param[in] src_partition - Source partition
 */
static void copy_partition(const esp_partition_t *dst_partition, const esp_partition_t *src_partition)
{
    const void *partition_bin = NULL;
    spi_flash_mmap_handle_t data_map;
    TEST_ESP_OK(esp_partition_mmap(src_partition, 0, src_partition->size, SPI_FLASH_MMAP_DATA, &partition_bin, &data_map));
    TEST_ESP_OK(esp_partition_erase_range(dst_partition, 0, dst_partition->size));
    TEST_ESP_OK(esp_partition_write(dst_partition, 0, (const void *)partition_bin, dst_partition->size));
    spi_flash_munmap(data_map);
}
#endif

/* @brief Get the next partition of OTA for the update.
 *
 * @return The next partition of OTA(OTA0-15).
 */
static const esp_partition_t * get_next_update_partition(void)
{
    const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);
    TEST_ASSERT_NOT_EQUAL(NULL, update_partition);
    ESP_LOGI(TAG, "Writing to partition subtype %d at offset 0x%x", update_partition->subtype, update_partition->address);
    return update_partition;
}

/* @brief Copies a current app to next partition (OTA0-15) and then configure OTA data for a new boot partition.
 *
 * @param[in] cur_app_partition - Current app.
 * @param[in] next_app_partition - Next app for boot.
 */
static void copy_current_app_to_next_part(const esp_partition_t *cur_app_partition, const esp_partition_t *next_app_partition)
{
    esp_ota_get_next_update_partition(NULL);
    TEST_ASSERT_NOT_EQUAL(NULL, next_app_partition);
    ESP_LOGI(TAG, "Writing to partition subtype %d at offset 0x%x", next_app_partition->subtype, next_app_partition->address);

    esp_ota_handle_t update_handle = 0;
    TEST_ESP_OK(esp_ota_begin(next_app_partition, OTA_SIZE_UNKNOWN, &update_handle));

    copy_app_partition(update_handle, cur_app_partition);

    TEST_ESP_OK(esp_ota_end(update_handle));
    TEST_ESP_OK(esp_ota_set_boot_partition(next_app_partition));
}

/* @brief Copies a current app to next partition (OTA0-15) and then configure OTA data for a new boot partition.
 *
 * @param[in] cur_app_partition - Current app.
 * @param[in] next_app_partition - Next app for boot.
 */
static void copy_current_app_to_next_part_with_offset(const esp_partition_t *cur_app_partition, const esp_partition_t *next_app_partition)
{
    esp_ota_get_next_update_partition(NULL);
    TEST_ASSERT_NOT_EQUAL(NULL, next_app_partition);
    ESP_LOGI(TAG, "Writing to partition subtype %d at offset 0x%x", next_app_partition->subtype, next_app_partition->address);

    esp_ota_handle_t update_handle = 0;
    TEST_ESP_OK(esp_ota_begin(next_app_partition, OTA_SIZE_UNKNOWN, &update_handle));

    copy_app_partition_with_offset(update_handle, cur_app_partition);

    TEST_ESP_OK(esp_ota_end(update_handle));
    TEST_ESP_OK(esp_ota_set_boot_partition(next_app_partition));
}

/* @brief Erase otadata partition
 */
static void erase_ota_data(void)
{
    const esp_partition_t *data_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_OTA, NULL);
    TEST_ASSERT_NOT_EQUAL(NULL, data_partition);
    TEST_ESP_OK(esp_partition_erase_range(data_partition, 0, 2 * SPI_FLASH_SEC_SIZE));
}

/* @brief Reboots ESP using mode deep sleep. This mode guaranty that RTC_DATA_ATTR variables is not reset.
 */
static void reboot_as_deep_sleep(void)
{
    ESP_LOGI(TAG, "reboot as deep sleep");
    esp_sleep_enable_timer_wakeup(2000);
    esp_deep_sleep_start();
}

/* @brief Copies a current app to next partition (OTA0-15), after that ESP is rebooting and run this (the next) OTAx.
 */
static void copy_current_app_to_next_part_and_reboot(void)
{
    const esp_partition_t *cur_app = esp_ota_get_running_partition();
    ESP_LOGI(TAG, "copy current app to next part");
    copy_current_app_to_next_part(cur_app, get_next_update_partition());
    reboot_as_deep_sleep();
}

/* @brief Copies a current app to next partition (OTA0-15) using esp_ota_write_with_offest(), after that ESP is rebooting and run this (the next) OTAx.
 */
static void copy_current_app_to_next_part_with_offset_and_reboot(void)
{
    const esp_partition_t *cur_app = esp_ota_get_running_partition();
    ESP_LOGI(TAG, "copy current app to next part");
    copy_current_app_to_next_part_with_offset(cur_app, get_next_update_partition());
    reboot_as_deep_sleep();
}

/* @brief Get running app.
 *
 * @return The next partition of OTA(OTA0-15).
 */
static const esp_partition_t* get_running_firmware(void)
{
    const esp_partition_t *configured = esp_ota_get_boot_partition();
    const esp_partition_t *running = esp_ota_get_running_partition();
    ESP_LOGI(TAG, "Running partition type %d subtype %d (offset 0x%08x)",
            running->type, running->subtype, running->address);
    ESP_LOGI(TAG, "Configured partition type %d subtype %d (offset 0x%08x)",
            configured->type, configured->subtype, configured->address);
    TEST_ASSERT_NOT_EQUAL(NULL, configured);
    TEST_ASSERT_NOT_EQUAL(NULL, running);
    if (running->subtype != ESP_PARTITION_SUBTYPE_APP_TEST) {
        TEST_ASSERT_EQUAL_PTR(running, configured);
    }
    return running;
}

// type of a corrupt ota_data
typedef enum {
    CORR_CRC_1_SECTOR_OTA_DATA       = (1 << 0),   /*!< Corrupt CRC only 1 sector of ota_data */
    CORR_CRC_2_SECTOR_OTA_DATA       = (1 << 1),   /*!< Corrupt CRC only 2 sector of ota_data */
} corrupt_ota_data_t;

/* @brief Get two copies ota_data from otadata partition.
 *
 * @param[in] otadata_partition - otadata partition.
 * @param[out] ota_data_0 - First copy from otadata_partition.
 * @param[out] ota_data_1 - Second copy from otadata_partition.
 */
static void get_ota_data(const esp_partition_t *otadata_partition, esp_ota_select_entry_t *ota_data_0, esp_ota_select_entry_t *ota_data_1)
{
    uint32_t offset = otadata_partition->address;
    uint32_t size   = otadata_partition->size;
    if (offset != 0) {
        const esp_ota_select_entry_t *ota_select_map;
        ota_select_map = bootloader_mmap(offset, size);
        TEST_ASSERT_NOT_EQUAL(NULL, ota_select_map);

        memcpy(ota_data_0, ota_select_map, sizeof(esp_ota_select_entry_t));
        memcpy(ota_data_1, (uint8_t *)ota_select_map + SPI_FLASH_SEC_SIZE, sizeof(esp_ota_select_entry_t));
        bootloader_munmap(ota_select_map);
    }
}

/* @brief Writes a ota_data into required sector of otadata_partition.
 *
 * @param[in] otadata_partition - Partition information otadata.
 * @param[in] ota_data - otadata structure.
 * @param[in] sec_id - Sector number 0 or 1.
 */
static void write_ota_data(const esp_partition_t *otadata_partition, esp_ota_select_entry_t *ota_data, int sec_id)
{
    esp_partition_write(otadata_partition, SPI_FLASH_SEC_SIZE * sec_id, &ota_data[sec_id], sizeof(esp_ota_select_entry_t));
}

/* @brief Makes a corrupt of ota_data.
 * @param[in] err - type error
 */
static void corrupt_ota_data(corrupt_ota_data_t err)
{
    esp_ota_select_entry_t ota_data[2];

    const esp_partition_t *otadata_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_OTA, NULL);
    TEST_ASSERT_NOT_EQUAL(NULL, otadata_partition);
    get_ota_data(otadata_partition, &ota_data[0], &ota_data[1]);

    if (err & CORR_CRC_1_SECTOR_OTA_DATA) {
        ota_data[0].crc = 0;
    }
    if (err & CORR_CRC_2_SECTOR_OTA_DATA) {
        ota_data[1].crc = 0;
    }
    TEST_ESP_OK(esp_partition_erase_range(otadata_partition, 0, otadata_partition->size));
    write_ota_data(otadata_partition, &ota_data[0], 0);
    write_ota_data(otadata_partition, &ota_data[1], 1);
}

#if defined(CONFIG_BOOTLOADER_FACTORY_RESET) || defined(CONFIG_BOOTLOADER_APP_TEST)
/* @brief Sets the pin number to output and sets output level as low. After reboot (deep sleep) this pin keep the same level.
 *
 * The output level of the pad will be force locked and can not be changed.
 * Power down or call gpio_hold_dis will disable this function.
 *
 * @param[in] num_pin - Pin number
 */
static void set_output_pin(uint32_t num_pin)
{
    TEST_ESP_OK(gpio_hold_dis(num_pin));

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << num_pin);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    TEST_ESP_OK(gpio_config(&io_conf));

    TEST_ESP_OK(gpio_set_level(num_pin, 0));
    TEST_ESP_OK(gpio_hold_en(num_pin));
}

/* @brief Unset the pin number hold function.
 */
static void reset_output_pin(uint32_t num_pin)
{
    TEST_ESP_OK(gpio_hold_dis(num_pin));
    TEST_ESP_OK(gpio_reset_pin(num_pin));
}
#endif

static void mark_app_valid(void)
{
#ifdef CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
    TEST_ESP_OK(esp_ota_mark_app_valid_cancel_rollback());
#endif
}

/* @brief Checks and prepares the partition so that the factory app is launched after that.
 */
static void start_test(void)
{
    ESP_LOGI(TAG, "boot count 1 - reset");
    boot_count = 1;
    erase_ota_data();
    ESP_LOGI(TAG, "ota_data erased");
    reboot_as_deep_sleep();
}

static void test_flow1(void)
{
    boot_count++;
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
    boot_count++;
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
    boot_count++;
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
TEST_CASE_MULTIPLE_STAGES("Switching between factory, OTA0, OTA1, currupt ota_sec2, OTA0", "[app_update][timeout=90][reset=DEEPSLEEP_RESET, DEEPSLEEP_RESET, DEEPSLEEP_RESET, DEEPSLEEP_RESET]", start_test, test_flow3, test_flow3, test_flow3, test_flow3);

#ifdef CONFIG_BOOTLOADER_FACTORY_RESET
#define STORAGE_NAMESPACE "update_ota"

static void test_flow4(void)
{
    boot_count++;
    ESP_LOGI(TAG, "boot count %d", boot_count);
    const esp_partition_t *cur_app = get_running_firmware();
    nvs_handle_t handle = 0;
    int boot_count_nvs = 0;
    switch (boot_count) {
        case 2:
            ESP_LOGI(TAG, "Factory");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, cur_app->subtype);

            TEST_ESP_OK(nvs_flash_erase());
            TEST_ESP_OK(nvs_flash_init());
            TEST_ESP_OK(nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &handle));
            TEST_ESP_OK(nvs_set_i32(handle, "boot_count", boot_count));
            TEST_ESP_OK(nvs_commit(handle));
            nvs_close(handle);
            nvs_flash_deinit();

            copy_current_app_to_next_part_and_reboot();
            break;
        case 3:
            ESP_LOGI(TAG, "OTA0");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_OTA_0, cur_app->subtype);
            mark_app_valid();
            TEST_ESP_OK(nvs_flash_init());
            TEST_ESP_OK(nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &handle));
            TEST_ESP_OK(nvs_get_i32(handle, "boot_count", &boot_count_nvs));
            TEST_ASSERT_EQUAL(boot_count_nvs + 1, boot_count);
            nvs_close(handle);
            nvs_flash_deinit();

            set_output_pin(CONFIG_BOOTLOADER_NUM_PIN_FACTORY_RESET);

            reboot_as_deep_sleep();
            break;
        case 4:
            reset_output_pin(CONFIG_BOOTLOADER_NUM_PIN_FACTORY_RESET);
            ESP_LOGI(TAG, "Factory");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, cur_app->subtype);

            int boot_count_nvs;
            TEST_ESP_OK(nvs_flash_init());
            TEST_ESP_OK(nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &handle));
            TEST_ESP_ERR(ESP_ERR_NVS_NOT_FOUND, nvs_get_i32(handle, "boot_count", &boot_count_nvs));
            nvs_close(handle);
            nvs_flash_deinit();

            erase_ota_data();
            break;
        default:
            reset_output_pin(CONFIG_BOOTLOADER_NUM_PIN_FACTORY_RESET);
            erase_ota_data();
            TEST_FAIL_MESSAGE("Unexpected stage");
            break;
    }
}
// 1 Stage: After POWER_RESET erase OTA_DATA for this test           -> reboot through deep sleep.
// 2 Stage: run factory -> check it -> copy factory to OTA0          -> reboot  --//--
// 3 Stage: run OTA0    -> check it -> set_pin_factory_reset         -> reboot  --//--
// 4 Stage: run factory -> check it -> erase OTA_DATA for next tests -> PASS
TEST_CASE_MULTIPLE_STAGES("Switching between factory, OTA0, sets pin_factory_reset, factory", "[app_update][timeout=90][ignore][reset=DEEPSLEEP_RESET, DEEPSLEEP_RESET, DEEPSLEEP_RESET]", start_test, test_flow4, test_flow4, test_flow4);
#endif

#ifdef CONFIG_BOOTLOADER_APP_TEST
static void test_flow5(void)
{
    boot_count++;
    ESP_LOGI(TAG, "boot count %d", boot_count);
    const esp_partition_t *cur_app = get_running_firmware();
    switch (boot_count) {
        case 2:
            ESP_LOGI(TAG, "Factory");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_FACTORY, cur_app->subtype);

            set_output_pin(CONFIG_BOOTLOADER_NUM_PIN_APP_TEST);

            copy_partition(esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_TEST, NULL), cur_app);
            esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_OTA, NULL);
            reboot_as_deep_sleep();
            break;
        case 3:
            reset_output_pin(CONFIG_BOOTLOADER_NUM_PIN_APP_TEST);
            ESP_LOGI(TAG, "Test");
            TEST_ASSERT_EQUAL(ESP_PARTITION_SUBTYPE_APP_TEST, cur_app->subtype);
            reboot_as_deep_sleep();
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
// 2 Stage: run factory    -> check it -> copy factory to Test and set pin_test_app -> reboot  --//--
// 3 Stage: run test       -> check it -> reset pin_test_app                        -> reboot  --//--
// 4 Stage: run factory    -> check it -> erase OTA_DATA for next tests             -> PASS
TEST_CASE_MULTIPLE_STAGES("Switching between factory, test, factory", "[app_update][timeout=90][ignore][reset=DEEPSLEEP_RESET, DEEPSLEEP_RESET, DEEPSLEEP_RESET]", start_test, test_flow5, test_flow5, test_flow5);
#endif

static const esp_partition_t* app_update(void)
{
    const esp_partition_t *cur_app = get_running_firmware();
    const esp_partition_t* update_partition = esp_ota_get_next_update_partition(NULL);
    TEST_ASSERT_NOT_NULL(update_partition);
    esp_ota_handle_t update_handle = 0;
    TEST_ESP_OK(esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle));
    copy_app_partition(update_handle, cur_app);
    TEST_ESP_OK(esp_ota_end(update_handle));
    TEST_ESP_OK(esp_ota_set_boot_partition(update_partition));
    return update_partition;
}


static void test_rollback1(void)
{
    boot_count++;
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
    boot_count = 5;
    esp_ota_img_states_t ota_state = 0x5555AAAA;
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
    boot_count++;
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
    boot_count = 5;
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
    boot_count++;
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
    boot_count = 5;
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
    boot_count++;
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
