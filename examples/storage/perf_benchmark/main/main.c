/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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
#include "esp_log.h"
#include "esp_check.h"
#include "esp_partition.h"
#include "esp_vfs_fat.h"
#include "esp_spiffs.h"
#include "soc/soc_caps.h"
#if SOC_SDMMC_HOST_SUPPORTED
#include "driver/sdmmc_host.h"
#endif
#include "driver/sdspi_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "wear_levelling.h"

#include "sdkconfig.h"
#include "tests.h"

static const char *TAG = "example";
static const char *flash_partition_label = "storage";
wl_handle_t s_wl_handle = WL_INVALID_HANDLE;
sdmmc_host_t host_g;
#ifdef CONFIG_EXAMPLE_USE_SDMMC
sdmmc_slot_config_t slot_config_g;
#else // CONFIG_EXAMPLE_USE_SDMMC
sdspi_device_config_t slot_config_g;
#endif // CONFIG_EXAMPLE_USE_SDSPI

#ifdef CONFIG_EXAMPLE_SD_FREQ_PROBING
#define EXAMPLE_SD_FREQ SDMMC_FREQ_PROBING
#elif CONFIG_EXAMPLE_SD_FREQ_DEFAULT
#define EXAMPLE_SD_FREQ SDMMC_FREQ_DEFAULT
#elif CONFIG_EXAMPLE_SD_FREQ_HIGHSPEED
#define EXAMPLE_SD_FREQ SDMMC_FREQ_HIGHSPEED
#elif CONFIG_EXAMPLE_SD_FREQ_CUSTOM
#define EXAMPLE_SD_FREQ CONFIG_EXAMPLE_SD_FREQ_CUSTOM_VAL
#else
#define EXAMPLE_SD_FREQ SDMMC_FREQ_DEFAULT
#endif

#ifdef CONFIG_EXAMPLE_USE_SDMMC
void init_sd_config(sdmmc_host_t *out_host, sdmmc_slot_config_t *out_slot_config, int freq_khz);
#else // CONFIG_EXAMPLE_USE_SDMMC
void init_sd_config(sdmmc_host_t *out_host, sdspi_device_config_t *out_slot_config, int freq_khz);
#endif // CONFIG_EXAMPLE_USE_SDSPI
void test_spiflash_raw(void);
void test_spiflash_fatfs(void);
void test_spiflash_spiffs(void);
void test_sd_raw(void);
void test_sd_fatfs(void);

void app_main(void)
{
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

#endif // CONFIG_EXAMPLE_TEST_SD_CARD
}

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

    spiflash_speed_test_raw_run();

    ret = wl_unmount(s_wl_handle);
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "WL layer unmounted");
}

void test_spiflash_fatfs(void)
{
    esp_err_t ret = ESP_OK;
    ESP_LOGI(TAG, "Mounting FATFS partition...");
    esp_vfs_fat_sdmmc_mount_config_t mount_config_spiflash = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};
    ret = esp_vfs_fat_spiflash_mount_rw_wl(FLASH_BASE_PATH, flash_partition_label, &mount_config_spiflash, &s_wl_handle);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "FATFS mounted to %s", FLASH_BASE_PATH);
    } else {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(ret));
        ESP_ERROR_CHECK(ret);
    }

    spiflash_speed_test_fs_run();

    // Unregister FATFS and unmount storage partition
    ret = esp_vfs_fat_spiflash_unmount_rw_wl(FLASH_BASE_PATH, s_wl_handle);
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "FATFS partition unmounted");
}

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

    spiflash_speed_test_fs_run();

    // Unregister SPIFFS and unmount storage partition
    ret = esp_vfs_spiffs_unregister(conf.partition_label);
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "SPIFFS partition unmounted");
}

void test_sd_raw(void)
{
    esp_err_t ret = ESP_OK;
    sdmmc_card_t *card;

    ESP_LOGI(TAG, "Mounting SD card - raw access");
    card = (sdmmc_card_t *)malloc(sizeof(sdmmc_card_t));
    if (card == NULL) {
        ESP_LOGE(TAG, "Failed to allocate sdmmc_card_t structure");
        ESP_ERROR_CHECK(ESP_ERR_NO_MEM);
    }
// Initialize the interface
#ifdef CONFIG_EXAMPLE_USE_SDMMC
    ret = sdmmc_host_init();
    ESP_ERROR_CHECK(ret);
    ret = sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config_g);
    ESP_ERROR_CHECK(ret);
