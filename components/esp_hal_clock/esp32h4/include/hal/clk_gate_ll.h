/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_attr.h"
#include "soc/pcr_struct.h"
#include "soc/soc.h"
#include "soc/reset_reasons.h"
#include "soc/usb_serial_jtag_struct.h"
#include "soc/lpperi_struct.h"
#include "soc/lp_clkrst_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enable or disable the clock gate for ref_8m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_8m_clk_en(bool enable)
{
    PCR.pll_div_clk_en.pll_8m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_8m_clk_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_ref_8m_clk_en(__VA_ARGS__)

/**
 * Enable or disable the clock gate for ref_16m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_16m_clk_en(bool enable)
{
    PCR.pll_div_clk_en.pll_16m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_16m_clk_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_ref_16m_clk_en(__VA_ARGS__)

/**
 * Enable or disable the clock gate for ref_32m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_32m_clk_en(bool enable)
{
    PCR.pll_div_clk_en.pll_32m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_32m_clk_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_ref_32m_clk_en(__VA_ARGS__)

/**
 * Enable or disable the clock gate for ref_48m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_48m_clk_en(bool enable)
{
    PCR.pll_div_clk_en.pll_48m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_48m_clk_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_ref_48m_clk_en(__VA_ARGS__)

/**
 * Enable or disable the clock gate for ref_64m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_64m_clk_en(bool enable)
{
    PCR.pll_div_clk_en.pll_64m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_64m_clk_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_ref_64m_clk_en(__VA_ARGS__)

/**
 * Enable or disable the clock gate for ref_96m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_96m_clk_en(bool enable)
{
    PCR.pll_div_clk_en.pll_96m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_96m_clk_en(...) (void)__DECLARE_RCC_ATOMIC_ENV; _clk_gate_ll_ref_96m_clk_en(__VA_ARGS__)

/**
 * @brief Configuration structure for peripheral clock gate settings
 */
typedef struct {
    bool disable_uart0_clk;          ///< Disable UART0 clock (when UART0 is not console)
    bool disable_uart1_clk;          ///< Disable UART1 clock (when UART1 is not console)
    bool disable_mspi_flash_clk;     ///< Disable MSPI flash clock (for PURE_RAM_APP)
    bool disable_assist_clk;         ///< Disable ASSIST Debug module clock
    bool disable_crypto_periph_clk;  ///< Disable crypto peripherals clock when TEE is not enabled
    bool disable_usb_serial_jtag;    ///< Disable USB-Serial-JTAG clock and pad (when not enabled)
    bool disable_pvt_clk;            ///< Disable PVT monitor clocks when PVT is not used
} periph_ll_clk_gate_config_t;

/**
 * @brief Set default clock gates: turn off unused peripheral root clocks at CPU start.
 *
 * @note LP_PERI clocks are gated on cold boot paths; efuse clock stays enabled.
 */
static inline void periph_ll_clk_gate_set_default(soc_reset_reason_t rst_reason, const periph_ll_clk_gate_config_t *config)
{
    if ((rst_reason != RESET_REASON_CPU0_MWDT0) && (rst_reason != RESET_REASON_CPU0_MWDT1)      \
            && (rst_reason != RESET_REASON_CPU0_SW) && (rst_reason != RESET_REASON_CPU0_RTC_WDT)    \
            && (rst_reason != RESET_REASON_CPU0_JTAG)) {
        /* Use independent ifs: console may be neither UART0 nor UART1 (e.g. USB), both need gating. */
        if (config->disable_uart0_clk) {
            PCR.uart0_conf.uart0_clk_en = 0;
            PCR.uart0_sclk_conf.uart0_sclk_en = 0;
        }
        if (config->disable_uart1_clk) {
            PCR.uart1_conf.uart1_clk_en = 0;
            PCR.uart1_sclk_conf.uart1_sclk_en = 0;
        }

        PCR.i2c0_conf.i2c0_clk_en = 0;
        PCR.i2c1_conf.i2c1_clk_en = 0;
        PCR.i2c0_sclk_conf.i2c0_sclk_en = 0;
        PCR.i2c1_sclk_conf.i2c1_sclk_en = 0;
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
        PCR.i2s_conf.i2s_clk_en = 0;
        PCR.i2s_tx_clkm_conf.i2s_tx_clkm_en = 0;
        PCR.i2s_rx_clkm_conf.i2s_rx_clkm_en = 0;
        PCR.pcnt_conf.pcnt_clk_en = 0;
        PCR.etm_conf.etm_clk_en = 0;
        PCR.pwm0_conf.pwm0_clk_en = 0;
        PCR.pwm0_clk_conf.pwm0_clkm_en = 0;
        PCR.pwm1_conf.pwm1_clk_en = 0;
        PCR.pwm1_clk_conf.pwm1_clkm_en = 0;
        PCR.parl_clk_rx_conf.parl_clk_rx_en = 0;
        PCR.parl_clk_tx_conf.parl_clk_tx_en = 0;
        PCR.parl_io_conf.parl_clk_en = 0;
        PCR.gdma_conf.gdma_clk_en = 0;
        PCR.regdma_conf.regdma_clk_en = 0;

        if (config->disable_mspi_flash_clk) {
            PCR.mspi_conf.mspi_clk_en = 0;
            PCR.mspi_conf.mspi_pll_clk_en = 0;
            PCR.mspi_clk_conf.mspi_func_clk_en = 0;
        }

        PCR.spi2_conf.spi2_clk_en = 0;
        PCR.spi2_clkm_conf.spi2_clkm_en = 0;
        PCR.spi3_conf.spi3_clk_en = 0;
        PCR.spi3_clkm_conf.spi3_clkm_en = 0;
        PCR.tsens_clk_conf.tsens_clk_en = 0;

        PCR.uhci_conf.uhci_clk_en = 0;
        PCR.saradc_conf.saradc_clk_en = 0;
        PCR.asrc_func_clk_conf.asrc_func_clk_en = 0;
        PCR.asrc_func_clk_conf.asrc_apb_clk_en = 0;
        PCR.zero_det_conf.zero_det_clk_en = 0;
        PCR.zero_det_clk_conf.zero_det_func_clk_en = 0;

        if (config->disable_assist_clk) {
            /* Disable ASSIST Debug module clock if PC recording is not used;
             * stack guard may re-enable in esp_hw_stack_guard_init */
            PCR.assist_conf.assist_clk_en = 0;
            PCR.assist_conf.assist1_clk_en = 0;
        }

        if (config->disable_crypto_periph_clk) {
            // NOTE: [ESP-TEE] The TEE is responsible for the AES and SHA peripherals
            PCR.aes_conf.aes_clk_en = 0;
            PCR.sha_conf.sha_clk_en = 0;
            PCR.ecc_conf.ecc_clk_en = 0;
            PCR.hmac_conf.hmac_clk_en = 0;
            PCR.km_conf.km_clk_en = 0;
            PCR.ecdsa_conf.ecdsa_clk_en = 0;
        }

        PCR.trace_conf.trace_clk_en = 0;
        PCR.tcm_mem_monitor_conf.tcm_mem_monitor_clk_en = 0;
        PCR.psram_mem_monitor_conf.psram_mem_monitor_clk_en = 0;
        if (config->disable_pvt_clk) {
            PCR.pvt_monitor_conf.pvt_monitor_clk_en = 0;
            PCR.pvt_monitor_func_clk_conf.pvt_monitor_func_clk_en = 0;
        }
        PCR.ctrl_clk_out_en.val = 0;

        if (config->disable_usb_serial_jtag) {
            // Disable USB-Serial-JTAG clock and its pad if not used
            USB_SERIAL_JTAG.serial_jtag_conf0.serial_jtag_usb_pad_enable = 0;
            PCR.usb_device_conf.usb_device_clk_en = 0;
            PCR.usb_otg11_conf.usb_otg11_clk_en = 0;
        }
    }

    if ((rst_reason == RESET_REASON_CHIP_POWER_ON) || (rst_reason == RESET_REASON_CHIP_BROWN_OUT) \
            || (rst_reason == RESET_REASON_SYS_RTC_WDT) || (rst_reason == RESET_REASON_SYS_SUPER_WDT)) {
        LPPERI.clk_en.val = 0;
        LPPERI.clk_en.efuse_ck_en = 1;  // keep efuse clock enabled
        LP_CLKRST.lp_clk_po_en.val = 0;
    }
}

#ifdef __cplusplus
}
#endif
