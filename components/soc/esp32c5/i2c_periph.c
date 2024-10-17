/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/i2c_periph.h"
#include "soc/gpio_sig_map.h"

/*
 Bunch of constants for every I2C peripheral: GPIO signals, irqs, hw addr of registers etc
*/
typedef enum
{
    LP_I2C_MUX_FUNC         = 0,
    LP_GPIO_MUX_FUNC        = 1,
    LP_IO_MUX_FUNC_NUM      = 2,
    LP_MUX_FUNC_NOT_USED    = 0xFF,
} lp_io_mux_func_t;

static_assert(SOC_I2C_NUM == (SOC_HP_I2C_NUM + SOC_LP_I2C_NUM));

const i2c_signal_conn_t i2c_periph_signal[SOC_I2C_NUM] = {
    /* I2C_NUM_0*/
    {
        .sda_out_sig = I2CEXT0_SDA_OUT_IDX,
        .sda_in_sig = I2CEXT0_SDA_IN_IDX,
        .scl_out_sig = I2CEXT0_SCL_OUT_IDX,
        .scl_in_sig = I2CEXT0_SCL_IN_IDX,
        .iomux_func = (uint8_t)LP_MUX_FUNC_NOT_USED,
        .irq = ETS_I2C_EXT0_INTR_SOURCE,
    },
    /* LP_I2C_NUM_0*/
    {
        .sda_out_sig = 0,
        .sda_in_sig = 0,
        .scl_out_sig = 0,
        .scl_in_sig = 0,
        .iomux_func = (uint8_t)LP_I2C_MUX_FUNC,
        .irq = ETS_LP_I2C_INTR_SOURCE,
    },
};

// I2C sleep retention entries
// I2C registers require set the reg_update bit to make the configuration take effect

/* I2C Registers Context
   Include: I2C_SCL_LOW_PERIOD_REG /
            I2C_CTR_REG / I2C_TO_REG / I2C_SLAVE_ADDR_REG / I2C_FIFO_CONF_REG
            I2C_INT_ENA_REG / I2C_SDA_HOLD_REG / I2C_SDA_SAMPLE_REG / I2C_SCL_START_HOLD_REG
            I2C_SCL_RSTART_SETUP_REG / I2C_SCL_STOP_HOLD_REG / I2C_SCL_STOP_SETUP_REG /I2C_FILTER_CFG_REG / I2C_CLK_CONF_REG / I2C_SCL_ST_TIME_OUT_REG / I2C_SCL_MAIN_ST_TIME_OUT_REG / I2C_SCL_SP_CONF_REG / I2C_SCL_STRETCH_CONF_REG
*/
#define I2C0_RETENTION_REGS_CNT  18
#define I2C0_RETENTION_MAP_BASE  I2C_SCL_LOW_PERIOD_REG(0)
static const uint32_t i2c0_regs_map[4] = {0xc03f345b, 0x3, 0, 0};
static const regdma_entries_config_t i2c0_regs_retention[] = {
    [0] = {.config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_I2C_LINK(0x00), I2C0_RETENTION_MAP_BASE, I2C0_RETENTION_MAP_BASE, I2C0_RETENTION_REGS_CNT, 0, 0, i2c0_regs_map[0], i2c0_regs_map[1], i2c0_regs_map[2], i2c0_regs_map[3]), \
           .owner = ENTRY(0) | ENTRY(2) }, \
    [1] = {.config = REGDMA_LINK_WRITE_INIT(REGDMA_I2C_LINK(0x01), I2C_CTR_REG(0), I2C_FSM_RST, I2C_FSM_RST_M, 1, 0), \
           .owner = ENTRY(0) | ENTRY(2) }, \
    [2] = {.config = REGDMA_LINK_WRITE_INIT(REGDMA_I2C_LINK(0x02), I2C_CTR_REG(0), 0x0, I2C_FSM_RST_M, 1, 0), \
           .owner = ENTRY(0) | ENTRY(2) }, \
    [3] = {.config = REGDMA_LINK_WRITE_INIT(REGDMA_I2C_LINK(0x03), I2C_CTR_REG(0), I2C_CONF_UPGATE, I2C_CONF_UPGATE_M, 1, 0), \
           .owner = ENTRY(0) | ENTRY(2) }, \
    [4] = {.config = REGDMA_LINK_WAIT_INIT(REGDMA_I2C_LINK(0x04), I2C_CTR_REG(0), 0x0, I2C_CONF_UPGATE_M, 1, 0), \
           .owner = ENTRY(0) | ENTRY(2) }, \
};

const i2c_reg_ctx_link_t i2c_regs_retention[SOC_HP_I2C_NUM] = {
    {i2c0_regs_retention, ARRAY_SIZE(i2c0_regs_retention), SLEEP_RETENTION_MODULE_I2C0},
};
