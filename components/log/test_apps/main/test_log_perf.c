/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <inttypes.h>
#include "sys/param.h"
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_rom_uart.h"
#include "sdkconfig.h"

typedef struct {
    SemaphoreHandle_t done;
    int stack_usage;
    int (*func)(void);
} perf_log_test_t;

static const char * TAG = "test";

const int max_loops = 5;

static void print_via_vprintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

static void print_via_esp_rom_vprintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    esp_rom_vprintf(format, args);
    va_end(args);
}

static int test_stack_usage_esp_rom_printf(void)
{
    UBaseType_t start, end;
    start = uxTaskGetStackHighWaterMark(NULL);
    esp_rom_printf("test msg with value %d\n", 1);
    end = uxTaskGetStackHighWaterMark(NULL);
    return start - end;
}

static int test_stack_usage_esp_rom_vprintf(void)
{
    UBaseType_t start, end;
    start = uxTaskGetStackHighWaterMark(NULL);
    print_via_esp_rom_vprintf("test msg with value %d\n", 2);
    end = uxTaskGetStackHighWaterMark(NULL);
    return start - end;
}

static int test_stack_usage_early_log(void)
{
    UBaseType_t start, end;
    start = uxTaskGetStackHighWaterMark(NULL);
    ESP_EARLY_LOGI(TAG, "test msg with value %d", 3);
    end = uxTaskGetStackHighWaterMark(NULL);
    return start - end;
}

static int test_stack_usage_dram_log(void)
{
    UBaseType_t start, end;
    start = uxTaskGetStackHighWaterMark(NULL);
    ESP_DRAM_LOGI(TAG, "test msg with value %d", 4);
    end = uxTaskGetStackHighWaterMark(NULL);
    return start - end;
}

static int test_stack_usage_printf(void)
{
    UBaseType_t start, end;
    start = uxTaskGetStackHighWaterMark(NULL);
    printf("test msg with value %d\n", 5);
    end = uxTaskGetStackHighWaterMark(NULL);
    return start - end;
}

static int test_stack_usage_vprintf(void)
{
    UBaseType_t start, end;
    start = uxTaskGetStackHighWaterMark(NULL);
    print_via_vprintf("test msg with value %d\n", 6);
    end = uxTaskGetStackHighWaterMark(NULL);
    return start - end;
}

static int test_stack_usage_esp_log(void)
{
    UBaseType_t start, end;
    start = uxTaskGetStackHighWaterMark(NULL);
    ESP_LOGI(TAG, "test msg with value %d", 7);
    end = uxTaskGetStackHighWaterMark(NULL);
    return start - end;
}

static void log_stack_usage_task(void* arg)
{
    perf_log_test_t *a = (perf_log_test_t *) arg;

    const uint32_t mask = esp_cpu_intr_get_enabled_mask();
    esp_cpu_intr_disable(0xFFFFFFFF);
    a->stack_usage = a->func();
    esp_cpu_intr_enable(mask);

    TEST_ASSERT(xSemaphoreGive(a->done));
    vTaskDelete(NULL);
}

TEST_CASE("Stack usage for log APIs", "[log]")
{
    int priority = uxTaskPriorityGet(NULL) + 1;
    perf_log_test_t args = {
        .done = xSemaphoreCreateBinary(),
        .stack_usage = 0,
        .func = NULL,
    };

    typedef struct {
        const char *name;
        int (*func)(void);
        int stack_usages;
    } log_test_case_t;

    log_test_case_t test_cases[] = {
        {"esp_rom_printf", test_stack_usage_esp_rom_printf, 0},
        {"esp_rom_vprintf", test_stack_usage_esp_rom_vprintf, 0},
        {"early_log", test_stack_usage_early_log, 0},
        {"dram_log", test_stack_usage_dram_log, 0},
        {"printf", test_stack_usage_printf, 0},
        {"vprintf", test_stack_usage_vprintf, 0},
        {"esp_log", test_stack_usage_esp_log, 0},
    };

    for (int i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
        args.func = test_cases[i].func;
        TEST_ASSERT(xTaskCreatePinnedToCore(log_stack_usage_task, test_cases[i].name, 4095, &args, priority, NULL, 0));
        TEST_ASSERT(xSemaphoreTake(args.done, 100 / portTICK_PERIOD_MS));
        test_cases[i].stack_usages = args.stack_usage;
    }

    printf("\nStack usage for %s chip:\n", CONFIG_IDF_TARGET);
    for (int i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
        printf("%-20s %d bytes\n", test_cases[i].name, test_cases[i].stack_usages);
    }

    vSemaphoreDelete(args.done);
}

static void mock_putc(char c)
{

}

static int mock_vprintf(const char *format, va_list args)
{
    return 0;
}

static int measuring_execution_time_esp_rom_printf(bool without_output)
{
    int diff;
    int64_t start, end;
    diff = INT32_MAX;
    for (int i = 0; i < max_loops; i++) {
        start = esp_timer_get_time();
        if (without_output) {
            esp_rom_printf("\r");
        } else {
            esp_rom_printf("test msg %d\n", i);
        }
        end = esp_timer_get_time();
        diff = MIN(diff, end - start);
    }
    return diff;
}

static int measuring_execution_time_esp_rom_vprintf(bool without_output)
{
    int diff;
    int64_t start, end;
    diff = INT32_MAX;
    for (int i = 0; i < max_loops; i++) {
        start = esp_timer_get_time();
        if (without_output) {
            print_via_esp_rom_vprintf("\r");
        } else {
            print_via_esp_rom_vprintf("test msg %d\n", i);
        }
        end = esp_timer_get_time();
        diff = MIN(diff, end - start);
    }
    return diff;
}

