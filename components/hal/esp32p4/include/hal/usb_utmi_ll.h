/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_attr.h"
#include "soc/lp_clkrst_struct.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/usb_utmi_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------- RCC Functions  ----------------------------- */

/**
 * @brief Enable the bus clock for the USB UTMI PHY and USB_DWC_HS controller
 *
 * @param clk_en True to enable, false to disable
 */
FORCE_INLINE_ATTR void usb_utmi_ll_enable_bus_clock(bool clk_en)
{
    // Enable/disable system clock for USB_UTMI and USB_DWC_HS
    HP_SYS_CLKRST.soc_clk_ctrl1.reg_usb_otg20_sys_clk_en = clk_en;
    // Enable PHY ref clock (48MHz) for USB UTMI PHY
    LP_AON_CLKRST.hp_usb_clkrst_ctrl1.usb_otg20_phyref_clk_en = clk_en;
}

// HP_SYS_CLKRST.soc_clk_ctrlx and LP_AON_CLKRST.hp_usb_clkrst_ctrlx are shared registers, so this function must be used in an atomic way
#define usb_utmi_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; usb_utmi_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the USB UTMI PHY and USB_DWC_HS controller
 */
FORCE_INLINE_ATTR void usb_utmi_ll_reset_register(void)
{
    // Reset the USB_UTMI and USB_DWC_HS
    LP_AON_CLKRST.hp_usb_clkrst_ctrl1.rst_en_usb_otg20 = 1;
    LP_AON_CLKRST.hp_usb_clkrst_ctrl1.rst_en_usb_otg20 = 0;
}

// P_AON_CLKRST.hp_usb_clkrst_ctrlx are shared registers, so this function must be used in an atomic way
#define usb_utmi_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; usb_utmi_ll_reset_register(__VA_ARGS__)

#ifdef __cplusplus
}
#endif
