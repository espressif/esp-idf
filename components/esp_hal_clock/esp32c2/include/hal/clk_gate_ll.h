/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "hal/assert.h"
#include "soc/periph_defs.h"
#include "soc/system_reg.h"
#include "soc/syscon_reg.h"
#include "soc/dport_access.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t periph_ll_get_clk_en_mask(shared_periph_module_t periph)
{
    switch (periph) {
    case PERIPH_TIMG0_MODULE:
        return SYSTEM_TIMERGROUP_CLK_EN;
    case PERIPH_SYSTIMER_MODULE:
        return SYSTEM_SYSTIMER_CLK_EN;
    case PERIPH_RNG_MODULE:
        return SYSTEM_WIFI_CLK_RNG_EN;
    case PERIPH_WIFI_MODULE:
        return SYSTEM_WIFI_CLK_WIFI_EN_M;
    case PERIPH_BT_MODULE:
        return SYSTEM_WIFI_CLK_BT_EN_M;
    case PERIPH_WIFI_BT_COMMON_MODULE:
        return SYSTEM_WIFI_CLK_WIFI_BT_COMMON_M;
    default:
        return 0;
    }
}

static inline uint32_t periph_ll_get_rst_en_mask(shared_periph_module_t periph, bool enable)
{

    (void)enable; // unused

    switch (periph) {
    case PERIPH_WIFI_MODULE:
        return SYSTEM_MAC_RST;
    case PERIPH_TIMG0_MODULE:
        return SYSTEM_TIMERGROUP_RST;
    case PERIPH_SYSTIMER_MODULE:
        return SYSTEM_SYSTIMER_RST;
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
        return SYSTEM_WIFI_CLK_EN_REG;

    default:
        return SYSTEM_PERIP_CLK_EN0_REG;
    }
}

static inline uint32_t periph_ll_get_rst_en_reg(shared_periph_module_t periph)
{
    switch (periph) {
    case PERIPH_RNG_MODULE:
    case PERIPH_WIFI_MODULE:
    case PERIPH_BT_MODULE:
    case PERIPH_WIFI_BT_COMMON_MODULE:
        return SYSTEM_WIFI_RST_EN_REG;

    default:
        return SYSTEM_PERIP_RST_EN0_REG;
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
    DPORT_SET_PERI_REG_MASK(SYSTEM_WIFI_CLK_EN_REG, SYSTEM_WIFI_CLK_WIFI_BT_COMMON_M);
}

static inline void IRAM_ATTR periph_ll_wifi_bt_module_disable_clk(void)
{
    DPORT_CLEAR_PERI_REG_MASK(SYSTEM_WIFI_CLK_EN_REG, SYSTEM_WIFI_CLK_WIFI_BT_COMMON_M);
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
    DPORT_SET_PERI_REG_MASK(SYSTEM_WIFI_CLK_EN_REG, SYSTEM_WIFI_CLK_WIFI_EN_M);
    DPORT_CLEAR_PERI_REG_MASK(SYSTEM_CORE_RST_EN_REG, 0);
}

static inline void periph_ll_wifi_module_disable_clk_set_rst(void)
{
    DPORT_CLEAR_PERI_REG_MASK(SYSTEM_WIFI_CLK_EN_REG, SYSTEM_WIFI_CLK_WIFI_EN_M);
    DPORT_SET_PERI_REG_MASK(SYSTEM_CORE_RST_EN_REG, 0);
}

static inline void periph_ll_phy_calibration_module_enable_clk_clear_rst(void)
{
    DPORT_SET_PERI_REG_MASK(SYSTEM_WIFI_CLK_EN_REG, SYSTEM_WIFI_CLK_PHY_EN_M);
    DPORT_CLEAR_PERI_REG_MASK(SYSTEM_CORE_RST_EN_REG, 0);
}

static inline void periph_ll_phy_calibration_module_disable_clk_set_rst(void)
{
    DPORT_CLEAR_PERI_REG_MASK(SYSTEM_WIFI_CLK_EN_REG, SYSTEM_WIFI_CLK_PHY_EN_M);
    DPORT_SET_PERI_REG_MASK(SYSTEM_CORE_RST_EN_REG, 0);
}

static inline void periph_ll_coex_module_enable_clk_clear_rst(void)
{
    DPORT_SET_PERI_REG_MASK(SYSTEM_WIFI_CLK_EN_REG, SYSTEM_WIFI_CLK_COEX_EN_M);
    DPORT_CLEAR_PERI_REG_MASK(SYSTEM_CORE_RST_EN_REG, 0);
}

static inline void periph_ll_coex_module_disable_clk_set_rst(void)
{
    DPORT_CLEAR_PERI_REG_MASK(SYSTEM_WIFI_CLK_EN_REG, SYSTEM_WIFI_CLK_COEX_EN_M);
    DPORT_SET_PERI_REG_MASK(SYSTEM_CORE_RST_EN_REG, 0);
}
#ifdef __cplusplus
}
#endif
