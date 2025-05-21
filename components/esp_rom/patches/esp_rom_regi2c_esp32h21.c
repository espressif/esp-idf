/**
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#include "esp_rom_sys.h"
#include "esp_attr.h"
#include "soc/i2c_ana_mst_reg.h"
#include "hal/regi2c_ctrl_ll.h"

/* SLAVE */
#define REGI2C_BBPLL       0x66    // regi2c_bbpll.h
#define REGI2C_BBTOP       0x67
#define REGI2C_DCDC        0x6D    // regi2c_pmu.h
#define REGI2C_PERIF       0x69    // regi2c_saradc.h
#define REGI2C_RFPLL       0x62
#define REGI2C_SDM         0x63
#define REGI2C_TXTOP       0x6B
#define REGI2C_ULP         0x61    // regi2c_lp_bias.h
#define REGI2C_BIAS        0x6A    // regi2c_bias.h

/* SLAVE END */

uint8_t esp_rom_regi2c_read(uint8_t block, uint8_t host_id, uint8_t reg_add) __attribute__((alias("regi2c_read_impl")));
uint8_t esp_rom_regi2c_read_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb) __attribute__((alias("regi2c_read_mask_impl")));
void esp_rom_regi2c_write(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data) __attribute__((alias("regi2c_write_impl")));
void esp_rom_regi2c_write_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb, uint8_t data) __attribute__((alias("regi2c_write_mask_impl")));

static IRAM_ATTR uint8_t regi2c_enable_block(uint8_t block)
{
    uint32_t i2c_sel = 0;
    assert(regi2c_ctrl_ll_master_is_clock_enabled());

    /* Before config I2C register, enable corresponding slave. */
    switch (block) {
        case REGI2C_BBPLL:
            i2c_sel = REG_GET_BIT(I2C_ANA_MST_ANA_CONF2_REG, REGI2C_CONF2_BBPLL_SEL);
            REG_WRITE(I2C_ANA_MST_ANA_CONF1_REG, ~REGI2C_CONF1_BBPLL_SEL & I2C_ANA_MST_ANA_CONF1_M);
            break;
        case REGI2C_BBTOP:
            i2c_sel = REG_GET_BIT(I2C_ANA_MST_ANA_CONF2_REG, REGI2C_CONF2_BBTOP_SEL);
            REG_WRITE(I2C_ANA_MST_ANA_CONF1_REG, ~REGI2C_CONF1_BBTOP_SEL & I2C_ANA_MST_ANA_CONF1_M);
            break;
        case REGI2C_DCDC:
            i2c_sel = REG_GET_BIT(I2C_ANA_MST_ANA_CONF2_REG, REGI2C_CONF2_PMU_SEL);
            REG_WRITE(I2C_ANA_MST_ANA_CONF1_REG, ~REGI2C_CONF1_PMU_SEL & I2C_ANA_MST_ANA_CONF1_M);
            break;
        case REGI2C_PERIF:
            i2c_sel = REG_GET_BIT(I2C_ANA_MST_ANA_CONF2_REG, REGI2C_CONF2_PERIF_SEL);
            REG_WRITE(I2C_ANA_MST_ANA_CONF1_REG, ~REGI2C_CONF1_PERIF_SEL & I2C_ANA_MST_ANA_CONF1_M);
            break;
        case REGI2C_RFPLL:
            i2c_sel = REG_GET_BIT(I2C_ANA_MST_ANA_CONF2_REG, REGI2C_CONF2_PLL_SEL);
            REG_WRITE(I2C_ANA_MST_ANA_CONF1_REG, ~REGI2C_CONF1_PLL_SEL   & I2C_ANA_MST_ANA_CONF1_M);
            break;
        case REGI2C_SDM:
            i2c_sel = REG_GET_BIT(I2C_ANA_MST_ANA_CONF2_REG, REGI2C_CONF2_SDM_SEL);
            REG_WRITE(I2C_ANA_MST_ANA_CONF1_REG, ~REGI2C_CONF1_SDM_SEL   & I2C_ANA_MST_ANA_CONF1_M);
            break;
        case REGI2C_TXTOP:
            i2c_sel = REG_GET_BIT(I2C_ANA_MST_ANA_CONF2_REG, REGI2C_CONF2_TXTOP_SEL);
            REG_WRITE(I2C_ANA_MST_ANA_CONF1_REG, ~REGI2C_CONF1_TXTOP_SEL & I2C_ANA_MST_ANA_CONF1_M);
            break;
        case REGI2C_ULP:
            i2c_sel = REG_GET_BIT(I2C_ANA_MST_ANA_CONF2_REG, REGI2C_CONF2_ULP_SEL);
            REG_WRITE(I2C_ANA_MST_ANA_CONF1_REG, ~REGI2C_CONF1_ULP_SEL   & I2C_ANA_MST_ANA_CONF1_M);
            break;
    }
    return (uint8_t)(i2c_sel ? 0 : 1);
}

uint8_t IRAM_ATTR regi2c_read_impl(uint8_t block, uint8_t host_id, uint8_t reg_add)
{
    (void)host_id;
    uint8_t i2c_sel = regi2c_enable_block(block);

    while (REG_GET_BIT(I2C_ANA_MST_I2C_CTRL_REG(i2c_sel), REGI2C_RTC_BUSY)); // wait i2c idle
    uint32_t temp = ((block & REGI2C_RTC_SLAVE_ID_V) << REGI2C_RTC_SLAVE_ID_S)
                    | (reg_add & REGI2C_RTC_ADDR_V) << REGI2C_RTC_ADDR_S;
    REG_WRITE(I2C_ANA_MST_I2C_CTRL_REG(i2c_sel), temp);
    while (REG_GET_BIT(I2C_ANA_MST_I2C_CTRL_REG(i2c_sel), REGI2C_RTC_BUSY));
    uint8_t ret = REG_GET_FIELD(I2C_ANA_MST_I2C_CTRL_REG(i2c_sel), REGI2C_RTC_DATA);

    return ret;
}

uint8_t IRAM_ATTR regi2c_read_mask_impl(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb)
{
    assert(msb - lsb < 8);
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

void IRAM_ATTR regi2c_write_impl(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data)
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

void IRAM_ATTR regi2c_write_mask_impl(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb, uint8_t data)
{
    (void)host_id;
    assert(msb - lsb < 8);
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
