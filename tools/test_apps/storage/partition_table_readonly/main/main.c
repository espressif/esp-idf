/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <esp_log.h>
#include <esp_attr.h>
#include "esp_flash.h"
#include <esp_partition.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_spiffs.h"
#include "esp_heap_caps.h"
#include "esp_flash_encrypt.h"
#include "esp_efuse_table.h"

static const char* TAG = "test_readonly_partition_feature";

#define NUM_OF_READONLY_PARTITIONS 4
const esp_partition_t* readonly_partitions[NUM_OF_READONLY_PARTITIONS];

// Partition names
const char *nvs_partition_name = "nvs_ro";
const char *fatfs_wl_partition_name = "fatfs_ro";
const char *fatfs_raw_partition_name = "fatfs_raw_ro";
const char *spiffs_partition_name = "spiffs_ro";

// Mount paths for partitions
#define FATFS_WL_BASE_PATH "/fatfs_wl"
#define FATFS_RAW_BASE_PATH "/fatfs_raw"
#define SPIFFS_BASE_PATH "/spiffs"

// Handle of the wear levelling library instance
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

// Data in each filesystem partition
const char cmp_string[] = "This is a file contained in the generated filesystem image on the host and flashed to the ESP device";
#define CMP_STRING_LEN sizeof(cmp_string)

static void fill_array_of_readonly_data_partitions(void)
{
    // This finds read-only partitions defined in the partition table
    const esp_partition_t* part_nvs = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_ANY, nvs_partition_name);
    const esp_partition_t* part_fatfs_wl = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_ANY, fatfs_wl_partition_name);
    const esp_partition_t* part_fatfs_raw = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_ANY, fatfs_raw_partition_name);
    const esp_partition_t* part_spiffs = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_ANY, spiffs_partition_name);
    TEST_ASSERT_NOT_NULL(part_nvs); // NULL means partition table set wrong
    TEST_ASSERT_NOT_NULL(part_fatfs_wl);
    TEST_ASSERT_NOT_NULL(part_fatfs_raw);
    TEST_ASSERT_NOT_NULL(part_spiffs);

    readonly_partitions[0] = part_nvs;
    readonly_partitions[1] = part_fatfs_wl;
    readonly_partitions[2] = part_fatfs_raw;
    readonly_partitions[3] = part_spiffs;
}

#if CONFIG_IDF_TARGET_ESP32
#define TARGET_CRYPT_CNT_EFUSE  ESP_EFUSE_FLASH_CRYPT_CNT
#define TARGET_CRYPT_CNT_WIDTH  7
#else
#define TARGET_CRYPT_CNT_EFUSE ESP_EFUSE_SPI_BOOT_CRYPT_CNT
#define TARGET_CRYPT_CNT_WIDTH  3
#endif

static void example_print_flash_encryption_status(void)
{
    uint32_t flash_crypt_cnt = 0;
    esp_efuse_read_field_blob(TARGET_CRYPT_CNT_EFUSE, &flash_crypt_cnt, TARGET_CRYPT_CNT_WIDTH);
    printf("FLASH_CRYPT_CNT eFuse value is %" PRIu32 "\n", flash_crypt_cnt);

    esp_flash_enc_mode_t mode = esp_get_flash_encryption_mode();
    if (mode == ESP_FLASH_ENC_MODE_DISABLED) {
        printf("Flash encryption feature is disabled\n");
    } else {
        printf("Flash encryption feature is enabled in %s mode\n",
            mode == ESP_FLASH_ENC_MODE_DEVELOPMENT ? "DEVELOPMENT" : "RELEASE");
    }
}

void app_main(void)
{
    example_print_flash_encryption_status();
    fill_array_of_readonly_data_partitions();
    unity_run_menu();
}

TEST_CASE("Read-only partition - SPI flash API", "[spi_flash]")
{
    esp_err_t err;
    char buf[11] = {0};
    const char some_data[] = "0123456789";
    for (int i = 0; i < NUM_OF_READONLY_PARTITIONS; i++) {
        const esp_partition_t *part = readonly_partitions[i];
        // Writing to the SPI flash on address overlapping read-only partition shouldn't be possible
        // and should return ESP_ERR_NOT_ALLOWED error
        err = esp_flash_write(part->flash_chip, some_data, part->address, strlen(some_data));
        ESP_LOGD(TAG, "Writing %u bytes to partition %s at 0x%lx, should return %s and returned %s (0x%x)",
            strlen(some_data), part->label, part->address, esp_err_to_name(ESP_ERR_NOT_ALLOWED), esp_err_to_name(err), err);
        TEST_ASSERT_EQUAL(ESP_ERR_NOT_ALLOWED, err);

        // Reading the SPI flash on address overlapping read-only partition should be possible without an error
        TEST_ESP_OK(esp_flash_read(part->flash_chip, &buf, part->address, strlen(some_data)));
    }
}

