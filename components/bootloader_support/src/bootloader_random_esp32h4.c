/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD

 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "bootloader_random.h"
#include "soc/soc.h"
#include "soc/rng_reg.h"
#include "esp_rom_sys.h"
#include "soc/pcr_reg.h"
#include "soc/pmu_reg.h"
#include "soc/apb_saradc_reg.h"
#include "esp_private/regi2c_ctrl.h"
#include "soc/regi2c_saradc.h"

static const uint32_t SAR2_CHANNEL = 9;
static const uint32_t SAR1_CHANNEL = 7;
static const uint32_t PATTERN_BIT_WIDTH = 6;
static const uint32_t SAR1_ATTEN = 3;
static const uint32_t SAR2_ATTEN = 3;

void sar_adc_enable(void)
{
    REG_SET_BIT(PCR_SARADC_CONF_REG, PCR_SARADC_RST_EN);
    REG_CLR_BIT(PCR_SARADC_CONF_REG, PCR_SARADC_RST_EN);

    REG_SET_BIT(PCR_SARADC_CONF_REG, PCR_SARADC_REG_CLK_EN);

    REG_SET_BIT(PCR_SARADC_CONF_REG, PCR_SARADC_REG_RST_EN);
    REG_CLR_BIT(PCR_SARADC_CONF_REG, PCR_SARADC_REG_RST_EN);

    REG_CLR_BIT(PCR_SARADC_CLKM_CONF_REG, PCR_SARADC_CLKM_EN);

    REG_SET_FIELD(PCR_SARADC_CLKM_CONF_REG, PCR_SARADC_CLKM_SEL, 0);

    REG_SET_FIELD(PCR_SARADC_CLKM_CONF_REG, PCR_SARADC_CLKM_DIV_NUM, 0);

    SET_PERI_REG_MASK(PMU_RF_PWC_REG, PMU_XPD_PERIF_I2C);

    REGI2C_WRITE_MASK(I2C_SARADC, I2C_SARADC_DTEST, 0);
    REGI2C_WRITE_MASK(I2C_SARADC, I2C_SARADC_ENT_SAR, 1);
    REGI2C_WRITE_MASK(I2C_SARADC, I2C_SARADC_SAR1_EN_TOUT, 1);
    REGI2C_WRITE_MASK(I2C_SARADC, I2C_SARADC_SAR2_EN_TOUT, 1);

    REGI2C_WRITE_MASK(I2C_SARADC, I2C_SARADC_SAR2_INIT_CODE_MSB, 0x08);
    REGI2C_WRITE_MASK(I2C_SARADC, I2C_SARADC_SAR2_INIT_CODE_LSB, 0x66);
    REGI2C_WRITE_MASK(I2C_SARADC, I2C_SARADC_SAR1_INIT_CODE_MSB, 0x08);
    REGI2C_WRITE_MASK(I2C_SARADC, I2C_SARADC_SAR1_INIT_CODE_LSB, 0x66);

    uint32_t pattern_one = (SAR2_CHANNEL << 2) | SAR2_ATTEN;
    uint32_t pattern_two = (SAR1_CHANNEL << 2) | SAR1_ATTEN;
    uint32_t pattern_table = 0 | (pattern_two << 3 * PATTERN_BIT_WIDTH) |pattern_one << 2 * PATTERN_BIT_WIDTH;
    REG_WRITE(APB_SARADC_SAR_PATT_TAB1_REG, pattern_table);

    REG_SET_FIELD(APB_SARADC_CTRL_REG, APB_SARADC_SARADC_SAR_PATT_LEN, 1);
    REG_SET_FIELD(APB_SARADC_CTRL_REG, APB_SARADC_SARADC_SAR_CLK_DIV, 15);
    REG_SET_FIELD(APB_SARADC_CTRL2_REG, APB_SARADC_SARADC_TIMER_TARGET, 200);
    REG_SET_BIT(APB_SARADC_CTRL2_REG, APB_SARADC_SARADC_TIMER_EN);
}

void sar_adc_disable(void)
{
    REG_CLR_BIT(APB_SARADC_CTRL2_REG, APB_SARADC_SARADC_TIMER_EN);
}

void bootloader_random_enable(void)
{
    // Enable source of saradc
    sar_adc_enable();

    // Enable source of ring
    REG_SET_BIT(RNG_CFG_REG, RNG_SAMPLE_ENABLE);

    // // For dieharder test
    // esp_rom_printf("H4: Random bytes (%s) follow:\n", "test");
    // while (1)
    // {
    //     uint32_t random_sync = REG_READ(RNG_DATA_SYNC_REG);
    //     // esp_rom_printf("sync_random_data: %d\n", random_sync);

    //     uart_tx_one_char(random_sync >> 24);
    //     uart_tx_one_char(random_sync >> 16);
    //     uart_tx_one_char(random_sync >> 8);
    //     uart_tx_one_char(random_sync);
    // }
}

void bootloader_random_disable(void)
{
    // Disable source of saradc
    sar_adc_disable();

    // Disable source of ring
    REG_CLR_BIT(RNG_CFG_REG, RNG_SAMPLE_ENABLE);
}
