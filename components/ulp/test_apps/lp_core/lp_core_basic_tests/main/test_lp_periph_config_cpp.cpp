/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

// Compile LP peripheral default-config macros as C++ to catch incompatibilities.
#include "sdkconfig.h"
#include "soc/soc_caps.h"

#if SOC_ULP_LP_UART_SUPPORTED
#include "lp_core_uart.h"

static void test_lp_uart_default_config_cpp(void) __attribute__((unused));
static void test_lp_uart_default_config_cpp(void)
{
    lp_core_uart_cfg_t cfg = LP_CORE_UART_DEFAULT_CONFIG();
    lp_core_uart_pin_cfg_t pin_cfg = LP_UART_DEFAULT_GPIO_CONFIG();
    lp_core_uart_proto_cfg_t proto_cfg = LP_UART_DEFAULT_PROTO_CONFIG();
    (void)cfg;
    (void)pin_cfg;
    (void)proto_cfg;
    (void)LP_UART_DEFAULT_CLOCK_CONFIG();
}
#endif /* SOC_ULP_LP_UART_SUPPORTED */

#if SOC_LP_CORE_SUPPORT_I2C
#include "lp_core_i2c.h"

static void test_lp_i2c_default_config_cpp(void) __attribute__((unused));
static void test_lp_i2c_default_config_cpp(void)
{
    lp_core_i2c_cfg_t cfg = LP_CORE_I2C_DEFAULT_CONFIG();
    lp_core_i2c_pin_cfg_t pin_cfg = LP_I2C_DEFAULT_GPIO_CONFIG();
    lp_core_i2c_timing_cfg_t fast_cfg = LP_I2C_FAST_MODE_CONFIG();
    lp_core_i2c_timing_cfg_t std_cfg = LP_I2C_STANDARD_MODE_CONFIG();
    (void)cfg;
    (void)pin_cfg;
    (void)fast_cfg;
    (void)std_cfg;
    (void)LP_I2C_DEFAULT_SRC_CLK();
}
#endif /* SOC_LP_CORE_SUPPORT_I2C */
