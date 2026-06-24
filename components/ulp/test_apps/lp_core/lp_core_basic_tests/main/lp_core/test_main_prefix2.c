/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* A global variable to make sure that g_var doesn't have the same address across different ULP binaries */
volatile int g_array[10];

/* Global variable with the same name across different ULP binaries */
volatile int g_var;

int main(void)
{
    int i;
    for (i = 0; i < 10; i++) {
        g_array[i] = i;
    }
    g_var = 2;

    while (1)
        ;

    return 0;
}
