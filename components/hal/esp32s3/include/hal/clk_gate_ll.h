/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "esp_attr.h"
#include "soc/periph_defs.h"
#include "soc/system_reg.h"
#include "soc/syscon_reg.h"
#include "soc/dport_access.h"

static inline uint32_t periph_ll_get_clk_en_mask(periph_module_t periph)
{
    switch (periph) {
    case PERIPH_SARADC_MODULE:
        return SYSTEM_APB_SARADC_CLK_EN;
    case PERIPH_RMT_MODULE:
        return SYSTEM_RMT_CLK_EN;
    case PERIPH_LEDC_MODULE:
        return SYSTEM_LEDC_CLK_EN;
    case PERIPH_UART0_MODULE:
        return SYSTEM_UART_CLK_EN;
    case PERIPH_UART1_MODULE:
        return SYSTEM_UART1_CLK_EN;
    case PERIPH_UART2_MODULE:
        return SYSTEM_UART2_CLK_EN;
    case PERIPH_USB_MODULE:
        return SYSTEM_USB_CLK_EN;
    case PERIPH_I2C0_MODULE:
        return SYSTEM_I2C_EXT0_CLK_EN;
    case PERIPH_I2C1_MODULE:
        return SYSTEM_I2C_EXT1_CLK_EN;
    case PERIPH_I2S0_MODULE:
        return SYSTEM_I2S0_CLK_EN;
    case PERIPH_I2S1_MODULE:
        return SYSTEM_I2S1_CLK_EN;
    case PERIPH_LCD_CAM_MODULE:
        return SYSTEM_LCD_CAM_CLK_EN;
    case PERIPH_TIMG0_MODULE:
        return SYSTEM_TIMERGROUP_CLK_EN;
    case PERIPH_TIMG1_MODULE:
        return SYSTEM_TIMERGROUP1_CLK_EN;
    case PERIPH_PWM0_MODULE:
        return SYSTEM_PWM0_CLK_EN;
    case PERIPH_PWM1_MODULE:
        return SYSTEM_PWM1_CLK_EN;
    case PERIPH_UHCI0_MODULE:
        return SYSTEM_UHCI0_CLK_EN;
    case PERIPH_UHCI1_MODULE:
        return SYSTEM_UHCI1_CLK_EN;
    case PERIPH_PCNT_MODULE:
        return SYSTEM_PCNT_CLK_EN;
    case PERIPH_SPI_MODULE:
        return SYSTEM_SPI01_CLK_EN;
    case PERIPH_SPI2_MODULE:
        return SYSTEM_SPI2_CLK_EN;
    case PERIPH_SPI3_MODULE:
        return SYSTEM_SPI3_CLK_EN;
    case PERIPH_SDMMC_MODULE:
        return SYSTEM_SDIO_HOST_CLK_EN;
    case PERIPH_TWAI_MODULE:
        return SYSTEM_TWAI_CLK_EN;
    case PERIPH_RNG_MODULE:
        return SYSTEM_WIFI_CLK_RNG_EN;
    case PERIPH_WIFI_MODULE:
        return SYSTEM_WIFI_CLK_WIFI_EN_M;
    case PERIPH_BT_MODULE:
        return SYSTEM_WIFI_CLK_BT_EN_M;
    case PERIPH_WIFI_BT_COMMON_MODULE:
        return SYSTEM_WIFI_CLK_WIFI_BT_COMMON_M;
    case PERIPH_BT_BASEBAND_MODULE:
        return SYSTEM_BT_BASEBAND_EN;
    case PERIPH_BT_LC_MODULE:
        return SYSTEM_BT_LC_EN;
    case PERIPH_SYSTIMER_MODULE:
        return SYSTEM_SYSTIMER_CLK_EN;
    case PERIPH_DEDIC_GPIO_MODULE:
        return SYSTEM_CLK_EN_DEDICATED_GPIO;
    case PERIPH_GDMA_MODULE:
        return SYSTEM_DMA_CLK_EN;
    case PERIPH_AES_MODULE:
        return SYSTEM_CRYPTO_AES_CLK_EN;
    case PERIPH_SHA_MODULE:
        return SYSTEM_CRYPTO_SHA_CLK_EN;
    case PERIPH_RSA_MODULE:
        return SYSTEM_CRYPTO_RSA_CLK_EN;
    case PERIPH_HMAC_MODULE:
        return SYSTEM_CRYPTO_HMAC_CLK_EN;
    case PERIPH_DS_MODULE:
        return SYSTEM_CRYPTO_DS_CLK_EN;
    default:
        return 0;
    }
}

