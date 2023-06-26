/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * In this example, we show you a way to make an ISR-based callback work during Flash operations, when the ISR-based
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


#define TIMER_RESOLUTION_HZ     (1 * 1000 * 1000) // 1MHz resolution
#define TIMER_ALARM_PERIOD_S    1                 // Alarm period 1s

#define RECORD_TIME_PREPARE()   uint32_t __t1, __t2
#define RECORD_TIME_START()     do {__t1 = esp_cpu_get_cycle_count();} while(0)
#define RECORD_TIME_END(p_time) do{__t2 = esp_cpu_get_cycle_count(); p_time = (__t2 - __t1);} while(0)
#define GET_US_BY_CCOUNT(t)     ((double)(t)/CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ)

const static char *TAG = "Example";
DRAM_ATTR static uint32_t s_t1;
DRAM_ATTR static uint32_t s_flash_func_t2;
DRAM_ATTR static uint32_t s_iram_func_t2;


static NOINLINE_ATTR void s_function_in_flash(void)
{
    /**
     * - Here we will have few instructions in .flash.text
     * - Cache will read from Flash to get the instructions synchronized.
     * - CPU will execute around 400 times.
     */
    for (int i = 0; i < 100; i++) {
        asm volatile("nop");
    }

    s_flash_func_t2 = esp_cpu_get_cycle_count();
}

static IRAM_ATTR NOINLINE_ATTR void s_function_in_iram(void)
{
    /**
     * - Here we will have few instructions in .iram0.text
     * - CPU will execute around 400 times.
     */
    for (int i = 0; i < 100; i++) {
        asm volatile("nop");
    }

    s_iram_func_t2 = esp_cpu_get_cycle_count();
}

static bool IRAM_ATTR on_gptimer_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    bool is_flash = *(bool *)user_ctx;
    s_t1 = esp_cpu_get_cycle_count();

    if (is_flash) {
        s_function_in_flash();
    } else {
        s_function_in_iram();
    }

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

void app_main(void)
{
    //Get the partition used for SPI1 erase operation
    const esp_partition_t *part = s_get_partition();
    ESP_LOGI(TAG, "found partition '%s' at offset 0x%"PRIx32" with size 0x%"PRIx32, part->label, part->address, part->size);
    //Erase whole region
    ESP_ERROR_CHECK(esp_flash_erase_region(part->flash_chip, part->address, part->size));

    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = TIMER_RESOLUTION_HZ,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 1 * 1000 * 1000,
        .flags.auto_reload_on_alarm = false,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = on_gptimer_alarm_cb,
    };
    bool is_flash = true;
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, &is_flash));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));

    uint32_t erase_time = 0;
    RECORD_TIME_PREPARE();

    RECORD_TIME_START();
    ESP_ERROR_CHECK(esp_flash_erase_region(part->flash_chip, part->address, part->size));
    RECORD_TIME_END(erase_time);

    ESP_ERROR_CHECK(gptimer_stop(gptimer));
    ESP_LOGI(TAG, "Flash Driver Erase Operation finishes, duration:\n\t\t%0.2f us", GET_US_BY_CCOUNT(erase_time));
    ESP_LOGI(TAG, "During Erase, ISR callback function(in flash) response time:\n\t\t%0.2f us", GET_US_BY_CCOUNT(s_flash_func_t2 - s_t1));

    //Let the timer alarm callback to run code reside in .iram0.text
    is_flash = false;
    ESP_ERROR_CHECK(gptimer_set_raw_count(gptimer, 0));
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    ESP_ERROR_CHECK(gptimer_start(gptimer));

    RECORD_TIME_START();
    //Erase whole region
    ESP_ERROR_CHECK(esp_flash_erase_region(part->flash_chip, part->address, part->size));
    RECORD_TIME_END(erase_time);

    ESP_ERROR_CHECK(gptimer_stop(gptimer));
    ESP_LOGI(TAG, "Flash Driver Erase Operation finishes, duration:\n\t\t%0.2f us", GET_US_BY_CCOUNT(erase_time));
    ESP_LOGI(TAG, "During Erase, ISR callback function(in iram) response time:\n\t\t%0.2f us", GET_US_BY_CCOUNT(s_iram_func_t2 - s_t1));

    ESP_LOGI(TAG, "Finish");
    ESP_ERROR_CHECK(gptimer_disable(gptimer));
    ESP_ERROR_CHECK(gptimer_del_timer(gptimer));
}
