/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "unity.h"
#include "unity_test_runner.h"

void app_main(void)
{
/**
 *  _   _    _    _       _   _ _____ ___ _     ____
 * | | | |  / \  | |     | | | |_   _|_ _| |   / ___|
 * | |_| | / _ \ | |     | | | | | |  | || |   \___ \
 * |  _  |/ ___ \| |___  | |_| | | |  | || |___ ___) |
 * |_| |_/_/   \_\_____|  \___/  |_| |___|_____|____/
 */
    printf("  _   _    _    _       _   _ _____ ___ _     ____  \r\n");
    printf(" | | | |  / \\  | |     | | | |_   _|_ _| |   / ___| \r\n");
    printf(" | |_| | / _ \\ | |     | | | | | |  | || |   \\___ \\ \r\n");
    printf(" |  _  |/ ___ \\| |___  | |_| | | |  | || |___ ___) |\r\n");
    printf(" |_| |_/_/   \\_\\_____|  \\___/  |_| |___|_____|____/ \r\n");

    unity_run_menu();
}
