/*
 * SPDX-FileCopyrightText: 2019-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hal/regi2c_impl.h"
#include "hal/assert.h"
#include "soc/lp_i2c_ana_mst_reg.h"
#include "hal/regi2c_ctrl_ll.h"
/**
 * DIG_REG      - 0x6D - BIT10
 * PLL_CPU      - 0x67 - BIT11
 * PLL_SDIO     - 0x62 - BIT6
 * BIAS         - 0x6A - BIT12
 * MSPI_XTAL    - 0x63 - BIT9
 * PLL_SYS      - 0x66 - BIT5
 * PLLA         - 0x6F - BIT8
 * SAR_I2C      - 0x69 - BIT7
*/

#define REGI2C_DIG_REG_MST_SEL     (BIT(10))
#define REGI2C_PLL_CPU_MST_SEL     (BIT(11))
#define REGI2C_PLL_SDIO_MST_SEL    (BIT(6))
#define REGI2C_BIAS_MST_SEL        (BIT(12))
#define REGI2C_MSPI_XTAL_MST_SEL   (BIT(9))
#define REGI2C_PLL_SYS_MST_SEL     (BIT(5))
#define REGI2C_PLLA_MST_SEL        (BIT(8))
#define REGI2C_SAR_I2C_MST_SEL     (BIT(7))

#define I2C_ANA_MST_I2C_CTRL_REG(n) (LP_I2C_ANA_MST_I2C0_CTRL_REG + n*4) // 0: I2C_ANA_MST_I2C0_CTRL_REG; 1: I2C_ANA_MST_I2C1_CTRL_REG

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
#define REGI2C_DIG_REG  (0x6d)
#define REGI2C_DIG_REG_HOSTID   0

#define REGI2C_CPU_PLL    (0x67)
#define REGI2C_CPU_PLL_HOSTID     0

#define REGI2C_SDIO_PLL    (0x62)
#define REGI2C_SDIO_PLL_HOSTID     0

#define REGI2C_BIAS     (0x6a)
#define REGI2C_BIAS_HOSTID      0

#define REGI2C_MSPI    (0x63)
#define REGI2C_MSPI_HOSTID     0

#define REGI2C_SYS_PLL    (0x66)
#define REGI2C_SYS_PLL_HOSTID     0

#define REGI2C_PLLA  (0x6f)
#define REGI2C_PLLA_HOSTID   0

#define REGI2C_SAR_I2C  (0x69)
#define REGI2C_SAR_I2C_HOSTID   0
/* SLAVE END */

static uint8_t regi2c_enable_block(uint8_t block)
{
    HAL_ASSERT(regi2c_ctrl_ll_master_is_clock_enabled());
    REG_SET_FIELD(LP_I2C_ANA_MST_ANA_CONF2_REG, LP_I2C_ANA_MST_ANA_CONF2, 0);
    REG_SET_FIELD(LP_I2C_ANA_MST_ANA_CONF1_REG, LP_I2C_ANA_MST_ANA_CONF1, 0);

    switch (block) {
    case REGI2C_DIG_REG:
        REG_SET_BIT(LP_I2C_ANA_MST_ANA_CONF2_REG, REGI2C_DIG_REG_MST_SEL);
        break;
    case REGI2C_CPU_PLL:
        REG_SET_BIT(LP_I2C_ANA_MST_ANA_CONF2_REG, REGI2C_PLL_CPU_MST_SEL);
        break;
    case REGI2C_SDIO_PLL:
        REG_SET_BIT(LP_I2C_ANA_MST_ANA_CONF2_REG, REGI2C_PLL_SDIO_MST_SEL);
        break;
    case REGI2C_BIAS:
        REG_SET_BIT(LP_I2C_ANA_MST_ANA_CONF2_REG, REGI2C_BIAS_MST_SEL);
        break;
    case REGI2C_MSPI:
        REG_SET_BIT(LP_I2C_ANA_MST_ANA_CONF2_REG, REGI2C_MSPI_XTAL_MST_SEL);
        break;
    case REGI2C_SYS_PLL:
        REG_SET_BIT(LP_I2C_ANA_MST_ANA_CONF2_REG, REGI2C_PLL_SYS_MST_SEL);
        break;
    case REGI2C_PLLA:
        REG_SET_BIT(LP_I2C_ANA_MST_ANA_CONF2_REG, REGI2C_PLLA_MST_SEL);
        break;
    case REGI2C_SAR_I2C:
        REG_SET_BIT(LP_I2C_ANA_MST_ANA_CONF2_REG, REGI2C_SAR_I2C_MST_SEL);
        break;
    }

    return 0;
}

uint8_t _regi2c_impl_read(uint8_t block, uint8_t host_id, uint8_t reg_add)
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
