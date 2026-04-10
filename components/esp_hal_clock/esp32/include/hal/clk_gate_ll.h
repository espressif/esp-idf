/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_attr.h"
#include "hal/assert.h"
#include "soc/periph_defs.h"
#include "soc/dport_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t periph_ll_get_clk_en_mask(shared_periph_module_t periph)
{
    switch (periph) {
    case PERIPH_UART1_MODULE:
        return DPORT_UART1_CLK_EN;
    case PERIPH_UART2_MODULE:
        return DPORT_UART2_CLK_EN;
    case PERIPH_I2S0_MODULE:
        return DPORT_I2S0_CLK_EN;
    case PERIPH_TIMG0_MODULE:
        return DPORT_TIMERGROUP_CLK_EN;
    case PERIPH_TIMG1_MODULE:
        return DPORT_TIMERGROUP1_CLK_EN;
    case PERIPH_UHCI0_MODULE:
        return DPORT_UHCI0_CLK_EN;
    case PERIPH_SPI_MODULE:
        return DPORT_SPI01_CLK_EN;
    case PERIPH_HSPI_MODULE:
        return DPORT_SPI2_CLK_EN;
    case PERIPH_VSPI_MODULE:
        return DPORT_SPI3_CLK_EN;
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
    default:
        return 0;
    }
}

static inline uint32_t periph_ll_get_rst_en_mask(shared_periph_module_t periph, bool enable)
{
    switch (periph) {
    case PERIPH_WIFI_MODULE:
        return DPORT_WIFIMAC_RST;
    case PERIPH_BT_MODULE:
        return (DPORT_BTBB_RST | DPORT_BTMAC_RST | DPORT_RW_BTMAC_RST | DPORT_RW_BTLP_RST);
    case PERIPH_UART1_MODULE:
        return DPORT_UART1_RST;
    case PERIPH_UART2_MODULE:
        return DPORT_UART2_RST;
    case PERIPH_I2S0_MODULE:
        return DPORT_I2S0_RST;
    case PERIPH_TIMG0_MODULE:
        return DPORT_TIMERGROUP_RST;
    case PERIPH_TIMG1_MODULE:
        return DPORT_TIMERGROUP1_RST;
    case PERIPH_UHCI0_MODULE:
        return DPORT_UHCI0_RST;
    case PERIPH_SPI_MODULE:
        return DPORT_SPI01_RST;
    case PERIPH_HSPI_MODULE:
        return DPORT_SPI2_RST;
    case PERIPH_VSPI_MODULE:
        return DPORT_SPI3_RST;
    default:
        return 0;
    }
}

static inline uint32_t periph_ll_get_clk_en_reg(shared_periph_module_t periph)
{
    switch (periph) {
    case PERIPH_RNG_MODULE:
    case PERIPH_WIFI_MODULE:
    case PERIPH_BT_MODULE:
    case PERIPH_WIFI_BT_COMMON_MODULE:
    case PERIPH_BT_BASEBAND_MODULE:
        return DPORT_WIFI_CLK_EN_REG;
    default:
        return DPORT_PERIP_CLK_EN_REG;
    }
}

static inline uint32_t periph_ll_get_rst_en_reg(shared_periph_module_t periph)
{
    switch (periph) {
    case PERIPH_RNG_MODULE:
    case PERIPH_WIFI_MODULE:
    case PERIPH_BT_MODULE:
    case PERIPH_WIFI_BT_COMMON_MODULE:
    case PERIPH_BT_BASEBAND_MODULE:
        return DPORT_CORE_RST_EN_REG;
    default:
        return DPORT_PERIP_RST_EN_REG;
    }
}

static inline void periph_ll_enable_clk_clear_rst(shared_periph_module_t periph)
{
    DPORT_SET_PERI_REG_MASK(periph_ll_get_clk_en_reg(periph), periph_ll_get_clk_en_mask(periph));
    DPORT_CLEAR_PERI_REG_MASK(periph_ll_get_rst_en_reg(periph), periph_ll_get_rst_en_mask(periph, true));
}

static inline void periph_ll_disable_clk_set_rst(shared_periph_module_t periph)
{
    DPORT_CLEAR_PERI_REG_MASK(periph_ll_get_clk_en_reg(periph), periph_ll_get_clk_en_mask(periph));
    DPORT_SET_PERI_REG_MASK(periph_ll_get_rst_en_reg(periph), periph_ll_get_rst_en_mask(periph, false));
}

static inline void IRAM_ATTR periph_ll_wifi_bt_module_enable_clk(void)
{
    DPORT_SET_PERI_REG_MASK(DPORT_WIFI_CLK_EN_REG, DPORT_WIFI_CLK_WIFI_BT_COMMON_M);
}

static inline void IRAM_ATTR periph_ll_wifi_bt_module_disable_clk(void)
{
    DPORT_CLEAR_PERI_REG_MASK(DPORT_WIFI_CLK_EN_REG, DPORT_WIFI_CLK_WIFI_BT_COMMON_M);
}

static inline void periph_ll_reset(shared_periph_module_t periph)
{
    DPORT_SET_PERI_REG_MASK(periph_ll_get_rst_en_reg(periph), periph_ll_get_rst_en_mask(periph, false));
    DPORT_CLEAR_PERI_REG_MASK(periph_ll_get_rst_en_reg(periph), periph_ll_get_rst_en_mask(periph, false));
}

static inline bool IRAM_ATTR periph_ll_periph_enabled(shared_periph_module_t periph)
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

static inline void periph_ll_phy_calibration_module_enable_clk_clear_rst(void)
{
    // No clock bit only for phy calibration on ESP32
}

static inline void periph_ll_phy_calibration_module_disable_clk_set_rst(void)
{
    // No clock bit only for phy calibration on ESP32
}

#ifdef __cplusplus
}
#endif
