/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Console example — various sleep-related commands

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <unistd.h>
#include "esp_log.h"
#include "esp_console.h"
#include "esp_chip_info.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"
#include "driver/uart.h"
#include "argtable3/argtable3.h"
#include "cmd_system.h"
#include "sdkconfig.h"

static const char *TAG = "cmd_system_sleep";

#if SOC_DEEP_SLEEP_SUPPORTED
/** 'deep_sleep' command puts the chip into deep sleep mode */
static struct {
    struct arg_int *wakeup_time;
#if SOC_PM_SUPPORT_EXT0_WAKEUP || SOC_PM_SUPPORT_EXT1_WAKEUP
    struct arg_int *wakeup_gpio_num;
    struct arg_int *wakeup_gpio_level;
#endif
    struct arg_end *end;
} deep_sleep_args;


static int deep_sleep(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &deep_sleep_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, deep_sleep_args.end, argv[0]);
        return 1;
    }
    if (deep_sleep_args.wakeup_time->count) {
        uint64_t timeout = 1000ULL * deep_sleep_args.wakeup_time->ival[0];
        ESP_LOGI(TAG, "Enabling timer wakeup, timeout=%lluus", timeout);
        ESP_ERROR_CHECK( esp_sleep_enable_timer_wakeup(timeout) );
    }

#if SOC_PM_SUPPORT_EXT1_WAKEUP
    if (deep_sleep_args.wakeup_gpio_num->count) {
        int io_num = deep_sleep_args.wakeup_gpio_num->ival[0];
        if (!esp_sleep_is_valid_wakeup_gpio(io_num)) {
            ESP_LOGE(TAG, "GPIO %d is not an RTC IO", io_num);
            return 1;
        }
        int level = 0;
        if (deep_sleep_args.wakeup_gpio_level->count) {
            level = deep_sleep_args.wakeup_gpio_level->ival[0];
            if (level != 0 && level != 1) {
                ESP_LOGE(TAG, "Invalid wakeup level: %d", level);
                return 1;
            }
        }
        ESP_LOGI(TAG, "Enabling wakeup on GPIO%d, wakeup on %s level",
                 io_num, level ? "HIGH" : "LOW");

        ESP_ERROR_CHECK( esp_sleep_enable_ext1_wakeup_io(1ULL << io_num, level) );
        ESP_LOGE(TAG, "GPIO wakeup from deep sleep currently unsupported on ESP32-C3");
    }
#endif // SOC_PM_SUPPORT_EXT1_WAKEUP

#if CONFIG_IDF_TARGET_ESP32
    rtc_gpio_isolate(GPIO_NUM_12);
#endif //CONFIG_IDF_TARGET_ESP32
    esp_deep_sleep_start();
    return 1;
}

