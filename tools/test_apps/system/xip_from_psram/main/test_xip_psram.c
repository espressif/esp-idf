/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include <string.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "esp_heap_caps.h"
#include "esp_flash.h"
#include "esp_partition.h"

__attribute__((unused)) const static char *TAG = "PSRAM";

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS && CONFIG_SPIRAM_RODATA
#include "esp_partition.h"
#include "esp_timer.h"
#include "esp32s3/rom/spi_flash.h"

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

static void NOINLINE_ATTR s_test_rodata(void* arg)
{
    s_timer_cb_exe_times ++;
    uint8_t cmp_buf[TEST_NUM] = TEST_BUF;
    TEST_ASSERT(memcmp(cmp_buf, s_test_buf, TEST_NUM) == 0);
}

void test_spi1_flash_with_xip_psram(void)
{
    //Get the partition used for SPI1 erase operation
    const esp_partition_t *part = s_get_partition();
    ESP_LOGI(TAG, "found partition '%s' at offset 0x%"PRIx32" with size 0x%"PRIx32, part->label, part->address, part->size);
    //Erase whole region
    TEST_ESP_OK(esp_flash_erase_region(part->flash_chip, part->address, part->size));


    esp_timer_handle_t oneshot_timer;
    const esp_timer_create_args_t oneshot_timer_args = {
        .callback = &s_test_rodata,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_ISR,
        .name = "one-shot"
    };
    ESP_ERROR_CHECK(esp_timer_create(&oneshot_timer_args, &oneshot_timer));

    esp_rom_spiflash_result_t ret;
    uint32_t start = part->address;
    ESP_LOGI(TAG, "test data partition: 0x%"PRIx32, start);
    uint32_t sector_num = start / SECTOR_LEN;

    ESP_ERROR_CHECK(esp_timer_start_periodic(oneshot_timer, 1 * 10 * 1000));
    ret = esp_rom_spiflash_erase_sector(sector_num);
    if (ret != ESP_ROM_SPIFLASH_RESULT_OK) {
        ESP_LOGE(TAG, "erase fail!");
        TEST_ASSERT(false);
    }

    TEST_ASSERT(s_timer_cb_exe_times > 0);
    printf("timer callback runs %"PRId32" times\n", s_timer_cb_exe_times);

    ESP_LOGI(TAG, "Finish");
    ESP_ERROR_CHECK(esp_timer_stop(oneshot_timer));
    ESP_ERROR_CHECK(esp_timer_delete(oneshot_timer));
}

void app_main(void)
{
	test_spi1_flash_with_xip_psram();
}
#endif  //CONFIG_SPIRAM_FETCH_INSTRUCTIONS && CONFIG_SPIRAM_RODATA
