/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_attr.h"
#include "soc/soc.h"
#include "soc/cnnt_sys_struct.h"
#include "soc/hp_alive_sys_struct.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/reg_base.h"
#include "soc/usb_utmi_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------- USB PHY Control  ---------------------------- */

#define USB_UTMI_LL_CNNT_SYS_REG ((volatile cnnt_dev_t *)DR_REG_CNNT_SYS_REG_BASE)

/**
 * @brief Configure Low-Speed mode
 *
 * @param[in] hw       Beginning address of the peripheral registers
 * @param[in] parallel Parallel or serial LS mode
 */
FORCE_INLINE_ATTR void usb_utmi_ll_configure_ls(usb_utmi_dev_t *hw, bool parallel)
{
    hw->fc_06.ls_par_en = parallel;
    hw->fc_06.ls_kpalv_en = 1;
}

/* ----------------------------- RCC Functions  ----------------------------- */

/**
 * @brief Enable/disable bus clock for USB UTMI PHY and USB OTG HS controller
 *
 * @param[in] clk_en True to enable, false to disable
 */
FORCE_INLINE_ATTR void _usb_utmi_ll_enable_bus_clock(bool clk_en)
{
    cnnt_sys_usb_otg20_ctrl_reg_t sys_usb_otg20_ctrl;

    HP_SYS_CLKRST.usb_otghs_ctrl0.reg_usb_otghs_apb_clk_en = clk_en;
    HP_SYS_CLKRST.usb_otghs_ctrl0.reg_usb_otghs_sys_clk_en = clk_en;

    sys_usb_otg20_ctrl.val = USB_UTMI_LL_CNNT_SYS_REG->sys_usb_otg20_ctrl.val;
    sys_usb_otg20_ctrl.sys_usb_otg20_utmifs_clk_en = clk_en;
    sys_usb_otg20_ctrl.sys_usb_otg20_phyref_clk_en = clk_en;
    USB_UTMI_LL_CNNT_SYS_REG->sys_usb_otg20_ctrl.val = sys_usb_otg20_ctrl.val;

    // Enable/disable PHY PLL (must be force-enabled on ESP32-S31, as the default is off)
    HP_ALIVE_SYS.usb_otghs_ctrl.reg_usb_otghs_phy_pll_force_en = clk_en;
    HP_ALIVE_SYS.usb_otghs_ctrl.reg_usb_otghs_phy_pll_en = clk_en;
}

// HP_SYS_CLKRST.usb_otghs_ctrl0 and sys_usb_otg20_ctrl only contain USB OTG fields, no atomic wrapper needed
#define usb_utmi_ll_enable_bus_clock(...)  _usb_utmi_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Get USB UTMI bus clock status
 *
 * @return true if enabled, false otherwise
 */
FORCE_INLINE_ATTR bool _usb_utmi_ll_bus_clock_is_enabled(void)
{
    return HP_SYS_CLKRST.usb_otghs_ctrl0.reg_usb_otghs_apb_clk_en &&
           HP_SYS_CLKRST.usb_otghs_ctrl0.reg_usb_otghs_sys_clk_en &&
           USB_UTMI_LL_CNNT_SYS_REG->sys_usb_otg20_ctrl.sys_usb_otg20_utmifs_clk_en &&
           USB_UTMI_LL_CNNT_SYS_REG->sys_usb_otg20_ctrl.sys_usb_otg20_phyref_clk_en;
}

/**
 * @brief Reset USB UTMI PHY and USB OTG HS controller
 *
 * @note PHY reset must be de-asserted before controller reset,
 *       so that the PHY is stable when the controller starts.
 */
FORCE_INLINE_ATTR void _usb_utmi_ll_reset_register(void)
{
    cnnt_sys_usb_otg20_ctrl_reg_t sys_usb_otg20_ctrl;

    sys_usb_otg20_ctrl.val = USB_UTMI_LL_CNNT_SYS_REG->sys_usb_otg20_ctrl.val;

    // Assert all resets
    sys_usb_otg20_ctrl.sys_usb_otg20_ahb_rst_en = 1;
    sys_usb_otg20_ctrl.sys_usb_otg20_apb_rst_en = 1;
    sys_usb_otg20_ctrl.sys_usb_otg20_phy_rst_en = 1;
    USB_UTMI_LL_CNNT_SYS_REG->sys_usb_otg20_ctrl.val = sys_usb_otg20_ctrl.val;

    // De-assert PHY reset first
    sys_usb_otg20_ctrl.sys_usb_otg20_phy_rst_en = 0;
    USB_UTMI_LL_CNNT_SYS_REG->sys_usb_otg20_ctrl.val = sys_usb_otg20_ctrl.val;

    // De-assert controller resets
    sys_usb_otg20_ctrl.sys_usb_otg20_ahb_rst_en = 0;
    sys_usb_otg20_ctrl.sys_usb_otg20_apb_rst_en = 0;
    USB_UTMI_LL_CNNT_SYS_REG->sys_usb_otg20_ctrl.val = sys_usb_otg20_ctrl.val;
}

// sys_usb_otg20_ctrl only contains USB OTG fields, no atomic wrapper needed
#define usb_utmi_ll_reset_register(...)  _usb_utmi_ll_reset_register(__VA_ARGS__)

/**
 * @brief Enable/disable 15k pulldown resistors on D+/D- lines
 *
 * In USB Host mode, 15k pulldown resistors must be connected on both D+ and D-.
 * In USB Device mode, pulldown resistors must be disconnected.
 *
 * @param[in] enable true to connect pulldowns (Host mode), false to disconnect (Device mode)
 */
FORCE_INLINE_ATTR void usb_utmi_ll_enable_data_pulldowns(bool enable)
{
    HP_ALIVE_SYS.usb_ctrl.usb_otghs_phy_dppulldown = enable;
    HP_ALIVE_SYS.usb_ctrl.usb_otghs_phy_dmpulldown = enable;
}

/**
 * @brief Enable precise VBUS/disconnection detection path
 *
 * @param[in] enable Enable/disable precise detection
 */
FORCE_INLINE_ATTR void usb_utmi_ll_enable_precise_detection(bool enable)
{
    HP_ALIVE_SYS.usb_otghs_ctrl.reg_usb_otghs_phy_otg_suspendm = enable;
    HP_ALIVE_SYS.usb_otghs_ctrl.reg_usb_otghs_phy_suspendm = enable;
}

#ifdef __cplusplus
}
#endif
