/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "sdkconfig.h"

#if CONFIG_SOC_ULP_LP_UART_SUPPORTED
#include "soc/lp_uart_reg.h"

uint32_t lp_sleep_uart_clkdiv_reg(void)
{
    return REG_READ(LP_UART_CLKDIV_SYNC_REG);
}
#endif
