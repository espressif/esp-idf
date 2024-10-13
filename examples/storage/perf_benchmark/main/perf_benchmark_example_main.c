/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

// This example uses SDMMC or SDSPI peripheral to communicate with SD card.

#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_partition.h"
#include "esp_vfs_fat.h"
#include "esp_spiffs.h"
#include "wear_levelling.h"

#include "esp_littlefs.h"

#include "sdkconfig.h"
#include "perf_benchmark_example_tests.h"
#include "perf_benchmark_example_sd_utils.h"

static const char *TAG = "example";
#ifdef CONFIG_EXAMPLE_TEST_SPIFLASH
static const char *flash_partition_label = "storage";
#endif // CONFIG_EXAMPLE_TEST_SPIFLASH
wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

void test_spiflash_raw(void);
void test_spiflash_fatfs(void);
void test_spiflash_spiffs(void);
void test_spiflash_littlefs(void);
void test_sd_raw(void);
void test_sd_fatfs(void);
void test_sd_littlefs(void);

void app_main(void)
{
    ESP_LOGI(TAG, "Starting benchmark test with target size %d bytes", CONFIG_EXAMPLE_TARGET_RW_SIZE);
#ifdef CONFIG_EXAMPLE_TEST_SPIFLASH
    ESP_LOGI(TAG, "Internal flash test");

/* SPI flash - raw access */
#ifdef CONFIG_EXAMPLE_TEST_SPIFLASH_RAW
    test_spiflash_raw();
#endif // CONFIG_EXAMPLE_TEST_SPIFLASH_RAW

/* SPI flash - FATFS */
#ifdef CONFIG_EXAMPLE_TEST_SPIFLASH_FATFS
    test_spiflash_fatfs();
#endif // CONFIG_EXAMPLE_TEST_SPIFLASH_FATFS

/* SPI flash - SPIFFS */
#ifdef CONFIG_EXAMPLE_TEST_SPIFLASH_SPIFFS
    test_spiflash_spiffs();
#endif // CONFIG_EXAMPLE_TEST_SPIFLASH_SPIFFS

#ifdef CONFIG_EXAMPLE_TEST_SPIFLASH_LITTLEFS
    test_spiflash_littlefs();
#endif // CONFIG_EXAMPLE_TEST_SPIFLASH_LITTLEFS

#endif // CONFIG_EXAMPLE_TEST_SPIFLASH

#ifdef CONFIG_EXAMPLE_TEST_SD_CARD
    ESP_LOGI(TAG, "SD card test");

    ESP_LOGI(TAG, "Initializing SD card");
    init_sd_config(&host_g, &slot_config_g, EXAMPLE_SD_FREQ);

/* SD card - raw access */
#ifdef CONFIG_EXAMPLE_TEST_SD_CARD_RAW
    test_sd_raw();
#endif // CONFIG_EXAMPLE_TEST_SD_CARD_RAW

/* SD card - FATFS */
#ifdef CONFIG_EXAMPLE_TEST_SD_CARD_FATFS
    test_sd_fatfs();
#endif // CONFIG_EXAMPLE_TEST_SD_CARD_FATFS

/* SD card - LittleFS */
#ifdef CONFIG_EXAMPLE_TEST_SD_CARD_LITTLEFS
    test_sd_littlefs();
#endif // CONFIG_EXAMPLE_TEST_SD_CARD_LITTLEFS

#endif // CONFIG_EXAMPLE_TEST_SD_CARD
}

#if CONFIG_EXAMPLE_TEST_SPIFLASH

