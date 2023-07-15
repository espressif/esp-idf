/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/periph_defs.h"
#include "soc/soc.h"
#include "soc/hp_sys_clkrst_reg.h"
#include "soc/lp_clkrst_reg.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t periph_ll_get_clk_en_mask(periph_module_t periph)
{
    switch (periph) {
    case PERIPH_MSPI_FLASH_MODULE:
        return HP_SYS_CLKRST_REG_FLASH_CORE_CLK_EN;
    case PERIPH_MSPI_PSRAM_MODULE:
        return HP_SYS_CLKRST_REG_PSRAM_CORE_CLK_EN;
    case PERIPH_EMAC_MODULE:
        return LP_CLKRST_HP_PAD_EMAC_TXRX_CLK_EN | LP_CLKRST_HP_PAD_EMAC_RX_CLK_EN | LP_CLKRST_HP_PAD_EMAC_TX_CLK_EN;
    case PERIPH_MIPI_DSI_MODULE:
        return HP_SYS_CLKRST_REG_MIPI_DSI_DPICLK_EN;
    // IDF-6500
    case PERIPH_MIPI_CSI_MODULE:
        return 0;
    case PERIPH_I2C0_MODULE:
        return HP_SYS_CLKRST_REG_I2C0_CLK_EN;
    case PERIPH_I2C1_MODULE:
        return HP_SYS_CLKRST_REG_I2C1_CLK_EN;
    case PERIPH_I2S0_MODULE:
        return HP_SYS_CLKRST_REG_I2S0_TX_CLK_EN | HP_SYS_CLKRST_REG_I2S0_RX_CLK_EN;
    case PERIPH_I2S1_MODULE:
        return HP_SYS_CLKRST_REG_I2S1_RX_CLK_EN | HP_SYS_CLKRST_REG_I2S1_TX_CLK_EN;
    case PERIPH_I2S2_MODULE:
        return HP_SYS_CLKRST_REG_I2S2_RX_CLK_EN | HP_SYS_CLKRST_REG_I2S2_TX_CLK_EN;
    case PERIPH_LCD_MODULE:
        return HP_SYS_CLKRST_REG_LCD_CLK_EN;
    case PERIPH_UART0_MODULE:
        return HP_SYS_CLKRST_REG_UART0_CLK_EN;
    case PERIPH_UART1_MODULE:
        return HP_SYS_CLKRST_REG_UART1_CLK_EN;
    case PERIPH_UART2_MODULE:
        return HP_SYS_CLKRST_REG_UART2_CLK_EN;
    case PERIPH_UART3_MODULE:
        return HP_SYS_CLKRST_REG_UART3_CLK_EN;
    case PERIPH_UART4_MODULE:
        return HP_SYS_CLKRST_REG_UART4_CLK_EN;
    case PERIPH_TWAI0_MODULE:
        return HP_SYS_CLKRST_REG_TWAI0_CLK_EN;
    case PERIPH_TWAI1_MODULE:
        return HP_SYS_CLKRST_REG_TWAI1_CLK_EN;
    case PERIPH_TWAI2_MODULE:
        return HP_SYS_CLKRST_REG_TWAI2_CLK_EN;
    case PERIPH_GPSPI_MODULE:
        return HP_SYS_CLKRST_REG_GPSPI2_HS_CLK_EN;
    case PERIPH_GPSPI2_MODULE:
        return HP_SYS_CLKRST_REG_GPSPI2_MST_CLK_EN;
    case PERIPH_GPSPI3_MODULE:
        return HP_SYS_CLKRST_REG_GPSPI3_MST_CLK_EN;
    case PERIPH_PARLIO_MODULE:
        return HP_SYS_CLKRST_REG_PARLIO_RX_CLK_EN | HP_SYS_CLKRST_REG_PARLIO_TX_CLK_EN;
    case PERIPH_I3C_MODULE:
        return HP_SYS_CLKRST_REG_I3C_MST_CLK_EN;
    case PERIPH_CAM_MODULE:
        return HP_SYS_CLKRST_REG_CAM_CLK_EN;
    case PERIPH_MCPWM0_MODULE:
        return HP_SYS_CLKRST_REG_MCPWM0_CLK_EN;
    case PERIPH_MCPWM1_MODULE:
        return HP_SYS_CLKRST_REG_MCPWM1_CLK_EN;
    case PERIPH_TIMG0_MODULE:
        return HP_SYS_CLKRST_REG_TIMERGRP0_T0_CLK_EN | HP_SYS_CLKRST_REG_TIMERGRP0_T1_CLK_EN | HP_SYS_CLKRST_REG_TIMERGRP0_WDT_CLK_EN;
    case PERIPH_TIMG1_MODULE:
        return HP_SYS_CLKRST_REG_TIMERGRP1_T0_CLK_EN | HP_SYS_CLKRST_REG_TIMERGRP1_T1_CLK_EN | HP_SYS_CLKRST_REG_TIMERGRP1_WDT_CLK_EN;
    case PERIPH_SYSTIMER_MODULE:
        return HP_SYS_CLKRST_REG_SYSTIMER_CLK_EN;
    case PERIPH_LEDC_MODULE:
        return HP_SYS_CLKRST_REG_LEDC_CLK_EN;
    case PERIPH_RMT_MODULE:
        return HP_SYS_CLKRST_REG_RMT_CLK_EN;
    case PERIPH_SARADC_MODULE:
        return HP_SYS_CLKRST_REG_ADC_CLK_EN;
    case PERIPH_PVT_MODULE:
        return HP_SYS_CLKRST_REG_PVT_CLK_EN;
    case PERIPH_AES_MODULE:
        return HP_SYS_CLKRST_REG_CRYPTO_AES_CLK_EN;
    case PERIPH_DS_MODULE:
        return HP_SYS_CLKRST_REG_CRYPTO_DS_CLK_EN;
    case PERIPH_ECC_MODULE:
        return HP_SYS_CLKRST_REG_CRYPTO_ECC_CLK_EN;
    case PERIPH_HMAC_MODULE:
        return HP_SYS_CLKRST_REG_CRYPTO_HMAC_CLK_EN;
    case PERIPH_RSA_MODULE:
        return HP_SYS_CLKRST_REG_CRYPTO_RSA_CLK_EN;
    case PERIPH_SEC_MODULE:
        return HP_SYS_CLKRST_REG_CRYPTO_SEC_CLK_EN;
    case PERIPH_SHA_MODULE:
        return HP_SYS_CLKRST_REG_CRYPTO_SHA_CLK_EN;
    case PERIPH_ECDSA_MODULE:
        return HP_SYS_CLKRST_REG_CRYPTO_ECDSA_CLK_EN;
    case PERIPH_ISP_MODULE:
        return HP_SYS_CLKRST_REG_ISP_CLK_EN;
    default:
	return 0;
    }
}

