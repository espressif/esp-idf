/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "unity.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "linenoise/linenoise.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static int do_hello_cmd(int argc, char **argv)
{
    printf("Hello World\n");
    return 0;
}

TEST_CASE("esp console init/deinit test", "[console]")
{
    esp_console_config_t console_config = ESP_CONSOLE_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_console_init(&console_config));
    const esp_console_cmd_t cmd = {
        .command = "hello",
        .help = "Print Hello World",
        .hint = NULL,
        .func = do_hello_cmd,
    };
    TEST_ESP_OK(esp_console_cmd_register(&cmd));
    // re-register the same command, just for test
    TEST_ESP_OK(esp_console_cmd_register(&cmd));
    TEST_ESP_OK(esp_console_deinit());
}

static esp_console_repl_t *s_repl = NULL;

/* handle 'quit' command */
static int do_cmd_quit(int argc, char **argv)
{
    printf("ByeBye\r\n");
    s_repl->del(s_repl);

    linenoiseHistoryFree(); // Free up memory

    return 0;
}

static esp_console_cmd_t s_quit_cmd = {
    .command = "quit",
    .help = "Quit REPL environment",
    .func = &do_cmd_quit
};

// Enter "quit" to exit REPL environment
/* Marked as ignore since it cannot run as a normal unity test case
   ran separately in test_console_repl  */
TEST_CASE("esp console repl test", "[console][ignore]")
{
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_console_new_repl_uart(&uart_config, &repl_config, &s_repl));

    TEST_ESP_OK(esp_console_cmd_register(&s_quit_cmd));

    TEST_ESP_OK(esp_console_start_repl(s_repl));
    vTaskDelay(pdMS_TO_TICKS(2000));
}

TEST_CASE("esp console help command", "[console][ignore]")
{
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_console_new_repl_uart(&uart_config, &repl_config, &s_repl));

    TEST_ESP_OK(esp_console_cmd_register(&s_quit_cmd));
    TEST_ESP_OK(esp_console_register_help_command());

    TEST_ESP_OK(esp_console_start_repl(s_repl));
    vTaskDelay(pdMS_TO_TICKS(5000));
}

TEST_CASE("esp console init/deinit test, minimal config", "[console]")
{
    /* Test with minimal init config */
    esp_console_config_t console_config = {
        .max_cmdline_length = 100,
    };

    TEST_ESP_OK(esp_console_init(&console_config));
    const esp_console_cmd_t cmd = {
        .command = "hello",
        .help = "Print Hello World",
        .hint = NULL,
        .func = do_hello_cmd,
    };

    TEST_ESP_OK(esp_console_cmd_register(&cmd));
    // re-register the same command, just for test
    TEST_ESP_OK(esp_console_cmd_register(&cmd));
    TEST_ESP_OK(esp_console_deinit());
}
