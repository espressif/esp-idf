/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_rom_sys.h"
#include "esp_attr.h"
#include "soc/lp_i2c_ana_mst_reg.h"
#include "modem/modem_lpcon_reg.h"
/**
 * BB    - 0x67 - BIT0
 * TXRF  - 0x6B - BIT1
 * SDM   - 0x63 - BIT2
 * PLL   - 0x62 - BIT3
 * BIAS  - 0x6A - BIT4
 * BBPLL - 0x66 - BIT5
 * ULP   - 0x61 - BIT6
 * SAR   - 0x69 - BIT7
 * PMU   - 0x6d - BIT8
*/
#define REGI2C_ULP_CAL_DEVICE_EN  (BIT(6))
#define REGI2C_SAR_I2C_DEVICE_EN  (BIT(7))
#define REGI2C_BBPLL_DEVICE_EN    (BIT(5))
#define REGI2C_BIAS_DEVICE_EN     (BIT(4))
#define REGI2C_DIG_REG_DEVICE_EN  (BIT(8))

#define REGI2C_RTC_BUSY           (BIT(25))
#define REGI2C_RTC_BUSY_M         (BIT(25))
#define REGI2C_RTC_BUSY_V         0x1
#define REGI2C_RTC_BUSY_S         25

#define REGI2C_RTC_WR_CNTL        (BIT(24))
#define REGI2C_RTC_WR_CNTL_M      (BIT(24))
#define REGI2C_RTC_WR_CNTL_V      0x1
#define REGI2C_RTC_WR_CNTL_S      24

#define REGI2C_RTC_DATA           0x000000FF
#define REGI2C_RTC_DATA_M         ((I2C_RTC_DATA_V)<<(I2C_RTC_DATA_S))
#define REGI2C_RTC_DATA_V         0xFF
#define REGI2C_RTC_DATA_S         16

#define REGI2C_RTC_ADDR           0x000000FF
#define REGI2C_RTC_ADDR_M         ((I2C_RTC_ADDR_V)<<(I2C_RTC_ADDR_S))
#define REGI2C_RTC_ADDR_V         0xFF
#define REGI2C_RTC_ADDR_S         8

#define REGI2C_RTC_SLAVE_ID       0x000000FF
#define REGI2C_RTC_SLAVE_ID_M     ((I2C_RTC_SLAVE_ID_V)<<(I2C_RTC_SLAVE_ID_S))
#define REGI2C_RTC_SLAVE_ID_V     0xFF
#define REGI2C_RTC_SLAVE_ID_S     0

/* SLAVE */

#define REGI2C_BBPLL              (0x66)
#define REGI2C_BBPLL_HOSTID       0

#define REGI2C_BIAS               (0x6a)
#define REGI2C_BIAS_HOSTID        0

#define REGI2C_DIG_REG            (0x6d)
#define REGI2C_DIG_REG_HOSTID     0

#define REGI2C_ULP_CAL            (0x61)
#define REGI2C_ULP_CAL_HOSTID     0

#define REGI2C_SAR_I2C            (0x69)
#define REGI2C_SAR_I2C_HOSTID     0

/* SLAVE END */

#define REGI2C_RTC_MAGIC_DEFAULT (0x1C610)

static IRAM_ATTR void regi2c_enable_block(uint8_t block)
{
    REG_SET_BIT(MODEM_LPCON_CLK_CONF_REG, MODEM_LPCON_CLK_I2C_MST_EN);
    REG_SET_BIT(LP_I2C_ANA_MST_DATE_REG, LP_I2C_ANA_MST_I2C_MAT_CLK_EN);

    /* Before config I2C register, enable corresponding slave. */
    switch (block) {
    case REGI2C_BBPLL  :
        REG_SET_BIT(LP_I2C_ANA_MST_DEVICE_EN_REG, REGI2C_BBPLL_DEVICE_EN);
        break;
    case REGI2C_BIAS   :
        REG_SET_BIT(LP_I2C_ANA_MST_DEVICE_EN_REG, REGI2C_BIAS_DEVICE_EN);
        break;
    case REGI2C_DIG_REG:
        REG_SET_BIT(LP_I2C_ANA_MST_DEVICE_EN_REG, REGI2C_DIG_REG_DEVICE_EN);
        break;
    case REGI2C_ULP_CAL:
        REG_SET_BIT(LP_I2C_ANA_MST_DEVICE_EN_REG, REGI2C_ULP_CAL_DEVICE_EN);
        break;
    case REGI2C_SAR_I2C:
        REG_SET_BIT(LP_I2C_ANA_MST_DEVICE_EN_REG, REGI2C_SAR_I2C_DEVICE_EN);
        break;
    default:
        return;
    }
}

static IRAM_ATTR void regi2c_disable_block(uint8_t block)
{
    switch (block) {
    case REGI2C_BBPLL  :
        REG_CLR_BIT(LP_I2C_ANA_MST_DEVICE_EN_REG, REGI2C_BBPLL_DEVICE_EN);
        break;
    case REGI2C_BIAS   :
        REG_CLR_BIT(LP_I2C_ANA_MST_DEVICE_EN_REG, REGI2C_BIAS_DEVICE_EN);
        break;
    case REGI2C_DIG_REG:
        REG_CLR_BIT(LP_I2C_ANA_MST_DEVICE_EN_REG, REGI2C_DIG_REG_DEVICE_EN);
        break;
    case REGI2C_ULP_CAL:
        REG_CLR_BIT(LP_I2C_ANA_MST_DEVICE_EN_REG, REGI2C_ULP_CAL_DEVICE_EN);
        break;
    case REGI2C_SAR_I2C:
        REG_CLR_BIT(LP_I2C_ANA_MST_DEVICE_EN_REG, REGI2C_SAR_I2C_DEVICE_EN);
        break;
    default:
        return;
    }
}

