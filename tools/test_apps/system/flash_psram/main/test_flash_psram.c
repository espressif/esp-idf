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
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_check.h"
#include "esp_attr.h"
#include "esp_pm.h"
#include "esp_private/esp_clk.h"
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

#define MHZ (1000000)
#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#if CONFIG_IDF_TARGET_ESP32
typedef  esp_pm_config_esp32_t esp_pm_config_t;
#define DEFAULT_CPU_FREQ_MHZ                CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ
#elif CONFIG_IDF_TARGET_ESP32S2
typedef  esp_pm_config_esp32s2_t esp_pm_config_t;
#define DEFAULT_CPU_FREQ_MHZ                CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ
#elif CONFIG_IDF_TARGET_ESP32S3
typedef  esp_pm_config_esp32s3_t esp_pm_config_t;
#define DEFAULT_CPU_FREQ_MHZ                CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ
#endif

static SemaphoreHandle_t DoneSemphr;
static SemaphoreHandle_t StopSemphr;

static void psram_read_write_task(void* arg)
{
    printf("----------SPI0 PSRAM Test----------\n");

    uint8_t *psram_wr_buf = (uint8_t *)heap_caps_malloc(LENGTH_PER_TIME, MALLOC_CAP_32BIT | MALLOC_CAP_SPIRAM);
    if (!psram_wr_buf) {
        printf("no memory\n");
        abort();
    }

    uint8_t *psram_rd_buf = (uint8_t *)heap_caps_malloc(SPI0_PSRAM_TEST_LEN, MALLOC_CAP_32BIT | MALLOC_CAP_SPIRAM);
    if (!psram_rd_buf) {
        printf("no memory\n");
        abort();
    }

    srand(399);
    for (uint32_t loop = 0; loop < (uint32_t)(arg); loop++) {
        for (int i = 0; i < SPI0_PSRAM_TEST_LEN / LENGTH_PER_TIME; i++) {
            for (int j = 0; j < sizeof(psram_wr_buf); j++) {
                psram_wr_buf[j] = rand();
            }
            memcpy(psram_rd_buf + i * LENGTH_PER_TIME, psram_wr_buf, LENGTH_PER_TIME);

            if (memcmp(psram_rd_buf + i * LENGTH_PER_TIME, psram_wr_buf, LENGTH_PER_TIME) != 0) {
                free(psram_rd_buf);
                free(psram_wr_buf);
                abort();
            }
        }
        xSemaphoreGive(DoneSemphr);
        vTaskDelay(10);
    }
    free(psram_rd_buf);
    free(psram_wr_buf);
    vTaskDelete(NULL);
}

static void pm_light_sleep_enable(void)
{
    int cur_freq_mhz = esp_clk_cpu_freq() / MHZ;
    int xtal_freq = esp_clk_xtal_freq() / MHZ;

    esp_pm_config_t pm_config = {
        .max_freq_mhz = cur_freq_mhz,
        .min_freq_mhz = xtal_freq,
        .light_sleep_enable = true
    };
    ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
}

static void pm_light_sleep_disable(void)
{
    int cur_freq_mhz = esp_clk_cpu_freq() / MHZ;

    esp_pm_config_t pm_config = {
        .max_freq_mhz = cur_freq_mhz,
        .min_freq_mhz = cur_freq_mhz,
    };
    ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
}

static void pm_switch_freq(int max_cpu_freq_mhz)
{
    int xtal_freq_mhz = esp_clk_xtal_freq() / MHZ;

    esp_pm_config_t pm_config = {
        .max_freq_mhz = max_cpu_freq_mhz,
        .min_freq_mhz = MIN(max_cpu_freq_mhz, xtal_freq_mhz),
    };
    ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
    printf("Waiting for frequency to be set to %d MHz...\n", max_cpu_freq_mhz);
    while (esp_clk_cpu_freq() / MHZ != max_cpu_freq_mhz)
    {
        vTaskDelay(pdMS_TO_TICKS(200));
        printf("Frequency is %d MHz\n", esp_clk_cpu_freq() / MHZ);
    }
}

static void goto_idle_and_check_stop(uint32_t period)
{
    if (xSemaphoreTake(StopSemphr, pdMS_TO_TICKS(period)) == pdTRUE) {
        pm_switch_freq(DEFAULT_CPU_FREQ_MHZ);
        vSemaphoreDelete(StopSemphr);
        vTaskDelete(NULL);
    }
}

