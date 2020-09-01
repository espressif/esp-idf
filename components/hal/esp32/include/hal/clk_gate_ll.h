// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "soc/periph_defs.h"
#include "soc/dport_reg.h"
#include "soc/dport_access.h"

static inline uint32_t periph_ll_get_clk_en_mask(periph_module_t periph)
{
    switch (periph) {
    case PERIPH_LEDC_MODULE:
        return DPORT_LEDC_CLK_EN;
    case PERIPH_UART0_MODULE:
        return DPORT_UART_CLK_EN;
    case PERIPH_UART1_MODULE:
        return DPORT_UART1_CLK_EN;
    case PERIPH_UART2_MODULE:
        return DPORT_UART2_CLK_EN;
    case PERIPH_I2C0_MODULE:
        return DPORT_I2C_EXT0_CLK_EN;
    case PERIPH_I2C1_MODULE:
        return DPORT_I2C_EXT1_CLK_EN;
    case PERIPH_I2S0_MODULE:
        return DPORT_I2S0_CLK_EN;
    case PERIPH_I2S1_MODULE:
        return DPORT_I2S1_CLK_EN;
    case PERIPH_TIMG0_MODULE:
        return DPORT_TIMERGROUP_CLK_EN;
    case PERIPH_TIMG1_MODULE:
        return DPORT_TIMERGROUP1_CLK_EN;
    case PERIPH_PWM0_MODULE:
        return DPORT_PWM0_CLK_EN;
    case PERIPH_PWM1_MODULE:
        return DPORT_PWM1_CLK_EN;
    case PERIPH_PWM2_MODULE:
        return DPORT_PWM2_CLK_EN;
    case PERIPH_PWM3_MODULE:
        return DPORT_PWM3_CLK_EN;
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
    case PERIPH_HSPI_MODULE:
        return DPORT_SPI2_CLK_EN;
    case PERIPH_VSPI_MODULE:
        return DPORT_SPI3_CLK_EN;
    case PERIPH_SPI_DMA_MODULE:
        return DPORT_SPI_DMA_CLK_EN;
    case PERIPH_SDMMC_MODULE:
        return DPORT_WIFI_CLK_SDIO_HOST_EN;
    case PERIPH_SDIO_SLAVE_MODULE:
        return DPORT_WIFI_CLK_SDIOSLAVE_EN;
    case PERIPH_TWAI_MODULE:
        return DPORT_TWAI_CLK_EN;
    case PERIPH_EMAC_MODULE:
        return DPORT_WIFI_CLK_EMAC_EN;
    case PERIPH_RNG_MODULE:
        return DPORT_WIFI_CLK_RNG_EN;
    case PERIPH_WIFI_MODULE:
        return DPORT_WIFI_CLK_WIFI_EN_M;
    case PERIPH_BT_MODULE:
        return DPORT_WIFI_CLK_BT_EN_M;
    case PERIPH_WIFI_BT_COMMON_MODULE:
        return DPORT_WIFI_CLK_WIFI_BT_COMMON_M;
    case PERIPH_BT_BASEBAND_MODULE:
        return DPORT_BT_BASEBAND_EN;
    case PERIPH_BT_LC_MODULE:
        return DPORT_BT_LC_EN;
    case PERIPH_AES_MODULE:
        return DPORT_PERI_EN_AES;
    case PERIPH_SHA_MODULE:
        return DPORT_PERI_EN_SHA;
    case PERIPH_RSA_MODULE:
        return DPORT_PERI_EN_RSA;
    default:
        return 0;
    }
}

