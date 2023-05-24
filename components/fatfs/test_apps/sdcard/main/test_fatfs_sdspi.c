/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/unistd.h>
#include "unity.h"
#include "esp_log.h"
#include "esp_random.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "ff.h"
#include "test_fatfs_common.h"
#include "soc/soc_caps.h"

#if CONFIG_IDF_TARGET_ESP32
#define SDSPI_MISO_PIN 2
#define SDSPI_MOSI_PIN 15
#define SDSPI_CLK_PIN  14
#define SDSPI_CS_PIN   13
#elif CONFIG_IDF_TARGET_ESP32S2
// Adapted for internal test board ESP-32-S3-USB-OTG-Ev-BOARD_V1.0 (with ESP32-S2-MINI-1 module)
#define SDSPI_MISO_PIN 37
#define SDSPI_MOSI_PIN 35
#define SDSPI_CLK_PIN  36
#define SDSPI_CS_PIN   34
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32C6
#define SDSPI_MISO_PIN 6
#define SDSPI_MOSI_PIN 4
#define SDSPI_CLK_PIN  5
#define SDSPI_CS_PIN   1
#define SPI_DMA_CHAN   SPI_DMA_CH_AUTO
#elif CONFIG_IDF_TARGET_ESP32H2
#define SDSPI_MISO_PIN 0
#define SDSPI_MOSI_PIN 5
#define SDSPI_CLK_PIN  4
#define SDSPI_CS_PIN   1
#define SPI_DMA_CHAN   SPI_DMA_CH_AUTO
#endif

#ifndef SPI_DMA_CHAN
#define SPI_DMA_CHAN   1
#endif //SPI_DMA_CHAN
#define SDSPI_HOST_ID  SPI2_HOST


typedef struct sdspi_mem {
    size_t heap_size;
    uint32_t* buf;
} sdspi_mem_t;

static const char* s_test_filename = "/sdcard/hello.txt";
static void sdspi_speed_test(void *buf, size_t buf_size, size_t file_size, bool write);

static void test_setup_sdspi(sdspi_mem_t* mem)
{
    HEAP_SIZE_CAPTURE(mem->heap_size);

    const size_t buf_size = 16 * 1024;
    mem->buf = (uint32_t*) calloc(1, buf_size);
    esp_fill_random(mem->buf, buf_size);

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = SDSPI_MOSI_PIN,
        .miso_io_num = SDSPI_MISO_PIN,
        .sclk_io_num = SDSPI_CLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    esp_err_t err = spi_bus_initialize(SDSPI_HOST_ID, &bus_cfg, SPI_DMA_CHAN);
    TEST_ESP_OK(err);
}

static void test_teardown_sdspi(sdspi_mem_t* mem)
{
    free(mem->buf);
    spi_bus_free(SDSPI_HOST_ID);
    HEAP_SIZE_CHECK(mem->heap_size, 0);
}

TEST_CASE("(SDSPI) write/read speed test", "[fatfs][sdspi]")
{
    sdspi_mem_t mem;
    size_t file_size = 1 * 1024 * 1024;

    test_setup_sdspi(&mem);

    sdspi_speed_test(mem.buf, 4 * 1024, file_size, true);
    sdspi_speed_test(mem.buf, 8 * 1024, file_size, true);
    sdspi_speed_test(mem.buf, 16 * 1024, file_size, true);

    sdspi_speed_test(mem.buf, 4 * 1024, file_size, false);
    sdspi_speed_test(mem.buf, 8 * 1024, file_size, false);
    sdspi_speed_test(mem.buf, 16 * 1024, file_size, false);

    test_teardown_sdspi(&mem);
}

static void sdspi_speed_test(void *buf, size_t buf_size, size_t file_size, bool write)
{
    const char path[] = "/sdcard";
    sdmmc_card_t *card;
    card = NULL;
    sdspi_device_config_t device_cfg = {
        .gpio_cs = SDSPI_CS_PIN,
        .host_id = SDSPI_HOST_ID,
        .gpio_cd = SDSPI_SLOT_NO_CD,
        .gpio_wp = SDSPI_SLOT_NO_WP,
        .gpio_int = SDSPI_SLOT_NO_INT,
    };

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = SDSPI_HOST_ID;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = write,
        .max_files = 5,
        .allocation_unit_size = 64 * 1024
    };
    TEST_ESP_OK(esp_vfs_fat_sdspi_mount(path, &host, &device_cfg, &mount_config, &card));

    test_fatfs_rw_speed("/sdcard/4mb.bin", buf, buf_size, file_size, write);

    TEST_ESP_OK(esp_vfs_fat_sdcard_unmount(path, card));
}

TEST_CASE("(SDSPI) can get partition info", "[fatfs][sdspi]")
{
    sdspi_mem_t mem;

    test_setup_sdspi(&mem);

    const char path[] = "/sdcard";
    sdmmc_card_t *card;
    card = NULL;
    sdspi_device_config_t device_cfg = {
        .gpio_cs = SDSPI_CS_PIN,
        .host_id = SDSPI_HOST_ID,
        .gpio_cd = SDSPI_SLOT_NO_CD,
        .gpio_wp = SDSPI_SLOT_NO_WP,
        .gpio_int = SDSPI_SLOT_NO_INT,
    };

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = SDSPI_HOST_ID;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 64 * 1024
    };
    TEST_ESP_OK(esp_vfs_fat_sdspi_mount(path, &host, &device_cfg, &mount_config, &card));

    test_fatfs_info("/sdcard", "/sdcard/test.txt");

    TEST_ESP_OK(esp_vfs_fat_sdcard_unmount(path, card));

    test_teardown_sdspi(&mem);
}

TEST_CASE("(SDSPI) can format card", "[fatfs][sdspi][timeout=180]")
{
    sdspi_mem_t mem;
    test_setup_sdspi(&mem);

    const char path[] = "/sdcard";
    sdmmc_card_t *card;
    card = NULL;
    sdspi_device_config_t device_cfg = {
        .gpio_cs = SDSPI_CS_PIN,
        .host_id = SDSPI_HOST_ID,
        .gpio_cd = SDSPI_SLOT_NO_CD,
        .gpio_wp = SDSPI_SLOT_NO_WP,
        .gpio_int = SDSPI_SLOT_NO_INT,
    };

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = SDSPI_HOST_ID;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 64 * 1024
    };
    TEST_ESP_OK(esp_vfs_fat_sdspi_mount(path, &host, &device_cfg, &mount_config, &card));
    TEST_ESP_OK(esp_vfs_fat_sdcard_format("/sdcard", card));
    test_fatfs_create_file_with_text(s_test_filename, fatfs_test_hello_str);
    test_fatfs_read_file(s_test_filename);
    TEST_ESP_OK(esp_vfs_fat_sdcard_unmount(path, card));
    test_teardown_sdspi(&mem);
}