static inline uint32_t periph_ll_get_rst_en_mask(periph_module_t periph, bool enable)
{
    switch (periph) {
    case PERIPH_PVT_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_PVT_TOP;
    case PERIPH_GDMA_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_GDMA;
    case PERIPH_MSPI_FLASH_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_MSPI_AXI;
    case PERIPH_MSPI_PSRAM_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_DUAL_MSPI_AXI;
    case PERIPH_MIPI_DSI_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_DSI_BRG;
    case PERIPH_MIPI_CSI_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_CSI_BRG;
    case PERIPH_ISP_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_ISP;
    case PERIPH_JPEG_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_JPEG;
    case PERIPH_DMA2D_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_DMA2D;
    case PERIPH_PPA_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_PPA;
    case PERIPH_AHB_PDMA_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_AHB_PDMA;
    case PERIPH_AXI_PDMA_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_AXI_PDMA;
    case PERIPH_SYSTIMER_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_STIMER;
    case PERIPH_TIMG0_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_TIMERGRP0;
    case PERIPH_TIMG1_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_TIMERGRP1;
    case PERIPH_UART0_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_UART0_CORE;
    case PERIPH_UART1_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_UART1_CORE;
    case PERIPH_UART2_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_UART2_CORE;
    case PERIPH_UART3_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_UART3_CORE;
    case PERIPH_UART4_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_UART4_CORE;
    case PERIPH_UHCI_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_UHCI;
    case PERIPH_I3C_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_I3CMST | HP_SYS_CLKRST_REG_RST_EN_I3CSLV;
    case PERIPH_I2C0_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_I2C0;
    case PERIPH_I2C1_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_I2C1;
    case PERIPH_RMT_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_RMT;
    case PERIPH_MCPWM0_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_PWM0;
    case PERIPH_MCPWM1_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_PWM1;
    case PERIPH_TWAI0_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_CAN0;
    case PERIPH_TWAI1_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_CAN1;
    case PERIPH_TWAI2_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_CAN2;
    case PERIPH_LEDC_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_LEDC;
    case PERIPH_PCNT_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_PCNT;
    case PERIPH_PARLIO_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_PARLIO | HP_SYS_CLKRST_REG_RST_EN_PARLIO_RX | HP_SYS_CLKRST_REG_RST_EN_PARLIO_TX;
    case PERIPH_I2S0_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_I2S0_APB;
    case PERIPH_I2S1_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_I2S1_APB;
    case PERIPH_I2S2_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_I2S2_APB;
    case PERIPH_GPSPI2_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_SPI2;
    case PERIPH_GPSPI3_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_SPI3;
    case PERIPH_LCD_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_LCDCAM;
    case PERIPH_SARADC_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_ADC;
    case PERIPH_AES_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_CRYPTO | HP_SYS_CLKRST_REG_RST_EN_AES;
    case PERIPH_DS_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_CRYPTO | HP_SYS_CLKRST_REG_RST_EN_DS;
    case PERIPH_ECC_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_CRYPTO | HP_SYS_CLKRST_REG_RST_EN_ECC;
    case PERIPH_HMAC_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_CRYPTO | HP_SYS_CLKRST_REG_RST_EN_HMAC;
    case PERIPH_RSA_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_CRYPTO | HP_SYS_CLKRST_REG_RST_EN_RSA;
    case PERIPH_SEC_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_CRYPTO | HP_SYS_CLKRST_REG_RST_EN_SEC;
    case PERIPH_SHA_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_CRYPTO | HP_SYS_CLKRST_REG_RST_EN_SHA;
    case PERIPH_ECDSA_MODULE:
        return HP_SYS_CLKRST_REG_RST_EN_CRYPTO | HP_SYS_CLKRST_REG_RST_EN_ECDSA;
    case PERIPH_SDMMC_MODULE:
        return LP_CLKRST_RST_EN_SDMMC;
    case PERIPH_EMAC_MODULE:
        return LP_CLKRST_RST_EN_EMAC;
    default:
	return 0;
    }
}

