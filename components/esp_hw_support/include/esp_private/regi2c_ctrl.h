/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "sdkconfig.h"
#include "esp_rom_regi2c.h"
#include "soc/regi2c_defs.h"
#include "soc/soc_caps.h"
#include "esp_private/periph_ctrl.h"
#include "hal/regi2c_ctrl_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BOOTLOADER_BUILD

// For bootloader, the strategy is to keep the analog i2c master clock always enabled if SOC_CLK_ANA_I2C_MST_HAS_ROOT_GATE (in bootloader_hardware_init())
#define ANALOG_CLOCK_ENABLE()
#define ANALOG_CLOCK_DISABLE()

#else // !BOOTLOADER_BUILD

#if SOC_CLK_ANA_I2C_MST_HAS_ROOT_GATE
// This clock needs to be enabled for regi2c write/read, pll calibaration, PHY, RNG, ADC, etc.
// Use reference count to manage the analog i2c master clock
#define ANALOG_CLOCK_ENABLE() \
      PERIPH_RCC_ACQUIRE_ATOMIC(PERIPH_ANA_I2C_MASTER_MODULE, ref_count) { \
            if (ref_count == 0) { \
                  regi2c_ctrl_ll_master_enable_clock(true); \
            } \
      }

#define ANALOG_CLOCK_DISABLE() \
      PERIPH_RCC_RELEASE_ATOMIC(PERIPH_ANA_I2C_MASTER_MODULE, ref_count) { \
            if (ref_count == 0) { \
                  regi2c_ctrl_ll_master_enable_clock(false); \
            } \
      }

#else
#define ANALOG_CLOCK_ENABLE()
#define ANALOG_CLOCK_DISABLE()
#endif

#endif // BOOTLOADER_BUILD

// regi2c write/read requires analog i2c master clock enabled
#define REGI2C_CLOCK_ENABLE()      ANALOG_CLOCK_ENABLE()
#define REGI2C_CLOCK_DISABLE()     ANALOG_CLOCK_DISABLE()


#define regi2c_read_reg_raw        esp_rom_regi2c_read
#define regi2c_read_reg_mask_raw   esp_rom_regi2c_read_mask
#define regi2c_write_reg_raw       esp_rom_regi2c_write
#define regi2c_write_reg_mask_raw  esp_rom_regi2c_write_mask


#ifdef BOOTLOADER_BUILD
/**
 * If compiling for the bootloader, ROM functions can be called directly,
 * without the need of a lock.
 */
#define regi2c_ctrl_read_reg         regi2c_read_reg_raw
#define regi2c_ctrl_read_reg_mask    regi2c_read_reg_mask_raw
#define regi2c_ctrl_write_reg        regi2c_write_reg_raw
#define regi2c_ctrl_write_reg_mask   regi2c_write_reg_mask_raw

#else

/* Access internal registers, don't use in application */
uint8_t regi2c_ctrl_read_reg(uint8_t block, uint8_t host_id, uint8_t reg_add);
uint8_t regi2c_ctrl_read_reg_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb);
void regi2c_ctrl_write_reg(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data);
void regi2c_ctrl_write_reg_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb, uint8_t data);

/* enter the critical section that protects internal registers. Don't use it in SDK. Use the functions above. */
void regi2c_enter_critical(void);
void regi2c_exit_critical(void);

#endif // BOOTLOADER_BUILD

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

/**
 * Restore regi2c analog calibration related configuration registers.
 * This is a workaround, and is fixed on later chips
 */
#if REGI2C_ANA_CALI_PD_WORKAROUND
void regi2c_analog_cali_reg_read(void);
void regi2c_analog_cali_reg_write(void);
#endif   //#if ADC_CALI_PD_WORKAROUND

#if SOC_TEMPERATURE_SENSOR_SUPPORT_SLEEP_RETENTION
// regi2c would be powered down in light sleep, but measure range is recorded
// in regi2c, so this function is used for record.
void regi2c_tsens_reg_read(void);
void regi2c_tsens_reg_write(void);
#endif

/* Enable/Disable regi2c_saradc with calling these two functions.
   With reference count protection inside.
   Internal use only.
*/
void regi2c_saradc_enable(void);
void regi2c_saradc_disable(void);

#ifdef __cplusplus
}
#endif
