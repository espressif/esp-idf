/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_attr.h"
#include "soc/soc.h"
#include "soc/reset_reasons.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/hp_alive_sys_struct.h"
#include "soc/bus_monitor_struct.h"
#include "soc/cnnt_sys_struct.h"
#include "soc/lp_clkrst_struct.h"
#include "soc/lp_clkrst_reg.h"
#include "soc/lp_peri_clkrst_struct.h"
#include "soc/lp_gpio_struct.h"
#include "soc/usb_serial_jtag_struct.h"
#include "soc/tee_struct.h"
#include "soc/lp_tee_struct.h"
#include "soc/hp_apm_struct.h"
#include "soc/hp_mem_apm_struct.h"
#include "soc/cpu_apm_struct.h"
#include "soc/lp_apm_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enable or disable the clock gate for rtc fast to lp periph
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_rtc_fast_to_lp_periph_en(bool enable)
{
    LP_CLKRST.root_clk_conf.fosc_clk_force_on = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_rtc_fast_to_lp_periph_en(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _clk_gate_ll_rtc_fast_to_lp_periph_en(__VA_ARGS__); \
    } while(0)

/**
 * Enable or disable the clock gate for ref_20m (derived from BBPLL).
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_20m_clk_en(bool enable)
{
    HP_SYS_CLKRST.ref_20m_ctrl0.reg_ref_20m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_20m_clk_en(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _clk_gate_ll_ref_20m_clk_en(__VA_ARGS__); \
    } while(0)

/**
 * Enable or disable the clock gate for ref_25m (derived from MPLL).
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_25m_clk_en(bool enable)
{
    HP_SYS_CLKRST.ref_25m_ctrl0.reg_ref_25m_clk_en = enable;
}

typedef struct {
    bool disable_uart0_clk;           ///< Gate UART0 when it is not the console UART
    bool disable_uart1_clk;           ///< Gate UART1 when it is not the console UART
    bool disable_uart2_clk;           ///< Gate UART2 when it is not the console UART
    bool disable_uart3_clk;           ///< Gate UART3 when it is not the console UART
    bool disable_mspi_flash_clk;    ///< Gate MSPI flash clocks (e.g. PURE_RAM_APP)
    bool keep_psram_hp_clk;         ///< Keep PSRAM MSPI root clocks when CONFIG_SPIRAM
    bool disable_assist_clk;        ///< Gate bus monitor (assist) clocks when PC record unused
    bool disable_crypto_periph_clk; ///< Gate crypto accelerators when TEE does not own them
    bool disable_usb_serial_jtag;   ///< Gate USB Serial/JTAG when disabled in menuconfig
} periph_ll_clk_gate_config_t;

/**
 * @brief Turn off unused peripheral root clocks after reset (HP_SYS_CLKRST + LP cleanup).
 *
 * Skips HP peripheral gating on CPU-local reset reasons so debug / SW reset keep clocks aligned
 * with retained peripheral state. Does not touch CPU/cache/ICM/intrmtx/iomux skeleton clocks.
 */
static inline void periph_ll_clk_gate_set_default(soc_reset_reason_t rst_reason, const periph_ll_clk_gate_config_t *config)
{
    const bool skip_hp_periph_gate = (rst_reason == RESET_REASON_CPU_MWDT) ||
                                     (rst_reason == RESET_REASON_CPU_SW) ||
                                     (rst_reason == RESET_REASON_CPU_RWDT) ||
                                     (rst_reason == RESET_REASON_CPU_JTAG) ||
                                     (rst_reason == RESET_REASON_CPU_LOCKUP);

    if (!skip_hp_periph_gate) {
        // UART0~3
        if (config->disable_uart0_clk) {
            HP_SYS_CLKRST.uart0_ctrl0.reg_uart0_sys_clk_en = 0;
            HP_SYS_CLKRST.uart0_ctrl0.reg_uart0_apb_clk_en = 0;
            HP_SYS_CLKRST.uart0_ctrl0.reg_uart0_clk_en = 0;
        }
        if (config->disable_uart1_clk) {
            HP_SYS_CLKRST.uart1_ctrl0.reg_uart1_sys_clk_en = 0;
            HP_SYS_CLKRST.uart1_ctrl0.reg_uart1_apb_clk_en = 0;
            HP_SYS_CLKRST.uart1_ctrl0.reg_uart1_clk_en = 0;
        }
        if (config->disable_uart2_clk) {
            HP_SYS_CLKRST.uart2_ctrl0.reg_uart2_sys_clk_en = 0;
            HP_SYS_CLKRST.uart2_ctrl0.reg_uart2_apb_clk_en = 0;
            HP_SYS_CLKRST.uart2_ctrl0.reg_uart2_clk_en = 0;
        }
        if (config->disable_uart3_clk) {
            HP_SYS_CLKRST.uart3_ctrl0.reg_uart3_sys_clk_en = 0;
            HP_SYS_CLKRST.uart3_ctrl0.reg_uart3_apb_clk_en = 0;
            HP_SYS_CLKRST.uart3_ctrl0.reg_uart3_clk_en = 0;
        }
        HP_ALIVE_SYS.hp_pad_uart0_ctrl.hp_pad_uart0_slp_clk_en = 0;
        HP_ALIVE_SYS.hp_pad_uart1_ctrl.hp_pad_uart1_slp_clk_en = 0;
        HP_ALIVE_SYS.hp_pad_uart2_ctrl.hp_pad_uart2_slp_clk_en = 0;
        HP_ALIVE_SYS.hp_pad_uart3_ctrl.hp_pad_uart3_slp_clk_en = 0;
        // ETM
        HP_SYS_CLKRST.etm_ctrl0.reg_soc_etm_clk_en = 0;
        // Trace & Bus Monitor (0)
        HP_SYS_CLKRST.trace_ctrl0.reg_trace_cpu_clk_en = 0;
        HP_SYS_CLKRST.trace_ctrl0.reg_trace_sys_clk_en = 0;
        // Assist Debug
        if (config->disable_assist_clk) {
            BUS_MONITOR.clock_gate.clk_en = 0;
        }
        // AXI DMA / AHB DMA / REGDMA / GDMA
        HP_SYS_CLKRST.axi_pdma_ctrl0.reg_axi_pdma_sys_clk_en    = 0;
        HP_SYS_CLKRST.ahb_pdma_ctrl0.reg_ahb_pdma_sys_clk_en    = 0;
        HP_SYS_CLKRST.regdma_ctrl0.reg_regdma_sys_clk_en        = 0;
        HP_ALIVE_SYS.hp_pad_parlio_ctrl.hp_pad_parlio_tx_clk_en = 0;
        HP_ALIVE_SYS.hp_pad_parlio_ctrl.hp_pad_parlio_rx_clk_en = 0;
        // I2S0-1
        HP_ALIVE_SYS.hp_pad_i2s0_ctrl.hp_pad_i2s0_mclk_en = 0;
        HP_ALIVE_SYS.hp_pad_i2s1_ctrl.hp_pad_i2s1_mclk_en = 0;
        // TIMG0-1
        HP_SYS_CLKRST.timergrp0_ctrl0.reg_timergrp0_apb_clk_en = 0;
        HP_SYS_CLKRST.timergrp0_ctrl0.reg_timergrp0_t0_clk_en = 0;
        HP_SYS_CLKRST.timergrp0_ctrl0.reg_timergrp0_t1_clk_en = 0;
        HP_SYS_CLKRST.timergrp0_ctrl0.reg_timergrp0_wdt_clk_en = 0;
        HP_SYS_CLKRST.timergrp1_ctrl0.reg_timergrp1_apb_clk_en = 0;
        HP_SYS_CLKRST.timergrp1_ctrl0.reg_timergrp1_t0_clk_en = 0;
        HP_SYS_CLKRST.timergrp1_ctrl0.reg_timergrp1_t1_clk_en = 0;
        HP_SYS_CLKRST.timergrp1_ctrl0.reg_timergrp1_wdt_clk_en = 0;
        // LCDCAM
        /*
         * Default lcdcam_lcdcam_ctrl0:
         *   reg_lcdcam_clk_src_sel: 1 (BBPLL 120 MHz path, shared by LCD and CAM)
         *   reg_lcdcam_clk_div_num: 1 (divide by 2 → 60 MHz)
         */
        HP_SYS_CLKRST.lcdcam_lcdcam_ctrl0.reg_lcdcam_clk_src_sel = 1;
        HP_SYS_CLKRST.lcdcam_lcdcam_ctrl0.reg_lcdcam_clk_div_num = 1;
        // ASRC
        HP_SYS_CLKRST.ahb_asrc_ctrl0.reg_ahb_asrc_sys_clk_en = 0;
        // Flash
        if (config->disable_mspi_flash_clk) {
            HP_SYS_CLKRST.flash_ctrl0.reg_flash_sys_clk_en = 0;
            HP_SYS_CLKRST.flash_ctrl0.reg_flash_pll_clk_en = 0;
            HP_SYS_CLKRST.flash_ctrl0.reg_flash_core_clk_en = 0;
        }
        // PSRAM
        if (!config->keep_psram_hp_clk) {
            HP_SYS_CLKRST.psram_ctrl0.reg_psram_sys_clk_en = 0;
            HP_SYS_CLKRST.psram_ctrl0.reg_psram_pll_clk_en = 0;
            HP_SYS_CLKRST.psram_ctrl0.reg_psram_core_clk_en = 0;
        }
        // MSPI
        if (config->disable_mspi_flash_clk && !config->keep_psram_hp_clk) {
            HP_SYS_CLKRST.mspi_pad_ctrl0.reg_mspi_pad_ctrl_apb_clk_en = 0;
        }
        // GPSPI
        HP_SYS_CLKRST.gpspi2_ctrl0.reg_gpspi2_sys_clk_en = 0;
        HP_SYS_CLKRST.gpspi2_ctrl0.reg_gpspi2_apb_clk_en = 0;
        HP_SYS_CLKRST.gpspi2_ctrl0.reg_gpspi2_hs_clk_en = 0;
        HP_SYS_CLKRST.gpspi2_ctrl0.reg_gpspi2_mst_clk_en = 0;
        HP_SYS_CLKRST.gpspi3_ctrl0.reg_gpspi3_sys_clk_en = 0;
        HP_SYS_CLKRST.gpspi3_ctrl0.reg_gpspi3_apb_clk_en = 0;
        HP_SYS_CLKRST.gpspi3_ctrl0.reg_gpspi3_hs_clk_en = 0;
        HP_SYS_CLKRST.gpspi3_ctrl0.reg_gpspi3_mst_clk_en = 0;
        // HP I2C master
        HP_SYS_CLKRST.hp_i2cmst_ctrl0.reg_hp_i2cmst_apb_clk_en = 0;
        // Crypto + security monitors (TEE/APM)
        if (config->disable_crypto_periph_clk) {
            HP_SYS_CLKRST.crypto_ctrl0.reg_crypto_sys_clk_en = 0;
            HP_SYS_CLKRST.crypto_ctrl0.reg_crypto_sec_clk_en = 0;
            HP_SYS_CLKRST.crypto_ctrl0.reg_crypto_aes_clk_en = 0;
            HP_SYS_CLKRST.crypto_ctrl0.reg_crypto_sha_clk_en = 0;
            HP_SYS_CLKRST.crypto_ctrl0.reg_crypto_rsa_clk_en = 0;
            HP_SYS_CLKRST.crypto_ctrl0.reg_crypto_ds_clk_en = 0;
            HP_SYS_CLKRST.crypto_ctrl0.reg_crypto_ecc_clk_en = 0;
            HP_SYS_CLKRST.crypto_ctrl0.reg_crypto_hmac_clk_en = 0;
            HP_SYS_CLKRST.crypto_ctrl0.reg_crypto_ecdsa_clk_en = 0;
            HP_SYS_CLKRST.crypto_ctrl0.reg_crypto_rma_clk_en = 0;
            HP_SYS_CLKRST.key_manager_ctrl0.reg_key_manager_sys_clk_en = 0;
            HP_SYS_CLKRST.key_manager_ctrl0.reg_crypto_km_clk_en = 0;
            TEE.clock_gate.clk_en = 0;
            LP_TEE.clock_gate.clk_en = 0;
            HP_APM.clock_gate.clk_en = 0;
            HP_MEM_APM.clock_gate.clk_en = 0;
            CPU_APM.clock_gate.clk_en = 0;
        }
        // UTMI
        CNNT_SYS_REG.sys_usb_otg20_ctrl.sys_usb_otg20_utmifs_clk_en = 0;
        // USB-Serial-JTAG
        if (config->disable_usb_serial_jtag) {
            USB_SERIAL_JTAG.conf0.usb_pad_enable = 0;
            HP_SYS_CLKRST.usb_device_ctrl0.reg_usb_device_apb_clk_en = 0;
            USB_SERIAL_JTAG.mem_conf.usb_mem_pd = 1;
            USB_SERIAL_JTAG.mem_conf.usb_mem_clk_en = 0;
            CNNT_SYS_REG.sys_hp_usb_device_ctrl.sys_usb_device_48m_clk_en = 0;
        }
    }

    if ((rst_reason == RESET_REASON_CHIP_POWER_ON) || (rst_reason == RESET_REASON_SYS_BROWN_OUT) ||
            (rst_reason == RESET_REASON_SYS_RWDT) || (rst_reason == RESET_REASON_SYS_SUPER_WDT) ||
            (rst_reason == RESET_REASON_CORE_PWR_GLITCH) || (rst_reason == RESET_REASON_CORE_EFUSE_CRC)) {
        LP_GPIO.clock_gate.clk_en = 0;
        while (LP_GPIO.clock_gate.clk_en != 0) {
            ;
        }
        LP_PERI_CLKRST.i2cmst_ctrl.lp_i2cmst_clk_en      = 0;
        LP_PERI_CLKRST.touch_ctrl.lp_touch_clk_en        = 0;
        LP_PERI_CLKRST.rng_ctrl.lp_rng_clk_en            = 0;
        LP_PERI_CLKRST.dm_ctrl.lp_dm_clk_en              = 0;
        if (config->disable_crypto_periph_clk) {
            LP_APM.clock_gate.clk_en = 0;
        }
    }
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_25m_clk_en(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _clk_gate_ll_ref_25m_clk_en(__VA_ARGS__); \
    } while(0)

/**
 * Enable or disable the clock gate for ref_50m (derived from MPLL).
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_50m_clk_en(bool enable)
{
    HP_SYS_CLKRST.ref_50m_ctrl0.reg_ref_50m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_50m_clk_en(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _clk_gate_ll_ref_50m_clk_en(__VA_ARGS__); \
    } while(0)

/**
 * Enable or disable the clock gate for ref_80m (derived from BBPLL).
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_80m_clk_en(bool enable)
{
    HP_SYS_CLKRST.ref_80m_ctrl0.reg_ref_80m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_80m_clk_en(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _clk_gate_ll_ref_80m_clk_en(__VA_ARGS__); \
    } while(0)

/**
 * Enable or disable the clock gate for ref_160m (derived from BBPLL).
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_160m_clk_en(bool enable)
{
    HP_SYS_CLKRST.ref_160m_ctrl0.reg_ref_160m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_160m_clk_en(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _clk_gate_ll_ref_160m_clk_en(__VA_ARGS__); \
    } while(0)

/**
 * Enable or disable the clock gate for ref_240m (derived from BBPLL).
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_240m_clk_en(bool enable)
{
    HP_SYS_CLKRST.ref_240m_ctrl0.reg_ref_240m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_240m_clk_en(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _clk_gate_ll_ref_240m_clk_en(__VA_ARGS__); \
    } while(0)

#ifdef __cplusplus
}
#endif
