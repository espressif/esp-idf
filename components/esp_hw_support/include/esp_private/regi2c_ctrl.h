/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "sdkconfig.h"
#include "esp_rom_regi2c.h"
#include "soc/regi2c_defs.h"

#ifdef __cplusplus
extern "C" {
#endif


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

uint8_t regi2c_ctrl_read_reg(uint8_t block, uint8_t host_id, uint8_t reg_add);
uint8_t regi2c_ctrl_read_reg_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb);
void regi2c_ctrl_write_reg(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data);
void regi2c_ctrl_write_reg_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb, uint8_t data);

#endif // BOOTLOADER_BUILD

/* Convenience macros for the above functions, these use register definitions
 * from regi2c_apll.h/regi2c_bbpll.h header files.
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

#ifdef __cplusplus
}
#endif
