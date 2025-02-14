/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ulp_lp_core_lp_uart_shared.h"
#include "hal/uart_hal.h"

/* LP UART HAL Context */
static uart_hal_context_t hal = {
    .dev = (uart_dev_t *)UART_LL_GET_HW(LP_UART_NUM_0),
};

esp_err_t lp_core_uart_wakeup_setup(const uart_wakeup_cfg_t *cfg)
{
    return uart_wakeup_setup(LP_UART_NUM_0, cfg);
}

void lp_core_uart_clear_buf(void)
{
    /* Reset Tx/Rx FIFOs */
    uart_ll_rxfifo_rst(hal.dev);
    uart_ll_txfifo_rst(hal.dev);
}
