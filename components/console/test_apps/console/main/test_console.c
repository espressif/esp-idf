/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
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
#include "freertos/semphr.h"

/*
 * NOTE: Most of these unit tests DO NOT work standalone. They require pytest to control
 * the application and check for correct output.
 * E.g., to run the test "esp console help command - reverse registration", type:
 * pytest --target esp32 -m "generic" -k test_console_help_reverse_registration
 * The pytest test cases are different than the unit test cases here, they can be found
 * in the pytest_*.py file in the root directory of this test project.
 * For more information on pytest, please refer to
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/contribute/esp-idf-tests-with-pytest.html.
 */

extern void set_leak_threshold(int threshold);

typedef struct {
    const char *in;
    const char *out;
} cmd_context_t;

static esp_console_repl_t *s_repl = NULL;

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

static int do_not_call(void* context, int argc, char **argv)
{
    TEST_ASSERT_MESSAGE(false, "This function is a dummy and must not be called\n");
    return 0;
}

TEST_CASE("esp console register with normal and context aware functions set fails", "[console]")
{
    esp_console_config_t console_config = ESP_CONSOLE_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_console_init(&console_config));
    const esp_console_cmd_t cmd = {
        .command = "valid_cmd",
        .help = "Command which is valid",
        .hint = NULL,
        .func = do_hello_cmd,
        .func_w_context = do_not_call,
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_console_cmd_register(&cmd));
    TEST_ESP_OK(esp_console_deinit());
}

TEST_CASE("esp console register with normal and context aware function set to NULL fails", "[console]")
{
    esp_console_config_t console_config = ESP_CONSOLE_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_console_init(&console_config));
    const esp_console_cmd_t cmd = {
        .command = "valid_cmd",
        .help = "Command which is valid",
        .hint = NULL,
        .func = do_hello_cmd,
        .func_w_context = do_not_call,
    };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_console_cmd_register(&cmd));
    TEST_ESP_OK(esp_console_deinit());
}

TEST_CASE("esp console init function NULL param fails", "[console]")
{
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_console_new_repl_uart(NULL, &repl_config, &s_repl));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_console_new_repl_uart(&uart_config, NULL, &s_repl));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_console_new_repl_uart(&uart_config, &repl_config, NULL));
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

TEST_CASE("esp console init/deinit with context test", "[console]")
{
    int dummy = 47;
    esp_console_config_t console_config = ESP_CONSOLE_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_console_init(&console_config));
    const esp_console_cmd_t cmd = {
        .command = "hello",
        .help = "Print Hello World",
        .hint = NULL,
        .func_w_context = do_not_call,
        .context = &dummy,
    };
    TEST_ESP_OK(esp_console_cmd_register(&cmd));
    // re-register the same command, just for test
    TEST_ESP_OK(esp_console_cmd_register(&cmd));
    TEST_ESP_OK(esp_console_deinit());
}

static bool can_terminate = false;
static SemaphoreHandle_t s_test_console_mutex = NULL;
static StaticSemaphore_t s_test_console_mutex_buf;

/* handle 'quit' command */
static int do_cmd_quit(int argc, char **argv)
{
    TEST_ASSERT_NOT_NULL(s_test_console_mutex);
    xSemaphoreTake(s_test_console_mutex, portMAX_DELAY);
    can_terminate = true;
    xSemaphoreGive(s_test_console_mutex);

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
    set_leak_threshold(400);

    s_test_console_mutex = xSemaphoreCreateMutexStatic(&s_test_console_mutex_buf);
    TEST_ASSERT_NOT_NULL(s_test_console_mutex);

    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_console_new_repl_uart(&uart_config, &repl_config, &s_repl));

    TEST_ESP_OK(esp_console_cmd_register(&s_quit_cmd));

    TEST_ESP_OK(esp_console_start_repl(s_repl));

    bool temp_can_terminate = false;
    do {
        xSemaphoreTake(s_test_console_mutex, portMAX_DELAY);
        temp_can_terminate = can_terminate;
        xSemaphoreGive(s_test_console_mutex);
    } while (temp_can_terminate != true);

    /* Wait to make sure the command has finished its execution */
    vTaskDelay(pdMS_TO_TICKS(500));

    esp_err_t ret = esp_console_stop_repl(s_repl);
    TEST_ESP_OK(ret);

    xSemaphoreTake(s_test_console_mutex, portMAX_DELAY);
    can_terminate = false;
    xSemaphoreGive(s_test_console_mutex);

    vTaskDelay(pdMS_TO_TICKS(500));

    printf("ByeBye\r\n");
}

