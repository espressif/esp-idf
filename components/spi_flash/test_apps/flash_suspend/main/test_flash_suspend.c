/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * In this test, we show you a way to make an ISR-based callback work during Flash operations, when the ISR-based
 * callback is put in Flash.
 *
 * Please read the README.md to know more details about this feature!
 */

#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_cpu.h"
#include "esp_partition.h"
#include "driver/gptimer.h"
#include "esp_flash.h"
#include "hal/gpio_hal.h"
#include "rom/cache.h"

#include "test_utils.h"

#define TIMER_RESOLUTION_HZ     (1 * 1000 * 1000) // 1MHz resolution
#define TIMER_ALARM_PERIOD_S    1                 // Alarm period 1s

#define RECORD_TIME_PREPARE()   uint32_t __t1, __t2
#define RECORD_TIME_START()     do {__t1 = esp_cpu_get_cycle_count();} while(0)
#define RECORD_TIME_END(p_time) do{__t2 = esp_cpu_get_cycle_count(); p_time = (__t2 - __t1);} while(0)
#define GET_US_BY_CCOUNT(t)     ((double)(t)/CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ)

const static char *TAG = "flash_suspend test";
DRAM_ATTR static uint32_t s_flash_func_t1;
DRAM_ATTR static uint32_t s_flash_func_t2;
DRAM_ATTR static uint32_t s_flash_func_time;
DRAM_ATTR static uint32_t s_isr_t1;
DRAM_ATTR static uint32_t s_isr_t2;
DRAM_ATTR static uint32_t s_isr_time;
DRAM_ATTR static uint32_t s_isr_interval_t1;
DRAM_ATTR static uint32_t s_isr_interval_t2;
DRAM_ATTR static uint32_t s_isr_interval_time;
DRAM_ATTR static uint32_t times = 0;


static NOINLINE_ATTR void func_in_flash(void)
{
    /**
     * - Here we will have few instructions in .flash.text
     * - Cache will read from Flash to get the instructions synchronized.
     * - CPU will execute around 90000 times.
     */

    for (int i = 0; i < 50000; i++) {
        asm volatile("nop");
    }

    s_flash_func_t2 = esp_cpu_get_cycle_count();
}

static bool IRAM_ATTR gptimer_alarm_suspend_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    s_isr_t1 = esp_cpu_get_cycle_count();
    if (s_isr_interval_t1 != 0 ) {
        s_isr_interval_t2 = esp_cpu_get_cycle_count();
        s_isr_interval_time += (s_isr_interval_t2 - s_isr_interval_t1);
    }
    s_isr_interval_t1 = esp_cpu_get_cycle_count();

    /*clear content in cache*/
#if CONFIG_IDF_TARGET_ESP32S3
    Cache_Invalidate_DCache_All();
#endif
    Cache_Invalidate_ICache_All();
    s_flash_func_t1 = esp_cpu_get_cycle_count();
    func_in_flash();

    s_flash_func_time += (s_flash_func_t2 - s_flash_func_t1);
    times++;
    s_isr_t2 = esp_cpu_get_cycle_count();
    s_isr_time += (s_isr_t2 - s_isr_t1);
    return false;
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

static const uint8_t large_const_buffer[16400] = {
    203, // first byte
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
    [50 ... 99] = 2,
    [1600 ... 2000] = 3,
    [8000 ... 9000] = 77,
    [15000 ... 16398] = 8,
    43 // last byte
};

TEST_CASE("flash suspend test", "[spi_flash][suspend]")
{
    //Get the partition used for SPI1 erase operation
    const esp_partition_t *part = s_get_partition();
    ESP_LOGI(TAG, "found partition '%s' at offset 0x%"PRIx32" with size 0x%"PRIx32, part->label, part->address, part->size);
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = TIMER_RESOLUTION_HZ,
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &gptimer));

    /**
    set alarm_count is 2500.
    So, in this case, ISR duration time is around 2240 and ISR interval time is around 2470
    so ISR_interval - ISR_duration is around 230us. Just a little bit larger than `tsus` value.
    */
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 2500,
        .flags.auto_reload_on_alarm = true,
    };
    TEST_ESP_OK(gptimer_set_alarm_action(gptimer, &alarm_config));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = gptimer_alarm_suspend_cb,
    };
    bool is_flash = true;
    TEST_ESP_OK(gptimer_register_event_callbacks(gptimer, &cbs, &is_flash));
    TEST_ESP_OK(gptimer_enable(gptimer));
    TEST_ESP_OK(gptimer_start(gptimer));

    uint32_t erase_time = 0;
    RECORD_TIME_PREPARE();

    RECORD_TIME_START();
    TEST_ESP_OK(esp_flash_erase_region(part->flash_chip, part->address, part->size));
    TEST_ESP_OK(esp_flash_write(part->flash_chip, large_const_buffer, part->address, sizeof(large_const_buffer)));

    RECORD_TIME_END(erase_time);

    TEST_ESP_OK(gptimer_stop(gptimer));
    ESP_LOGI(TAG, "Flash Driver Erase Operation finishes, duration:\n\t\t%0.2f us", GET_US_BY_CCOUNT(erase_time));
    ESP_LOGI(TAG, "During Erase, ISR callback function(in flash) response time:\n\t\t%0.2f us", GET_US_BY_CCOUNT(s_flash_func_time / times));
    ESP_LOGI(TAG, "During Erase, ISR duration time:\n\t\t%0.2f us", GET_US_BY_CCOUNT(s_isr_time / times));
    ESP_LOGI(TAG, "During Erase, ISR interval time:\n\t\t%0.2f us", GET_US_BY_CCOUNT(s_isr_interval_time / (times - 1)));


    ESP_LOGI(TAG, "Finish");
    TEST_ESP_OK(gptimer_disable(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));
}
