/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hal/regi2c_impl.h"
#include "hal/assert.h"
#include "modem/i2c_ana_mst_reg.h"
#include "hal/regi2c_ctrl_ll.h"
#include "hal/config.h"

/* SLAVE */
#define REGI2C_BB           (0X67)
#define REGI2C_TXRF         (0X6B)
#define REGI2C_SDM          (0X63)
#define REGI2C_RFPLL        (0X62)
#define REGI2C_BIAS         (0X6A)
#define REGI2C_BBPLL        (0x66)
#define REGI2C_ULP          (0x61)
#define REGI2C_SAR_MASTER   (0X10)
#define REGI2C_SAR_SLAVE    (0X11)
#define REGI2C_PERIF        (0X69)
#define REGI2C_APLL         (0X0C)
#define REGI2C_CPLL         (0X0A)
#define REGI2C_MPLL         (0X0B)
#define REGI2C_DIG_REG      (0X6D)
/* SLAVE END */

static uint8_t regi2c_enable_block(uint8_t block)
{
#if !HAL_CONFIG(ENV_FPGA) // on FPGA, unable to manipulate modem registers, skip the check
    HAL_ASSERT(regi2c_ctrl_ll_master_is_clock_enabled());
#endif

    /* Before config I2C register, enable corresponding slave. */
    uint16_t bit_mask = 0;
    switch (block) {
    case REGI2C_BB:
        bit_mask = REGI2C_BB_BIT_MASK;
        break;
    case REGI2C_TXRF:
        bit_mask = REGI2C_TXRF_BIT_MASK;
        break;
    case REGI2C_SDM:
        bit_mask = REGI2C_SDM_BIT_MASK;
        break;
    case REGI2C_RFPLL:
        bit_mask = REGI2C_RFPLL_BIT_MASK;
        break;
    case REGI2C_BIAS:
        bit_mask = REGI2C_BIAS_BIT_MASK;
        break;
    case REGI2C_BBPLL:
        bit_mask = REGI2C_BBPLL_BIT_MASK;
        break;
    case REGI2C_ULP:
        bit_mask = REGI2C_ULP_BIT_MASK;
        break;
    case REGI2C_SAR_MASTER:
        bit_mask = REGI2C_SAR_MASTER_BIT_MASK;
        break;
    case REGI2C_SAR_SLAVE:
        bit_mask = REGI2C_SAR_SLAVE_BIT_MASK;
        break;
    case REGI2C_PERIF:
        bit_mask = REGI2C_PERIF_BIT_MASK;
        break;
    case REGI2C_APLL:
        bit_mask = REGI2C_APLL_BIT_MASK;
        break;
    case REGI2C_CPLL:
        bit_mask = REGI2C_CPLL_BIT_MASK;
        break;
    case REGI2C_MPLL:
        bit_mask = REGI2C_MPLL_BIT_MASK;
        break;
    case REGI2C_DIG_REG:
        bit_mask = REGI2C_DIG_REG_BIT_MASK;
        break;
    default:
        break;
    }

    uint32_t i2c_sel = REG_GET_BIT(I2C_ANA_MST_ANA_CONF2_REG, bit_mask << REGI2C_CONF2_SLAVE_SEL_V);
    REG_WRITE(I2C_ANA_MST_ANA_CONF1_REG, ~(bit_mask << REGI2C_CONF1_SLAVE_SEL_V) & I2C_ANA_MST_ANA_CONF1_M);
    return (uint8_t)(i2c_sel ? 0 : 1);
}

uint8_t _regi2c_impl_read(uint8_t block, uint8_t host_id, uint8_t reg_add)
{
    (void)host_id;
    uint8_t i2c_sel = regi2c_enable_block(block);

    while (REG_GET_BIT(I2C_ANA_MST_I2C_CTRL_REG(i2c_sel), REGI2C_RTC_BUSY));          // wait i2c idle
    uint32_t temp = ((block & REGI2C_RTC_SLAVE_ID_V) << REGI2C_RTC_SLAVE_ID_S)
                    | (reg_add & REGI2C_RTC_ADDR_V) << REGI2C_RTC_ADDR_S;
    REG_WRITE(I2C_ANA_MST_I2C_CTRL_REG(i2c_sel), temp);
    while (REG_GET_BIT(I2C_ANA_MST_I2C_CTRL_REG(i2c_sel), REGI2C_RTC_BUSY));
    uint8_t ret = REG_GET_FIELD(I2C_ANA_MST_I2C_CTRL_REG(i2c_sel), REGI2C_RTC_DATA);

    return ret;
}

