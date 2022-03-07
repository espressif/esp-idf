// Copyright 2019-2020 Espressif Systems (Shanghai) PTE LTD
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

#include "soc/soc.h"
#include "soc/apb_ctrl_reg.h"
#include "regi2c_ctrl.h"
#include "regi2c_brownout.h"
#include "esp_attr.h"

#define I2C_RTC_WIFI_CLK_EN (APB_CTRL_WIFI_CLK_EN_REG)

#define I2C_RTC_CLK_GATE_EN    (BIT(18))
#define I2C_RTC_CLK_GATE_EN_M  (BIT(18))
#define I2C_RTC_CLK_GATE_EN_V  0x1
#define I2C_RTC_CLK_GATE_EN_S  18

#define I2C_RTC_CONFIG0  0x6000e048

#define I2C_RTC_MAGIC_CTRL 0x00001FFF
#define I2C_RTC_MAGIC_CTRL_M  ((I2C_RTC_MAGIC_CTRL_V)<<(I2C_RTC_MAGIC_CTRL_S))
#define I2C_RTC_MAGIC_CTRL_V  0x1FFF
#define I2C_RTC_MAGIC_CTRL_S  4

#define I2C_RTC_CONFIG1  0x6000e044

#define I2C_RTC_BOD_MASK (BIT(22))
#define I2C_RTC_BOD_MASK_M  (BIT(22))
#define I2C_RTC_BOD_MASK_V  0x1
#define I2C_RTC_BOD_MASK_S  22

#define I2C_RTC_SAR_MASK (BIT(18))
#define I2C_RTC_SAR_MASK_M  (BIT(18))
#define I2C_RTC_SAR_MASK_V  0x1
#define I2C_RTC_SAR_MASK_S  18

#define I2C_RTC_BBPLL_MASK (BIT(17))
#define I2C_RTC_BBPLL_MASK_M  (BIT(17))
#define I2C_RTC_BBPLL_MASK_V  0x1
#define I2C_RTC_BBPLL_MASK_S  17

#define I2C_RTC_APLL_MASK (BIT(14))
#define I2C_RTC_APLL_MASK_M  (BIT(14))
#define I2C_RTC_APLL_MASK_V  0x1
#define I2C_RTC_APLL_MASK_S  14

#define I2C_RTC_ALL_MASK 0x00007FFF
#define I2C_RTC_ALL_MASK_M  ((I2C_RTC_ALL_MASK_V)<<(I2C_RTC_ALL_MASK_S))
#define I2C_RTC_ALL_MASK_V  0x7FFF
#define I2C_RTC_ALL_MASK_S  8

#define I2C_RTC_CONFIG2  0x6000e000

#define I2C_RTC_BUSY (BIT(25))
#define I2C_RTC_BUSY_M  (BIT(25))
#define I2C_RTC_BUSY_V  0x1
#define I2C_RTC_BUSY_S  25

#define I2C_RTC_WR_CNTL (BIT(24))
#define I2C_RTC_WR_CNTL_M  (BIT(24))
#define I2C_RTC_WR_CNTL_V  0x1
#define I2C_RTC_WR_CNTL_S  24

#define I2C_RTC_DATA 0x000000FF
#define I2C_RTC_DATA_M  ((I2C_RTC_DATA_V)<<(I2C_RTC_DATA_S))
#define I2C_RTC_DATA_V  0xFF
#define I2C_RTC_DATA_S  16

#define I2C_RTC_ADDR 0x000000FF
#define I2C_RTC_ADDR_M  ((I2C_RTC_ADDR_V)<<(I2C_RTC_ADDR_S))
#define I2C_RTC_ADDR_V  0xFF
#define I2C_RTC_ADDR_S  8

#define I2C_RTC_SLAVE_ID 0x000000FF
#define I2C_RTC_SLAVE_ID_M  ((I2C_RTC_SLAVE_ID_V)<<(I2C_RTC_SLAVE_ID_S))
#define I2C_RTC_SLAVE_ID_V  0xFF
#define I2C_RTC_SLAVE_ID_S  0

#define I2C_RTC_MAGIC_DEFAULT (0x1c40)

static void i2c_rtc_enable_block(uint8_t block)
{
    REG_SET_FIELD(I2C_RTC_CONFIG0, I2C_RTC_MAGIC_CTRL, I2C_RTC_MAGIC_DEFAULT);
    REG_SET_FIELD(I2C_RTC_CONFIG1, I2C_RTC_ALL_MASK, I2C_RTC_ALL_MASK_V);
    REG_SET_BIT(I2C_RTC_WIFI_CLK_EN, I2C_RTC_CLK_GATE_EN);
    switch (block) {
    case I2C_APLL:
        REG_CLR_BIT(I2C_RTC_CONFIG1, I2C_RTC_APLL_MASK);
        break;
    case I2C_BBPLL:
        REG_CLR_BIT(I2C_RTC_CONFIG1, I2C_RTC_BBPLL_MASK);
        break;
    case I2C_SAR_ADC:
        REG_CLR_BIT(I2C_RTC_CONFIG1, I2C_RTC_SAR_MASK);
        break;
    case I2C_BOD:
        REG_CLR_BIT(I2C_RTC_CONFIG1, I2C_RTC_BOD_MASK);
        break;
    }
}

