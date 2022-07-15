/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_check.h"
#include "esp_attr.h"
#include "esp_flash.h"
#include "esp_partition.h"
#if CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/spi_flash.h"
#include "esp32s3/rom/opi_flash.h"
#endif


//-----------------------------------------SPI0 PSRAM TEST-----------------------------------------------//
#if CONFIG_SPIRAM

#if CONFIG_SPIRAM_MODE_OCT
#define SPI0_PSRAM_TEST_LEN    (512 * 1024)
#define LENGTH_PER_TIME        1024
#else
#define SPI0_PSRAM_TEST_LEN    (128 * 1024)
#define LENGTH_PER_TIME        1024
#endif

static esp_err_t spi0_psram_test(void)
{
    printf("----------SPI0 PSRAM Test----------\n");

    uint8_t *psram_wr_buf = (uint8_t *)heap_caps_malloc(LENGTH_PER_TIME, MALLOC_CAP_32BIT | MALLOC_CAP_SPIRAM);
    if (!psram_wr_buf) {
        printf("no memory\n");
        abort();
    }

    uint32_t *psram_rd_buf = (uint32_t *)heap_caps_malloc(SPI0_PSRAM_TEST_LEN, MALLOC_CAP_32BIT | MALLOC_CAP_SPIRAM);
    if (!psram_rd_buf) {
        printf("no memory\n");
        abort();
    }

    srand(399);
    for (int i = 0; i < SPI0_PSRAM_TEST_LEN / LENGTH_PER_TIME; i++) {
        for (int j = 0; j < sizeof(psram_wr_buf); j++) {
            psram_wr_buf[j] = rand();
        }
        memcpy(psram_rd_buf + i * LENGTH_PER_TIME, psram_wr_buf, LENGTH_PER_TIME);

        if (memcmp(psram_rd_buf + i * LENGTH_PER_TIME, psram_wr_buf, LENGTH_PER_TIME) != 0) {
            printf("Fail\n");
            free(psram_rd_buf);
            free(psram_wr_buf);
            return ESP_FAIL;
        }
    }

    free(psram_rd_buf);
    free(psram_wr_buf);
    printf(DRAM_STR("----------SPI0 PSRAM Test Success----------\n\n"));
    return ESP_OK;
}
#endif


//-----------------------------------------SPI1 FLASH TEST-----------------------------------------------//
#define SPI1_FLASH_TEST_LEN     512
#define SECTOR_LEN              4096
#define SPI1_FLASH_TEST_NUM     (SECTOR_LEN / SPI1_FLASH_TEST_LEN)
#define SPI1_FLASH_TEST_ADDR    0x2a0000

extern void spi_flash_disable_interrupts_caches_and_other_cpu(void);
extern void spi_flash_enable_interrupts_caches_and_other_cpu(void);
static uint8_t rd_buf[SPI1_FLASH_TEST_LEN];
static uint8_t wr_buf[SPI1_FLASH_TEST_LEN];

static const esp_partition_t *get_test_flash_partition(void)
{
    /* This finds "flash_test" partition defined in partition_table_unit_test_app.csv */
    const esp_partition_t *result = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
            ESP_PARTITION_SUBTYPE_ANY, "flash_test");
    assert(result != NULL); /* means partition table set wrong */
    return result;
}

static NOINLINE_ATTR IRAM_ATTR esp_err_t spi1_flash_test(void)
{
    printf(DRAM_STR("----------SPI1 Flash Test----------\n"));

    //We need to use SPI1
    const esp_partition_t* part = get_test_flash_partition();
    esp_flash_erase_region(part->flash_chip, part->address, part->size);

    for (int i = 0; i < SPI1_FLASH_TEST_NUM; i++) {
        for (int j = i + 10; j < SPI1_FLASH_TEST_LEN; j++) {
            wr_buf[j] = j;
        }

        uint32_t test_flash_addr = SPI1_FLASH_TEST_ADDR;
        esp_flash_write(part->flash_chip, wr_buf, part->address, sizeof(wr_buf));
        esp_flash_read(part->flash_chip, rd_buf, part->address, sizeof(rd_buf));

        if (memcmp(wr_buf, rd_buf, SPI1_FLASH_TEST_LEN) != 0) {
            printf(DRAM_STR("error happened between 0x%x and 0x%x!!!!\n"), test_flash_addr, test_flash_addr + SPI1_FLASH_TEST_LEN);
            for (int i = 0; i < SPI1_FLASH_TEST_LEN; i++) {
                if (wr_buf[i] != rd_buf[i]) {
                    printf(DRAM_STR("err: wr[%d]: 0x%02x -- rd[%d]: 0x%02x\n"), i, wr_buf[i], i, rd_buf[i]);
                }
            }
            return ESP_FAIL;
        }
        memset(rd_buf, 0x0, SPI1_FLASH_TEST_LEN);
    }

    printf(DRAM_STR("----------SPI1 Flash Test Success----------\n\n"));

    return ESP_OK;
}

//-----------------------------------------SPI0 FLASH TEST-----------------------------------------------//
#define SPI0_FLASH_TEST_LEN    32
#define SPI0_FLASH_TEST_BUF    {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, \
                                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F}

static const uint8_t flash_rd_buf[SPI0_FLASH_TEST_LEN] = SPI0_FLASH_TEST_BUF;
extern int _flash_rodata_start;
extern int _rodata_reserved_end;


static IRAM_ATTR esp_err_t spi0_flash_test(void)
{
    printf("----------SPI0 Flash Test----------\n");
    //Check if the flash_rd_buf is in .rodata
    assert(((intptr_t)flash_rd_buf >= (intptr_t)&_flash_rodata_start) && ((intptr_t)flash_rd_buf < (intptr_t)&_rodata_reserved_end));

    uint8_t cmp_buf[SPI0_FLASH_TEST_LEN] = SPI0_FLASH_TEST_BUF;

    for (int i = 0; i < SPI0_FLASH_TEST_LEN; i++) {
        if (flash_rd_buf[i] != cmp_buf[i]) {
            return ESP_FAIL;
        }
    }
    printf(DRAM_STR("----------SPI0 Flash Test Success----------\n\n"));

    return ESP_OK;
}

void app_main(void)
{
    ESP_ERROR_CHECK(spi0_flash_test());

#if CONFIG_SPIRAM
    ESP_ERROR_CHECK(spi0_psram_test());
#endif
    ESP_ERROR_CHECK(spi1_flash_test());

    printf("flash psram test success\n");
}