TEST_CASE("esp console repl deinit", "[console][ignore]")
{
    set_leak_threshold(400);

    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_console_new_repl_uart(&uart_config, &repl_config, &s_repl));

    /* start the repl task */
    TEST_ESP_OK(esp_console_start_repl(s_repl));

    /* wait to make sure the task reaches linenoiseEdit function
     * and gets stuck in the select */
    vTaskDelay(pdMS_TO_TICKS(500));

    /* call the delete function, this returns only when the repl task terminated */
    const esp_err_t res = esp_console_stop_repl(s_repl);

    vTaskDelay(pdMS_TO_TICKS(500));

    /* if this point is reached, the repl environment has been deleted successfully */
    TEST_ASSERT(res == ESP_OK);
}

static const esp_console_cmd_t cmd_a = {
    .command = "aaa",
    .help = "should appear first in help",
    .hint = NULL,
    .func = do_hello_cmd,
};
static const esp_console_cmd_t cmd_z = {
    .command = "zzz",
    .help = "should appear last in help",
    .hint = NULL,
    .func = do_hello_cmd,
};

TEST_CASE("esp console help command - sorted registration", "[console][ignore]")
{
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_console_new_repl_uart(&uart_config, &repl_config, &s_repl));

    TEST_ESP_OK(esp_console_cmd_register(&cmd_a));
    TEST_ESP_OK(esp_console_register_help_command());
    TEST_ESP_OK(esp_console_cmd_register(&s_quit_cmd));
    TEST_ESP_OK(esp_console_cmd_register(&cmd_z));

    TEST_ESP_OK(esp_console_start_repl(s_repl));
    vTaskDelay(pdMS_TO_TICKS(5000));
}

/**
 * The commands in the 'help'-command's output should be alphabetically sorted,
 * regardless of their registration order.
 */
TEST_CASE("esp console help command - reverse registration", "[console][ignore]")
{
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_console_new_repl_uart(&uart_config, &repl_config, &s_repl));

    TEST_ESP_OK(esp_console_cmd_register(&cmd_z));
    TEST_ESP_OK(esp_console_cmd_register(&s_quit_cmd));
    TEST_ESP_OK(esp_console_register_help_command());
    TEST_ESP_OK(esp_console_cmd_register(&cmd_a));

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

    cmd_context_t context0 = {
        .in = "c1",
        .out = NULL,
    };

    cmd_context_t context1 = {
        .in = "c2",
        .out = NULL,
    };

    const esp_console_cmd_t cmd0 = {
        .command = "hello-c1",
        .help = "Print Hello World in context c1",
        .hint = NULL,
        .func_w_context = do_hello_cmd_with_context,
        .context = &context0,
    };

    const esp_console_cmd_t cmd1 = {
        .command = "hello-c2",
        .help = "Print Hello World in context c2",
        .hint = NULL,
        .func_w_context = do_hello_cmd_with_context,
        .context = &context1,
    };

    TEST_ESP_OK(esp_console_cmd_register(&cmd0));
    TEST_ESP_OK(esp_console_cmd_register(&cmd1));

    int ret;
    TEST_ESP_OK(esp_console_run(cmd0.command, &ret));
    TEST_ASSERT_EQUAL(ret, 0);
    TEST_ASSERT_EQUAL(context0.in, context0.out);

    TEST_ESP_OK(esp_console_run(cmd1.command, &ret));
    TEST_ASSERT_EQUAL(ret, 0);
    TEST_ASSERT_EQUAL(context1.in, context1.out);

    TEST_ESP_OK(esp_console_deinit());
}

