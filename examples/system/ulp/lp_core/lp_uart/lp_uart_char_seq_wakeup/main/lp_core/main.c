/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_print.h"
#include "ulp_lp_core_lp_uart_shared.h"

int main (void)
{
    lp_core_printf("Hello world\r\n");

    // If you want to make it possible to wake up from UART after sleep,
    // you have to reset the wakeup register and the UART buffer manually.
    // ulp_lp_core_lp_uart_reset_wakeup_en();
    // lp_core_uart_clear_buf();
    return 0;
}