uint8_t _regi2c_impl_read_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb)
{
    HAL_ASSERT(msb - lsb < 8);
    uint8_t i2c_sel = regi2c_enable_block(block);

    (void)host_id;
    while (REG_GET_BIT(I2C_ANA_MST_I2C_CTRL_REG(i2c_sel), REGI2C_RTC_BUSY)); // wait i2c idle
    uint32_t temp = ((block & REGI2C_RTC_SLAVE_ID_V) << REGI2C_RTC_SLAVE_ID_S)
                    | (reg_add & REGI2C_RTC_ADDR_V) << REGI2C_RTC_ADDR_S;
    REG_WRITE(I2C_ANA_MST_I2C_CTRL_REG(i2c_sel), temp);
    while (REG_GET_BIT(I2C_ANA_MST_I2C_CTRL_REG(i2c_sel), REGI2C_RTC_BUSY));
    uint32_t data = REG_GET_FIELD(I2C_ANA_MST_I2C_CTRL_REG(i2c_sel), REGI2C_RTC_DATA);
    uint8_t ret = (uint8_t)((data >> lsb) & (~(0xFFFFFFFF << (msb - lsb + 1))));

    return ret;
}

void _regi2c_impl_write(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data)
{
    (void)host_id;
    uint8_t i2c_sel = regi2c_enable_block(block);

    while (REG_GET_BIT(I2C_ANA_MST_I2C_CTRL_REG(i2c_sel), REGI2C_RTC_BUSY)); // wait i2c idle
    uint32_t temp = ((block & REGI2C_RTC_SLAVE_ID_V) << REGI2C_RTC_SLAVE_ID_S)
                    | ((reg_add & REGI2C_RTC_ADDR_V) << REGI2C_RTC_ADDR_S)
                    | ((0x1 & REGI2C_RTC_WR_CNTL_V) << REGI2C_RTC_WR_CNTL_S) // 0: READ I2C register; 1: Write I2C register;
                    | (((uint32_t)data & REGI2C_RTC_DATA_V) << REGI2C_RTC_DATA_S);
    REG_WRITE(I2C_ANA_MST_I2C_CTRL_REG(i2c_sel), temp);
    while (REG_GET_BIT(I2C_ANA_MST_I2C_CTRL_REG(i2c_sel), REGI2C_RTC_BUSY));
}

void _regi2c_impl_write_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb, uint8_t data)
{
    (void)host_id;
    HAL_ASSERT(msb - lsb < 8);
    uint8_t i2c_sel = regi2c_enable_block(block);

    while (REG_GET_BIT(I2C_ANA_MST_I2C_CTRL_REG(i2c_sel), REGI2C_RTC_BUSY));
    /*Read the i2c bus register*/
    uint32_t temp = ((block & REGI2C_RTC_SLAVE_ID_V) << REGI2C_RTC_SLAVE_ID_S)
                    | (reg_add & REGI2C_RTC_ADDR_V) << REGI2C_RTC_ADDR_S;
    REG_WRITE(I2C_ANA_MST_I2C_CTRL_REG(i2c_sel), temp);
    while (REG_GET_BIT(I2C_ANA_MST_I2C_CTRL_REG(i2c_sel), REGI2C_RTC_BUSY));
    temp = REG_GET_FIELD(I2C_ANA_MST_I2C_CTRL_REG(i2c_sel), REGI2C_RTC_DATA);
    /*Write the i2c bus register*/
    temp &= ((~(0xFFFFFFFF << lsb)) | (0xFFFFFFFF << (msb + 1)));
    temp = (((uint32_t)data & (~(0xFFFFFFFF << (msb - lsb + 1)))) << lsb) | temp;
    temp = ((block & REGI2C_RTC_SLAVE_ID_V) << REGI2C_RTC_SLAVE_ID_S)
           | ((reg_add & REGI2C_RTC_ADDR_V) << REGI2C_RTC_ADDR_S)
           | ((0x1 & REGI2C_RTC_WR_CNTL_V) << REGI2C_RTC_WR_CNTL_S)
           | ((temp & REGI2C_RTC_DATA_V) << REGI2C_RTC_DATA_S);
    REG_WRITE(I2C_ANA_MST_I2C_CTRL_REG(i2c_sel), temp);
    while (REG_GET_BIT(I2C_ANA_MST_I2C_CTRL_REG(i2c_sel), REGI2C_RTC_BUSY));
}
