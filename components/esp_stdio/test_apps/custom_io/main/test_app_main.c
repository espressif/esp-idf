/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "unity.h"
#include "unity_test_runner.h"
#include "mock_vfs.h"

void setUp(void)
{
    mock_vfs_reset();
}

void app_main(void)
{
    unity_run_menu();
}
