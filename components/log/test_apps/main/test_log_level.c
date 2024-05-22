/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "unity.h"
#include "esp_rom_uart.h"
#include "sdkconfig.h"

/*
 * The debug logs in this file will be compiled into a binary file,
 * even though CONFIG_LOG_DEFAULT_LEVEL and CONFIG_LOG_MAXIMUM_LEVEL
 * are at INFO level.
 */
#define LOG_LOCAL_LEVEL (ESP_LOG_DEBUG)
#include "esp_log.h"

static const char * TAG1 = "ESP_LOG";
static const char * TAG2 = "ESP_EARLY_LOG";

#define BUFFER_SIZE (256)
static unsigned s_counter = 0;
static char s_print_buffer[BUFFER_SIZE];

static void reset_buffer(void)
{
    s_counter = 0;
    s_print_buffer[0] = 0;
}

static unsigned get_counter(void)
{
    return s_counter;
}

static char* get_buffer(void)
{
    return s_print_buffer;
}

void putc_to_buffer(char c)
{
    s_print_buffer[s_counter++] = c;
    if (c == '\n') {
        s_print_buffer[s_counter] = 0;
        printf("%s", s_print_buffer);
    }
}

static int print_to_buffer(const char *format, va_list args)
{
    int ret = vsnprintf(s_print_buffer, BUFFER_SIZE, format, args);
    printf(s_print_buffer);
    s_counter += ret;
    assert(s_counter < BUFFER_SIZE);
    return ret;
}

TEST_CASE("LOG_LOCAL_LEVEL can be re-defined", "[log]")
{
    TEST_ASSERT_EQUAL(CONFIG_LOG_DEFAULT_LEVEL, ESP_LOG_INFO);
    TEST_ASSERT_EQUAL(CONFIG_LOG_MAXIMUM_LEVEL, ESP_LOG_INFO);
#if CONFIG_LOG_MASTER_LEVEL
    // ignore the master log feature in this test
    esp_log_set_level_master(ESP_LOG_VERBOSE);
#endif
    /*
     * LOG_LOCAL_LEVEL == ESP_LOG_DEBUG. It defines which log levels
     * are included in binary from a particular file.
     * They are not enabled by default to print. Use esp_log_level_set()
     * to raise the log level.
     */

    // for ESP_LOGx
    vprintf_like_t old_vprintf = esp_log_set_vprintf(print_to_buffer);
    reset_buffer();
    ESP_LOGV(TAG1, "No VERBOSE log");
    TEST_ASSERT_EQUAL(0, get_counter());

    reset_buffer();
    ESP_LOGD(TAG1, "There is a debug log");
    TEST_ASSERT_EQUAL(0, get_counter());

    reset_buffer();
    esp_log_level_set("*", ESP_LOG_DEBUG);
    ESP_LOGD(TAG1, "There is a debug log");
    TEST_ASSERT_GREATER_THAN(17, get_counter());
    TEST_ASSERT_NOT_NULL(strstr(get_buffer(), "There is a debug log"));

    reset_buffer();
    ESP_LOGI(TAG1, "There is an info log");
    TEST_ASSERT_GREATER_THAN(17, get_counter());
    TEST_ASSERT_NOT_NULL(strstr(get_buffer(), "There is an info log"));

    reset_buffer();
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set(TAG1, ESP_LOG_DEBUG);
    ESP_LOGD(TAG1, "There is a debug log");
    TEST_ASSERT_NOT_NULL(strstr(get_buffer(), "There is a debug log"));

    reset_buffer();
    ESP_LOGD(TAG1, "There is 2nd debug log");
    TEST_ASSERT_NOT_NULL(strstr(get_buffer(), "There is 2nd debug log"));

    esp_log_level_set("*", ESP_LOG_ERROR);
    reset_buffer();
    ESP_LOGI(TAG1, "There is an info log");
    TEST_ASSERT_EQUAL(0, get_counter());

    reset_buffer();
    ESP_LOGE(TAG1, "There is an error log");
    TEST_ASSERT_GREATER_THAN(17, get_counter());
    TEST_ASSERT_NOT_NULL(strstr(get_buffer(), "There is an error log"));
    esp_log_set_vprintf(old_vprintf);
    esp_log_level_set("*", ESP_LOG_INFO);

    // for ESP_EARLY_LOGx
    esp_rom_install_channel_putc(1, putc_to_buffer);
    reset_buffer();
    ESP_EARLY_LOGV(TAG2, "No VERBOSE log");
    TEST_ASSERT_EQUAL(0, get_counter());

    reset_buffer();
    ESP_EARLY_LOGD(TAG1, "There is a debug log");
    TEST_ASSERT_EQUAL(0, get_counter());

    reset_buffer();
    esp_log_level_set("*", ESP_LOG_DEBUG);
    ESP_EARLY_LOGD(TAG1, "There is a debug log");
    TEST_ASSERT_GREATER_THAN(17, get_counter());
    TEST_ASSERT_NOT_NULL(strstr(get_buffer(), "There is a debug log"));

    reset_buffer();
    ESP_EARLY_LOGI(TAG2, "There is an info log");
    TEST_ASSERT_GREATER_THAN(17, get_counter());
    TEST_ASSERT_NOT_NULL(strstr(get_buffer(), "There is an info log"));

    esp_log_level_set("*", ESP_LOG_ERROR);
    reset_buffer();
    ESP_EARLY_LOGI(TAG2, "There is an info log");
    TEST_ASSERT_EQUAL(0, get_counter());

    reset_buffer();
    ESP_EARLY_LOGE(TAG2, "There is an error log");
    TEST_ASSERT_GREATER_THAN(17, get_counter());
    TEST_ASSERT_NOT_NULL(strstr(get_buffer(), "There is an error log"));
    esp_rom_install_uart_printf();
    esp_log_level_set("*", ESP_LOG_INFO);
}
