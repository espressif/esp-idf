/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <catch2/catch_session.hpp>

extern "C" void app_main(void)
{
    const char *argv[] = {
        "target_test_main",
        "--durations",
        "yes",
        NULL
    };
    int argc = sizeof(argv)/sizeof(argv[0]) - 1;

    auto result = Catch::Session().run(argc, argv);
    if (result != 0) {
        printf("Test failed with result %d\n", result);
    } else {
        printf("Test passed.\n");
    }
}