TEST_CASE("Read-only partition - Partition API", "[partition]")
{
    esp_err_t err;
    // Writing to the partition should not be possible and should return ESP_ERR_NOT_ALLOWED error
    const char some_data[] = "0123456789";
    for (int i = 0; i < NUM_OF_READONLY_PARTITIONS; i++) {
        err = esp_partition_write(readonly_partitions[i], 0, some_data, strlen(some_data));
        ESP_LOGD(TAG, "esp_partition_write on readonly_partitions[%d] should return %s and returned %s (0x%x)",
            i, esp_err_to_name(ESP_ERR_NOT_ALLOWED), esp_err_to_name(err), err);
        TEST_ASSERT_EQUAL(ESP_ERR_NOT_ALLOWED, err);
    }

    // Reading the partition should be possible without an error
    char buf[strlen(some_data)];
    for (int i = 0; i < NUM_OF_READONLY_PARTITIONS; i++) {
        err = esp_partition_read(readonly_partitions[i], 0, buf, sizeof(buf));
        TEST_ESP_OK(err);
    }
}

TEST_CASE("Read-only partition - NVS API", "[nvs]")
{
    nvs_handle_t handle;
    esp_err_t err;

    err = nvs_flash_init_partition(nvs_partition_name);
    TEST_ESP_OK(err);

    // NVS partition flagged as read-only should be possible to open in read-only mode
    err = nvs_open_from_partition(nvs_partition_name, "storage", NVS_READONLY, &handle);
    TEST_ESP_OK(err);

    // Read test
    int32_t i32_val = 0;
    err = nvs_get_i32(handle, "i32_key", &i32_val);
    TEST_ESP_OK(err);
    TEST_ASSERT_EQUAL(-2147483648, i32_val);
    nvs_close(handle);

    // NVS partition flagged as read-only shouldn't be possible to open in read-write mode
    err = nvs_open_from_partition(nvs_partition_name, "storage", NVS_READWRITE, &handle);
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_ALLOWED, err);
    nvs_close(handle);
}

void test_c_api_common(const char* base_path)
{
    char hello_txt[64];
    char new_txt[64];
    snprintf(hello_txt, sizeof(hello_txt), "%s%s", base_path, "/hello.txt");
    snprintf(new_txt, sizeof(new_txt), "%s%s", base_path, "/new.txt");

    FILE *f;
    int fd, status;
    char buf[CMP_STRING_LEN] = {0};

    // Test write mode is not possible
    f = fopen(hello_txt, "w");
    TEST_ASSERT_NULL(f);
    fd = open(hello_txt, O_CREAT|O_WRONLY, 0666);
    TEST_ASSERT_EQUAL(-1, fd);
    TEST_ASSERT_EQUAL(EROFS, errno);

    f = fopen(hello_txt, "w+");
    TEST_ASSERT_NULL(f);
    fd = open(hello_txt, O_CREAT|O_RDWR, 0666);
    TEST_ASSERT_EQUAL(-1, fd);
    TEST_ASSERT_EQUAL(EROFS, errno);

    f = fopen(hello_txt, "a");
    TEST_ASSERT_NULL(f);
    fd = open(hello_txt, O_CREAT|O_WRONLY|O_APPEND, 0666);
    TEST_ASSERT_EQUAL(-1, fd);
    TEST_ASSERT_EQUAL(EROFS, errno);

    f = fopen(hello_txt, "a+");
    TEST_ASSERT_NULL(f);
    fd = open(hello_txt, O_CREAT|O_RDWR|O_APPEND, 0666);
    TEST_ASSERT_EQUAL(-1, fd);
    TEST_ASSERT_EQUAL(EROFS, errno);

    f = fopen(hello_txt, "r+");
    TEST_ASSERT_NULL(f);
    fd = open(hello_txt, O_RDWR);
    TEST_ASSERT_EQUAL(-1, fd);
    TEST_ASSERT_EQUAL(EROFS, errno);

    fd = creat(new_txt, 0666); // == open(new_txt, O_WRONLY|O_CREAT|O_TRUNC, 0666)
    TEST_ASSERT_EQUAL(-1, fd);
    TEST_ASSERT_EQUAL(EROFS, errno);

    status = link(hello_txt, new_txt);
    TEST_ASSERT_EQUAL(-1, status);
    TEST_ASSERT_EQUAL(EROFS, errno);

    status = rename(hello_txt, new_txt);
    TEST_ASSERT_EQUAL(-1, status);
    TEST_ASSERT_EQUAL(EROFS, errno);

    status = unlink(hello_txt);
    TEST_ASSERT_EQUAL(-1, status);
    TEST_ASSERT_EQUAL(EROFS, errno);

    status = truncate(hello_txt, 10);
    TEST_ASSERT_EQUAL(-1, status);
    TEST_ASSERT_EQUAL(EROFS, errno);

    // Test read is still possible
    fd = open(hello_txt, O_RDONLY);
    TEST_ASSERT_GREATER_THAN(0, fd);

    status = ftruncate(fd, 10);
    TEST_ASSERT_EQUAL(-1, status);
    TEST_ASSERT_EQUAL(EROFS, errno);
    close(fd);

    f = fopen(hello_txt, "r");
    TEST_ASSERT_NOT_NULL(f);
    fread(buf, 1, sizeof(buf) - 1, f);
    ESP_LOGD(TAG, "Read from file: %s", buf);
    TEST_ASSERT_EQUAL(0, strcmp(buf, cmp_string));
    memset(buf, 0, sizeof(buf));

    char str[] = "Should not be written";
    fseek(f, 0, SEEK_SET);
    status = fwrite(str, 1, sizeof(str), f); // Writing should do nothing
    TEST_ASSERT_EQUAL(0, status);
    TEST_ASSERT_EQUAL(EBADF, errno);

    fread(buf, 1, sizeof(buf) - 1, f);
    ESP_LOGD(TAG, "Read from file: %s", buf);
    TEST_ASSERT_EQUAL(0, strcmp(buf, cmp_string)); // Test if the file content is still the same
    fclose(f);
}

