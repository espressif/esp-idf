/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
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
#include "soc/hp_sys_clkrst_reg.h"
#include "soc/lp_clkrst_struct.h"
#include "soc/lp_clkrst_reg.h"
#include "soc/lp_system_reg.h"
#include "soc/mipi_dsi_bridge_reg.h"
#include "soc/sdmmc_reg.h"
#include "soc/spi_mem_c_reg.h"
#include "soc/spi_mem_s_reg.h"
#include "soc/assist_debug_reg.h"
#include "soc/trace_reg.h"
#include "soc/usb_serial_jtag_reg.h"
#include "soc/lp_uart_reg.h"
#include "soc/lp_gpio_reg.h"
#include "soc/lpperi_reg.h"
#include "soc/uart_reg.h"
#include "soc/usb_dwc_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enable or disable the clock gate for ref_20m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_20m_clk_en(bool enable)
{
    HP_SYS_CLKRST.ref_clk_ctrl2.reg_ref_20m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_20m_clk_en(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _clk_gate_ll_ref_20m_clk_en(__VA_ARGS__); \
    } while(0)

/**
 * Enable or disable the clock gate for ref_20m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_25m_clk_en(bool enable)
{
    HP_SYS_CLKRST.ref_clk_ctrl1.reg_ref_25m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_25m_clk_en(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _clk_gate_ll_ref_25m_clk_en(__VA_ARGS__); \
    } while(0)

/**
 * Enable or disable the clock gate for ref_20m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_80m_clk_en(bool enable)
{
    HP_SYS_CLKRST.ref_clk_ctrl2.reg_ref_80m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_80m_clk_en(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _clk_gate_ll_ref_80m_clk_en(__VA_ARGS__); \
    } while(0)

/**
 * Enable or disable the clock gate for ref_20m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_160m_clk_en(bool enable)
{
    HP_SYS_CLKRST.ref_clk_ctrl2.reg_ref_160m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_160m_clk_en(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _clk_gate_ll_ref_160m_clk_en(__VA_ARGS__); \
    } while(0)

/**
 * Enable or disable the clock gate for ref_120m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_120m_clk_en(bool enable)
{
    HP_SYS_CLKRST.ref_clk_ctrl2.reg_ref_120m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_120m_clk_en(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _clk_gate_ll_ref_120m_clk_en(__VA_ARGS__); \
    } while(0)

/**
 * Enable or disable the clock gate for ref_20m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_240m_clk_en(bool enable)
{
    HP_SYS_CLKRST.ref_clk_ctrl1.reg_ref_240m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_240m_clk_en(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _clk_gate_ll_ref_240m_clk_en(__VA_ARGS__); \
    } while(0)

/**
 * Enable or disable the clock gate for xtal to lp periph
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_xtal_to_lp_periph_en(bool enable)
{
    LP_AON_CLKRST.lp_clk_en.xtal_clk_force_on = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_xtal_to_lp_periph_en(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _clk_gate_ll_xtal_to_lp_periph_en(__VA_ARGS__); \
    } while(0)

/**
 * Enable or disable the clock gate for rtc fast to lp periph
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_rtc_fast_to_lp_periph_en(bool enable)
{
    LP_AON_CLKRST.lp_clk_en.fosc_clk_force_on = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_rtc_fast_to_lp_periph_en(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _clk_gate_ll_rtc_fast_to_lp_periph_en(__VA_ARGS__); \
    } while(0)

/**
 * Enable or disable the clock gate for ref_50m.
 * @param  enable Enable / disable
 */
FORCE_INLINE_ATTR void _clk_gate_ll_ref_50m_clk_en(bool enable)
{
    HP_SYS_CLKRST.ref_clk_ctrl1.reg_ref_50m_clk_en = enable;
}
/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define clk_gate_ll_ref_50m_clk_en(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _clk_gate_ll_ref_50m_clk_en(__VA_ARGS__); \
    } while(0)

/**
 * @brief Configuration structure for peripheral clock gate settings
 */
typedef struct {
    bool disable_uart0_clk;              ///< Disable UART0 clock
    bool disable_uart1_clk;              ///< Disable UART1 clock
    bool disable_mspi_flash_clk;         ///< Disable MSPI flash clock
    bool disable_mspi_flash_timing_clk;  ///< Disable MSPI flash timing calibration clock
    bool disable_mspi_psram_timing_clk;  ///< Disable MSPI PSRAM timing calibration clock
    bool disable_mspi_psram_clk;         ///< Disable MSPI PSRAM clock
    bool disable_pvt_clk;                ///< Disable PVT monitor clock
    bool disable_spiram_boot_clk;        ///< Disable SPI RAM boot clock
    bool disable_assist_clk;             ///< Disable ASSIST Debug module clock
    bool disable_usb_serial_jtag;        ///< Disable USB-Serial-JTAG clock
} periph_ll_clk_gate_config_t;

/**
 * @brief Set the default clock gate configuration
 * @param config The configuration structure
 */
static inline void periph_ll_clk_gate_set_default(soc_reset_reason_t rst_reason, const periph_ll_clk_gate_config_t *config)
{
    if ((rst_reason == RESET_REASON_CHIP_POWER_ON)   || (rst_reason == RESET_REASON_CORE_PMU_PWR_DOWN) ||
            (rst_reason == RESET_REASON_SYS_BROWN_OUT)   || (rst_reason == RESET_REASON_SYS_RWDT) ||
            (rst_reason == RESET_REASON_SYS_SUPER_WDT)   || (rst_reason == RESET_REASON_CORE_SW) ||
            (rst_reason == RESET_REASON_CORE_MWDT)       || (rst_reason == RESET_REASON_CORE_RWDT) ||
            (rst_reason == RESET_REASON_CORE_PWR_GLITCH) || (rst_reason == RESET_REASON_CORE_EFUSE_CRC) ||
            (rst_reason == RESET_REASON_CORE_USB_JTAG)   || (rst_reason == RESET_REASON_CORE_USB_UART)
       ) {
        // Not gate HP_SYS_CLKRST_REG_L2MEM_MEM_CLK_FORCE_ON since the hardware will not automatically ungate when DMA accesses L2 MEM.
        REG_CLR_BIT(HP_SYS_CLKRST_CLK_FORCE_ON_CTRL0_REG,
                    HP_SYS_CLKRST_REG_CPUICM_GATED_CLK_FORCE_ON
                    | HP_SYS_CLKRST_REG_TCM_CPU_CLK_FORCE_ON
                    | HP_SYS_CLKRST_REG_BUSMON_CPU_CLK_FORCE_ON
                    | HP_SYS_CLKRST_REG_TRACE_CPU_CLK_FORCE_ON
                    | HP_SYS_CLKRST_REG_TRACE_SYS_CLK_FORCE_ON);
        REG_CLR_BIT(HP_SYS_CLKRST_CLK_FORCE_ON_CTRL0_REG,
                    HP_SYS_CLKRST_REG_SAR1_CLK_FORCE_ON
                    | HP_SYS_CLKRST_REG_SAR2_CLK_FORCE_ON
                    | HP_SYS_CLKRST_REG_GMAC_TX_CLK_FORCE_ON);

        // hp_sys_clkrst register gets reset only if chip reset or pmu powers down hp
        // but at core reset and above, we will also disable HP modules' clock gating to save power consumption
        REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL1_REG,
                    HP_SYS_CLKRST_REG_AHB_PDMA_SYS_CLK_EN |
                    HP_SYS_CLKRST_REG_AXI_PDMA_SYS_CLK_EN |
                    HP_SYS_CLKRST_REG_REGDMA_SYS_CLK_EN |
                    HP_SYS_CLKRST_REG_BITSCRAMBLER_SYS_CLK_EN |
                    HP_SYS_CLKRST_REG_BITSCRAMBLER_RX_SYS_CLK_EN |
                    HP_SYS_CLKRST_REG_BITSCRAMBLER_TX_SYS_CLK_EN);
        // HP_SYS_CLKRST_REG_PARLIO_SYS_CLK_EN, HP_SYS_CLKRST_REG_ETM_SYS_CLK_EN default to 0, removed
        // HP_SYS_CLKRST_REG_PARLIO_APB_CLK_EN, HP_SYS_CLKRST_REG_ETM_APB_CLK_EN default to 0, removed

        // Non-Console UART
        if (config->disable_uart0_clk) {
            REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL2_REG, HP_SYS_CLKRST_REG_UART0_APB_CLK_EN);
            REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL1_REG, HP_SYS_CLKRST_REG_UART0_SYS_CLK_EN);
            REG_CLR_BIT(HP_SYS_CLKRST_PERI_CLK_CTRL110_REG, HP_SYS_CLKRST_REG_UART0_CLK_EN);
        } else if (config->disable_uart1_clk) {
            REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL2_REG, HP_SYS_CLKRST_REG_UART1_APB_CLK_EN);
            REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL1_REG, HP_SYS_CLKRST_REG_UART1_SYS_CLK_EN);
            REG_CLR_BIT(HP_SYS_CLKRST_PERI_CLK_CTRL111_REG, HP_SYS_CLKRST_REG_UART1_CLK_EN);
        }

        REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL2_REG, HP_SYS_CLKRST_REG_UART2_APB_CLK_EN |
                    HP_SYS_CLKRST_REG_UART3_APB_CLK_EN |
                    HP_SYS_CLKRST_REG_UART4_APB_CLK_EN);
        REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL1_REG, HP_SYS_CLKRST_REG_UART2_SYS_CLK_EN |
                    HP_SYS_CLKRST_REG_UART3_SYS_CLK_EN |
                    HP_SYS_CLKRST_REG_UART4_SYS_CLK_EN);
        REG_CLR_BIT(HP_SYS_CLKRST_PERI_CLK_CTRL112_REG, HP_SYS_CLKRST_REG_UART2_CLK_EN);
        REG_CLR_BIT(HP_SYS_CLKRST_PERI_CLK_CTRL113_REG, HP_SYS_CLKRST_REG_UART3_CLK_EN);
        REG_CLR_BIT(HP_SYS_CLKRST_PERI_CLK_CTRL114_REG, HP_SYS_CLKRST_REG_UART4_CLK_EN);

        REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL2_REG, HP_SYS_CLKRST_REG_TIMERGRP0_APB_CLK_EN |
                    HP_SYS_CLKRST_REG_TIMERGRP1_APB_CLK_EN);
        REG_CLR_BIT(HP_SYS_CLKRST_PERI_CLK_CTRL20_REG, HP_SYS_CLKRST_REG_TIMERGRP0_T0_CLK_EN |
                    HP_SYS_CLKRST_REG_TIMERGRP0_T1_CLK_EN);
        REG_CLR_BIT(HP_SYS_CLKRST_PERI_CLK_CTRL21_REG, HP_SYS_CLKRST_REG_TIMERGRP1_T0_CLK_EN |
                    HP_SYS_CLKRST_REG_TIMERGRP1_T1_CLK_EN);

        REG_CLR_BIT(DSI_BRG_HOST_CTRL_REG, DSI_BRG_DSI_CFG_REF_CLK_EN);
        REG_CLR_BIT(SDHOST_CLK_EDGE_SEL_REG, SDHOST_CCLK_EN);

        // Disable MSPI flash clock
        if (config->disable_mspi_flash_clk) {
            REG_CLR_BIT(HP_SYS_CLKRST_PERI_CLK_CTRL00_REG, HP_SYS_CLKRST_REG_FLASH_CORE_CLK_EN |
                        HP_SYS_CLKRST_REG_FLASH_PLL_CLK_EN);
            REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL0_REG, HP_SYS_CLKRST_REG_FLASH_SYS_CLK_EN);
        }

        // Disable flash timing calibration clock
        if (config->disable_mspi_flash_timing_clk) {
            REG_CLR_BIT(SPI_MEM_C_TIMING_CALI_REG, SPI_MEM_C_TIMING_CLK_ENA);
            REG_CLR_BIT(SPI_MEM_C_SMEM_TIMING_CALI_REG, SPI_MEM_C_SMEM_TIMING_CLK_ENA);
        }

        // Disable PSRAM timing calibration clock
        if (config->disable_mspi_psram_timing_clk) {
            REG_CLR_BIT(SPI_MEM_S_TIMING_CALI_REG, SPI_MEM_S_TIMING_CLK_ENA);
            REG_CLR_BIT(SPI_MEM_S_SMEM_TIMING_CALI_REG, SPI_MEM_S_SMEM_TIMING_CLK_ENA);
        }

        // Disable PSRAM clock
        if (config->disable_mspi_psram_clk) {
            REG_CLR_BIT(HP_SYS_CLKRST_PERI_CLK_CTRL00_REG, HP_SYS_CLKRST_REG_PSRAM_CORE_CLK_EN |
                        HP_SYS_CLKRST_REG_PSRAM_PLL_CLK_EN);
            REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL0_REG, HP_SYS_CLKRST_REG_PSRAM_SYS_CLK_EN);
        }

        // Disable GP SPI2 and SPI3 clock
        REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL1_REG, HP_SYS_CLKRST_REG_GPSPI2_SYS_CLK_EN |
                    HP_SYS_CLKRST_REG_GPSPI3_SYS_CLK_EN);
        // HP_SYS_CLKRST_REG_CSI_HOST_SYS_CLK_EN defaults to 0, removed
        REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL2_REG, HP_SYS_CLKRST_REG_GPSPI2_APB_CLK_EN |
                    HP_SYS_CLKRST_REG_GPSPI3_APB_CLK_EN);
        REG_CLR_BIT(HP_SYS_CLKRST_PERI_CLK_CTRL116_REG, HP_SYS_CLKRST_REG_GPSPI2_HS_CLK_EN |
                    HP_SYS_CLKRST_REG_GPSPI2_MST_CLK_EN |
                    HP_SYS_CLKRST_REG_GPSPI3_HS_CLK_EN);
        REG_CLR_BIT(HP_SYS_CLKRST_PERI_CLK_CTRL117_REG, HP_SYS_CLKRST_REG_GPSPI3_MST_CLK_EN);

        if (config->disable_assist_clk) {
            /* Disable ASSIST Debug module clock if PC recoreding function is not used,
             * if stack guard function needs it, it will be re-enabled at esp_hw_stack_guard_init */
            REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL0_REG, HP_SYS_CLKRST_REG_BUSMON_CPU_CLK_EN);
            REG_CLR_BIT(ASSIST_DEBUG_CLOCK_GATE_REG, ASSIST_DEBUG_CLK_EN);
        }

        // Trace & Bus Monitor (0)
        REG_CLR_BIT(TRACE_CLOCK_GATE_REG(0), TRACE_CLK_EN);
        REG_CLR_BIT(TRACE_CLOCK_GATE_REG(1), TRACE_CLK_EN);

        // Crypto Modules
        REG_CLR_BIT(HP_SYS_CLKRST_PERI_CLK_CTRL25_REG, HP_SYS_CLKRST_REG_CRYPTO_AES_CLK_EN |
                    HP_SYS_CLKRST_REG_CRYPTO_DS_CLK_EN |
                    HP_SYS_CLKRST_REG_CRYPTO_ECC_CLK_EN |
                    HP_SYS_CLKRST_REG_CRYPTO_HMAC_CLK_EN |
                    HP_SYS_CLKRST_REG_CRYPTO_RSA_CLK_EN |
                    HP_SYS_CLKRST_REG_CRYPTO_SHA_CLK_EN);

        /*** USB sys & phy & pad & clock initialization for power saving ***/
        // Force the USB 2.0 PHY to enter suspend mode before disabling the clock.
        REG_SET_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL1_REG, HP_SYS_CLKRST_REG_USB_OTG20_SYS_CLK_EN);
        REG_SET_BIT(LP_CLKRST_HP_USB_CLKRST_CTRL1_REG, LP_CLKRST_USB_OTG20_PHYREF_CLK_EN);
        USB_DWC_HS.gotgctl_reg.bvalidoven = 1;
        USB_DWC_HS.pcgcctl_reg.stoppclk = 1;
        // USB1.1 & USB OTG2.0 sys clock gating
        REG_CLR_BIT(LP_CLKRST_HP_USB_CLKRST_CTRL0_REG, LP_CLKRST_USB_OTG11_BK_SYS_CLK_EN |
                    LP_CLKRST_USB_OTG11_48M_CLK_EN |
                    LP_CLKRST_USB_OTG20_BK_SYS_CLK_EN);
        REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL1_REG, HP_SYS_CLKRST_REG_USB_OTG11_SYS_CLK_EN |
                    HP_SYS_CLKRST_REG_USB_OTG20_SYS_CLK_EN |
                    HP_SYS_CLKRST_REG_UHCI_SYS_CLK_EN);
        // USB2.0 phy & ULPI clock gating
        REG_CLR_BIT(LP_CLKRST_HP_USB_CLKRST_CTRL1_REG, LP_CLKRST_USB_OTG20_PHYREF_CLK_EN |
                    LP_CLKRST_USB_OTG20_ULPI_CLK_EN);
        // UHCI clock gating
        REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL2_REG, HP_SYS_CLKRST_REG_UHCI_APB_CLK_EN);

        if (config->disable_usb_serial_jtag) {
            // Disable USB-Serial-JTAG clock and it's pad if not used
            REG_CLR_BIT(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_USB_PAD_ENABLE);
            REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL2_REG, HP_SYS_CLKRST_REG_USB_DEVICE_APB_CLK_EN);
            REG_CLR_BIT(LP_CLKRST_HP_USB_CLKRST_CTRL0_REG, LP_CLKRST_USB_DEVICE_48M_CLK_EN);
            REG_SET_BIT(USB_SERIAL_JTAG_MEM_CONF_REG, USB_SERIAL_JTAG_USB_MEM_PD);
            REG_CLR_BIT(USB_SERIAL_JTAG_MEM_CONF_REG, USB_SERIAL_JTAG_USB_MEM_CLK_EN);
        }
    }

    // HP modules' clock source gating control
    if ((rst_reason == RESET_REASON_CHIP_POWER_ON) || (rst_reason == RESET_REASON_CORE_PMU_PWR_DOWN)) {
        // Only safe to disable these clock source gatings if all HP modules clock configurations has been reset
        REG_CLR_BIT(HP_SYS_CLKRST_REF_CLK_CTRL1_REG, HP_SYS_CLKRST_REG_REF_50M_CLK_EN |
                    HP_SYS_CLKRST_REG_REF_25M_CLK_EN);
        // 240M CLK is for Key Management use, should not be gated
        if (config->disable_pvt_clk) {
            REG_CLR_BIT(HP_SYS_CLKRST_REF_CLK_CTRL2_REG, HP_SYS_CLKRST_REG_REF_160M_CLK_EN);
        }
        // 160M CLK is for PVT use, should not be gated
        REG_CLR_BIT(HP_SYS_CLKRST_REF_CLK_CTRL2_REG, HP_SYS_CLKRST_REG_REF_120M_CLK_EN |
                    HP_SYS_CLKRST_REG_REF_80M_CLK_EN |
                    HP_SYS_CLKRST_REG_REF_20M_CLK_EN);
    }

    // LP related clock control
    if ((rst_reason == RESET_REASON_CHIP_POWER_ON) || (rst_reason == RESET_REASON_SYS_SUPER_WDT) \
            || (rst_reason == RESET_REASON_SYS_RWDT) || (rst_reason == RESET_REASON_SYS_BROWN_OUT)) {
        // lpperi,lp peripheral registers get reset for reset level equal or higher than system reset
        REG_CLR_BIT(LP_UART_CLK_CONF_REG, LP_UART_TX_SCLK_EN | LP_UART_RX_SCLK_EN);
        REG_CLR_BIT(LPPERI_CLK_EN_REG, LPPERI_CK_EN_LP_UART);
        REG_CLR_BIT(LP_GPIO_CLK_EN_REG, LP_GPIO_REG_CLK_EN);
        while (REG_GET_BIT(LP_GPIO_CLK_EN_REG, LP_GPIO_REG_CLK_EN) != 0) {
            ;
        }

        if (rst_reason == RESET_REASON_CHIP_POWER_ON) {
            // lp_aon_clkrst, lp_system registers get reset only if chip reset
            REG_CLR_BIT(LP_CLKRST_HP_CLK_CTRL_REG, LP_CLKRST_HP_PAD_UART0_SLP_CLK_EN |
                        LP_CLKRST_HP_PAD_UART1_SLP_CLK_EN |
                        LP_CLKRST_HP_PAD_UART2_SLP_CLK_EN |
                        LP_CLKRST_HP_PAD_UART3_SLP_CLK_EN |
                        LP_CLKRST_HP_PAD_UART4_SLP_CLK_EN);
            REG_CLR_BIT(LP_CLKRST_HP_CLK_CTRL_REG, LP_CLKRST_HP_PAD_PARLIO_TX_CLK_EN |
                        LP_CLKRST_HP_PAD_PARLIO_RX_CLK_EN |
                        LP_CLKRST_HP_PAD_I2S2_MCLK_EN |
                        LP_CLKRST_HP_PAD_I2S1_MCLK_EN |
                        LP_CLKRST_HP_PAD_I2S0_MCLK_EN |
                        LP_CLKRST_HP_PAD_EMAC_TX_CLK_EN |
                        LP_CLKRST_HP_PAD_EMAC_RX_CLK_EN |
                        LP_CLKRST_HP_PAD_EMAC_TXRX_CLK_EN |
                        LP_CLKRST_HP_PLL_8M_CLK_EN |
                        LP_CLKRST_HP_AUDIO_PLL_CLK_EN |
                        LP_CLKRST_HP_SDIO_PLL2_CLK_EN |
                        LP_CLKRST_HP_SDIO_PLL1_CLK_EN |
                        LP_CLKRST_HP_SDIO_PLL0_CLK_EN);
            if (config->disable_spiram_boot_clk) {
                REG_CLR_BIT(LP_CLKRST_HP_CLK_CTRL_REG, LP_CLKRST_HP_MPLL_500M_CLK_EN);
            }
            REG_CLR_BIT(LP_SYSTEM_REG_HP_ROOT_CLK_CTRL_REG, LP_SYSTEM_REG_CPU_CLK_EN);
        }
    }

}

#ifdef __cplusplus
}
#endif
