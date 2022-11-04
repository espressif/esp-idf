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
#include "unity.h"
#include "esp_heap_caps.h"
#include "esp_private/esp_psram_io.h"
#include "esp_psram.h"
#include "esp_private/esp_psram_extram.h"
#include "esp_flash.h"
#include "esp_partition.h"

__attribute__((unused)) const static char *TAG = "PSRAM";


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
    ESP_LOGI(TAG, "test ext buffer start addr is 0x%x, end addr is 0x%x", start, end);
    TEST_ASSERT(esp_psram_check_ptr_addr((void *)start) && esp_psram_check_ptr_addr((void *)end));

    for (int i = 0; i < TEST_ALLOC_SIZE / sizeof(uint32_t); i++) {
        ext_buffer[i] = (i + 1) ^ 0xaaaaaaaa;
    }

    for (int i = 0; i < TEST_ALLOC_SIZE / sizeof(uint32_t); i++) {
        TEST_ASSERT(ext_buffer[i] == ((i + 1) ^ 0xaaaaaaaa));
    }

    free(ext_buffer);
}


#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS && CONFIG_SPIRAM_RODATA
#include "esp_partition.h"
#include "driver/gptimer.h"
#include "esp_rom_spiflash.h"

#define SECTOR_LEN              4096
#define TEST_NUM                10
#define TEST_BUF                {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9}

static uint32_t s_timer_cb_exe_times;
static const uint8_t s_test_buf[TEST_NUM] = TEST_BUF;

static const esp_partition_t *s_get_partition(void)
{
    //Find the "storage1" partition defined in `partitions.csv`
    const esp_partition_t *result = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage1");
    if (!result) {
        ESP_LOGE(TAG, "Can't find the partition, please define it correctly in `partitions.csv`");
        abort();
    }
    return result;
}

static bool NOINLINE_ATTR s_test_rodata(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    s_timer_cb_exe_times ++;
    uint8_t cmp_buf[TEST_NUM] = TEST_BUF;
    TEST_ASSERT(memcmp(cmp_buf, s_test_buf, TEST_NUM) == 0);

    return false;
}

TEST_CASE("test spi1 flash operation after putting .text and .rodata into psram", "[psram]")
{
    //Get the partition used for SPI1 erase operation
    const esp_partition_t *part = s_get_partition();
    ESP_LOGI(TAG, "found partition '%s' at offset 0x%x with size 0x%x", part->label, part->address, part->size);
    //Erase whole region
    TEST_ESP_OK(esp_flash_erase_region(part->flash_chip, part->address, part->size));

    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .resolution_hz = 1 * 1000 * 1000,
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &gptimer));

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 10, // 10us
        .flags.auto_reload_on_alarm = true,
    };
    TEST_ESP_OK(gptimer_set_alarm_action(gptimer, &alarm_config));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = s_test_rodata,
    };
    TEST_ESP_OK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));

    esp_rom_spiflash_result_t ret;
    size_t start = part->address;
    ESP_LOGI(TAG, "test data partition: 0x%x", start);
    uint32_t sector_num = start / SECTOR_LEN;

    TEST_ESP_OK(gptimer_enable(gptimer));
    TEST_ESP_OK(gptimer_start(gptimer));

    ret = esp_rom_spiflash_erase_sector(sector_num);
    if (ret != ESP_ROM_SPIFLASH_RESULT_OK) {
        ESP_LOGE(TAG, "erase fail!");
        TEST_ASSERT(false);
    }

    TEST_ESP_OK(gptimer_stop(gptimer));
    TEST_ASSERT(s_timer_cb_exe_times > 0);
    printf("timer callback runs %d times\n", s_timer_cb_exe_times);

    ESP_LOGI(TAG, "Finish");
    TEST_ESP_OK(gptimer_disable(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));
}
#endif  //CONFIG_SPIRAM_FETCH_INSTRUCTIONS && CONFIG_SPIRAM_RODATA