uint8_t i2c_rtc_read_reg(uint8_t block, uint8_t host_id, uint8_t reg_add)
{
    i2c_rtc_enable_block(block);

    uint32_t temp = ((block & I2C_RTC_SLAVE_ID_V) << I2C_RTC_SLAVE_ID_S)
                    | (reg_add & I2C_RTC_ADDR_V) << I2C_RTC_ADDR_S;
    REG_WRITE(I2C_RTC_CONFIG2, temp);
    while (REG_GET_BIT(I2C_RTC_CONFIG2, I2C_RTC_BUSY));
    return REG_GET_FIELD(I2C_RTC_CONFIG2, I2C_RTC_DATA);
}

uint8_t i2c_rtc_read_reg_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb)
{
    assert(msb - lsb < 8);
    i2c_rtc_enable_block(block);

    uint32_t temp = ((block & I2C_RTC_SLAVE_ID_V) << I2C_RTC_SLAVE_ID_S)
                    | (reg_add & I2C_RTC_ADDR_V) << I2C_RTC_ADDR_S;
    REG_WRITE(I2C_RTC_CONFIG2, temp);
    while (REG_GET_BIT(I2C_RTC_CONFIG2, I2C_RTC_BUSY));
    uint32_t data = REG_GET_FIELD(I2C_RTC_CONFIG2, I2C_RTC_DATA);
    return (uint8_t)((data >> lsb) & (~(0xFFFFFFFF << (msb - lsb + 1))));
}

void i2c_rtc_write_reg(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data)
{
    i2c_rtc_enable_block(block);

    uint32_t temp = ((block & I2C_RTC_SLAVE_ID_V) << I2C_RTC_SLAVE_ID_S)
                    | ((reg_add & I2C_RTC_ADDR_V) << I2C_RTC_ADDR_S)
                    | ((0x1 & I2C_RTC_WR_CNTL_V) << I2C_RTC_WR_CNTL_S)
                    | (((uint32_t)data & I2C_RTC_DATA_V) << I2C_RTC_DATA_S);
    REG_WRITE(I2C_RTC_CONFIG2, temp);
    while (REG_GET_BIT(I2C_RTC_CONFIG2, I2C_RTC_BUSY));
}

void i2c_rtc_write_reg_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb, uint8_t data)
{
    assert(msb - lsb < 8);
    i2c_rtc_enable_block(block);

    /*Read the i2c bus register*/
    uint32_t temp = ((block & I2C_RTC_SLAVE_ID_V) << I2C_RTC_SLAVE_ID_S)
                    | (reg_add & I2C_RTC_ADDR_V) << I2C_RTC_ADDR_S;
    REG_WRITE(I2C_RTC_CONFIG2, temp);
    while (REG_GET_BIT(I2C_RTC_CONFIG2, I2C_RTC_BUSY));
    temp = REG_GET_FIELD(I2C_RTC_CONFIG2, I2C_RTC_DATA);
    /*Write the i2c bus register*/
    temp &= ((~(0xFFFFFFFF << lsb)) | (0xFFFFFFFF << (msb + 1)));
    temp = (((uint32_t)data & (~(0xFFFFFFFF << (msb - lsb + 1)))) << lsb) | temp;
    temp = ((block & I2C_RTC_SLAVE_ID_V) << I2C_RTC_SLAVE_ID_S)
            | ((reg_add & I2C_RTC_ADDR_V) << I2C_RTC_ADDR_S)
            | ((0x1 & I2C_RTC_WR_CNTL_V) << I2C_RTC_WR_CNTL_S)
            | ((temp & I2C_RTC_DATA_V) << I2C_RTC_DATA_S);
    REG_WRITE(I2C_RTC_CONFIG2, temp);
    while (REG_GET_BIT(I2C_RTC_CONFIG2, I2C_RTC_BUSY));
}

/**
 * Restore regi2c analog calibration related configuration registers.
 * This is a workaround, and is fixed on later chips
 */

static DRAM_ATTR uint8_t reg_val[REGI2C_ANA_CALI_BYTE_NUM];

void IRAM_ATTR regi2c_analog_cali_reg_read(void)
{
    for (int i = 0; i < REGI2C_ANA_CALI_BYTE_NUM; i++) {
        reg_val[i] = regi2c_ctrl_read_reg(I2C_SAR_ADC, I2C_SAR_ADC_HOSTID, i);
    }
}

void IRAM_ATTR regi2c_analog_cali_reg_write(void)
{
    for (int i = 0; i < REGI2C_ANA_CALI_BYTE_NUM; i++) {
        regi2c_ctrl_write_reg(I2C_SAR_ADC, I2C_SAR_ADC_HOSTID, i, reg_val[i]);
    }
}
