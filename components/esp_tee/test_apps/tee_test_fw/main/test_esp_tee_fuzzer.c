/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"
#include "esp_tee.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "unity.h"

#define MAX_ARGS         9
#define MAX_CMD_FUZZ_LEN 384

static const char *TAG = "test_esp_tee_fuzzer";

static struct {
    struct arg_int *arg_count;
    struct arg_int *srv_id;
    struct arg_str *arg_list;
    struct arg_end *end;
} fuzz_args;

static int fuzz(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &fuzz_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, fuzz_args.end, argv[0]);
        return ESP_ERR_INVALID_ARG;
    }

    const int arg_count = fuzz_args.arg_count->ival[0];
    const int srv_id = fuzz_args.srv_id->ival[0];

    if (arg_count < 0 || arg_count > MAX_ARGS) {
        ESP_LOGE(TAG, "arg_count %d out of range [0, %d]", arg_count, MAX_ARGS);
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t arg_val[MAX_ARGS] = {0};

    if (arg_count > 0 && fuzz_args.arg_list->count > 0) {
        char arg_list_buf[MAX_CMD_FUZZ_LEN + 1];
        strlcpy(arg_list_buf, fuzz_args.arg_list->sval[0], sizeof(arg_list_buf));

        char *saveptr = NULL;
        char *token = strtok_r(arg_list_buf, ";", &saveptr);
        for (int i = 0; i < arg_count && token != NULL; i++) {
            arg_val[i] = (uint32_t)strtoul(token, NULL, 0);
            token = strtok_r(NULL, ";", &saveptr);
        }
    }

    /*
     * All service arguments are passed as uint32_t through the variadic interface.
     * The TEE dispatcher reads only (arg_count + 1) values; extra trailing args
     * are harmless since they remain untouched in registers / on the stack.
     * Skipping per-service type casts is intentional: for fuzzing we want the
     * full 32-bit garbage to reach the service, not a truncated subset.
     */
    uint32_t ret = esp_tee_service_call(arg_count + 1, srv_id,
                                        arg_val[0], arg_val[1], arg_val[2],
                                        arg_val[3], arg_val[4], arg_val[5],
                                        arg_val[6]);
    ESP_LOGI(TAG, "Service call returned 0x%08lx", ret);

    return ESP_OK;
}

TEST_CASE("Test TEE service call Fuzzer", "[fuzzing]")
{
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.max_cmdline_length = MAX_CMD_FUZZ_LEN;

    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    TEST_ESP_OK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));

    fuzz_args.arg_count = arg_int1("c", "count", "<count>", "Argument count");
    fuzz_args.srv_id = arg_int1("i", "id", "<srv_id>", "Service ID");
    fuzz_args.arg_list = arg_str0(NULL, NULL, "<arg_list>", "Argument list");
    fuzz_args.end = arg_end(2);

    const esp_console_cmd_t fuzz_cmd = {
        .command = "fuzz",
        .help = "Fuzz TEE service",
        .hint = NULL,
        .func = &fuzz,
        .argtable = &fuzz_args
    };

    TEST_ESP_OK(esp_console_cmd_register(&fuzz_cmd));
    TEST_ESP_OK(esp_console_register_help_command());
    TEST_ESP_OK(esp_console_start_repl(repl));

    vTaskSuspend(NULL);
}
