/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "hal/assert.h"
#include "soc/periph_defs.h"
#include "soc/pcr_reg.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t periph_ll_get_clk_en_mask(periph_module_t periph)
{
    switch (periph) {
        case PERIPH_SARADC_MODULE:
            return PCR_SARADC_CLK_EN;
        case PERIPH_RMT_MODULE:
            return PCR_RMT_CLK_EN;
        case PERIPH_PCNT_MODULE:
            return PCR_PCNT_CLK_EN;
        case PERIPH_LEDC_MODULE:
            return PCR_LEDC_CLK_EN;
        case PERIPH_UART0_MODULE:
            return PCR_UART0_CLK_EN;
        case PERIPH_UART1_MODULE:
            return PCR_UART1_CLK_EN;
        case PERIPH_I2C0_MODULE:
            return PCR_I2C_CLK_EN;
        case PERIPH_I2S1_MODULE:
            return PCR_I2S_CLK_EN;
        case PERIPH_TIMG0_MODULE:
            return PCR_TG0_CLK_EN;
        case PERIPH_TIMG1_MODULE:
            return PCR_TG1_CLK_EN;
        case PERIPH_UHCI0_MODULE:
            return PCR_UHCI_CLK_EN;
        case PERIPH_SYSTIMER_MODULE:
            return PCR_SYSTIMER_CLK_EN;
        case PERIPH_SPI_MODULE:
            return PCR_MSPI_CLK_EN;
        case PERIPH_SPI2_MODULE:
            return PCR_SPI2_CLK_EN;
        case PERIPH_TWAI0_MODULE:
            return PCR_TWAI0_CLK_EN;
        case PERIPH_TWAI1_MODULE:
            return PCR_TWAI1_CLK_EN;
        case PERIPH_GDMA_MODULE:
            return PCR_GDMA_CLK_EN;
        case PERIPH_MCPWM0_MODULE:
            return PCR_PWM_CLK_EN;
        case PERIPH_ETM_MODULE:
            return PCR_ETM_CLK_EN;
        case PERIPH_PARLIO_MODULE:
            return PCR_PARL_CLK_EN;
        case PERIPH_AES_MODULE:
            return PCR_AES_CLK_EN;
        case PERIPH_SHA_MODULE:
            return PCR_SHA_CLK_EN;
        case PERIPH_ECC_MODULE:
            return PCR_ECC_CLK_EN;
        case PERIPH_RSA_MODULE:
            return PCR_RSA_CLK_EN;
        case PERIPH_HMAC_MODULE:
            return PCR_HMAC_CLK_EN;
        case PERIPH_DS_MODULE:
            return PCR_DS_CLK_EN;
        case PERIPH_TEMPSENSOR_MODULE:
            return PCR_TSENS_CLK_EN;
        case PERIPH_SDIO_SLAVE_MODULE:
            return PCR_SDIO_SLAVE_CLK_EN;
        case PERIPH_REGDMA_MODULE:
            return PCR_REGDMA_CLK_EN;
        case PERIPH_ASSIST_DEBUG_MODULE:
            return PCR_ASSIST_CLK_EN;
        default:
            return 0;
    }
}

