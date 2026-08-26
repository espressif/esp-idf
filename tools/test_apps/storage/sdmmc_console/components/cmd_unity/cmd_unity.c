/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ctype.h>
#include "esp_err.h"
#include "esp_console.h"
#include "unity.h"
#include "unity_test_runner.h"
#include "argtable3/argtable3.h"
#include "cmd_unity.h"

static void register_test(void);

void register_unity(void)
{
    register_test();
}

static struct {
    struct arg_str *spec;
    struct arg_end *end;
} test_args;

static int test_handler(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &test_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, test_args.end, argv[0]);
        return 1;
    }

    if (test_args.spec->count == 0) {
        printf("Available tests:\n");
        int test_count = unity_get_test_count();
        for (int i = 0; i < test_count; ++i) {
            test_desc_t test_desc;
            if (!unity_get_test_info(i, &test_desc)) {
                return 1;
            }
            printf("(%d) %s %s\n", i + 1, test_desc.name, test_desc.desc);
        }
        return 0;
    }

    const char* spec = test_args.spec->sval[0];

    size_t idx = 0;
    bool invert = false;
    if (spec[idx] == '!') {
        invert = true;
        ++idx;
    }

    UNITY_BEGIN();
    if (spec[idx] == '*') {
        unity_run_all_tests();
    } else if (spec[idx] == '[') {
        unity_run_tests_by_tag(spec + idx, invert);
    } else if (isdigit((int) spec[idx])) {
        int test_index = strtol(spec + idx, NULL, 10) - 1;
        if (test_index < 0 || test_index >= unity_get_test_count()) {
            printf("Invalid test index: %d\n", test_index);
        }
        unity_run_test_by_index(test_index);
    } else {
        unity_run_test_by_name(spec + idx);
    }
    UNITY_END();

    return 0;
}

static void register_test(void)
{
    test_args.spec = arg_str0(NULL, NULL, "index|name|[tag]|*",
                              "Run test by index or name or tag. If not given, the list of tests is printed");
    test_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
            .command = "test",
            .help = "Run unit test",
            .func = &test_handler,
            .argtable = &test_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}
