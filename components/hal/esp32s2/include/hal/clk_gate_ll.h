/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
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
        return DPORT_APB_SARADC_CLK_EN;
    case PERIPH_LEDC_MODULE:
        return DPORT_LEDC_CLK_EN;
    case PERIPH_UART0_MODULE:
        return DPORT_UART_CLK_EN;
    case PERIPH_UART1_MODULE:
        return DPORT_UART1_CLK_EN;
    case PERIPH_USB_MODULE:
        return DPORT_USB_CLK_EN;
    case PERIPH_I2C0_MODULE:
        return DPORT_I2C_EXT0_CLK_EN;
    case PERIPH_I2C1_MODULE:
        return DPORT_I2C_EXT1_CLK_EN;
    case PERIPH_I2S0_MODULE:
        return DPORT_I2S0_CLK_EN;
    case PERIPH_TIMG0_MODULE:
        return DPORT_TIMERGROUP_CLK_EN;
    case PERIPH_TIMG1_MODULE:
        return DPORT_TIMERGROUP1_CLK_EN;
    case PERIPH_UHCI0_MODULE:
        return DPORT_UHCI0_CLK_EN;
    case PERIPH_UHCI1_MODULE:
        return DPORT_UHCI1_CLK_EN;
    case PERIPH_RMT_MODULE:
        return DPORT_RMT_CLK_EN;
    case PERIPH_PCNT_MODULE:
        return DPORT_PCNT_CLK_EN;
    case PERIPH_SPI_MODULE:
        return DPORT_SPI01_CLK_EN;
    case PERIPH_FSPI_MODULE:
        return DPORT_SPI2_CLK_EN;
    case PERIPH_HSPI_MODULE:
        return DPORT_SPI3_CLK_EN;
    case PERIPH_SPI2_DMA_MODULE:
        return DPORT_SPI2_DMA_CLK_EN;
    case PERIPH_SPI3_DMA_MODULE:
        return DPORT_SPI3_DMA_CLK_EN;
    case PERIPH_TWAI_MODULE:
        return DPORT_TWAI_CLK_EN;
    case PERIPH_RNG_MODULE:
        return DPORT_WIFI_CLK_RNG_EN;
    case PERIPH_WIFI_MODULE:
        return DPORT_WIFI_CLK_WIFI_EN_M;
    case PERIPH_WIFI_BT_COMMON_MODULE:
        return DPORT_WIFI_CLK_WIFI_BT_COMMON_M;
    case PERIPH_SYSTIMER_MODULE:
        return DPORT_SYSTIMER_CLK_EN;
    case PERIPH_DEDIC_GPIO_MODULE:
        return DPORT_CLK_EN_DEDICATED_GPIO;
    case PERIPH_AES_MODULE:
        return DPORT_CRYPTO_AES_CLK_EN;
    case PERIPH_SHA_MODULE:
        return DPORT_CRYPTO_SHA_CLK_EN;
    case PERIPH_RSA_MODULE:
        return DPORT_CRYPTO_RSA_CLK_EN;
    case PERIPH_CRYPTO_DMA_MODULE:
        return DPORT_CRYPTO_DMA_CLK_EN;
    case PERIPH_SHA_DMA_MODULE:
        return DPORT_CRYPTO_DMA_CLK_EN | DPORT_CRYPTO_SHA_CLK_EN;
    case PERIPH_AES_DMA_MODULE:
        return DPORT_CRYPTO_DMA_CLK_EN | DPORT_CRYPTO_AES_CLK_EN;
    default:
        return 0;
    }
}

