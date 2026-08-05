/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_cpu.h"
#include "esp_err.h"
#include "esp_partition.h"
#include "esp_flash.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"

/* Periodic ISR interval. Dirty 64KB erase is typically tens~hundreds of ms,
 * so 1ms yields well above 10 IRQs per erase on supported targets. */
#define EXAMPLE_TIMER_INTERVAL_US    (1000)

static void periodic_timer_callback(void* arg);
static void cb_in_psram(void);
static void cb_in_iram(void);
static const esp_partition_t *s_get_partition(void);
static esp_err_t s_prepare_partition_for_erase(const esp_partition_t *part);
static esp_err_t s_run_callbacks_during_erase(esp_timer_handle_t timer, bool instructions_in_psram,
                                              const esp_partition_t *part);

static volatile uint32_t s_irq_count;

const static char *TAG = "example";

void app_main(void)
{
    bool instructions_in_psram = true;

    esp_timer_handle_t periodic_timer;
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &periodic_timer_callback,
        .arg = &instructions_in_psram,
        .dispatch_method = ESP_TIMER_ISR,
        .name = "periodic",
        .skip_unhandled_events = true,
    };
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));

    const esp_partition_t *part = s_get_partition();
    ESP_LOGI(TAG, "found partition '%s' at offset 0x%"PRIx32" with size 0x%"PRIx32, part->label, part->address, part->size);

    instructions_in_psram = true;
    ESP_ERROR_CHECK(s_run_callbacks_during_erase(periodic_timer, true, part));

    instructions_in_psram = false;
    ESP_ERROR_CHECK(s_run_callbacks_during_erase(periodic_timer, false, part));

    ESP_ERROR_CHECK(esp_timer_delete(periodic_timer));
}

static esp_err_t s_prepare_partition_for_erase(const esp_partition_t *part)
{
    /* Program non-0xFF data so erase cannot take the already-erased fast path. */
    static uint8_t write_buf[256];
    memset(write_buf, 0xA5, sizeof(write_buf));

    for (uint32_t offset = 0; offset < part->size; offset += sizeof(write_buf)) {
        uint32_t write_size = part->size - offset;
        if (write_size > sizeof(write_buf)) {
            write_size = sizeof(write_buf);
        }
        esp_err_t err = esp_flash_write(part->flash_chip, write_buf, part->address + offset, write_size);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "failed to program partition before erase: %s", esp_err_to_name(err));
            return err;
        }
    }
    return ESP_OK;
}

static esp_err_t s_run_callbacks_during_erase(esp_timer_handle_t timer, bool instructions_in_psram,
                                              const esp_partition_t *part)
{
    const char *where = instructions_in_psram ? "PSRAM" : "IRAM";

    esp_err_t err = s_prepare_partition_for_erase(part);
    if (err != ESP_OK) {
        return err;
    }

    s_irq_count = 0;

    const int64_t t_start_us = esp_timer_get_time();
    err = esp_timer_start_periodic(timer, EXAMPLE_TIMER_INTERVAL_US);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "start periodic timer failed: %s", esp_err_to_name(err));
        return err;
    }

    esp_err_t erase_err = esp_flash_erase_region(part->flash_chip, part->address, part->size);

    /* Stop immediately after erase returns so later IRQs are not counted. */
    err = esp_timer_stop_blocking(timer, portMAX_DELAY);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "stop periodic timer failed: %s", esp_err_to_name(err));
        return err;
    }
    const int64_t t_end_us = esp_timer_get_time();

    const uint32_t irq_count = s_irq_count;
    const double duration_ms = (double)(t_end_us - t_start_us) / 1000.0;
    const double interval_ms = (double)EXAMPLE_TIMER_INTERVAL_US / 1000.0;
    const double expected = (double)(t_end_us - t_start_us) / (double)EXAMPLE_TIMER_INTERVAL_US;

    ESP_LOGI(TAG,
             "erase with callback in %s: duration_ms=%.3f, interval_ms=%.3f, irq_count=%"PRIu32", expected=%.2f",
             where, duration_ms, interval_ms, irq_count, expected);

    if (erase_err != ESP_OK) {
        ESP_LOGE(TAG, "flash erase failed: %s", esp_err_to_name(erase_err));
        return erase_err;
    }

    if ((double)irq_count < 0.8 * expected) {
        ESP_LOGE(TAG, "irq_count=%"PRIu32" < 0.8 * expected(%.2f) during erase (callback in %s)",
                 irq_count, expected, where);
        return ESP_FAIL;
    }

    return ESP_OK;
}

static IRAM_ATTR void NOINLINE_ATTR periodic_timer_callback(void* arg)
{
    bool in_psram = *(bool*) arg;
    if (in_psram == true) {
        cb_in_psram();
    } else {
        cb_in_iram();
    }
    s_irq_count++;
}

static IRAM_ATTR NOINLINE_ATTR void cb_in_iram(void)
{
    for (int i = 0; i < 100; i++) {
        asm volatile("nop");
    }
}

static NOINLINE_ATTR void cb_in_psram(void)
{
    for (int i = 0; i < 100; i++) {
        asm volatile("nop");
    }
}

static const esp_partition_t *s_get_partition(void)
{
    const esp_partition_t *result = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage1");
    if (!result) {
        ESP_LOGE(TAG, "Can't find the partition, please define it correctly in `partitions.csv`");
        abort();
    }
    return result;
}
