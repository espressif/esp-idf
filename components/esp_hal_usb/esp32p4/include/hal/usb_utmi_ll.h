/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_attr.h"
#include "soc/lp_clkrst_struct.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/hp_system_struct.h"
#include "soc/usb_utmi_struct.h"
#include "hal/config.h"
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
#include "soc/lp_system_struct.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------- USB PHY Control  ---------------------------- */

/**
 * @brief Configure Low-Speed mode
 *
 * @param[in] hw       Beginning address of the peripheral registers
 * @param[in] parallel Parallel or serial LS mode
 * @return FORCE_INLINE_ATTR
 */
FORCE_INLINE_ATTR void usb_utmi_ll_configure_ls(usb_utmi_dev_t *hw, bool parallel)
{
    hw->fc_06.ls_par_en = parallel;
    hw->fc_06.ls_kpalv_en = 1;
}

/* ----------------------------- RCC Functions  ----------------------------- */

/**
 * @brief Enable the bus clock for the USB UTMI PHY and USB_DWC_HS controller
 *
 * @param[in] clk_en True to enable, false to disable
 */
FORCE_INLINE_ATTR void _usb_utmi_ll_enable_bus_clock(bool clk_en)
{
    // Enable/disable system clock for USB_UTMI and USB_DWC_HS
    HP_SYS_CLKRST.soc_clk_ctrl1.reg_usb_otg20_sys_clk_en = clk_en;
    // Enable PHY ref clock (48MHz) for USB UTMI PHY
    LP_AON_CLKRST.hp_usb_clkrst_ctrl1.usb_otg20_phyref_clk_en = clk_en;
}

// HP_SYS_CLKRST.soc_clk_ctrlx and LP_AON_CLKRST.hp_usb_clkrst_ctrlx are shared registers, so this function must be used in an atomic way
#define usb_utmi_ll_enable_bus_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _usb_utmi_ll_enable_bus_clock(__VA_ARGS__); \
    } while(0)

/**
 * Get the enable status of the USB UTMI PHY bus clock
 *
 * @return Return true if USB UTMI PHY bus clock is enabled
 */
FORCE_INLINE_ATTR bool _usb_utmi_ll_bus_clock_is_enabled(void)
{
    return (HP_SYS_CLKRST.soc_clk_ctrl1.reg_usb_otg20_sys_clk_en && LP_AON_CLKRST.hp_usb_clkrst_ctrl1.usb_otg20_phyref_clk_en);
}

/**
 * @brief Reset the USB UTMI PHY and USB_DWC_HS controller
 */
FORCE_INLINE_ATTR void _usb_utmi_ll_reset_register(void)
{
    // Reset the USB_UTMI and USB_DWC_HS
    LP_AON_CLKRST.hp_usb_clkrst_ctrl1.rst_en_usb_otg20 = 1;
    LP_AON_CLKRST.hp_usb_clkrst_ctrl1.rst_en_usb_otg20_phy = 1;
    LP_AON_CLKRST.hp_usb_clkrst_ctrl1.rst_en_usb_otg20_phy = 0;
    LP_AON_CLKRST.hp_usb_clkrst_ctrl1.rst_en_usb_otg20 = 0;
}

// P_AON_CLKRST.hp_usb_clkrst_ctrlx is shared register, so this function must be used in an atomic way
#define usb_utmi_ll_reset_register(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _usb_utmi_ll_reset_register(__VA_ARGS__); \
    } while(0)

/**
 * @brief Enable/disable 15k pulldown resistors on D+/D- lines
 *
 * In USB Host mode, 15k pulldown resistors must be connected on both D+ and D-.
 * In USB Device mode, pulldown resistors must be disconnected.
 *
 * @note On ESP32-P4 v3+, pulldowns are no longer controlled by USB-OTG peripheral
 *       and must be controlled by software via LP_SYS registers.
 *       On earlier revisions, pulldowns are controlled by USB-OTG hardware.
 *
 * @param[in] enable true to connect pulldowns (Host mode), false to disconnect (Device mode)
 */
FORCE_INLINE_ATTR void usb_utmi_ll_enable_data_pulldowns(bool enable)
{
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
    LP_SYS.hp_usb_otghs_phy_ctrl.hp_utmiotg_dppulldown = enable;
    LP_SYS.hp_usb_otghs_phy_ctrl.hp_utmiotg_dmpulldown = enable;
#else
    // On pre-v3 ESP32-P4, pulldowns are controlled by the USB-OTG peripheral
    (void)enable;
#endif
}

/**
 * @brief Enable precise detection of VBUS
 *
 * @param[in] enable Enable/Disable precise detection
 */
FORCE_INLINE_ATTR void usb_utmi_ll_enable_precise_detection(bool enable)
{
    // Enable VBUS precise detection
    HP_SYSTEM.sys_usbotg20_ctrl.sys_otg_suspendm = enable;
}

#ifdef __cplusplus
}
#endif
