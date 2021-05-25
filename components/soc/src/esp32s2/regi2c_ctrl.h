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
#include "regi2c_ulp.h"
#include "regi2c_saradc.h"

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
/* Clear to enable SAR */
#define I2C_SAR_M       (BIT(18))

#define ANA_CONFIG2_REG 0x6000E048
#define ANA_SAR_CFG2_M  (BIT(16))

/* Read/Write internal control bus */
uint8_t i2c_rtc_read_reg(uint8_t block, uint8_t host_id, uint8_t reg_add);
uint8_t i2c_rtc_read_reg_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb);
void i2c_rtc_write_reg(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data);
void i2c_rtc_write_reg_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb, uint8_t data);
void i2c_rtc_init(void);

#ifdef BOOTLOADER_BUILD

/**
 * If compiling for the bootloader, internal functions can be called directly,
 * without the need of a lock.
 */
#define regi2c_ctrl_read_reg         i2c_rtc_read_reg
#define regi2c_ctrl_read_reg_mask    i2c_rtc_read_reg_mask
#define regi2c_ctrl_write_reg        i2c_rtc_write_reg
#define regi2c_ctrl_write_reg_mask   i2c_rtc_write_reg_mask

#else

#define i2c_read_reg_raw        i2c_rtc_read_reg
#define i2c_read_reg_mask_raw   i2c_rtc_read_reg_mask
#define i2c_write_reg_raw       i2c_rtc_write_reg
#define i2c_write_reg_mask_raw  i2c_rtc_write_reg_mask

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

#ifdef __cplusplus
}
#endif