static uint32_t periph_ll_get_clk_en_reg(periph_module_t periph)
{
    switch (periph) {
    case PERIPH_MSPI_FLASH_MODULE:
    case PERIPH_MSPI_PSRAM_MODULE:
        return HP_SYS_CLKRST_PERI_CLK_CTRL00_REG;
    case PERIPH_MIPI_DSI_MODULE:
        return HP_SYS_CLKRST_PERI_CLK_CTRL03_REG;
    case PERIPH_I2C0_MODULE:
    case PERIPH_I2C1_MODULE:
        return HP_SYS_CLKRST_PERI_CLK_CTRL10_REG;
    case PERIPH_LCD_MODULE:
        return HP_SYS_CLKRST_PERI_CLK_CTRL110_REG;
    case PERIPH_UART0_MODULE:
        return HP_SYS_CLKRST_PERI_CLK_CTRL111_REG;
    case PERIPH_UART1_MODULE:
        return HP_SYS_CLKRST_PERI_CLK_CTRL112_REG;
    case PERIPH_UART2_MODULE:
        return HP_SYS_CLKRST_PERI_CLK_CTRL113_REG;
    case PERIPH_UART3_MODULE:
        return HP_SYS_CLKRST_PERI_CLK_CTRL114_REG;
    case PERIPH_UART4_MODULE:
        return HP_SYS_CLKRST_PERI_CLK_CTRL115_REG;
    case PERIPH_TWAI0_MODULE:
    case PERIPH_TWAI1_MODULE:
    case PERIPH_TWAI2_MODULE:
        return HP_SYS_CLKRST_PERI_CLK_CTRL116_REG;
    case PERIPH_GPSPI_MODULE:
    case PERIPH_GPSPI2_MODULE:
    case PERIPH_GPSPI3_MODULE:
        return HP_SYS_CLKRST_PERI_CLK_CTRL117_REG;
    case PERIPH_I3C_MODULE:
    case PERIPH_CAM_MODULE:
        return HP_SYS_CLKRST_PERI_CLK_CTRL119_REG;
    case PERIPH_MCPWM0_MODULE:
    case PERIPH_MCPWM1_MODULE:
    case PERIPH_TIMG0_MODULE:
        return HP_SYS_CLKRST_PERI_CLK_CTRL20_REG;
    case PERIPH_TIMG1_MODULE:
    case PERIPH_SYSTIMER_MODULE:
    case PERIPH_LEDC_MODULE:
    case PERIPH_RMT_MODULE:
        return HP_SYS_CLKRST_PERI_CLK_CTRL21_REG;
    case PERIPH_SARADC_MODULE:
        return HP_SYS_CLKRST_PERI_CLK_CTRL22_REG;
    case PERIPH_PVT_MODULE:
    case PERIPH_AES_MODULE:
    case PERIPH_DS_MODULE:
    case PERIPH_ECC_MODULE:
    case PERIPH_HMAC_MODULE:
    case PERIPH_RSA_MODULE:
    case PERIPH_SEC_MODULE:
    case PERIPH_SHA_MODULE:
    case PERIPH_ECDSA_MODULE:
        return HP_SYS_CLKRST_PERI_CLK_CTRL24_REG;
    case PERIPH_ISP_MODULE:
        return HP_SYS_CLKRST_PERI_CLK_CTRL25_REG;
    case PERIPH_EMAC_MODULE:
        return LP_CLKRST_HP_CLK_CTRL_REG;
    default:
        abort();
	return 0;
    }
}