static int measuring_execution_time_early_log(bool without_output)
{
    int diff;
    int64_t start, end;
    diff = INT32_MAX;
    if (without_output) {
        esp_rom_install_channel_putc(1, mock_putc);
    }
    for (int i = 0; i < max_loops; i++) {
        start = esp_timer_get_time();
        if (without_output) {
            ESP_EARLY_LOGI(TAG, "");
        } else {
            ESP_EARLY_LOGI(TAG, "test msg %d", i);
        }
        end = esp_timer_get_time();
        diff = MIN(diff, end - start);
    }
    if (without_output) {
        esp_rom_install_channel_putc(1, esp_rom_output_putc);
    }
    return diff;
}

static int measuring_execution_time_dram_log(bool without_output)
{
    int diff;
    int64_t start, end;
    diff = INT32_MAX;
    if (without_output) {
        esp_rom_install_channel_putc(1, mock_putc);
    }
    for (int i = 0; i < max_loops; i++) {
        start = esp_timer_get_time();
        if (without_output) {
            ESP_DRAM_LOGI(TAG, "");
        } else {
            ESP_DRAM_LOGI(TAG, "test msg %d", i);
        }
        end = esp_timer_get_time();
        diff = MIN(diff, end - start);
    }
    if (without_output) {
        esp_rom_install_channel_putc(1, esp_rom_output_putc);
    }
    return diff;
}

static int measuring_execution_time_printf(bool without_output)
{
    int diff;
    int64_t start, end;
    diff = INT32_MAX;
    for (int i = 0; i < max_loops; i++) {
        start = esp_timer_get_time();
        if (without_output) {
            printf("\r");
        } else {
            printf("test msg %d\n", i);
        }
        end = esp_timer_get_time();
        diff = MIN(diff, end - start);
    }
    return diff;
}

static int measuring_execution_time_vprintf(bool without_output)
{
    int diff;
    int64_t start, end;
    diff = INT32_MAX;
    for (int i = 0; i < max_loops; i++) {
        start = esp_timer_get_time();
        if (without_output) {
            print_via_vprintf("\r");
        } else {
            print_via_vprintf("test msg %d\n", i);
        }
        end = esp_timer_get_time();
        diff = MIN(diff, end - start);
    }
    return diff;
}

static int measuring_execution_time_esp_log_macro(bool without_output)
{
    int diff;
    int64_t start, end;
    diff = INT32_MAX;
    vprintf_like_t original_vprintf;
    if (without_output) {
        original_vprintf = esp_log_set_vprintf(mock_vprintf);
    }
    for (int i = 0; i < max_loops; i++) {
        start = esp_timer_get_time();
        if (without_output) {
            ESP_LOGI(TAG, "\r");
        } else {
            ESP_LOGI(TAG, "test msg %d", i);
        }
        end = esp_timer_get_time();
        diff = MIN(diff, end - start);
    }
    if (without_output) {
        esp_log_set_vprintf(original_vprintf);
    }
    return diff;
}

TEST_CASE("Performance measurement for log APIs", "[log]")
{
#if ESP_LOG_VERSION == 2
    const int count_prints = 3;
#else
    const int count_prints = 1;
#endif

    const uint32_t mask = esp_cpu_intr_get_enabled_mask();
    esp_cpu_intr_disable(0xFFFFFFFF);

    int m_esp_rom_printf = measuring_execution_time_esp_rom_printf(true);
    int m_esp_rom_printf_output = measuring_execution_time_esp_rom_printf(false);

    int m_esp_rom_vprintf = measuring_execution_time_esp_rom_vprintf(true);
    int m_esp_rom_vprintf_output = measuring_execution_time_esp_rom_vprintf(false);

    int m_early_log = measuring_execution_time_early_log(true);
    int m_early_log_output = measuring_execution_time_early_log(false);

    int m_dram_log_printf = measuring_execution_time_dram_log(true);
    int m_dram_log_printf_output = measuring_execution_time_dram_log(false);

    int m_printf = measuring_execution_time_printf(true);
    int m_printf_output = measuring_execution_time_printf(false);

    int m_vprintf = measuring_execution_time_vprintf(true);
    int m_vprintf_output = measuring_execution_time_vprintf(false);

    int m_esp_log_macro = measuring_execution_time_esp_log_macro(true);
    int m_esp_log_macro_output = measuring_execution_time_esp_log_macro(false);

    esp_cpu_intr_enable(mask);

    esp_rom_printf("\nPerformance measurements for %s chip:\n", CONFIG_IDF_TARGET);
    esp_rom_printf("Function         w/o output (us) | w/ output (us)\n");
    esp_rom_printf("esp_rom_printf   %d                 %d\n", m_esp_rom_printf, m_esp_rom_printf_output);
    esp_rom_printf("esp_rom_vprintf  %d                 %d\n", m_esp_rom_vprintf, m_esp_rom_vprintf_output);
    esp_rom_printf("ESP_EARLY_LOGI   %d + %d            %d\n", m_early_log, count_prints * m_esp_rom_printf, m_early_log_output);
    esp_rom_printf("ESP_DRAM_LOGI    %d + %d            %d\n", m_dram_log_printf, count_prints * m_esp_rom_vprintf, m_dram_log_printf_output);
    esp_rom_printf("printf           %d                %d\n", m_printf, m_printf_output);
    esp_rom_printf("vprintf          %d                %d\n", m_vprintf, m_vprintf_output);
    esp_rom_printf("ESP_LOGI         %d + %d           %d\n", m_esp_log_macro, count_prints * m_vprintf, m_esp_log_macro_output);
}