static inline uint32_t periph_ll_get_rst_en_mask(periph_module_t periph, bool enable)
{
    switch (periph) {
    case PERIPH_LEDC_MODULE:
        return DPORT_LEDC_RST;
    case PERIPH_UART0_MODULE:
        return DPORT_UART_RST;
    case PERIPH_UART1_MODULE:
        return DPORT_UART1_RST;
    case PERIPH_UART2_MODULE:
        return DPORT_UART2_RST;
    case PERIPH_I2C0_MODULE:
        return DPORT_I2C_EXT0_RST;
    case PERIPH_I2C1_MODULE:
        return DPORT_I2C_EXT1_RST;
    case PERIPH_I2S0_MODULE:
        return DPORT_I2S0_RST;
    case PERIPH_I2S1_MODULE:
        return DPORT_I2S1_RST;
    case PERIPH_TIMG0_MODULE:
        return DPORT_TIMERGROUP_RST;
    case PERIPH_TIMG1_MODULE:
        return DPORT_TIMERGROUP1_RST;
    case PERIPH_PWM0_MODULE:
        return DPORT_PWM0_RST;
    case PERIPH_PWM1_MODULE:
        return DPORT_PWM1_RST;
    case PERIPH_PWM2_MODULE:
        return DPORT_PWM2_RST;
    case PERIPH_PWM3_MODULE:
        return DPORT_PWM3_RST;
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
    case PERIPH_HSPI_MODULE:
        return DPORT_SPI2_RST;
    case PERIPH_VSPI_MODULE:
        return DPORT_SPI3_RST;
    case PERIPH_SPI_DMA_MODULE:
        return DPORT_SPI_DMA_RST;
    case PERIPH_SDMMC_MODULE:
        return DPORT_SDIO_HOST_RST;
    case PERIPH_SDIO_SLAVE_MODULE:
        return DPORT_SDIO_RST;
    case PERIPH_TWAI_MODULE:
        return DPORT_TWAI_RST;
    case PERIPH_EMAC_MODULE:
        return DPORT_EMAC_RST;
    case PERIPH_AES_MODULE:
        if (enable == true) {
            // Clear reset on digital signature & secure boot units, otherwise AES unit is held in reset also.
            return (DPORT_PERI_EN_AES | DPORT_PERI_EN_DIGITAL_SIGNATURE | DPORT_PERI_EN_SECUREBOOT);
        } else {
            //Don't return other units to reset, as this pulls reset on RSA & SHA units, respectively.
            return DPORT_PERI_EN_AES;
        }
    case PERIPH_SHA_MODULE:
        if (enable == true) {
            // Clear reset on secure boot, otherwise SHA is held in reset
            return (DPORT_PERI_EN_SHA | DPORT_PERI_EN_SECUREBOOT);
        } else {
            // Don't assert reset on secure boot, otherwise AES is held in reset
            return DPORT_PERI_EN_SHA;
        }
    case PERIPH_RSA_MODULE:
        if (enable == true) {
            // Also clear reset on digital signature, otherwise RSA is held in reset
            return (DPORT_PERI_EN_RSA | DPORT_PERI_EN_DIGITAL_SIGNATURE);
        } else {
            // Don't reset digital signature unit, as this resets AES also
            return DPORT_PERI_EN_RSA;
        }
    default:
        return 0;
    }
}

static inline uint32_t periph_ll_get_clk_en_reg(periph_module_t periph)
{
    switch (periph) {
    case PERIPH_AES_MODULE:
    case PERIPH_SHA_MODULE:
    case PERIPH_RSA_MODULE:
        return DPORT_PERI_CLK_EN_REG;
    case PERIPH_SDMMC_MODULE:
    case PERIPH_SDIO_SLAVE_MODULE:
    case PERIPH_EMAC_MODULE:
    case PERIPH_RNG_MODULE:
    case PERIPH_WIFI_MODULE:
    case PERIPH_BT_MODULE:
    case PERIPH_WIFI_BT_COMMON_MODULE:
    case PERIPH_BT_BASEBAND_MODULE:
    case PERIPH_BT_LC_MODULE:
        return DPORT_WIFI_CLK_EN_REG;
    default:
        return DPORT_PERIP_CLK_EN_REG;
    }
}

static inline uint32_t periph_ll_get_rst_en_reg(periph_module_t periph)
{
    switch (periph) {
    case PERIPH_AES_MODULE:
    case PERIPH_SHA_MODULE:
    case PERIPH_RSA_MODULE:
        return DPORT_PERI_RST_EN_REG;
    case PERIPH_SDMMC_MODULE:
    case PERIPH_SDIO_SLAVE_MODULE:
    case PERIPH_EMAC_MODULE:
    case PERIPH_RNG_MODULE:
    case PERIPH_WIFI_MODULE:
    case PERIPH_BT_MODULE:
    case PERIPH_WIFI_BT_COMMON_MODULE:
    case PERIPH_BT_BASEBAND_MODULE:
    case PERIPH_BT_LC_MODULE:
        return DPORT_CORE_RST_EN_REG;
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

#ifdef __cplusplus
}
#endif