static inline uint32_t periph_ll_get_rst_en_mask(periph_module_t periph, bool enable)
{
    (void)enable; // unused

    switch (periph) {
        case PERIPH_SARADC_MODULE:
            return PCR_SARADC_REG_RST_EN;
        case PERIPH_RMT_MODULE:
            return PCR_RMT_RST_EN;
        case PERIPH_PCNT_MODULE:
            return PCR_PCNT_RST_EN;
        case PERIPH_LEDC_MODULE:
            return PCR_LEDC_RST_EN;
        case PERIPH_UART0_MODULE:
            return PCR_UART0_RST_EN;
        case PERIPH_UART1_MODULE:
            return PCR_UART1_RST_EN;
        case PERIPH_I2C0_MODULE:
            return PCR_I2C_RST_EN;
        case PERIPH_I2S1_MODULE:
            return PCR_I2S_RST_EN;
        case PERIPH_TIMG0_MODULE:
            return PCR_TG0_RST_EN;
        case PERIPH_TIMG1_MODULE:
            return PCR_TG1_RST_EN;
        case PERIPH_UHCI0_MODULE:
            return PCR_UHCI_RST_EN;
        case PERIPH_SYSTIMER_MODULE:
            return PCR_SYSTIMER_RST_EN;
        case PERIPH_SPI_MODULE:
            return PCR_MSPI_RST_EN;
        case PERIPH_SPI2_MODULE:
            return PCR_SPI2_RST_EN;
        case PERIPH_TWAI0_MODULE:
            return PCR_TWAI0_RST_EN;
        case PERIPH_TWAI1_MODULE:
            return PCR_TWAI1_RST_EN;
        case PERIPH_GDMA_MODULE:
            return PCR_GDMA_RST_EN;
        case PERIPH_MCPWM0_MODULE:
            return PCR_PWM_RST_EN;
        case PERIPH_ETM_MODULE:
            return PCR_ETM_RST_EN;
        case PERIPH_PARLIO_MODULE:
            return PCR_PARL_RST_EN;
        case PERIPH_ECC_MODULE:
            return PCR_ECC_RST_EN;
        case PERIPH_TEMPSENSOR_MODULE:
            return PCR_TSENS_RST_EN;
        case PERIPH_AES_MODULE:
            if (enable == true) {
                // Clear reset on digital signature, otherwise AES unit is held in reset
                CLEAR_PERI_REG_MASK(PCR_DS_CONF_REG, PCR_DS_RST_EN);
            }
            return PCR_AES_RST_EN;
        case PERIPH_SHA_MODULE:
            if (enable == true) {
                // Clear reset on digital signature and HMAC, otherwise SHA is held in reset
                CLEAR_PERI_REG_MASK(PCR_DS_CONF_REG, PCR_DS_RST_EN);
                CLEAR_PERI_REG_MASK(PCR_HMAC_CONF_REG, PCR_HMAC_RST_EN);
            }
            return PCR_SHA_RST_EN;
        case PERIPH_RSA_MODULE:
            if (enable == true) {
                // Clear reset on digital signature, otherwise RSA is held in reset
                CLEAR_PERI_REG_MASK(PCR_DS_CONF_REG, PCR_DS_RST_EN);
            }
            return PCR_RSA_RST_EN;
        case PERIPH_HMAC_MODULE:
            return PCR_HMAC_RST_EN;
        case PERIPH_DS_MODULE:
            return PCR_DS_RST_EN;
        case PERIPH_SDIO_SLAVE_MODULE:
            return PCR_SDIO_SLAVE_RST_EN;
        case PERIPH_REGDMA_MODULE:
            return PCR_REGDMA_RST_EN;
        case PERIPH_ASSIST_DEBUG_MODULE:
            return PCR_ASSIST_RST_EN;
        default:
            return 0;
    }
}

static inline uint32_t periph_ll_get_clk_en_reg(periph_module_t periph)
{
    switch (periph) {
        case PERIPH_SARADC_MODULE:
            return PCR_SARADC_CONF_REG;
        case PERIPH_RMT_MODULE:
            return PCR_RMT_CONF_REG;
        case PERIPH_PCNT_MODULE:
            return PCR_PCNT_CONF_REG;
        case PERIPH_LEDC_MODULE:
            return PCR_LEDC_CONF_REG;
        case PERIPH_UART0_MODULE:
            return PCR_UART0_CONF_REG;
        case PERIPH_UART1_MODULE:
            return PCR_UART1_CONF_REG;
        case PERIPH_I2C0_MODULE:
            return PCR_I2C_CONF_REG;
        case PERIPH_I2S1_MODULE:
            return PCR_I2S_CONF_REG;
        case PERIPH_TIMG0_MODULE:
            return PCR_TIMERGROUP0_CONF_REG;
        case PERIPH_TIMG1_MODULE:
            return PCR_TIMERGROUP1_CONF_REG;
        case PERIPH_UHCI0_MODULE:
            return PCR_UHCI_CONF_REG;
        case PERIPH_SYSTIMER_MODULE:
            return PCR_SYSTIMER_CONF_REG;
        case PERIPH_SPI_MODULE:
            return PCR_MSPI_CONF_REG;
        case PERIPH_SPI2_MODULE:
            return PCR_SPI2_CONF_REG;
        case PERIPH_TWAI0_MODULE:
            return PCR_TWAI0_CONF_REG;
        case PERIPH_TWAI1_MODULE:
            return PCR_TWAI1_CONF_REG;
        case PERIPH_GDMA_MODULE:
            return PCR_GDMA_CONF_REG;
        case PERIPH_MCPWM0_MODULE:
            return PCR_PWM_CONF_REG;
        case PERIPH_ETM_MODULE:
            return PCR_ETM_CONF_REG;
        case PERIPH_PARLIO_MODULE:
            return PCR_PARL_IO_CONF_REG;
        case PERIPH_AES_MODULE:
            return PCR_AES_CONF_REG;
        case PERIPH_SHA_MODULE:
            return PCR_SHA_CONF_REG;
        case PERIPH_ECC_MODULE:
            return PCR_ECC_CONF_REG;
        case PERIPH_RSA_MODULE:
            return PCR_RSA_CONF_REG;
        case PERIPH_HMAC_MODULE:
            return PCR_HMAC_CONF_REG;
        case PERIPH_DS_MODULE:
            return PCR_DS_CONF_REG;
        case PERIPH_TEMPSENSOR_MODULE:
            return PCR_TSENS_CLK_CONF_REG;
        case PERIPH_SDIO_SLAVE_MODULE:
            return PCR_SDIO_SLAVE_CONF_REG;
        case PERIPH_REGDMA_MODULE:
            return PCR_REGDMA_CONF_REG;
        case PERIPH_ASSIST_DEBUG_MODULE:
            return PCR_ASSIST_CONF_REG;
    default:
        return 0;
    }
}