#elif CONFIG_EXAMPLE_USE_SDSPI // CONFIG_EXAMPLE_USE_SDMMC
    int card_handle = -1;
    ret = sdspi_host_init_device((const sdspi_device_config_t *)&slot_config_g, &card_handle);
    ESP_ERROR_CHECK(ret);
#endif // CONFIG_EXAMPLE_USE_SDSPI

    ret = sdmmc_card_init(&host_g, card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SD card (%s)", esp_err_to_name(ret));
        ESP_LOGE(TAG, "If you were using SDMMC and switched to SPI reinsert the SD card or power cycle the board");
        free(card);
        ESP_ERROR_CHECK(ret);
    }
    ESP_LOGI(TAG, "SD card mounted - raw access");
    sdmmc_card_print_info(stdout, card);

    sdcard_speed_test_raw_run(card);

// Unmount SD card
#ifdef CONFIG_EXAMPLE_USE_SDMMC
    sdmmc_host_deinit();
#else // CONFIG_EXAMPLE_USE_SDMMC
    sdspi_host_deinit();
#endif // CONFIG_EXAMPLE_USE_SDSPI
    free(card);
    ESP_LOGI(TAG, "SD card unmounted - raw access");
}

void test_sd_fatfs(void)
{
    esp_err_t ret = ESP_OK;
    sdmmc_card_t *card;

    ESP_LOGI(TAG, "Mounting SD card - FATFS");
    esp_vfs_fat_sdmmc_mount_config_t mount_config_sdcard = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};
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

    sdcard_speed_test_fatfs_run();

    // Unmount SD card
    esp_vfs_fat_sdcard_unmount(SD_BASE_PATH, card);
    ESP_LOGI(TAG, "SD card unmounted - FATFS");
}

#ifdef CONFIG_EXAMPLE_TEST_SD_CARD
#ifdef CONFIG_EXAMPLE_USE_SDMMC
void init_sd_config(sdmmc_host_t *out_host, sdmmc_slot_config_t *out_slot_config, int freq_khz) {
#else // CONFIG_EXAMPLE_USE_SDMMC
void init_sd_config(sdmmc_host_t *out_host, sdspi_device_config_t *out_slot_config, int freq_khz) {
#endif // CONFIG_EXAMPLE_USE_SDSPI

    // By default, SD card frequency is initialized to SDMMC_FREQ_DEFAULT (20MHz)
    // For setting a specific frequency, use host.max_freq_khz
    // (range 400kHz - 40MHz for SDMMC, 400kHz - 20MHz for SDSPI)
    // Example: for fixed frequency of 10MHz, use host.max_freq_khz = 10000;
#ifdef CONFIG_EXAMPLE_USE_SDMMC
    ESP_LOGI(TAG, "Using SDMMC peripheral");
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = freq_khz;

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    // Set bus width to use:
    #ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
        slot_config.width = 4;
    #else
        slot_config.width = 1;
    #endif

    // On chips where the GPIOs used for SD card can be configured, set them in
    // the slot_config structure:
    #ifdef CONFIG_SOC_SDMMC_USE_GPIO_MATRIX
        slot_config.clk = CONFIG_EXAMPLE_PIN_CLK;
        slot_config.cmd = CONFIG_EXAMPLE_PIN_CMD;
        slot_config.d0 = CONFIG_EXAMPLE_PIN_D0;
        #ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
            slot_config.d1 = CONFIG_EXAMPLE_PIN_D1;
            slot_config.d2 = CONFIG_EXAMPLE_PIN_D2;
            slot_config.d3 = CONFIG_EXAMPLE_PIN_D3;
        #endif  // CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
    #endif  // CONFIG_SOC_SDMMC_USE_GPIO_MATRIX

    // Enable internal pullups on enabled pins. The internal pullups
    // are insufficient however, please make sure 10k external pullups are
    // connected on the bus. This is for debug / example purpose only.
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
#else // CONFIG_EXAMPLE_USE_SDMMC
    ESP_LOGI(TAG, "Using SPI peripheral");
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.max_freq_khz = freq_khz;

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = CONFIG_EXAMPLE_PIN_MOSI,
        .miso_io_num = CONFIG_EXAMPLE_PIN_MISO,
        .sclk_io_num = CONFIG_EXAMPLE_PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    esp_err_t ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    ESP_ERROR_CHECK(ret);

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = CONFIG_EXAMPLE_PIN_CS;
    slot_config.host_id = host.slot;
#endif // CONFIG_EXAMPLE_USE_SDSPI
    *out_host = host;
    *out_slot_config = slot_config;
}
#endif // CONFIG_EXAMPLE_TEST_SD_CARD