#ifdef CONFIG_EXAMPLE_TEST_SPIFLASH_RAW
void test_spiflash_raw(void)
{
    esp_err_t ret = ESP_OK;
    ESP_LOGI(TAG, "Mountig WL layer...");
    const esp_partition_t *data_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                                                     ESP_PARTITION_SUBTYPE_DATA_UNDEFINED, flash_partition_label);
    if (data_partition == NULL) {
        ESP_LOGE(TAG, "Could not find partition \"%s\"", flash_partition_label);
        ESP_ERROR_CHECK(ESP_ERR_NOT_FOUND);
    }
    ret = wl_mount(data_partition, &s_wl_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount wear levelling layer, ret = %i", ret);
        ESP_ERROR_CHECK(ret);
    }
    ESP_LOGI(TAG, "WL layer mounted");

    spiflash_speed_test_raw_run(CONFIG_EXAMPLE_TEST_TRIES);

    ret = wl_unmount(s_wl_handle);
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "WL layer unmounted");
}
#endif // CONFIG_EXAMPLE_TEST_SPIFLASH_RAW

#ifdef CONFIG_EXAMPLE_TEST_SPIFLASH_FATFS
void test_spiflash_fatfs(void)
{
    esp_err_t ret = ESP_OK;
    esp_vfs_fat_sdmmc_mount_config_t mount_config_spiflash = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = CONFIG_EXAMPLE_FATFS_SPIFLASH_CLUSTER_SIZE,
    };

    ESP_LOGI(TAG, "Mounting FATFS partition, with cluster size %d bytes", mount_config_spiflash.allocation_unit_size);

    ret = esp_vfs_fat_spiflash_mount_rw_wl(FLASH_BASE_PATH, flash_partition_label, &mount_config_spiflash, &s_wl_handle);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "FATFS mounted to %s", FLASH_BASE_PATH);
    } else {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(ret));
        ESP_ERROR_CHECK(ret);
    }

    spiflash_speed_test_fatfs_run(CONFIG_EXAMPLE_TEST_TRIES);

    // Unregister FATFS and unmount storage partition
    ret = esp_vfs_fat_spiflash_unmount_rw_wl(FLASH_BASE_PATH, s_wl_handle);
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "FATFS partition unmounted");
}
#endif // CONFIG_EXAMPLE_TEST_SPIFLASH_FATFS

#ifdef CONFIG_EXAMPLE_TEST_SPIFLASH_SPIFFS
void test_spiflash_spiffs(void)
{
    esp_err_t ret = ESP_OK;
    ESP_LOGI(TAG, "Mounting SPIFFS partition...");
    esp_vfs_spiffs_conf_t conf = {
        .base_path = FLASH_BASE_PATH,
        .partition_label = flash_partition_label,
        .max_files = 5,
        .format_if_mount_failed = true};

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    ret = esp_vfs_spiffs_register(&conf);

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "SPIFFS mounted to %s", FLASH_BASE_PATH);
    } else {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        ESP_ERROR_CHECK(ret);
    }

    spiflash_speed_test_spiffs_run(1); // if run multiple times, it will fail, because spiffs can't delete files

    // Unregister SPIFFS and unmount storage partition
    ret = esp_vfs_spiffs_unregister(conf.partition_label);
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "SPIFFS partition unmounted");
}
#endif // CONFIG_EXAMPLE_TEST_SPIFLASH_SPIFFS

#ifdef CONFIG_EXAMPLE_TEST_SPIFLASH_LITTLEFS
void test_spiflash_littlefs(void)
{
    esp_err_t ret = ESP_OK;
    ESP_LOGI(TAG, "Mounting LittleFS partition...");
    esp_vfs_littlefs_conf_t conf = {
        .base_path = FLASH_BASE_PATH,
        .partition_label = flash_partition_label,
        .format_if_mount_failed = true,
    };

    // Use settings defined above to initialize and mount LittleFS filesystem.
    // Note: esp_vfs_littlefs_register is an all-in-one convenience function.
    ret = esp_vfs_littlefs_register(&conf);

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "LittleFS mounted to %s", FLASH_BASE_PATH);
    } else {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find LittleFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize LittleFS (%s)", esp_err_to_name(ret));
        }
    }

    spiflash_speed_test_littlefs_run(CONFIG_EXAMPLE_TEST_TRIES);

    // Unregister LittleFS and unmount storage partition
    ret = esp_vfs_littlefs_unregister(conf.partition_label);
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "LittleFS partition unmounted");
}
#endif // CONFIG_EXAMPLE_TEST_SPIFLASH_LITTLEFS
#endif // CONFIG_EXAMPLE_TEST_SPIFLASH

