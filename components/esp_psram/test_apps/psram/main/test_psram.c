/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <sys/param.h>
#include <string.h>
#include "inttypes.h"
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

TEST_CASE("test psram heap allocable", "[psram]")
{
    size_t largest_size = heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM);
    ESP_LOGI(TAG, "largest size is %zu", largest_size);

    uint32_t *ext_buffer = (uint32_t *)heap_caps_calloc(largest_size, 1, MALLOC_CAP_SPIRAM);
    TEST_ASSERT(ext_buffer);

    intptr_t start = (intptr_t)ext_buffer;
    intptr_t end = (intptr_t)ext_buffer + largest_size;
    ESP_LOGI(TAG, "test ext buffer start addr is 0x%"PRIxPTR", end addr is 0x%"PRIxPTR, start, end);
    TEST_ASSERT(esp_psram_check_ptr_addr((void *)start) && esp_psram_check_ptr_addr((void *)end));

    for (int i = 0; i < largest_size / sizeof(uint32_t); i++) {
        ext_buffer[i] = (i + 1) ^ 0xaaaaaaaa;
    }

    for (int i = 0; i < largest_size / sizeof(uint32_t); i++) {
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
    ESP_LOGI(TAG, "found partition '%s' at offset 0x%"PRIx32" with size 0x%"PRIx32, part->label, part->address, part->size);
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
    uint32_t start = part->address;
    ESP_LOGI(TAG, "test data partition: 0x%"PRIx32, start);
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
    printf("timer callback runs %"PRId32" times\n", s_timer_cb_exe_times);

    ESP_LOGI(TAG, "Finish");
    TEST_ESP_OK(gptimer_disable(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));
}
#endif  //CONFIG_SPIRAM_FETCH_INSTRUCTIONS && CONFIG_SPIRAM_RODATA

TEST_CASE("test psram unaligned access", "[psram]")
{
    size_t largest_size = heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    ESP_LOGI(TAG, "largest size is %zu", largest_size);

    uint8_t *ext_buffer = (uint8_t *)heap_caps_calloc(largest_size, 1, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    for (int i = 0; i < largest_size; i++) {
        ext_buffer[i] = i & 0xff;
    }

    for (int i = 0; i < largest_size - 4; i += 4) {

        uint8_t *ptr_base = (uint8_t *)(ext_buffer + i);

        for (int j = 1; j < 4; j++) {
            uint8_t *unaligned_ptr = (uint8_t *)(ptr_base + j);
            ESP_LOGV(TAG, "i is %d, j is %d, unaligned_ptr addr is %p", i, j, unaligned_ptr);

            uint8_t val_8bit = *unaligned_ptr;
            ESP_LOGV(TAG, "i is %d, j is %d, val_8bit val  is 0x%"PRIx8, i, j, val_8bit);
            uint8_t first_byte = (i + j) & 0xff;
            uint8_t expected_val_8bit = first_byte;
            TEST_ASSERT(val_8bit == expected_val_8bit);

            /**
             * If the vaddr doesn't support unaligned access, below codes will generate `LoadStoreAlignment` error.
             *
             * This is because below lines includes 16-bit load and 32-bit load:
             * - l16ui
             * - l32i.n
             *
             * Whereas we use an `add.n` to adding an offset (from 0 to 3) to the original buffer address.
             *
             * Therefore we get unaligned access
             */

            uint16_t val_16bit = *(uint16_t *)unaligned_ptr;
            ESP_LOGV(TAG, "i is %d, j is %d, val_16bit val  is 0x%"PRIx16, i, j, val_16bit);
            uint32_t val_32bit = *(uint32_t *)unaligned_ptr;
            ESP_LOGV(TAG, "i is %d, j is %d, val_32bit val  is 0x%"PRIx32, i, j, val_32bit);

            uint8_t second_byte = ((i + j) & 0xff) + 1;
            uint8_t third_byte = ((i + j) & 0xff) + 2;
            uint8_t fourth_byte = ((i + j) & 0xff) + 3;

            uint16_t expected_val_16bit = (second_byte << 8) | first_byte;
            ESP_LOGV(TAG, "i is %d, j is %d, expected_val_16bit val  is 0x%"PRIx16, i, j, expected_val_16bit);
            TEST_ASSERT(val_16bit == expected_val_16bit);
            uint32_t expected_val_32bit = (fourth_byte << 24) | (third_byte << 16) | (second_byte << 8) | first_byte;
            ESP_LOGV(TAG, "i is %d, j is %d, expected_val_32bit val  is 0x%" PRIx32, i, j, expected_val_32bit);
            TEST_ASSERT(val_32bit == expected_val_32bit);
        }
    }

    heap_caps_free(ext_buffer);
}
