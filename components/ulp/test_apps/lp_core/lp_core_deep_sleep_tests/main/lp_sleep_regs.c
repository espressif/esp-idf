/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "sdkconfig.h"

#if CONFIG_SOC_LP_CORE_SUPPORT_I2C
#include "soc/lp_i2c_reg.h"

uint32_t lp_sleep_i2c_timing_reg(void)
{
#ifdef LP_I2C_SCL_LOW_PERIOD_REG
    return REG_READ(LP_I2C_SCL_LOW_PERIOD_REG);
#else
    /* ESP32-S31 names this I2C_SCL_LOW_PERIOD_REG and bases it on DR_REG_I2C_BASE,
     * which is not defined. SCL low period is the first LP I2C register. */
    return REG_READ(DR_REG_LP_I2C_BASE);
#endif
}
#endif

#if CONFIG_SOC_ULP_LP_UART_SUPPORTED
#include "soc/lp_uart_reg.h"

uint32_t lp_sleep_uart_clkdiv_reg(void)
{
    return REG_READ(LP_UART_CLKDIV_SYNC_REG);
}
#endif
