/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>

#include "unity.h"
#include "spi_flash_mmap.h"
#include "esp_ota_ops.h"
#include "esp_flash.h"

#if CONFIG_SPI_FLASH_DANGEROUS_WRITE_ABORTS || CONFIG_SPI_FLASH_DANGEROUS_WRITE_FAILS

static const char *data = "blah blah blah";

#if CONFIG_SPI_FLASH_DANGEROUS_WRITE_FAILS
#define TEST_TAGS "[spi_flash][esp_flash]"
#else // ABORTS
#define TEST_TAGS "[spi_flash][esp_flash][ignore]"
#endif

TEST_CASE("can't overwrite bootloader", TEST_TAGS)
{
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_flash_write(NULL, data, 0x1000, strlen(data)));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_flash_write(NULL, data, 0x0FF8, strlen(data)));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_flash_write(NULL, data, 0x1400, strlen(data)));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_flash_erase_region(NULL, 0x8000, 0x2000));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_flash_erase_region(NULL, 0x7000, 0x2000));
}

TEST_CASE("can't overwrite current running app", TEST_TAGS)
{
    const esp_partition_t *p = esp_ota_get_running_partition();
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_flash_write(NULL, data, p->address + 1024, strlen(data)));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_flash_erase_region(NULL, p->address + 4096, 8192));
}

#endif // FAILS || ABORTS