static inline uint32_t periph_ll_get_rst_en_mask(periph_module_t periph, bool enable)
{
    switch (periph) {
    case PERIPH_SARADC_MODULE:
        return SYSTEM_APB_SARADC_RST;
    case PERIPH_RMT_MODULE:
        return SYSTEM_RMT_RST;
    case PERIPH_LEDC_MODULE:
        return SYSTEM_LEDC_RST;
    case PERIPH_BT_MODULE:
        return  (SYSTEM_BTBB_RST | SYSTEM_BTBB_REG_RST | SYSTEM_RW_BTMAC_RST | SYSTEM_RW_BTLP_RST | SYSTEM_RW_BTMAC_REG_RST | SYSTEM_RW_BTLP_REG_RST);
    case PERIPH_UART0_MODULE:
        return SYSTEM_UART_RST;
    case PERIPH_UART1_MODULE:
        return SYSTEM_UART1_RST;
    case PERIPH_UART2_MODULE:
        return SYSTEM_UART2_RST;
    case PERIPH_USB_MODULE:
        return SYSTEM_USB_RST;
    case PERIPH_I2C0_MODULE:
        return SYSTEM_I2C_EXT0_RST;
    case PERIPH_I2C1_MODULE:
        return SYSTEM_I2C_EXT1_RST;
    case PERIPH_I2S0_MODULE:
        return SYSTEM_I2S0_RST;
    case PERIPH_I2S1_MODULE:
        return SYSTEM_I2S1_RST;
    case PERIPH_LCD_CAM_MODULE:
        return SYSTEM_LCD_CAM_RST;
    case PERIPH_TIMG0_MODULE:
        return SYSTEM_TIMERGROUP_RST;
    case PERIPH_TIMG1_MODULE:
        return SYSTEM_TIMERGROUP1_RST;
    case PERIPH_PWM0_MODULE:
        return SYSTEM_PWM0_RST;
    case PERIPH_PWM1_MODULE:
        return SYSTEM_PWM1_RST;
    case PERIPH_UHCI0_MODULE:
        return SYSTEM_UHCI0_RST;
    case PERIPH_UHCI1_MODULE:
        return SYSTEM_UHCI1_RST;
    case PERIPH_PCNT_MODULE:
        return SYSTEM_PCNT_RST;
    case PERIPH_SPI_MODULE:
        return SYSTEM_SPI01_RST;
    case PERIPH_SPI2_MODULE:
        return SYSTEM_SPI2_RST;
    case PERIPH_SPI3_MODULE:
        return SYSTEM_SPI3_RST;
    case PERIPH_SDMMC_MODULE:
        return SYSTEM_SDIO_HOST_RST;
    case PERIPH_TWAI_MODULE:
        return SYSTEM_TWAI_RST;
    case PERIPH_SYSTIMER_MODULE:
        return SYSTEM_SYSTIMER_RST;
    case PERIPH_DEDIC_GPIO_MODULE:
        return SYSTEM_RST_EN_DEDICATED_GPIO;
    case PERIPH_GDMA_MODULE:
        return SYSTEM_DMA_RST;
    case PERIPH_HMAC_MODULE:
        return SYSTEM_CRYPTO_HMAC_RST;
    case PERIPH_DS_MODULE:
        return SYSTEM_CRYPTO_DS_RST;
    case PERIPH_AES_MODULE:
        if (enable == true) {
            // Clear reset on digital signature, otherwise AES unit is held in reset also.
            return (SYSTEM_CRYPTO_AES_RST | SYSTEM_CRYPTO_DS_RST);
        } else {
            //Don't return other units to reset, as this pulls reset on RSA & SHA units, respectively.
            return SYSTEM_CRYPTO_AES_RST;
        }
    case PERIPH_SHA_MODULE:
        if (enable == true) {
            // Clear reset on digital signature and HMAC, otherwise SHA is held in reset
            return (SYSTEM_CRYPTO_SHA_RST | SYSTEM_CRYPTO_DS_RST | SYSTEM_CRYPTO_HMAC_RST) ;
        } else {
            // Don't assert reset on secure boot, otherwise AES is held in reset
            return SYSTEM_CRYPTO_SHA_RST;
        }
    case PERIPH_RSA_MODULE:
        if (enable == true) {
            /* also clear reset on digital signature, otherwise RSA is held in reset */
            return (SYSTEM_CRYPTO_RSA_RST | SYSTEM_CRYPTO_DS_RST);
        } else {
            /* don't reset digital signature unit, as this resets AES also */
            return SYSTEM_CRYPTO_RSA_RST;
        }
    default:
        return 0;
    }
}

static uint32_t periph_ll_get_clk_en_reg(periph_module_t periph)
{
    switch (periph) {
    case PERIPH_DEDIC_GPIO_MODULE:
        return SYSTEM_CPU_PERI_CLK_EN_REG;
    case PERIPH_RNG_MODULE:
    case PERIPH_WIFI_MODULE:
    case PERIPH_BT_MODULE:
    case PERIPH_WIFI_BT_COMMON_MODULE:
    case PERIPH_BT_BASEBAND_MODULE:
    case PERIPH_BT_LC_MODULE:
        return SYSTEM_WIFI_CLK_EN_REG ;
    case PERIPH_UART2_MODULE:
    case PERIPH_SDMMC_MODULE:
    case PERIPH_LCD_CAM_MODULE:
    case PERIPH_GDMA_MODULE:
    case PERIPH_HMAC_MODULE:
    case PERIPH_DS_MODULE:
    case PERIPH_AES_MODULE:
    case PERIPH_SHA_MODULE:
    case PERIPH_RSA_MODULE:
        return SYSTEM_PERIP_CLK_EN1_REG;
    default:
        return SYSTEM_PERIP_CLK_EN0_REG;
    }
}

