/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_print.h"
#include "ulp_lp_core_lp_uart_shared.h"
#include "lp_core_uart.h"
#include "ulp_lp_core_uart.h"

int main(void)
{
    lp_core_printf("Hello world\r\n");
    ulp_lp_core_delay_us(5000);
    lp_core_uart_clear_buf();
    ulp_lp_core_lp_uart_reset_wakeup_en();
}
