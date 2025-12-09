/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
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
#include "soc/lp_clkrst_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t periph_ll_get_clk_en_mask(shared_periph_module_t periph)
{
    switch (periph) {
        case PERIPH_SARADC_MODULE:
            return PCR_SARADC_CLK_EN;
        case PERIPH_TIMG0_MODULE:
            return PCR_TG0_CLK_EN;
        case PERIPH_TIMG1_MODULE:
            return PCR_TG1_CLK_EN;
        case PERIPH_UHCI0_MODULE:
            return PCR_UHCI_CLK_EN;
        case PERIPH_SYSTIMER_MODULE:
            return PCR_SYSTIMER_CLK_EN;
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
        case PERIPH_SDIO_SLAVE_MODULE:
            return PCR_SDIO_SLAVE_CLK_EN;
        case PERIPH_ASSIST_DEBUG_MODULE:
            return PCR_ASSIST_CLK_EN;
        default:
            return 0;
    }
}

static inline uint32_t periph_ll_get_rst_en_mask(shared_periph_module_t periph, bool enable)
{
    (void)enable; // unused

    switch (periph) {
        case PERIPH_SARADC_MODULE:
            return PCR_SARADC_REG_RST_EN;
        case PERIPH_TIMG0_MODULE:
            return PCR_TG0_RST_EN;
        case PERIPH_TIMG1_MODULE:
            return PCR_TG1_RST_EN;
        case PERIPH_UHCI0_MODULE:
            return PCR_UHCI_RST_EN;
        case PERIPH_SYSTIMER_MODULE:
            return PCR_SYSTIMER_RST_EN;
        case PERIPH_ECC_MODULE:
            return PCR_ECC_RST_EN;
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
        case PERIPH_ASSIST_DEBUG_MODULE:
            return PCR_ASSIST_RST_EN;
        default:
            return 0;
    }
}

static inline uint32_t periph_ll_get_clk_en_reg(shared_periph_module_t periph)
{
    switch (periph) {
        case PERIPH_SARADC_MODULE:
            return PCR_SARADC_CONF_REG;
        case PERIPH_TIMG0_MODULE:
            return PCR_TIMERGROUP0_CONF_REG;
        case PERIPH_TIMG1_MODULE:
            return PCR_TIMERGROUP1_CONF_REG;
        case PERIPH_UHCI0_MODULE:
            return PCR_UHCI_CONF_REG;
        case PERIPH_SYSTIMER_MODULE:
            return PCR_SYSTIMER_CONF_REG;
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
        case PERIPH_SDIO_SLAVE_MODULE:
            return PCR_SDIO_SLAVE_CONF_REG;
        case PERIPH_ASSIST_DEBUG_MODULE:
            return PCR_ASSIST_CONF_REG;
    default:
        return 0;
    }
}

static inline uint32_t periph_ll_get_rst_en_reg(shared_periph_module_t periph)
{
    switch (periph) {
        case PERIPH_SARADC_MODULE:
            return PCR_SARADC_CONF_REG;
        case PERIPH_TIMG0_MODULE:
            return PCR_TIMERGROUP0_CONF_REG;
        case PERIPH_TIMG1_MODULE:
            return PCR_TIMERGROUP1_CONF_REG;
        case PERIPH_UHCI0_MODULE:
            return PCR_UHCI_CONF_REG;
        case PERIPH_SYSTIMER_MODULE:
            return PCR_SYSTIMER_CONF_REG;
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
        case PERIPH_SDIO_SLAVE_MODULE:
            return PCR_SDIO_SLAVE_CONF_REG;
        case PERIPH_ASSIST_DEBUG_MODULE:
            return PCR_ASSIST_CONF_REG;
    default:
        return 0;
    }
}

static inline void periph_ll_enable_clk_clear_rst(shared_periph_module_t periph)
{
    SET_PERI_REG_MASK(periph_ll_get_clk_en_reg(periph), periph_ll_get_clk_en_mask(periph));
    CLEAR_PERI_REG_MASK(periph_ll_get_rst_en_reg(periph), periph_ll_get_rst_en_mask(periph, true));
}

static inline void periph_ll_disable_clk_set_rst(shared_periph_module_t periph)
{
    CLEAR_PERI_REG_MASK(periph_ll_get_clk_en_reg(periph), periph_ll_get_clk_en_mask(periph));
    SET_PERI_REG_MASK(periph_ll_get_rst_en_reg(periph), periph_ll_get_rst_en_mask(periph, false));
}

static inline void periph_ll_reset(shared_periph_module_t periph)
{
    SET_PERI_REG_MASK(periph_ll_get_rst_en_reg(periph), periph_ll_get_rst_en_mask(periph, false));
    CLEAR_PERI_REG_MASK(periph_ll_get_rst_en_reg(periph), periph_ll_get_rst_en_mask(periph, false));
}

static inline bool IRAM_ATTR periph_ll_periph_enabled(shared_periph_module_t periph)
{
    return REG_GET_BIT(periph_ll_get_rst_en_reg(periph), periph_ll_get_rst_en_mask(periph, false)) == 0 &&
           REG_GET_BIT(periph_ll_get_clk_en_reg(periph), periph_ll_get_clk_en_mask(periph)) != 0;
}

/**
 * Enable or disable the clock gate for rtc_fast to lp periph
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_rtc_fast_to_lp_periph_en(bool enable)
{
    LP_CLKRST.lp_clk_en.fast_ori_gate = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_rtc_fast_to_lp_periph_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_rtc_fast_to_lp_periph_en(__VA_ARGS__)

#ifdef __cplusplus
}
#endif