static inline uint32_t periph_ll_get_rst_en_reg(periph_module_t periph)
{
    switch (periph) {
        case PERIPH_SARADC_MODULE:
            return PCR_SARADC_CONF_REG;
        case PERIPH_RMT_MODULE:
            return PCR_RMT_CONF_REG;
        case PERIPH_PCNT_MODULE:
            return PCR_PCNT_CONF_REG;
        case PERIPH_LEDC_MODULE:
            return PCR_LEDC_CONF_REG;
        case PERIPH_UART0_MODULE:
            return PCR_UART0_CONF_REG;
        case PERIPH_UART1_MODULE:
            return PCR_UART1_CONF_REG;
        case PERIPH_I2C0_MODULE:
            return PCR_I2C_CONF_REG;
        case PERIPH_I2S1_MODULE:
            return PCR_I2S_CONF_REG;
        case PERIPH_TIMG0_MODULE:
            return PCR_TIMERGROUP0_CONF_REG;
        case PERIPH_TIMG1_MODULE:
            return PCR_TIMERGROUP1_CONF_REG;
        case PERIPH_UHCI0_MODULE:
            return PCR_UHCI_CONF_REG;
        case PERIPH_SYSTIMER_MODULE:
            return PCR_SYSTIMER_CONF_REG;
        case PERIPH_SPI_MODULE:
            return PCR_MSPI_CONF_REG;
        case PERIPH_SPI2_MODULE:
            return PCR_SPI2_CONF_REG;
        case PERIPH_TWAI0_MODULE:
            return PCR_TWAI0_CONF_REG;
        case PERIPH_TWAI1_MODULE:
            return PCR_TWAI1_CONF_REG;
        case PERIPH_GDMA_MODULE:
            return PCR_GDMA_CONF_REG;
        case PERIPH_MCPWM0_MODULE:
            return PCR_PWM_CONF_REG;
        case PERIPH_ETM_MODULE:
            return PCR_ETM_CONF_REG;
        case PERIPH_PARLIO_MODULE:
            return PCR_PARL_IO_CONF_REG;
        case PERIPH_AES_MODULE:
            return PCR_AES_CONF_REG;
        case PERIPH_SHA_MODULE:
            return PCR_SHA_CONF_REG;
        case PERIPH_ECC_MODULE:
            return PCR_ECC_CONF_REG;
        case PERIPH_RSA_MODULE:
            return PCR_RSA_CONF_REG;
        case PERIPH_HMAC_MODULE:
            return PCR_HMAC_CONF_REG;
        case PERIPH_DS_MODULE:
            return PCR_DS_CONF_REG;
        case PERIPH_TEMPSENSOR_MODULE:
            return PCR_TSENS_CLK_CONF_REG;
        case PERIPH_SDIO_SLAVE_MODULE:
            return PCR_SDIO_SLAVE_CONF_REG;
        case PERIPH_REGDMA_MODULE:
            return PCR_REGDMA_CONF_REG;
        case PERIPH_ASSIST_DEBUG_MODULE:
            return PCR_ASSIST_CONF_REG;
    default:
        return 0;
    }
}

static inline void periph_ll_enable_clk_clear_rst(periph_module_t periph)
{
    SET_PERI_REG_MASK(periph_ll_get_clk_en_reg(periph), periph_ll_get_clk_en_mask(periph));
    CLEAR_PERI_REG_MASK(periph_ll_get_rst_en_reg(periph), periph_ll_get_rst_en_mask(periph, true));
}

static inline void periph_ll_disable_clk_set_rst(periph_module_t periph)
{
    CLEAR_PERI_REG_MASK(periph_ll_get_clk_en_reg(periph), periph_ll_get_clk_en_mask(periph));
    SET_PERI_REG_MASK(periph_ll_get_rst_en_reg(periph), periph_ll_get_rst_en_mask(periph, false));
}

static inline void periph_ll_reset(periph_module_t periph)
{
    SET_PERI_REG_MASK(periph_ll_get_rst_en_reg(periph), periph_ll_get_rst_en_mask(periph, false));
    CLEAR_PERI_REG_MASK(periph_ll_get_rst_en_reg(periph), periph_ll_get_rst_en_mask(periph, false));
}

static inline bool IRAM_ATTR periph_ll_periph_enabled(periph_module_t periph)
{
    return REG_GET_BIT(periph_ll_get_rst_en_reg(periph), periph_ll_get_rst_en_mask(periph, false)) == 0 &&
           REG_GET_BIT(periph_ll_get_clk_en_reg(periph), periph_ll_get_clk_en_mask(periph)) != 0;
}

#ifdef __cplusplus
}
#endif
