/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "unity.h"

/* Macros */
#define RUN_TEST_SAFE(func) safe_unity_run_test(func, #func, __LINE__)

/* Enums */
enum {
    SAFE_UNITY_TEST_PASSED,
    SAFE_UNITY_TEST_FAILED,
    SAFE_UNITY_TEST_CRASHED,
};

/* Public interfaces */
void safe_unity_run_test(UnityTestFunction func, const char* func_name, const int func_line_num);