TEST_CASE("Read-only partition - C file I/O API (using FATFS WL)", "[vfs][fatfs]")
{
    const esp_vfs_fat_mount_config_t mount_config = {
        .max_files = 4,
        .format_if_mount_failed = false,
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE
    };

    esp_err_t err;
    int status;

    err = esp_vfs_fat_spiflash_mount_rw_wl(FATFS_WL_BASE_PATH, fatfs_wl_partition_name, &mount_config, &s_wl_handle);
    TEST_ESP_OK(err);

    // FATFS WL itself is read-write capable, but we are restricting it to read-only mode via esp_partition layer
    // Opening a file in a write mode on read-only partition is checked in vfs

    test_c_api_common(FATFS_WL_BASE_PATH);

    // Test directories
    DIR *dir;
    status = mkdir(FATFS_WL_BASE_PATH "/dir1", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    TEST_ASSERT_EQUAL(-1, status);
    TEST_ASSERT_EQUAL(EROFS, errno);

    dir = opendir(FATFS_WL_BASE_PATH "/dir1");
    TEST_ASSERT_NULL(dir);

    status = rmdir(FATFS_WL_BASE_PATH "/dir");
    TEST_ASSERT_EQUAL(-1, status);
    TEST_ASSERT_EQUAL(EROFS, errno);

    dir = opendir(FATFS_WL_BASE_PATH "/dir");
    TEST_ASSERT_NOT_NULL(dir);
    closedir(dir);

    TEST_ESP_OK(esp_vfs_fat_spiflash_unmount_rw_wl(FATFS_WL_BASE_PATH, s_wl_handle));
}

TEST_CASE("Read-only partition - C file I/O API (using FATFS RAW)", "[vfs][fatfs]")
{
    const esp_vfs_fat_mount_config_t mount_config = {
        .max_files = 4,
        .format_if_mount_failed = false,
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE
    };

    esp_err_t err;
    int status;

    err = esp_vfs_fat_spiflash_mount_ro(FATFS_RAW_BASE_PATH, fatfs_raw_partition_name, &mount_config);
    TEST_ESP_OK(err);

    // FATFS RAW is read-only itself, but esp_parition read-only adds another layer
    // Opening a file in a write mode on read-only partition is checked in vfs

    test_c_api_common(FATFS_RAW_BASE_PATH);

    // Test directories
    DIR *dir;
    status = mkdir(FATFS_RAW_BASE_PATH "/dir1", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    TEST_ASSERT_EQUAL(-1, status);
    TEST_ASSERT_EQUAL(EROFS, errno);

    dir = opendir(FATFS_RAW_BASE_PATH "/dir1");
    TEST_ASSERT_NULL(dir);

    status = rmdir(FATFS_RAW_BASE_PATH "/dir");
    TEST_ASSERT_EQUAL(-1, status);
    TEST_ASSERT_EQUAL(EROFS, errno);

    dir = opendir(FATFS_RAW_BASE_PATH "/dir");
    TEST_ASSERT_NOT_NULL(dir);
    closedir(dir);

    TEST_ESP_OK(esp_vfs_fat_spiflash_unmount_ro(FATFS_RAW_BASE_PATH, fatfs_raw_partition_name));
}

TEST_CASE("Read-only partition - C file I/O API (using SPIFFS)", "[vfs][spiffs]")
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = SPIFFS_BASE_PATH,
        .partition_label = spiffs_partition_name,
        .max_files = 5,
        .format_if_mount_failed = false
    };

    esp_err_t err;
    err = esp_vfs_spiffs_register(&conf);
    TEST_ESP_OK(err);

    // SPIFFS is read-write capable, but we are restricting it to read-only mode via esp_partition layer

    test_c_api_common(SPIFFS_BASE_PATH);

    // SPIFFS doesn't support directories

    TEST_ESP_OK(esp_vfs_spiffs_unregister(spiffs_partition_name));
}