TEST_CASE("esp console help command - set verbose level = 0", "[console][ignore]")
{
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_console_new_repl_uart(&uart_config, &repl_config, &s_repl));
    TEST_ESP_OK(esp_console_register_help_command());
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_console_set_help_verbose_level(ESP_CONSOLE_HELP_VERBOSE_LEVEL_MAX_NUM));
    TEST_ESP_OK(esp_console_set_help_verbose_level(ESP_CONSOLE_HELP_VERBOSE_LEVEL_0));
    TEST_ESP_OK(esp_console_start_repl(s_repl));
    vTaskDelay(pdMS_TO_TICKS(5000));
}

TEST_CASE("esp console help command - set verbose level = 1", "[console][ignore]")
{
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_console_new_repl_uart(&uart_config, &repl_config, &s_repl));
    TEST_ESP_OK(esp_console_register_help_command());
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_console_set_help_verbose_level(ESP_CONSOLE_HELP_VERBOSE_LEVEL_MAX_NUM));
    TEST_ESP_OK(esp_console_set_help_verbose_level(ESP_CONSOLE_HELP_VERBOSE_LEVEL_1));
    TEST_ESP_OK(esp_console_start_repl(s_repl));
    vTaskDelay(pdMS_TO_TICKS(5000));
}

TEST_CASE("esp console help command - --verbose sub command", "[console][ignore]")
{
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_console_new_repl_uart(&uart_config, &repl_config, &s_repl));
    TEST_ESP_OK(esp_console_register_help_command());
    TEST_ESP_OK(esp_console_start_repl(s_repl));
    vTaskDelay(pdMS_TO_TICKS(5000));
}

TEST_CASE("esp console deregister commands", "[console][ignore]")
{
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_console_new_repl_uart(&uart_config, &repl_config, &s_repl));

    TEST_ESP_OK(esp_console_cmd_register(&cmd_a));
    TEST_ESP_OK(esp_console_cmd_register(&s_quit_cmd));
    TEST_ESP_OK(esp_console_register_help_command());
    TEST_ESP_OK(esp_console_cmd_register(&cmd_z));
    // deregister a non-existing cmd, should return "ESP_ERR_INVALID_ARG"
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_console_cmd_deregister("abcdefg"));
    // deregister cmd_a
    TEST_ESP_OK(esp_console_cmd_deregister(cmd_a.command));
    TEST_ESP_OK(esp_console_start_repl(s_repl));
    vTaskDelay(pdMS_TO_TICKS(5000));
}

TEST_CASE("esp console re-register commands", "[console][ignore]")
{
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_console_new_repl_uart(&uart_config, &repl_config, &s_repl));

    TEST_ESP_OK(esp_console_cmd_register(&cmd_a));
    TEST_ESP_OK(esp_console_cmd_register(&s_quit_cmd));
    TEST_ESP_OK(esp_console_cmd_register(&cmd_z));
    TEST_ESP_OK(esp_console_register_help_command());

    // deregister cmd_z and cmd_a
    TEST_ESP_OK(esp_console_cmd_deregister(cmd_z.command));
    TEST_ESP_OK(esp_console_cmd_deregister(cmd_a.command));

    // re-register cmd_z and cmd_a
    TEST_ESP_OK(esp_console_cmd_register(&cmd_z));
    TEST_ESP_OK(esp_console_cmd_register(&cmd_a));

    TEST_ESP_OK(esp_console_start_repl(s_repl));
    vTaskDelay(pdMS_TO_TICKS(5000));
}
