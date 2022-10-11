/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_cpu.h"
#include "esp_err.h"
#include "esp_partition.h"
#include "esp_flash.h"
#include "esp_timer.h"

static void oneshot_timer_callback(void* arg);
static void cb_in_psram(void);
static void cb_in_iram(void);
static const esp_partition_t *s_get_partition(void);

static int time_cb_start;                    //Time when ISR callback start
static int time_cb_end;                      //Time when ISR callback end

const static char *TAG = "example";

void app_main(void)
{
    bool instructions_in_psram = true;  //Flags to indicate where the instructions in

    esp_timer_handle_t oneshot_timer;
    const esp_timer_create_args_t oneshot_timer_args = {
        .callback = &oneshot_timer_callback,
        .arg = &instructions_in_psram,
        .dispatch_method = ESP_TIMER_ISR,
        .name = "one-shot"
    };
    ESP_ERROR_CHECK(esp_timer_create(&oneshot_timer_args, &oneshot_timer));

    const esp_partition_t *part = s_get_partition();
    ESP_LOGI(TAG, "found partition '%s' at offset 0x%"PRIx32" with size 0x%"PRIx32, part->label, part->address, part->size);
    ESP_ERROR_CHECK(esp_flash_erase_region(part->flash_chip, part->address, part->size));

    ESP_ERROR_CHECK(esp_timer_start_once(oneshot_timer, 1 * 10 * 1000));
    ESP_ERROR_CHECK(esp_flash_erase_region(part->flash_chip, part->address, part->size));

    ESP_LOGI(TAG, "callback(in PSRAM) response time: %d us", time_cb_end - time_cb_start);

    instructions_in_psram = false;

    ESP_ERROR_CHECK(esp_timer_start_once(oneshot_timer, 1 * 10 * 1000));
    ESP_ERROR_CHECK(esp_flash_erase_region(part->flash_chip, part->address, part->size));

    ESP_LOGI(TAG, "callback(in IRAM) response time: %d us", time_cb_end - time_cb_start);

    ESP_ERROR_CHECK(esp_timer_delete(oneshot_timer));
}

static IRAM_ATTR void NOINLINE_ATTR oneshot_timer_callback(void* arg)
{
    bool in_psram = (bool*) arg;
    time_cb_start = esp_timer_get_time();
    if (in_psram == true) {
        cb_in_psram();
     } else {
        cb_in_iram();
    }
}

static IRAM_ATTR NOINLINE_ATTR void cb_in_iram(void)
{
    for (int i = 0; i < 100; i++) {
        asm volatile("nop");
    }
    time_cb_end = esp_timer_get_time();
}

static NOINLINE_ATTR void cb_in_psram(void)
{
    for (int i = 0; i < 100; i++) {
        asm volatile("nop");
    }
    time_cb_end = esp_timer_get_time();
}

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