static void pm_switch_task(void *arg)
{
    pm_light_sleep_disable();
    uint32_t period = 100;
    StopSemphr = xSemaphoreCreateBinary();
    while (1) {
        pm_light_sleep_enable();
        goto_idle_and_check_stop(period);
        pm_light_sleep_disable();
        goto_idle_and_check_stop(period);
        pm_switch_freq(10);
        goto_idle_and_check_stop(period);
        pm_switch_freq(80);
        goto_idle_and_check_stop(period);
        pm_switch_freq(40);
        goto_idle_and_check_stop(period);
    }
}

static esp_err_t spi0_psram_test(void)
{
    DoneSemphr = xSemaphoreCreateCounting(1, 0);
    xTaskCreate(psram_read_write_task, "", 2048, (void *)(1), 3, NULL);
    if (xSemaphoreTake(DoneSemphr, pdMS_TO_TICKS(1000)) == pdTRUE) {
        printf(DRAM_STR("----------SPI0 PSRAM Test Success----------\n\n"));
    } else {
        printf(DRAM_STR("----------SPI0 PSRAM Test Timeout----------\n\n"));
        return ESP_FAIL;
    }

    vSemaphoreDelete(DoneSemphr);
    /* Wait for test_task to finish up */
    vTaskDelay(100);
    return ESP_OK;
}

static esp_err_t spi0_psram_with_dfs_test(void)
{
    printf("----------Access SPI0 PSRAM with DFS Test----------\n");

    uint32_t test_loop = 50;
    DoneSemphr = xSemaphoreCreateCounting(test_loop, 0);

    xTaskCreatePinnedToCore(pm_switch_task, "", 4096, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(psram_read_write_task, "", 2048, (void *)(test_loop), 3, NULL, 1);

    int cnt = 0;
    while (cnt < test_loop) {
        if (xSemaphoreTake(DoneSemphr, pdMS_TO_TICKS(1000)) == pdTRUE) {
            cnt++;
        } else {
            vSemaphoreDelete(DoneSemphr);
            printf(DRAM_STR("----------SPI0 PSRAM Test Timeout----------\n\n"));
            return ESP_FAIL;
        }
    }
    xSemaphoreGive(StopSemphr);
    vSemaphoreDelete(DoneSemphr);
    /* Wait for test_task to finish up */
    vTaskDelay(pdMS_TO_TICKS(500));
    printf(DRAM_STR("----------Access SPI0 PSRAM with DFS Test Success----------\n\n"));
    return ESP_OK;
}
#endif


//-----------------------------------------SPI1 FLASH TEST-----------------------------------------------//
#define SPI1_FLASH_TEST_LEN     512
#define SECTOR_LEN              4096
#define SPI1_FLASH_TEST_NUM     (SECTOR_LEN / SPI1_FLASH_TEST_LEN)
#define SPI1_FLASH_TEST_ADDR    0x200000

extern void spi_flash_disable_interrupts_caches_and_other_cpu(void);
extern void spi_flash_enable_interrupts_caches_and_other_cpu(void);
static DRAM_ATTR uint8_t rd_buf[SPI1_FLASH_TEST_LEN];
static DRAM_ATTR uint8_t wr_buf[SPI1_FLASH_TEST_LEN];

static NOINLINE_ATTR IRAM_ATTR esp_err_t spi1_flash_test(void)
{
    printf(DRAM_STR("----------SPI1 Flash Test----------\n"));

    //We need to use SPI1
    spi_flash_disable_interrupts_caches_and_other_cpu();
    uint32_t sector_num = SPI1_FLASH_TEST_ADDR / SECTOR_LEN;
    esp_rom_spiflash_erase_sector(sector_num);
    spi_flash_enable_interrupts_caches_and_other_cpu();

    for (int i = 0; i < SPI1_FLASH_TEST_NUM; i++) {
        for (int j = i + 10; j < SPI1_FLASH_TEST_LEN; j++) {
            wr_buf[j] = j;
        }

        spi_flash_disable_interrupts_caches_and_other_cpu();
        uint32_t test_flash_addr = SPI1_FLASH_TEST_ADDR + i * SPI1_FLASH_TEST_LEN;
        esp_rom_spiflash_write(test_flash_addr, (uint32_t*)wr_buf, SPI1_FLASH_TEST_LEN);
        esp_rom_spiflash_read(test_flash_addr, (uint32_t*)rd_buf, SPI1_FLASH_TEST_LEN);
        spi_flash_enable_interrupts_caches_and_other_cpu();

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
    ESP_ERROR_CHECK(spi0_psram_with_dfs_test());
#endif
    ESP_ERROR_CHECK(spi1_flash_test());

    printf("flash psram test success\n");
}
