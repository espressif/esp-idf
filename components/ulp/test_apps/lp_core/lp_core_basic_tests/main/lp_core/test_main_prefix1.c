/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Global variable with the same name across different ULP binaries */
volatile int g_var;

int main(void)
{
    g_var = 1;

    while (1)
        ;

    return 0;
}
