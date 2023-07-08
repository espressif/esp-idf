/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "ulp_lp_core_print.h"
#include "ulp_lp_core_utils.h"

int main (void)
{
    static int iteration = 0;
    const char separator[] = "**************************";

    lp_core_printf("Hello from the LP core!!\r\n");
    lp_core_printf("This program has run %d times\r\n", ++iteration);
    lp_core_printf("%s", separator);
    lp_core_printf("\n");

    return 0;
}