static inline uint32_t periph_ll_get_rst_en_mask(periph_module_t periph, bool enable)
{
    (void)enable; // unused

    switch (periph) {
    case PERIPH_SARADC_MODULE:
        return DPORT_APB_SARADC_RST;
    case PERIPH_LEDC_MODULE:
        return DPORT_LEDC_RST;
    case PERIPH_UART0_MODULE:
        return DPORT_UART_RST;
    case PERIPH_UART1_MODULE:
        return DPORT_UART1_RST;
    case PERIPH_USB_MODULE:
        return DPORT_USB_RST;
    case PERIPH_I2C0_MODULE:
        return DPORT_I2C_EXT0_RST;
    case PERIPH_I2C1_MODULE:
        return DPORT_I2C_EXT1_RST;
    case PERIPH_I2S0_MODULE:
        return DPORT_I2S0_RST;
    case PERIPH_TIMG0_MODULE:
        return DPORT_TIMERGROUP_RST;
    case PERIPH_TIMG1_MODULE:
        return DPORT_TIMERGROUP1_RST;
    case PERIPH_UHCI0_MODULE:
        return DPORT_UHCI0_RST;
    case PERIPH_UHCI1_MODULE:
        return DPORT_UHCI1_RST;
    case PERIPH_RMT_MODULE:
        return DPORT_RMT_RST;
    case PERIPH_PCNT_MODULE:
        return DPORT_PCNT_RST;
    case PERIPH_SPI_MODULE:
        return DPORT_SPI01_RST;
    case PERIPH_FSPI_MODULE:
        return DPORT_SPI2_RST;
    case PERIPH_HSPI_MODULE:
        return DPORT_SPI3_RST;
    case PERIPH_SPI2_DMA_MODULE:
        return DPORT_SPI2_DMA_RST;
    case PERIPH_SPI3_DMA_MODULE:
        return DPORT_SPI3_DMA_RST;
    case PERIPH_TWAI_MODULE:
        return DPORT_TWAI_RST;
    case PERIPH_SYSTIMER_MODULE:
        return DPORT_SYSTIMER_RST;
    case PERIPH_DEDIC_GPIO_MODULE:
        return DPORT_RST_EN_DEDICATED_GPIO;
    case PERIPH_AES_MODULE:
        if (enable == true) {
            // Clear reset on digital signature, otherwise AES unit is held in reset also.
            return (DPORT_CRYPTO_AES_RST | DPORT_CRYPTO_DS_RST);
        } else {
            //Don't return other units to reset, as this pulls reset on RSA & SHA units, respectively.
            return DPORT_CRYPTO_AES_RST;
        }
    case PERIPH_SHA_MODULE:
        if (enable == true) {
            // Clear reset on digital signature and HMAC, otherwise SHA is held in reset
            return (DPORT_CRYPTO_SHA_RST | DPORT_CRYPTO_DS_RST | DPORT_CRYPTO_HMAC_RST);
        } else {
            // Don't assert reset on secure boot, otherwise AES is held in reset
            return DPORT_CRYPTO_SHA_RST;
        }
    case PERIPH_RSA_MODULE:
        if (enable == true) {
            /* also clear reset on digital signature, otherwise RSA is held in reset */
            return (DPORT_CRYPTO_RSA_RST | DPORT_CRYPTO_DS_RST);
        } else {
            /* don't reset digital signature unit, as this resets AES also */
            return DPORT_CRYPTO_RSA_RST;
        }
    case PERIPH_CRYPTO_DMA_MODULE:
        return DPORT_CRYPTO_DMA_RST;

    case PERIPH_AES_DMA_MODULE:
        if (enable == true) {
            // Clear reset on digital signature, otherwise AES unit is held in reset also.
            return (DPORT_CRYPTO_AES_RST | DPORT_CRYPTO_DS_RST | DPORT_CRYPTO_DMA_RST);
        } else {
            //Don't return other units to reset, as this pulls reset on RSA & SHA units, respectively.
            return (DPORT_CRYPTO_AES_RST | DPORT_CRYPTO_DMA_RST);
        }
    case PERIPH_SHA_DMA_MODULE:
        if (enable == true) {
            // Clear reset on digital signature and HMAC, otherwise SHA is held in reset
            return (DPORT_CRYPTO_SHA_RST | DPORT_CRYPTO_DS_RST | DPORT_CRYPTO_HMAC_RST | DPORT_CRYPTO_DMA_RST);
        } else {
            // Don't assert reset on secure boot, otherwise AES is held in reset
            return (DPORT_CRYPTO_SHA_RST | DPORT_CRYPTO_DMA_RST);
        }
    default:
        return 0;
    }
}

static inline uint32_t periph_ll_get_clk_en_reg(periph_module_t periph)
{
    switch (periph) {
    case PERIPH_DEDIC_GPIO_MODULE:
        return DPORT_CPU_PERI_CLK_EN_REG;
    case PERIPH_RNG_MODULE:
    case PERIPH_WIFI_MODULE:
    case PERIPH_WIFI_BT_COMMON_MODULE:
        return DPORT_WIFI_CLK_EN_REG;
    case PERIPH_AES_MODULE:
    case PERIPH_SHA_MODULE:
    case PERIPH_RSA_MODULE:
    case PERIPH_CRYPTO_DMA_MODULE:
    case PERIPH_AES_DMA_MODULE:
    case PERIPH_SHA_DMA_MODULE:
        return DPORT_PERIP_CLK_EN1_REG;
    default:
        return DPORT_PERIP_CLK_EN_REG;
    }
}

static inline uint32_t periph_ll_get_rst_en_reg(periph_module_t periph)
{
    switch (periph) {
    case PERIPH_DEDIC_GPIO_MODULE:
        return DPORT_CPU_PERI_RST_EN_REG;
    case PERIPH_RNG_MODULE:
    case PERIPH_WIFI_MODULE:
    case PERIPH_WIFI_BT_COMMON_MODULE:
        return DPORT_CORE_RST_EN_REG;
    case PERIPH_AES_MODULE:
    case PERIPH_SHA_MODULE:
    case PERIPH_RSA_MODULE:
    case PERIPH_CRYPTO_DMA_MODULE:
    case PERIPH_AES_DMA_MODULE:
    case PERIPH_SHA_DMA_MODULE:
        return DPORT_PERIP_RST_EN1_REG;
    default:
        return DPORT_PERIP_RST_EN_REG;
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
    DPORT_SET_PERI_REG_MASK(DPORT_WIFI_CLK_EN_REG, DPORT_WIFI_CLK_WIFI_BT_COMMON_M);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_CORE_RST_EN_REG, 0);
}

static inline void IRAM_ATTR periph_ll_wifi_bt_module_disable_clk_set_rst(void)
{
    DPORT_CLEAR_PERI_REG_MASK(DPORT_WIFI_CLK_EN_REG, DPORT_WIFI_CLK_WIFI_BT_COMMON_M);
    DPORT_SET_PERI_REG_MASK(DPORT_CORE_RST_EN_REG, 0);
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
    DPORT_SET_PERI_REG_MASK(DPORT_WIFI_CLK_EN_REG, DPORT_WIFI_CLK_WIFI_EN_M);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_CORE_RST_EN_REG, 0);
}

static inline void periph_ll_wifi_module_disable_clk_set_rst(void)
{
    DPORT_CLEAR_PERI_REG_MASK(DPORT_WIFI_CLK_EN_REG, DPORT_WIFI_CLK_WIFI_EN_M);
    DPORT_SET_PERI_REG_MASK(DPORT_CORE_RST_EN_REG, 0);
}
#ifdef __cplusplus
}
#endif
