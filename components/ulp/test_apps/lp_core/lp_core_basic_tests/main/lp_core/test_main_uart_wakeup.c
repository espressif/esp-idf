/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_print.h"
#include "ulp_lp_core_uart.h"

int main(void)
{
    lp_core_printf("Hello world\r\n");
    lp_core_uart_tx_flush(LP_UART_NUM_0);

    return 0;
}
