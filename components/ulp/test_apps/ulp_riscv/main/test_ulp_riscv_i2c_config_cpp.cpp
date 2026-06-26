/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

// Compile ULP RISC-V I2C default-config macros as C++ to catch incompatibilities.
#include "ulp_riscv_i2c.h"

static void test_ulp_riscv_i2c_default_config_cpp(void) __attribute__((unused));
static void test_ulp_riscv_i2c_default_config_cpp(void)
{
    ulp_riscv_i2c_cfg_t cfg = ULP_RISCV_I2C_DEFAULT_CONFIG();
    ulp_riscv_i2c_pin_cfg_t pin_cfg = ULP_RISCV_I2C_DEFAULT_GPIO_CONFIG();
    ulp_riscv_i2c_timing_cfg_t fast_cfg = ULP_RISCV_I2C_FAST_MODE_CONFIG();
    ulp_riscv_i2c_timing_cfg_t std_cfg = ULP_RISCV_I2C_STANDARD_MODE_CONFIG();
    (void)cfg;
    (void)pin_cfg;
    (void)fast_cfg;
    (void)std_cfg;
}