#ifdef CONFIG_EXAMPLE_TEST_SD_CARD

#ifdef CONFIG_EXAMPLE_TEST_SD_CARD_RAW
void test_sd_raw(void)
{
    esp_err_t ret = ESP_OK;
    sdmmc_card_t *card;

    ESP_LOGI(TAG, "Mounting SD card - raw access");
    ret = init_sd_card(&card);
    ESP_ERROR_CHECK(ret);

    sdcard_speed_test_raw_run(card, CONFIG_EXAMPLE_TEST_TRIES);

    deinit_sd_card(&card);
    ESP_LOGI(TAG, "SD card unmounted - raw access");
}
#endif // CONFIG_EXAMPLE_TEST_SD_CARD_RAW

#ifdef CONFIG_EXAMPLE_TEST_SD_CARD_FATFS
void test_sd_fatfs(void)
{
    esp_err_t ret = ESP_OK;
    sdmmc_card_t *card;

    esp_vfs_fat_sdmmc_mount_config_t mount_config_sdcard = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = CONFIG_EXAMPLE_FATFS_SD_CARD_CLUSTER_SIZE,
    };

    ESP_LOGI(TAG, "Mounting SD card - FATFS, with cluster size %d bytes", mount_config_sdcard.allocation_unit_size);

#ifdef CONFIG_EXAMPLE_USE_SDMMC
    ret = esp_vfs_fat_sdmmc_mount(SD_BASE_PATH, &host_g, &slot_config_g, &mount_config_sdcard, &card);
#else // CONFIG_EXAMPLE_USE_SDMMC
    ret = esp_vfs_fat_sdspi_mount(SD_BASE_PATH, &host_g, &slot_config_g, &mount_config_sdcard, &card);
#endif // CONFIG_EXAMPLE_USE_SDSPI

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                      "Make sure SD card lines have pull-up resistors in place.",
                 esp_err_to_name(ret));
        ESP_ERROR_CHECK(ret);
    }
    sdmmc_card_print_info(stdout, card);
    ESP_LOGI(TAG, "SD card mounted - FATFS");

    sdcard_speed_test_fatfs_run(CONFIG_EXAMPLE_TEST_TRIES);

    // Unmount SD card
    esp_vfs_fat_sdcard_unmount(SD_BASE_PATH, card);
    ESP_LOGI(TAG, "SD card unmounted - FATFS");
}
#endif // CONFIG_EXAMPLE_TEST_SD_CARD_FATFS

#ifdef CONFIG_EXAMPLE_TEST_SD_CARD_LITTLEFS
void test_sd_littlefs(void)
{
    esp_err_t ret = ESP_OK;
    sdmmc_card_t *card;

    ESP_LOGI(TAG, "Mounting SD card - LittleFS");

    ret = init_sd_card(&card);
    ESP_ERROR_CHECK(ret);

    esp_vfs_littlefs_conf_t conf = {
        .base_path = SD_BASE_PATH,
        .sdcard = card,
        .format_if_mount_failed = true,
    };

    ret = esp_vfs_littlefs_register(&conf);

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "LittleFS mounted to %s", SD_BASE_PATH);
    } else {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find LittleFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize LittleFS (%s)", esp_err_to_name(ret));
        }
    }
    ESP_LOGI(TAG, "SD card mounted - LittleFS");

    sdcard_speed_test_littlefs_run(CONFIG_EXAMPLE_TEST_TRIES);

    // Unmount SD card
    esp_vfs_littlefs_unregister_sdmmc(card);
    deinit_sd_card(&card);
    ESP_LOGI(TAG, "SD card unmounted - LittleFS");
}
#endif // CONFIG_EXAMPLE_TEST_SD_CARD_LITTLEFS
#endif // CONFIG_EXAMPLE_TEST_SD_CARD