uint8_t IRAM_ATTR esp_rom_regi2c_read(uint8_t block, uint8_t host_id, uint8_t reg_add)
{
    regi2c_enable_block(block);

    (void)host_id;
    uint32_t temp = ((block & REGI2C_RTC_SLAVE_ID_V) << REGI2C_RTC_SLAVE_ID_S)
                    | (reg_add & REGI2C_RTC_ADDR_V) << REGI2C_RTC_ADDR_S;
    REG_WRITE(LP_I2C_ANA_MST_I2C0_CTRL_REG, temp);
    while (REG_GET_BIT(LP_I2C_ANA_MST_I2C0_CTRL_REG, LP_I2C_ANA_MST_I2C0_BUSY));
    uint8_t ret = REG_GET_FIELD(LP_I2C_ANA_MST_I2C0_DATA_REG, LP_I2C_ANA_MST_I2C0_RDATA);

    regi2c_disable_block(block);

    return ret;
}

uint8_t IRAM_ATTR esp_rom_regi2c_read_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb)
{
    assert(msb - lsb < 8);
    regi2c_enable_block(block);

    (void)host_id;
    uint32_t temp = ((block & REGI2C_RTC_SLAVE_ID_V) << REGI2C_RTC_SLAVE_ID_S)
                    | (reg_add & REGI2C_RTC_ADDR_V) << REGI2C_RTC_ADDR_S;
    REG_WRITE(LP_I2C_ANA_MST_I2C0_CTRL_REG, temp);
    while (REG_GET_BIT(LP_I2C_ANA_MST_I2C0_CTRL_REG, LP_I2C_ANA_MST_I2C0_BUSY));
    uint32_t data = REG_GET_FIELD(LP_I2C_ANA_MST_I2C0_DATA_REG, LP_I2C_ANA_MST_I2C0_RDATA);
    uint8_t ret =  (uint8_t)((data >> lsb) & (~(0xFFFFFFFF << (msb - lsb + 1))));

    regi2c_disable_block(block);

    return ret;
}

void IRAM_ATTR esp_rom_regi2c_write(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data)
{
    (void)host_id;
    regi2c_enable_block(block);

    uint32_t temp = ((block & REGI2C_RTC_SLAVE_ID_V) << REGI2C_RTC_SLAVE_ID_S)
                    | ((reg_add & REGI2C_RTC_ADDR_V) << REGI2C_RTC_ADDR_S)
                    | ((0x1 & REGI2C_RTC_WR_CNTL_V) << REGI2C_RTC_WR_CNTL_S) // 0: READ I2C register; 1: Write I2C register;
                    | (((uint32_t)data & REGI2C_RTC_DATA_V) << REGI2C_RTC_DATA_S);
    REG_WRITE(LP_I2C_ANA_MST_I2C0_CTRL_REG, temp);
    while (REG_GET_BIT(LP_I2C_ANA_MST_I2C0_CTRL_REG, LP_I2C_ANA_MST_I2C0_BUSY));

    regi2c_disable_block(block);
}

void IRAM_ATTR esp_rom_regi2c_write_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb, uint8_t data)
{
    (void)host_id;
    assert(msb - lsb < 8);
    regi2c_enable_block(block);

    /*Read the i2c bus register*/
    uint32_t temp = ((block & REGI2C_RTC_SLAVE_ID_V) << REGI2C_RTC_SLAVE_ID_S)
                    | (reg_add & REGI2C_RTC_ADDR_V) << REGI2C_RTC_ADDR_S;
    REG_WRITE(LP_I2C_ANA_MST_I2C0_CTRL_REG, temp);
    while (REG_GET_BIT(LP_I2C_ANA_MST_I2C0_CTRL_REG, LP_I2C_ANA_MST_I2C0_BUSY));
    temp = REG_GET_FIELD(LP_I2C_ANA_MST_I2C0_DATA_REG, LP_I2C_ANA_MST_I2C0_RDATA);
    /*Write the i2c bus register*/
    temp &= ((~(0xFFFFFFFF << lsb)) | (0xFFFFFFFF << (msb + 1)));
    temp = (((uint32_t)data & (~(0xFFFFFFFF << (msb - lsb + 1)))) << lsb) | temp;
    temp = ((block & REGI2C_RTC_SLAVE_ID_V) << REGI2C_RTC_SLAVE_ID_S)
            | ((reg_add & REGI2C_RTC_ADDR_V) << REGI2C_RTC_ADDR_S)
            | ((0x1 & REGI2C_RTC_WR_CNTL_V) << REGI2C_RTC_WR_CNTL_S)
            | ((temp & REGI2C_RTC_DATA_V) << REGI2C_RTC_DATA_S);
    REG_WRITE(LP_I2C_ANA_MST_I2C0_CTRL_REG, temp);
    while (REG_GET_BIT(LP_I2C_ANA_MST_I2C0_CTRL_REG, LP_I2C_ANA_MST_I2C0_BUSY));

    regi2c_disable_block(block);
}
