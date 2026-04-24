/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* Test fixture: valid C file with no matching log tags */

#include <stdio.h>

void regular_function(int a, int b) {
    int c = a + b;
    printf("Result: %d\n", c);
}
