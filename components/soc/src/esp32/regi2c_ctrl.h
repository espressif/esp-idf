// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdint.h>
#include "regi2c_apll.h"
#include "regi2c_bbpll.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Analog function control register */
#define ANA_CONFIG_REG  0x6000E044
#define ANA_CONFIG_S    (8)
#define ANA_CONFIG_M    (0x3FF)
/* Clear to enable APLL */
#define I2C_APLL_M      (BIT(14))
/* Clear to enable BBPLL */
#define I2C_BBPLL_M     (BIT(17))

/* ROM functions which read/write internal control bus */
uint8_t rom_i2c_readReg(uint8_t block, uint8_t host_id, uint8_t reg_add);
uint8_t rom_i2c_readReg_Mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb);
void rom_i2c_writeReg(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data);
void rom_i2c_writeReg_Mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb, uint8_t data);

#ifdef BOOTLOADER_BUILD

/**
 * If compiling for the bootloader, ROM functions can be called directly,
 * without the need of a lock.
 */
#define regi2c_ctrl_read_reg         rom_i2c_readReg
#define regi2c_ctrl_read_reg_mask    rom_i2c_readReg_Mask
#define regi2c_ctrl_write_reg        rom_i2c_writeReg
#define regi2c_ctrl_write_reg_mask   rom_i2c_writeReg_Mask

#else

#define i2c_read_reg_raw        rom_i2c_readReg
#define i2c_read_reg_mask_raw   rom_i2c_readReg_Mask
#define i2c_write_reg_raw       rom_i2c_writeReg
#define i2c_write_reg_mask_raw  rom_i2c_writeReg_Mask

uint8_t regi2c_ctrl_read_reg(uint8_t block, uint8_t host_id, uint8_t reg_add);
uint8_t regi2c_ctrl_read_reg_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb);
void regi2c_ctrl_write_reg(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data);
void regi2c_ctrl_write_reg_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb, uint8_t data);

/* enter the critical section that protects internal registers. Don't use it in SDK. Use the functions above. */
void regi2c_enter_critical(void);
void regi2c_exit_critical(void);

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

#ifdef __cplusplus
}
#endif