static uint32_t periph_ll_get_rst_en_reg(periph_module_t periph)
{
    switch (periph) {
    case PERIPH_DEDIC_GPIO_MODULE:
        return SYSTEM_CPU_PERI_RST_EN_REG;
    case PERIPH_RNG_MODULE:
    case PERIPH_WIFI_MODULE:
    case PERIPH_BT_MODULE:
    case PERIPH_WIFI_BT_COMMON_MODULE:
    case PERIPH_BT_BASEBAND_MODULE:
    case PERIPH_BT_LC_MODULE:
        return SYSTEM_CORE_RST_EN_REG;
    case PERIPH_UART2_MODULE:
    case PERIPH_SDMMC_MODULE:
    case PERIPH_LCD_CAM_MODULE:
    case PERIPH_GDMA_MODULE:
    case PERIPH_HMAC_MODULE:
    case PERIPH_DS_MODULE:
    case PERIPH_AES_MODULE:
    case PERIPH_SHA_MODULE:
    case PERIPH_RSA_MODULE:
        return SYSTEM_PERIP_RST_EN1_REG;
    default:
        return SYSTEM_PERIP_RST_EN0_REG;
    }
}

static inline void periph_ll_enable_clk_clear_rst(periph_module_t periph)
{
    DPORT_SET_PERI_REG_MASK(periph_ll_get_clk_en_reg(periph), periph_ll_get_clk_en_mask(periph));
    DPORT_CLEAR_PERI_REG_MASK(periph_ll_get_rst_en_reg(periph), periph_ll_get_rst_en_mask(periph, true));
}

static inline void periph_ll_disable_clk_set_rst(periph_module_t periph)
{
    DPORT_CLEAR_PERI_REG_MASK(periph_ll_get_clk_en_reg(periph), periph_ll_get_clk_en_mask(periph));
    DPORT_SET_PERI_REG_MASK(periph_ll_get_rst_en_reg(periph), periph_ll_get_rst_en_mask(periph, false));
}

static inline void IRAM_ATTR periph_ll_wifi_bt_module_enable_clk_clear_rst(void)
{
    DPORT_SET_PERI_REG_MASK(SYSTEM_WIFI_CLK_EN_REG, SYSTEM_WIFI_CLK_WIFI_BT_COMMON_M);
    DPORT_CLEAR_PERI_REG_MASK(SYSTEM_CORE_RST_EN_REG, 0);
}

static inline void IRAM_ATTR periph_ll_wifi_bt_module_disable_clk_set_rst(void)
{
    DPORT_CLEAR_PERI_REG_MASK(SYSTEM_WIFI_CLK_EN_REG, SYSTEM_WIFI_CLK_WIFI_BT_COMMON_M);
    DPORT_SET_PERI_REG_MASK(SYSTEM_CORE_RST_EN_REG, 0);
}

static inline void periph_ll_reset(periph_module_t periph)
{
    DPORT_SET_PERI_REG_MASK(periph_ll_get_rst_en_reg(periph), periph_ll_get_rst_en_mask(periph, false));
    DPORT_CLEAR_PERI_REG_MASK(periph_ll_get_rst_en_reg(periph), periph_ll_get_rst_en_mask(periph, false));
}

static inline bool IRAM_ATTR periph_ll_periph_enabled(periph_module_t periph)
{
    return DPORT_REG_GET_BIT(periph_ll_get_rst_en_reg(periph), periph_ll_get_rst_en_mask(periph, false)) == 0 &&
           DPORT_REG_GET_BIT(periph_ll_get_clk_en_reg(periph), periph_ll_get_clk_en_mask(periph)) != 0;
}

static inline void periph_ll_wifi_module_enable_clk_clear_rst(void)
{
    DPORT_SET_PERI_REG_MASK(SYSTEM_WIFI_CLK_EN_REG, SYSTEM_WIFI_CLK_WIFI_EN_M);
    DPORT_CLEAR_PERI_REG_MASK(SYSTEM_CORE_RST_EN_REG, 0);
}

static inline void periph_ll_wifi_module_disable_clk_set_rst(void)
{
    DPORT_CLEAR_PERI_REG_MASK(SYSTEM_WIFI_CLK_EN_REG, SYSTEM_WIFI_CLK_WIFI_EN_M);
    DPORT_SET_PERI_REG_MASK(SYSTEM_CORE_RST_EN_REG, 0);
}
#ifdef __cplusplus
}
#endif
