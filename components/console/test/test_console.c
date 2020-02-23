#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
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

// Enter "quit" to exit REPL environment
TEST_CASE("esp console repl test", "[console][ignore]")
{
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_console_repl_init(&repl_config));
    TEST_ESP_OK(esp_console_repl_start());
    vTaskDelay(pdMS_TO_TICKS(2000));
}