static uint32_t periph_ll_get_rst_en_reg(periph_module_t periph)
{
    switch (periph) {
    case PERIPH_PVT_MODULE:
    case PERIPH_GDMA_MODULE:
    case PERIPH_MSPI_FLASH_MODULE:
    case PERIPH_MSPI_PSRAM_MODULE:
    case PERIPH_ISP_MODULE:
    case PERIPH_JPEG_MODULE:
    case PERIPH_DMA2D_MODULE:
    case PERIPH_PPA_MODULE:
    case PERIPH_AHB_PDMA_MODULE:
    case PERIPH_AXI_PDMA_MODULE:
        return HP_SYS_CLKRST_HP_RST_EN0_REG;
    case PERIPH_SYSTIMER_MODULE:
    case PERIPH_TIMG0_MODULE:
    case PERIPH_TIMG1_MODULE:
    case PERIPH_UART0_MODULE:
    case PERIPH_UART1_MODULE:
    case PERIPH_UART2_MODULE:
    case PERIPH_UART3_MODULE:
    case PERIPH_UART4_MODULE:
    case PERIPH_UHCI_MODULE:
    case PERIPH_I3C_MODULE:
    case PERIPH_I2C0_MODULE:
    case PERIPH_I2C1_MODULE:
    case PERIPH_RMT_MODULE:
    case PERIPH_MCPWM0_MODULE:
    case PERIPH_MCPWM1_MODULE:
    case PERIPH_TWAI0_MODULE:
    case PERIPH_TWAI1_MODULE:
    case PERIPH_TWAI2_MODULE:
    case PERIPH_LEDC_MODULE:
    case PERIPH_PCNT_MODULE:
    case PERIPH_PARLIO_MODULE:
    case PERIPH_I2S0_MODULE:
        return HP_SYS_CLKRST_HP_RST_EN1_REG;
    case PERIPH_I2S1_MODULE:
    case PERIPH_I2S2_MODULE:
    case PERIPH_GPSPI2_MODULE:
    case PERIPH_GPSPI3_MODULE:
    case PERIPH_CAM_MODULE:
    case PERIPH_SARADC_MODULE:
    case PERIPH_AES_MODULE:
    case PERIPH_DS_MODULE:
    case PERIPH_ECC_MODULE:
    case PERIPH_HMAC_MODULE:
    case PERIPH_RSA_MODULE:
    case PERIPH_SEC_MODULE:
    case PERIPH_SHA_MODULE:
    case PERIPH_ECDSA_MODULE:
        return HP_SYS_CLKRST_HP_RST_EN2_REG;
    case PERIPH_SDMMC_MODULE:
    case PERIPH_EMAC_MODULE:
        return LP_CLKRST_HP_SDMMC_EMAC_RST_CTRL_REG;
    default:
        abort();
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
