/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_attr.h"
#include "soc/lp_system_struct.h"
#include "soc/lp_clkrst_struct.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/usb_serial_jtag_struct.h"
#include "hal/usb_serial_jtag_types.h"

// This header is temporarily disabled until USJ is supported on the P4 (IDF-7496)
#if SOC_USB_SERIAL_JTAG_SUPPORTED

/* ----------------------------- Macros & Types ----------------------------- */

#define USB_SERIAL_JTAG_LL_SELECT_PHY_SUPPORTED     1   // Can route to an external FSLS PHY

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------- USB PHY Control  ---------------------------- */

/**
 * @brief Sets PHY defaults
 *
 * Some PHY register fields/features of the USJ are redundant on the ESP32-P4.
 * This function those fields are set to the appropriate default values.
 */
FORCE_INLINE_ATTR void usb_serial_jtag_ll_phy_set_defaults(void)
{
    // External FSLS PHY is not supported
    USB_SERIAL_JTAG.conf0.phy_sel = 0;
    USB_SERIAL_JTAG.conf0.usb_pad_enable = 1;
}

/**
 * @brief Select the internal USB FSLS PHY for the USJ
 *
 * @param phy_idx Selected PHY's index
 */
FORCE_INLINE_ATTR void usb_serial_jtag_ll_phy_select(unsigned int phy_idx)
{
    // Enable SW control mapping USB_WRAP and USJ to USB FSLS PHY 0 and 1
    LP_SYS.usb_ctrl.sw_hw_usb_phy_sel = 1;
    /*
    For 'sw_usb_phy_sel':
    False - USJ mapped to USB FSLS PHY 0, USB_WRAP mapped to USB FSLS PHY 1
    True  - USJ mapped to USB FSLS PHY 1, USB_WRAP mapped to USB FSLS PHY 0
    */
    switch (phy_idx) {
    case 0:
        LP_SYS.usb_ctrl.sw_usb_phy_sel = false;
    case 1:
        LP_SYS.usb_ctrl.sw_usb_phy_sel = true;
    default:
        break;
    }
}

/**
 * @brief Enables/disables exchanging of the D+/D- pins USB PHY
 *
 * @param enable Enables pin exchange, disabled otherwise
 */
FORCE_INLINE_ATTR void usb_serial_jtag_ll_phy_enable_pin_exchg(bool enable)
{
    if (enable) {
        USB_SERIAL_JTAG.conf0.exchg_pins = 1;
        USB_SERIAL_JTAG.conf0.exchg_pins_override = 1;
    } else {
        USB_SERIAL_JTAG.conf0.exchg_pins_override = 0;
        USB_SERIAL_JTAG.conf0.exchg_pins = 0;
    }
}

/**
 * @brief Enables and sets voltage threshold overrides for USB FSLS PHY single-ended inputs
 *
 * @param vrefh_step High voltage threshold. 0 to 3 indicating 80mV steps from 1.76V to 2V.
 * @param vrefl_step Low voltage threshold. 0 to 3 indicating 80mV steps from 0.8V to 1.04V.
 */
FORCE_INLINE_ATTR void usb_serial_jtag_ll_phy_enable_vref_override(unsigned int vrefh_step, unsigned int vrefl_step)
{
    USB_SERIAL_JTAG.conf0.vrefh = vrefh_step;
    USB_SERIAL_JTAG.conf0.vrefl = vrefl_step;
    USB_SERIAL_JTAG.conf0.vref_override = 1;
}

/**
 * @brief Disables voltage threshold overrides for USB FSLS PHY single-ended inputs
 */
FORCE_INLINE_ATTR void usb_serial_jtag_ll_phy_disable_vref_override(void)
{
    USB_SERIAL_JTAG.conf0.vref_override = 0;
}

/**
 * @brief Enable override of USB FSLS PHY's pull up/down resistors
 *
 * @param vals Override values to set
 */
