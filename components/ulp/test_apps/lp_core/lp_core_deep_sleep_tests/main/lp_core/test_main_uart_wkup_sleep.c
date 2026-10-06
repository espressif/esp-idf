/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_uart.h"
#include "ulp_lp_core_print.h"
#include "lp_sleep_payload.h"
#include "lp_sleep_finish.h"

volatile uint32_t uart_wkup_cause;

int main(void)
{
    uart_wkup_cause = ulp_lp_core_get_wakeup_cause();
    lp_core_printf(LP_SLEEP_UART_WAKE_REPLY "\r\n");
    lp_core_uart_tx_flush(LP_UART_NUM_0);
    lp_sleep_finish();
}
