/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "unity.h"
#include "unity_test_utils.h"
#include "esp_rom_serial_output.h"
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
    int ret = vsnprintf(&s_print_buffer[s_counter], BUFFER_SIZE, format, args);
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

#if CONFIG_LOG_VERSION_2
/*
 * Test ESP_LOG behavior in constrained environments (ISR, cache disabled).
 *
 * CONFIG_LOG_API_CONSTRAINED_ENV_SAFE controls how esp_log_vprintf() prints
 * when the constrained_env flag is set:
 *  - Enabled:  esp_log() uses esp_rom_vprintf (in IRAM) for output.
 *              ESP_LOG is safe to call from ISR or with cache disabled.
 *  - Disabled: esp_log() uses the standard vprintf callback (typically in flash).
 *              ESP_LOG from ISR or with cache disabled will crash because the
 *              print function is not in internal memory.
 *
 * We install print_to_buffer as the vprintf callback and disconnect the putc
 * channel (used by esp_rom_vprintf). Then we call esp_log() with
 * constrained_env=true and check where the output went:
 *  - Enabled:  output goes to esp_rom_vprintf -> putc channel (disconnected)
 *              -> buffer stays empty.
 *  - Disabled: output goes to the vprintf callback (print_to_buffer)
 *              -> buffer contains the message.
 */
TEST_CASE("ESP_LOG constrained_env uses rom vprintf only when constrained_env_safe is enabled", "[log]")
{
    vprintf_like_t old_vprintf = esp_log_set_vprintf(print_to_buffer);

    /* Simulate a log call from a constrained environment (ISR / cache disabled) */
    esp_log_config_t cfg = ESP_LOG_CONFIG_INIT(ESP_LOG_INFO
                                               | ESP_LOG_CONFIG_CONSTRAINED_ENV);

    /* Disconnect the putc channel so esp_rom_vprintf output is not captured.
     * esp_rom_install_uart_printf() alone does not reset the software _putc1
     * on chips without ESP_ROM_HAS_ETS_PRINTF_BUG (e.g. ESP32), so explicitly
     * restore the default putc via esp_rom_install_channel_putc(). */
    esp_rom_install_channel_putc(1, esp_rom_output_putc);
    esp_rom_install_uart_printf();
    reset_buffer();
    esp_log(cfg, TAG1, "constrained %d", 42);

#if CONFIG_LOG_API_CONSTRAINED_ENV_SAFE
    /* esp_rom_vprintf was used (IRAM-safe) -> nothing in our vprintf buffer */
    TEST_ASSERT_EQUAL(0, get_counter());
#else
    /* Standard vprintf was used (flash-based, would crash in real ISR/cache-off) */
    TEST_ASSERT_NOT_NULL(strstr(get_buffer(), "constrained 42"));
#endif

    /* Restore */
    esp_log_set_vprintf(old_vprintf);
    esp_log_level_set("*", ESP_LOG_INFO);
}

#if CONFIG_LOG_API_CONSTRAINED_ENV_SAFE
/*
 * When constrained_env_safe is enabled and LOG_IN_IRAM=y (default), esp_log()
 * code is in IRAM and uses esp_rom_vprintf (ROM) for output. This means
 * esp_log() can safely be called with cache disabled, as long as the format
 * string and tag are also in DRAM/IRAM.
 *
 * Verify this by actually disabling flash cache and calling esp_log() with
 * constrained_env=true and a DRAM format string. If it doesn't crash, the
 * IRAM path is correctly used.
 */
static void IRAM_ATTR test_log_cache_disabled_cb(void *arg)
{
    /* All strings must be in DRAM since flash is inaccessible */
    static const DRAM_ATTR char tag[] = "test";
    static const DRAM_ATTR char fmt[] = "cache_off %d\n";
    esp_log_config_t cfg = ESP_LOG_CONFIG_INIT(ESP_LOG_INFO
                                               | ESP_LOG_CONFIG_CONSTRAINED_ENV);
    esp_log(cfg, tag, fmt, 99);
}

TEST_CASE("ESP_LOG with constrained_env_safe works when cache is disabled", "[log]")
{
    /* If esp_log() tries to use a flash-resident vprintf, this will crash */
    unity_utils_run_cache_disable_stub(test_log_cache_disabled_cb, NULL);
    /* Reaching here without a crash proves the IRAM/ROM path was used */
}
#endif // CONFIG_LOG_API_CONSTRAINED_ENV_SAFE
#endif // CONFIG_LOG_VERSION_2
