/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "hal/regi2c_impl.h"
#include "soc/soc_caps.h"
#include "esp_private/periph_ctrl.h"
#include "hal/regi2c_ctrl_ll.h"

#ifndef BOOTLOADER_BUILD
#include "esp_private/esp_clk_tree_common.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BOOTLOADER_BUILD

// For bootloader, the strategy is to keep the analog i2c master clock always enabled if ANA_I2C_MST_CLK_HAS_ROOT_GATING (in bootloader_hardware_init())
#define ANALOG_CLOCK_ENABLE()
#define ANALOG_CLOCK_DISABLE()
#define ANALOG_CLOCK_IS_ENABLED()

#else // !BOOTLOADER_BUILD

static inline __attribute__((always_inline)) void ANA_I2C_SRC_CLOCK_ENABLE(bool enable) {
#if SOC_CLK_ANA_I2C_MST_DEPENDS_ON_MODEM_APB
    esp_clk_tree_enable_src(SOC_MOD_CLK_MODEM_APB, enable);
#endif
}

#if ANA_I2C_MST_CLK_HAS_ROOT_GATING
// This clock needs to be enabled for regi2c write/read, pll calibaration, PHY, RNG, ADC, etc.
// Use reference count to manage the analog i2c master clock
#define ANALOG_CLOCK_ENABLE() { \
      ANA_I2C_SRC_CLOCK_ENABLE(true); \
      PERIPH_RCC_ACQUIRE_ATOMIC(PERIPH_ANA_I2C_MASTER_MODULE, ref_count) { \
            if (ref_count == 0) { \
                  regi2c_ctrl_ll_master_enable_clock(true); \
            } \
      } \
   }

#define ANALOG_CLOCK_DISABLE() { \
      PERIPH_RCC_RELEASE_ATOMIC(PERIPH_ANA_I2C_MASTER_MODULE, ref_count) { \
            if (ref_count == 0) { \
                  regi2c_ctrl_ll_master_enable_clock(false); \
            } \
      } \
      ANA_I2C_SRC_CLOCK_ENABLE(false); \
   }

#define ANALOG_CLOCK_IS_ENABLED() regi2c_ctrl_ll_master_is_clock_enabled()
#else
#define ANALOG_CLOCK_ENABLE()
#define ANALOG_CLOCK_DISABLE()
#define ANALOG_CLOCK_IS_ENABLED()
#endif

#endif // BOOTLOADER_BUILD

// regi2c write/read requires analog i2c master clock enabled
#define REGI2C_CLOCK_ENABLE()      ANALOG_CLOCK_ENABLE()
#define REGI2C_CLOCK_DISABLE()     ANALOG_CLOCK_DISABLE()


#if NON_OS_BUILD
/**
 * If compiling for the non-FreeRTOS builds (e.g. bootloader), regi2c_impl functions can be called directly,
 * without the need of a lock.
 */
#define regi2c_ctrl_read_reg         regi2c_impl_read
#define regi2c_ctrl_read_reg_mask    regi2c_impl_read_mask
#define regi2c_ctrl_write_reg        regi2c_impl_write
#define regi2c_ctrl_write_reg_mask   regi2c_impl_write_mask

#define REGI2C_ENTER_CRITICAL()
#define REGI2C_EXIT_CRITICAL()
#else

/* Access regi2c registers, don't use in application
 * The following four functions have considered the atomicity of the operations and the clock gating enable/disable for
 * the analog i2c master clock.
 */
uint8_t regi2c_ctrl_read_reg(uint8_t block, uint8_t host_id, uint8_t reg_add);
uint8_t regi2c_ctrl_read_reg_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb);
void regi2c_ctrl_write_reg(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data);
void regi2c_ctrl_write_reg_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb, uint8_t data);

/* Enter the critical section that protects regi2c operations.
 * Use it if multiple regi2c operations are called in a row, a lock wrapped outside saves lock acquire/release time.
 */
void regi2c_enter_critical(void);
void regi2c_exit_critical(void);
#define REGI2C_ENTER_CRITICAL()  int __DECLARE_REGI2C_ATOMIC_ENV __attribute__((unused)); regi2c_enter_critical()
#define REGI2C_EXIT_CRITICAL()   regi2c_exit_critical()
#endif // NON_OS_BUILD

/* Convenience macros for the above functions, these use register definitions
 * from regi2c_xxx.h header files.
 */
#define REGI2C_WRITE_MASK(block, reg_add, indata) \
      regi2c_ctrl_write_reg_mask(block, block##_HOSTID,  reg_add,  reg_add##_MSB,  reg_add##_LSB,  indata)

#define REGI2C_READ_MASK(block, reg_add) \
      regi2c_ctrl_read_reg_mask(block, block##_HOSTID,  reg_add,  reg_add##_MSB,  reg_add##_LSB)

#define REGI2C_WRITE(block, reg_add, indata) \
      regi2c_ctrl_write_reg(block, block##_HOSTID,  reg_add, indata)

#define REGI2C_READ(block, reg_add) \
      regi2c_ctrl_read_reg(block, block##_HOSTID,  reg_add)

#ifdef __cplusplus
}
#endif
