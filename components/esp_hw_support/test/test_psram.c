/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <sys/param.h>
#include <string.h>
#include "esp_log.h"
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "test_utils.h"
#include "unity.h"
#include "esp_heap_caps.h"
#if CONFIG_SPIRAM
#include "spiram.h"

const static char *TAG = "PSRAM";

#if CONFIG_SPIRAM_MODE_OCT
#define TEST_ALLOC_SIZE    (4 * 1024 * 1024)
#else
#define TEST_ALLOC_SIZE    (1 * 1024 * 1024)
#endif

TEST_CASE("test psram heap allocable","[psram]")
{
    uint32_t *ext_buffer = (uint32_t *)heap_caps_calloc(TEST_ALLOC_SIZE, 1, MALLOC_CAP_SPIRAM);
    TEST_ASSERT(ext_buffer);

    uintptr_t start = (uintptr_t)ext_buffer;
    uintptr_t end = (uintptr_t)ext_buffer + TEST_ALLOC_SIZE;
    ESP_LOGI(TAG, "test ext buffer start addr is %x, end addr is %x", start, end);
    TEST_ASSERT((start >= SOC_EXTRAM_DATA_LOW) && (end <= SOC_EXTRAM_DATA_HIGH));


    for (int i = 0; i < TEST_ALLOC_SIZE / sizeof(uint32_t); i++) {
        ext_buffer[i] = (i + 1) ^ 0xaaaaaaaa;
    }

    for (int i = 0; i < TEST_ALLOC_SIZE / sizeof(uint32_t); i++) {
        TEST_ASSERT(ext_buffer[i] == ((i + 1) ^ 0xaaaaaaaa));
    }

    free(ext_buffer);
}

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS && CONFIG_SPIRAM_RODATA
#include "esp_timer.h"
#include "esp32s3/rom/spi_flash.h"

#define SECTOR_LEN              4096
#define TEST_NUM                10
#define TEST_BUF                {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9}

static uint32_t s_timer_cb_exe_times;
static const uint8_t s_test_buf[TEST_NUM] = TEST_BUF;

static void NOINLINE_ATTR s_test_printf(void *arg)
{
    s_timer_cb_exe_times ++;
    uint8_t cmp_buf[TEST_NUM] = TEST_BUF;
    TEST_ASSERT(memcmp(cmp_buf, s_test_buf, TEST_NUM) == 0);
}

TEST_CASE("test spi1 flash operation after putting .text and .rodata into psram", "[psram]")
{
    //Create flash partition for test
    const esp_partition_t *part = get_test_data_partition();
    size_t start = part->address;
    ESP_LOGI(TAG, "test data partition: 0x%x", start);

    esp_timer_handle_t timer;
    esp_timer_create_args_t timer_args = {
        .callback = &s_test_printf,
    };
    TEST_ESP_OK(esp_timer_create(&timer_args, &timer));

    esp_rom_spiflash_result_t ret;
    uint32_t sector_num = start / SECTOR_LEN;
    TEST_ESP_OK(esp_timer_start_periodic(timer, 1 * 10));

    ret = esp_rom_spiflash_erase_sector(sector_num);
    if (ret != ESP_ROM_SPIFLASH_RESULT_OK) {
        ESP_LOGE(TAG, "erase fail!");
        TEST_ASSERT(false);
    }

    TEST_ESP_OK(esp_timer_stop(timer));
    TEST_ASSERT(s_timer_cb_exe_times > 0);
    printf("timer callback runs %d times\n", s_timer_cb_exe_times);
}
#endif  //CONFIG_SPIRAM_FETCH_INSTRUCTIONS && CONFIG_SPIRAM_RODATA

#endif  //#if CONFIG_SPIRAM
