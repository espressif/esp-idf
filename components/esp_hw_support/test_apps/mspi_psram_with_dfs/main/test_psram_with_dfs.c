/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_check.h"
#include "esp_attr.h"
#include "esp_flash.h"
#include "esp_partition.h"
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

static SemaphoreHandle_t DoneSemphr;
static SemaphoreHandle_t StopSemphr;

static void psram_read_write_task(void* arg)
{
    printf("----------SPI0 PSRAM Access Test----------\n");

    uint8_t *psram_rd_buf = (uint8_t *)heap_caps_malloc(LENGTH_PER_TIME, MALLOC_CAP_32BIT | MALLOC_CAP_SPIRAM);
    if (!psram_rd_buf) {
        printf("no memory\n");
        abort();
    }

    uint8_t *psram_wr_buf = (uint8_t *)heap_caps_malloc(SPI0_PSRAM_TEST_LEN, MALLOC_CAP_32BIT | MALLOC_CAP_SPIRAM);
    if (!psram_wr_buf) {
        printf("no memory\n");
        abort();
    }

    srand(399);
    for (uint32_t loop = 0; loop < (uint32_t)(arg); loop++) {
        for (int i = 0; i < SPI0_PSRAM_TEST_LEN / LENGTH_PER_TIME; i++) {
            for (int j = 0; j < sizeof(psram_rd_buf); j++) {
                psram_rd_buf[j] = rand();
            }
            memcpy(psram_wr_buf + i * LENGTH_PER_TIME, psram_rd_buf, LENGTH_PER_TIME);

            if (memcmp(psram_wr_buf + i * LENGTH_PER_TIME, psram_rd_buf, LENGTH_PER_TIME) != 0) {
                free(psram_wr_buf);
                free(psram_rd_buf);
                TEST_FAIL_MESSAGE("SPI0 PSRAM Test Fail");
            }
        }
        xSemaphoreGive(DoneSemphr);
        vTaskDelay(10);
    }
    free(psram_wr_buf);
    free(psram_rd_buf);
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
    TEST_ESP_OK( esp_pm_configure(&pm_config) );
}

static void pm_light_sleep_disable(void)
{
    int cur_freq_mhz = esp_clk_cpu_freq() / MHZ;

    esp_pm_config_t pm_config = {
        .max_freq_mhz = cur_freq_mhz,
        .min_freq_mhz = cur_freq_mhz,
    };
    TEST_ESP_OK( esp_pm_configure(&pm_config) );
}

static void pm_switch_freq(int max_cpu_freq_mhz)
{
    int xtal_freq_mhz = esp_clk_xtal_freq() / MHZ;

    esp_pm_config_t pm_config = {
        .max_freq_mhz = max_cpu_freq_mhz,
        .min_freq_mhz = MIN(max_cpu_freq_mhz, xtal_freq_mhz),
    };
    TEST_ESP_OK( esp_pm_configure(&pm_config) );
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
        pm_switch_freq(CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ);
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

TEST_CASE("MSPI: Test_SPI0_PSRAM with DFS", "[mspi]")
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
            TEST_FAIL_MESSAGE(DRAM_STR("SPI0 PSRAM Test Timeout"));
        }
    }
    xSemaphoreGive(StopSemphr);
    vSemaphoreDelete(DoneSemphr);
    /* Wait for test_task to finish up */
    vTaskDelay(pdMS_TO_TICKS(500));
    printf(DRAM_STR("----------Access SPI0 PSRAM with DFS Test Success----------\n\n"));
}
#endif
