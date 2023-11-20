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

typedef struct {
        const char *in;
        const char *out;
} cmd_context_t;

static int do_hello_cmd_with_context(void *context, int argc, char **argv)
{
    cmd_context_t *cmd_context = (cmd_context_t *)context;
    cmd_context->out = cmd_context->in;
    return 0;
}

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

TEST_CASE("esp console test with context", "[console]")
{
    /* Test with minimal init config */
    esp_console_config_t console_config = {
        .max_cmdline_args = 2,
        .max_cmdline_length = 100,
    };

    TEST_ESP_OK(esp_console_init(&console_config));

    const esp_console_cmd_t cmds[] = {
        {
            .command = "hello-c1",
            .help = "Print Hello World in context c1",
            .hint = NULL,
            .func_context = do_hello_cmd_with_context,
        },
        {
            .command = "hello-c2",
            .help = "Print Hello World in context c2",
            .hint = NULL,
            .func_context = do_hello_cmd_with_context,
        },
    };
    cmd_context_t contexts[] = {
        {
            .in = "c1",
            .out = NULL,
        },
        {
            .in = "c2",
            .out = NULL,
        },
    };
    TEST_ASSERT_EQUAL(sizeof(contexts) / sizeof(contexts[0]), sizeof(cmds) / sizeof(cmds[0]));

    for (int i=0; i < sizeof(cmds) / sizeof(cmds[0]); i++) {
        TEST_ESP_OK(esp_console_cmd_register(&cmds[i]));
        TEST_ESP_OK(esp_console_cmd_set_context(cmds[i].command, &contexts[i]));
    }
    TEST_ASSERT_EQUAL(esp_console_cmd_set_context(NULL, NULL), ESP_ERR_INVALID_ARG);
    TEST_ASSERT_EQUAL(esp_console_cmd_set_context("invalid", NULL), ESP_ERR_NOT_FOUND);

    for (int i=0; i < sizeof(cmds) / sizeof(cmds[0]); i++) {
        int ret;
        TEST_ESP_OK(esp_console_run(cmds[i].command, &ret));
        TEST_ASSERT_EQUAL(ret, 0);
        TEST_ASSERT_EQUAL(contexts[i].in, contexts[i].out);
    }

    TEST_ESP_OK(esp_console_deinit());
}
