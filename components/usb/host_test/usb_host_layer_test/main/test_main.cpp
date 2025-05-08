/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>

extern "C" void app_main(void)
{
    int argc = 1;
    const char *argv[2] = {
        "target_test_main",
        NULL
    };

    auto result = Catch::Session().run(argc, argv);
    if (result != 0) {
        printf("Test failed with result %d\n", result);
    } else {
        printf("Test passed.\n");
    }
    fflush(stdout);
    exit(result);
}