FORCE_INLINE_ATTR void usb_serial_jtag_ll_phy_enable_pull_override(const usb_serial_jtag_pull_override_vals_t *vals)
{
    USB_SERIAL_JTAG.conf0.dp_pullup = vals->dp_pu;
    USB_SERIAL_JTAG.conf0.dp_pulldown = vals->dp_pd;
    USB_SERIAL_JTAG.conf0.dm_pullup = vals->dm_pu;
    USB_SERIAL_JTAG.conf0.dm_pulldown = vals->dm_pd;
    USB_SERIAL_JTAG.conf0.pad_pull_override = 1;
}

/**
 * @brief Disable override of USB FSLS PHY pull up/down resistors
 */
FORCE_INLINE_ATTR void usb_serial_jtag_ll_phy_disable_pull_override(void)
{
    USB_SERIAL_JTAG.conf0.pad_pull_override = 0;
}

/**
 * @brief Sets the strength of the pullup resistor
 *
 * @param strong True is a ~1.4K pullup, false is a ~2.4K pullup
 */
FORCE_INLINE_ATTR void usb_serial_jtag_ll_phy_set_pullup_strength(bool strong)
{
    USB_SERIAL_JTAG.conf0.pullup_value = strong;
}

/**
 * @brief Check if USB FSLS PHY pads are enabled
 *
 * @return True if enabled, false otherwise
 */
FORCE_INLINE_ATTR bool usb_serial_jtag_ll_phy_is_pad_enabled(void)
{
    return USB_SERIAL_JTAG.conf0.usb_pad_enable;
}

/**
 * @brief Enable the USB FSLS PHY pads
 *
 * @param enable Whether to enable the USB FSLS PHY pads
 */
FORCE_INLINE_ATTR void usb_serial_jtag_ll_phy_enable_pad(bool enable)
{
    USB_SERIAL_JTAG.conf0.usb_pad_enable = enable;
}

/* ----------------------------- RCC Functions  ----------------------------- */

/**
 * @brief Enable the bus clock for USJ module
 * @param clk_en True if enable the clock of USJ module
 */
FORCE_INLINE_ATTR void usb_serial_jtag_ll_enable_bus_clock(bool clk_en)
{
    HP_SYS_CLKRST.soc_clk_ctrl2.reg_usb_device_apb_clk_en = clk_en;
    // Enable PHY clock (48MHz) for USB FSLS PHY 0
    LP_AON_CLKRST.hp_usb_clkrst_ctrl0.usb_device_48m_clk_en = clk_en;
}

// HP_SYS_CLKRST.soc_clk_ctrlx and LP_AON_CLKRST.hp_usb_clkrst_ctrlx are shared registers, so this function must be used in an atomic way
#define usb_serial_jtag_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; usb_serial_jtag_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the USJ module
 */
FORCE_INLINE_ATTR void usb_serial_jtag_ll_reset_register(void)
{
    LP_AON_CLKRST.hp_usb_clkrst_ctrl1.rst_en_usb_device = 1;
    LP_AON_CLKRST.hp_usb_clkrst_ctrl1.rst_en_usb_device = 0;
}

// HP_SYS_CLKRST.soc_clk_ctrlx and LP_AON_CLKRST.hp_usb_clkrst_ctrlx are shared registers, so this function must be used in an atomic way
#define usb_serial_jtag_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; usb_serial_jtag_ll_reset_register(__VA_ARGS__)

/**
 * Get the enable status of the USJ module
 *
 * @return Return true if USJ module is enabled
 */
FORCE_INLINE_ATTR bool usb_serial_jtag_ll_module_is_enabled(void)
{
    return (HP_SYS_CLKRST.soc_clk_ctrl2.reg_usb_device_apb_clk_en && !LP_AON_CLKRST.hp_usb_clkrst_ctrl1.rst_en_usb_device);
}

// HP_SYS_CLKRST.soc_clk_ctrlx and LP_AON_CLKRST.hp_usb_clkrst_ctrlx are shared registers, so this function must be used in an atomic way
#define usb_serial_jtag_ll_module_is_enabled(...) ({    \
    (void)__DECLARE_RCC_ATOMIC_ENV;                     \
    usb_serial_jtag_ll_module_is_enabled(__VA_ARGS__);  \
})

#ifdef __cplusplus
}
#endif

#endif // #if SOC_USB_SERIAL_JTAG_SUPPORTED
