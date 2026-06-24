/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "esp_macros.h"

/* test macros */
#define foo_args(...) 1
#define foo_no_args() 2
#if defined(__cplusplus) && (__cplusplus >  201703L)
#define foo(...) CHOOSE_MACRO_VA_ARG(foo_args, foo_no_args __VA_OPT__(,) __VA_ARGS__)(__VA_ARGS__)
#else
#define foo(...) CHOOSE_MACRO_VA_ARG(foo_args, foo_no_args, ##__VA_ARGS__)(__VA_ARGS__)
#endif

ESP_STATIC_ASSERT(foo() == 2, "CHOOSE_MACRO_VA_ARG() result does not match for 0 arguments");
ESP_STATIC_ASSERT(foo(42) == 1, "CHOOSE_MACRO_VA_ARG() result does not match for 1 argument");
#if defined(__cplusplus) && (__cplusplus >  201703L)
ESP_STATIC_ASSERT(foo(42, 87) == 1, "CHOOSE_MACRO_VA_ARG() result does not match for n arguments");
#endif
