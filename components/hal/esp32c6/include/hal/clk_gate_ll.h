/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "hal/assert.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/reset_reasons.h"
#include "soc/periph_defs.h"
#include "soc/pcr_reg.h"
#include "soc/pcr_struct.h"
#include "soc/gdma_struct.h"
#include "soc/lpperi_struct.h"
#include "soc/lp_uart_struct.h"
#include "soc/uart_struct.h"
#include "soc/usb_serial_jtag_struct.h"
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
                PCR.ds_conf.ds_rst_en = 0;
            }
            return PCR_AES_RST_EN;
        case PERIPH_SHA_MODULE:
            if (enable == true) {
                // Clear reset on digital signature and HMAC, otherwise SHA is held in reset
                PCR.ds_conf.ds_rst_en = 0;
                PCR.hmac_conf.hmac_rst_en = 0;
            }
            return PCR_SHA_RST_EN;
        case PERIPH_RSA_MODULE:
            if (enable == true) {
                // Clear reset on digital signature, otherwise RSA is held in reset
                PCR.ds_conf.ds_rst_en = 0;
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

/**
 * @brief Configuration structure for peripheral clock gate settings
 */
typedef struct {
    bool disable_uart0_clk;          ///< Disable UART0 clock
    bool disable_uart1_clk;          ///< Disable UART1 clock
    bool disable_mspi_flash_clk;     ///< Disable MSPI flash clock
    bool disable_assist_clk;         ///< Disable ASSIST Debug module clock
    bool disable_crypto_periph_clk;  ///< Disable crypto peripherals clock
    bool disable_usb_serial_jtag;    ///< Disable USB-Serial-JTAG clock
    bool disable_pvt_clk;            ///< Disable PVT clock
} periph_ll_clk_gate_config_t;

/**
 * @brief Set the default clock gate configuration
 * @param config The configuration structure
 */
static inline void periph_ll_clk_gate_set_default(soc_reset_reason_t rst_reason, const periph_ll_clk_gate_config_t *config)
{
    if ((rst_reason != RESET_REASON_CPU0_MWDT0) && (rst_reason != RESET_REASON_CPU0_MWDT1)      \
            && (rst_reason != RESET_REASON_CPU0_SW) && (rst_reason != RESET_REASON_CPU0_RTC_WDT)    \
            && (rst_reason != RESET_REASON_CPU0_JTAG)) {

        if (config->disable_uart0_clk) {
            PCR.uart0_conf.uart0_clk_en = 0;
            PCR.uart0_sclk_conf.uart0_sclk_en = 0;
        }

        else if (config->disable_uart1_clk) {
            PCR.uart1_sclk_conf.uart1_sclk_en = 0;
            PCR.uart1_conf.uart1_clk_en = 0;
        }

        PCR.i2c_conf.i2c_clk_en = 0;
        PCR.i2c_sclk_conf.i2c_sclk_en = 0;
        PCR.rmt_conf.rmt_clk_en = 0;
        PCR.rmt_sclk_conf.rmt_sclk_en = 0;
        PCR.ledc_sclk_conf.ledc_sclk_en = 0;
        PCR.ledc_conf.ledc_clk_en = 0;
        PCR.timergroup0_timer_clk_conf.tg0_timer_clk_en = 0;
        PCR.timergroup1_timer_clk_conf.tg1_timer_clk_en = 0;
        PCR.timergroup0_conf.tg0_clk_en = 0;
        PCR.timergroup1_conf.tg1_clk_en = 0;
        PCR.twai0_func_clk_conf.twai0_func_clk_en = 0;
        PCR.twai0_conf.twai0_clk_en = 0;
        PCR.twai1_func_clk_conf.twai1_func_clk_en = 0;
        PCR.twai1_conf.twai1_clk_en = 0;
        PCR.i2s_conf.i2s_clk_en = 0;
        PCR.i2s_tx_clkm_conf.i2s_tx_clkm_en = 0;
        PCR.i2s_rx_clkm_conf.i2s_rx_clkm_en = 0;
        PCR.pcnt_conf.pcnt_clk_en = 0;
        PCR.etm_conf.etm_clk_en = 0;
        PCR.pwm_conf.pwm_clk_en = 0;
        PCR.pwm_clk_conf.pwm_clkm_en = 0;
        PCR.parl_clk_rx_conf.parl_clk_rx_en = 0;
        PCR.parl_clk_tx_conf.parl_clk_tx_en = 0;
        PCR.parl_io_conf.parl_clk_en = 0;
        PCR.gdma_conf.gdma_clk_en = 0;

        if (config->disable_mspi_flash_clk) {
            PCR.mspi_conf.mspi_clk_en = 0;
        }

        PCR.spi2_conf.spi2_clk_en = 0;
        PCR.tsens_clk_conf.tsens_clk_en = 0;

        PCR.uhci_conf.uhci_clk_en = 0;
        PCR.saradc_conf.saradc_clk_en = 0;
        PCR.sdio_slave_conf.sdio_slave_clk_en = 0;

        if (config->disable_assist_clk) {
            /* Disable ASSIST Debug module clock if PC recoreding function is not used,
            * if stack guard function needs it, it will be re-enabled at esp_hw_stack_guard_init */
            PCR.assist_conf.assist_clk_en = 0;
        }

        if (config->disable_crypto_periph_clk) {
            // NOTE: [ESP-TEE] The TEE is responsible for the AES and SHA peripherals
            PCR.aes_conf.aes_clk_en = 0;
            PCR.sha_conf.sha_clk_en = 0;
            PCR.hmac_conf.hmac_clk_en = 0;
            PCR.ds_conf.ds_clk_en = 0;
            PCR.ecc_conf.ecc_clk_en = 0;
            PCR.rsa_conf.rsa_clk_en = 0;
        }

        PCR.ctrl_tick_conf.tick_enable = 0;
        PCR.trace_conf.trace_clk_en = 0;
        PCR.mem_monitor_conf.mem_monitor_clk_en = 0;
        if (config->disable_pvt_clk) {
            PCR.pvt_monitor_conf.pvt_monitor_clk_en = 0;
            PCR.pvt_monitor_func_clk_conf.pvt_monitor_func_clk_en = 0;
        }
        PCR.ctrl_clk_out_en.val = 0;

        if (config->disable_usb_serial_jtag) {
            // Disable USB-Serial-JTAG clock and it's pad if not used
            USB_SERIAL_JTAG.conf0.usb_pad_enable = 0;
            PCR.usb_device_conf.usb_device_clk_en = 0;
        }
    }

    if ((rst_reason == RESET_REASON_CHIP_POWER_ON) || (rst_reason == RESET_REASON_CHIP_BROWN_OUT) \
            || (rst_reason == RESET_REASON_SYS_RTC_WDT) || (rst_reason == RESET_REASON_SYS_SUPER_WDT)) {
        LP_CLKRST.lp_clk_po_en.val = 0;
        LPPERI.clk_en.val = 0;
        LPPERI.clk_en.efuse_ck_en = 1;  // keep efuse clock enabled
        LP_UART.clk_conf.val = 0;
    }
}

#ifdef __cplusplus
}
#endif