void register_system_deep_sleep(void)
{
    int num_args = 1;
    deep_sleep_args.wakeup_time =
        arg_int0("t", "time", "<t>", "Wake up time, ms");
#if SOC_PM_SUPPORT_EXT0_WAKEUP || SOC_PM_SUPPORT_EXT1_WAKEUP
    deep_sleep_args.wakeup_gpio_num =
        arg_int0(NULL, "io", "<n>",
                 "If specified, wakeup using GPIO with given number");
    deep_sleep_args.wakeup_gpio_level =
        arg_int0(NULL, "io_level", "<0|1>", "GPIO level to trigger wakeup");
    num_args += 2;
#endif
    deep_sleep_args.end = arg_end(num_args);

    const esp_console_cmd_t cmd = {
        .command = "deep_sleep",
        .help = "Enter deep sleep mode. "
#if SOC_PM_SUPPORT_EXT0_WAKEUP || SOC_PM_SUPPORT_EXT1_WAKEUP
        "Two wakeup modes are supported: timer and GPIO. "
#else
        "Timer wakeup mode is supported. "
#endif
        "If no wakeup option is specified, will sleep indefinitely.",
        .hint = NULL,
        .func = &deep_sleep,
        .argtable = &deep_sleep_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}
#endif // SOC_DEEP_SLEEP_SUPPORTED

#if SOC_LIGHT_SLEEP_SUPPORTED
/** 'light_sleep' command puts the chip into light sleep mode */
static struct {
    struct arg_int *wakeup_time;
    struct arg_int *wakeup_gpio_num;
    struct arg_int *wakeup_gpio_level;
    struct arg_end *end;
} light_sleep_args;

static int light_sleep(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &light_sleep_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, light_sleep_args.end, argv[0]);
        return 1;
    }
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    if (light_sleep_args.wakeup_time->count) {
        uint64_t timeout = 1000ULL * light_sleep_args.wakeup_time->ival[0];
        ESP_LOGI(TAG, "Enabling timer wakeup, timeout=%lluus", timeout);
        ESP_ERROR_CHECK( esp_sleep_enable_timer_wakeup(timeout) );
    }
    int io_count = light_sleep_args.wakeup_gpio_num->count;
    if (io_count != light_sleep_args.wakeup_gpio_level->count) {
        ESP_LOGE(TAG, "Should have same number of 'io' and 'io_level' arguments");
        return 1;
    }
    for (int i = 0; i < io_count; ++i) {
        int io_num = light_sleep_args.wakeup_gpio_num->ival[i];
        int level = light_sleep_args.wakeup_gpio_level->ival[i];
        if (level != 0 && level != 1) {
            ESP_LOGE(TAG, "Invalid wakeup level: %d", level);
            return 1;
        }
        ESP_LOGI(TAG, "Enabling wakeup on GPIO%d, wakeup on %s level",
                 io_num, level ? "HIGH" : "LOW");

        ESP_ERROR_CHECK( gpio_wakeup_enable(io_num, level ? GPIO_INTR_HIGH_LEVEL : GPIO_INTR_LOW_LEVEL) );
    }
    if (io_count > 0) {
        ESP_ERROR_CHECK( esp_sleep_enable_gpio_wakeup() );
    }
    if (CONFIG_ESP_CONSOLE_UART_NUM >= 0 && CONFIG_ESP_CONSOLE_UART_NUM <= UART_NUM_1) {
        ESP_LOGI(TAG, "Enabling UART wakeup (press ENTER to exit light sleep)");
        ESP_ERROR_CHECK( uart_set_wakeup_threshold(CONFIG_ESP_CONSOLE_UART_NUM, 3) );
        ESP_ERROR_CHECK( esp_sleep_enable_uart_wakeup(CONFIG_ESP_CONSOLE_UART_NUM) );
    }
    fflush(stdout);
    fsync(fileno(stdout));
    esp_light_sleep_start();
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

    const char *cause_str;
    switch (cause) {
    case ESP_SLEEP_WAKEUP_GPIO:
        cause_str = "GPIO";
        break;
    case ESP_SLEEP_WAKEUP_UART:
        cause_str = "UART";
        break;
    case ESP_SLEEP_WAKEUP_TIMER:
        cause_str = "timer";
        break;
    default:
        cause_str = "unknown";
        printf("%d\n", cause);
    }
    ESP_LOGI(TAG, "Woke up from: %s", cause_str);
    return 0;
}

void register_system_light_sleep(void)
{
    light_sleep_args.wakeup_time =
        arg_int0("t", "time", "<t>", "Wake up time, ms");
    light_sleep_args.wakeup_gpio_num =
        arg_intn(NULL, "io", "<n>", 0, 8,
                 "If specified, wakeup using GPIO with given number");
    light_sleep_args.wakeup_gpio_level =
        arg_intn(NULL, "io_level", "<0|1>", 0, 8, "GPIO level to trigger wakeup");
    light_sleep_args.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "light_sleep",
        .help = "Enter light sleep mode. "
        "Two wakeup modes are supported: timer and GPIO. "
        "Multiple GPIO pins can be specified using pairs of "
        "'io' and 'io_level' arguments. "
        "Will also wake up on UART input.",
        .hint = NULL,
        .func = &light_sleep,
        .argtable = &light_sleep_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}
#endif // SOC_LIGHT_SLEEP_SUPPORTED
