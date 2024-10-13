/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bootloader_random.h"
#include "soc/soc.h"
#include "soc/adc_reg.h"
#include "soc/pmu_reg.h"
#include "soc/regi2c_saradc.h"
#include "soc/hp_sys_clkrst_reg.h"
#include "soc/lp_adc_reg.h"
#include "esp_private/regi2c_ctrl.h"
#include "esp_rom_regi2c.h"

// TODO IDF-6497: once ADC API is supported, use the API instead of defining functions and constants here

#define I2C_SAR_ADC_INIT_CODE_VAL 2166

typedef struct {
    int atten;
    int channel;
} pattern_item;

typedef struct {
    pattern_item item[4];
} pattern_table;

static void adc1_fix_initcode_set(uint32_t initcode_value)
{
    uint32_t msb = initcode_value >> 8;
    uint32_t lsb = initcode_value & 0xff;
    REGI2C_WRITE_MASK(I2C_SAR_ADC, I2C_SAR_ADC_SAR1_INIT_CODE_MSB, msb);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, I2C_SAR_ADC_SAR1_INIT_CODE_LSB, lsb);
}

//total 4 tables
static void hpadc_sar1_pattern_table_cfg(unsigned int table_idx, pattern_table table)
{
    uint32_t wdata = 0;
    wdata = (table.item[0].channel << 20 | table.item[0].atten << 18 |
            table.item[1].channel << 14|table.item[1].atten << 12 |
            table.item[2].channel << 8 |table.item[2].atten << 6 |
            table.item[3].channel << 2 |table.item[3].atten);
    WRITE_PERI_REG(ADC_SAR1_PATT_TAB1_REG + table_idx * 4, wdata);
}

void bootloader_random_enable(void)
{
    pattern_table sar1_table[4] = {};
    uint32_t pattern_len = 0;

    SET_PERI_REG_MASK(HP_SYS_CLKRST_SOC_CLK_CTRL2_REG, HP_SYS_CLKRST_REG_ADC_APB_CLK_EN);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_PERI_CLK_CTRL23_REG, HP_SYS_CLKRST_REG_ADC_CLK_EN);

    SET_PERI_REG_MASK(RTCADC_MEAS1_MUX_REG, RTCADC_SAR1_DIG_FORCE);
    SET_PERI_REG_MASK(PMU_RF_PWC_REG,PMU_XPD_PERIF_I2C);

    uint32_t sar1_clk_div_num = GET_PERI_REG_BITS2((HP_SYS_CLKRST_PERI_CLK_CTRL24_REG),
            (HP_SYS_CLKRST_REG_ADC_SAR1_CLK_DIV_NUM_M),
            (HP_SYS_CLKRST_REG_ADC_SAR1_CLK_DIV_NUM_S));

    SET_PERI_REG_MASK(ADC_CTRL_REG_REG, ADC_START_FORCE); //start force 1

    adc1_fix_initcode_set(I2C_SAR_ADC_INIT_CODE_VAL);

    //  cfg pattern table
    sar1_table[0].item[0].channel = 10; //rand() % 6;
    sar1_table[0].item[0].atten = 3;
    sar1_table[0].item[1].channel = 10;
    sar1_table[0].item[1].atten = 3;
    sar1_table[0].item[2].channel = 10;
    sar1_table[0].item[2].atten = 3;
    sar1_table[0].item[3].channel = 10;
    sar1_table[0].item[3].atten = 3;

    hpadc_sar1_pattern_table_cfg(0, sar1_table[0]);
    SET_PERI_REG_BITS(ADC_CTRL_REG_REG, ADC_SAR1_PATT_LEN, pattern_len, ADC_SAR1_PATT_LEN_S);

    SET_PERI_REG_BITS(ADC_CTRL_REG_REG, ADC_XPD_SAR1_FORCE, 3, ADC_XPD_SAR1_FORCE_S);
    SET_PERI_REG_BITS(ADC_CTRL_REG_REG, ADC_XPD_SAR2_FORCE, 3, ADC_XPD_SAR2_FORCE_S);

    REGI2C_WRITE_MASK(I2C_SAR_ADC, I2C_SAR_ADC_ENT_VDD_GRP1, 1);
    REGI2C_WRITE_MASK(I2C_SAR_ADC, I2C_SAR_ADC_DTEST_VDD_GRP1, 0);

    CLEAR_PERI_REG_MASK(ADC_CTRL_REG_REG, ADC_START_FORCE);
    SET_PERI_REG_MASK(ADC_CTRL2_REG, ADC_TIMER_EN);
    SET_PERI_REG_BITS(ADC_CTRL2_REG, ADC_TIMER_TARGET, sar1_clk_div_num * 25, ADC_TIMER_TARGET_S);

    while (GET_PERI_REG_MASK(ADC_INT_RAW_REG, ADC_SAR1_DONE_INT_RAW) == 0) { }

    SET_PERI_REG_MASK(ADC_INT_CLR_REG, ADC_APB_SARADC1_DONE_INT_CLR);
}

void bootloader_random_disable(void)
{
    // No-op for now TODO IDF-6497
    // ADC should be set to defaults here, once ADC API is implemented
    // OR just keep this empty and let application continue to use RNG initialized by the bootloader
}
