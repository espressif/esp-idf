/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_attr.h"
#include "soc/soc.h"
#include "register/soc/pcr_struct.h"
#include "register/soc/usb_wrap_struct.h"
#include "hal/usb_wrap_types.h"

/* ----------------------------- Macros & Types ----------------------------- */

#define USB_WRAP_LL_EXT_PHY_SUPPORTED           0   // Cannot route to an external FSLS PHY


#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------- USB PHY Control  ---------------------------- */

/**
 * @brief Sets default
 *
 * Some register fields and features of the USB WRAP are redundant on the ESP32-H4.
 * This function sets those fields to their appropriate default values.
 *
 * @param hw Start address of the USB Wrap registers
 */
FORCE_INLINE_ATTR void usb_wrap_ll_phy_set_defaults(usb_wrap_dev_t *hw)
{
    // Always select internal PHY for H4
    hw->wrap_otg_conf.wrap_phy_sel = 0;
    hw->wrap_otg_conf.wrap_usb_pad_enable = 1;
}

/**
 * @brief Enables and sets the override value for the session end signal
 *
 * @param hw Start address of the USB Wrap registers
 * @param sessend Session end override value. True means VBus < 0.2V, false means VBus > 0.8V
 */
FORCE_INLINE_ATTR void usb_wrap_ll_phy_enable_srp_sessend_override(usb_wrap_dev_t *hw, bool sessend)
{
    hw->wrap_otg_conf.wrap_srp_sessend_value = sessend;
    hw->wrap_otg_conf.wrap_srp_sessend_override = 1;
}

/**
 * @brief Disable session end override
 *
 * @param hw Start address of the USB Wrap registers
 */
FORCE_INLINE_ATTR void usb_wrap_ll_phy_disable_srp_sessend_override(usb_wrap_dev_t *hw)
{
    hw->wrap_otg_conf.wrap_srp_sessend_override = 0;
}

/**
 * @brief Enables/disables exchanging of the D+/D- pins USB PHY
 *
 * @param hw Start address of the USB Wrap registers
 * @param enable Enables pin exchange, disabled otherwise
 */
FORCE_INLINE_ATTR void usb_wrap_ll_phy_enable_pin_exchg(usb_wrap_dev_t *hw, bool enable)
{
    if (enable) {
    hw->wrap_otg_conf.wrap_exchg_pins = 1;
        hw->wrap_otg_conf.wrap_exchg_pins_override = 1;
    } else {
    hw->wrap_otg_conf.wrap_exchg_pins_override = 0;
    hw->wrap_otg_conf.wrap_exchg_pins = 0;
    }
}

/**
 * @brief Enables and sets voltage threshold overrides for USB FSLS PHY single-ended inputs
 *
 * @param hw Start address of the USB Wrap registers
 * @param vrefh_step High voltage threshold. 0 to 3 indicating 80mV steps from 1.76V to 2V.
 * @param vrefl_step Low voltage threshold. 0 to 3 indicating 80mV steps from 0.8V to 1.04V.
 */
FORCE_INLINE_ATTR void usb_wrap_ll_phy_enable_vref_override(usb_wrap_dev_t *hw, unsigned int vrefh_step, unsigned int vrefl_step)
{
    hw->wrap_otg_conf.wrap_vrefh = vrefh_step;
    hw->wrap_otg_conf.wrap_vrefl = vrefl_step;
    hw->wrap_otg_conf.wrap_vref_override = 1;
}

/**
 * @brief Disables voltage threshold overrides for USB FSLS PHY single-ended inputs
 *
 * @param hw Start address of the USB Wrap registers
 */
FORCE_INLINE_ATTR void usb_wrap_ll_phy_disable_vref_override(usb_wrap_dev_t *hw)
{
    hw->wrap_otg_conf.wrap_vref_override = 0;
}

/**
 * @brief Enable override of USB FSLS PHY's pull up/down resistors
 *
 * @param hw Start address of the USB Wrap registers
 * @param vals Override values to set
 */
FORCE_INLINE_ATTR void usb_wrap_ll_phy_enable_pull_override(usb_wrap_dev_t *hw, const usb_wrap_pull_override_vals_t *vals)
{
    hw->wrap_otg_conf.wrap_dp_pullup = vals->dp_pu;
    hw->wrap_otg_conf.wrap_dp_pulldown = vals->dp_pd;
    hw->wrap_otg_conf.wrap_dm_pullup = vals->dm_pu;
    hw->wrap_otg_conf.wrap_dm_pulldown = vals->dm_pd;
    hw->wrap_otg_conf.wrap_pad_pull_override = 1;
}

/**
 * @brief Disable override of USB FSLS PHY pull up/down resistors
 *
 * @param hw Start address of the USB Wrap registers
 */
FORCE_INLINE_ATTR void usb_wrap_ll_phy_disable_pull_override(usb_wrap_dev_t *hw)
{
    hw->wrap_otg_conf.wrap_pad_pull_override = 0;
}

/**
 * @brief Sets the strength of the pullup resistor
 *
 * @param hw Start address of the USB Wrap registers
 * @param strong True is a ~1.4K pullup, false is a ~2.4K pullup
 */
FORCE_INLINE_ATTR void usb_wrap_ll_phy_set_pullup_strength(usb_wrap_dev_t *hw, bool strong)
{
    hw->wrap_otg_conf.wrap_pullup_value = strong;
}

/**
 * @brief Check if USB FSLS PHY pads are enabled
 *
 * @param hw Start address of the USB Wrap registers
 * @return True if enabled, false otherwise
 */
FORCE_INLINE_ATTR bool usb_wrap_ll_phy_is_pad_enabled(usb_wrap_dev_t *hw)
{
    return hw->wrap_otg_conf.wrap_usb_pad_enable;
}

/**
 * @brief Enable the USB FSLS PHY pads
 *
 * @param hw Start address of the USB Wrap registers
 * @param enable Whether to enable the USB FSLS PHY pads
 */
FORCE_INLINE_ATTR void usb_wrap_ll_phy_enable_pad(usb_wrap_dev_t *hw, bool enable)
{
    hw->wrap_otg_conf.wrap_usb_pad_enable = enable;
}

/**
 * @brief Set USB FSLS PHY TX output clock edge
 *
 * @param hw Start address of the USB Wrap registers
 * @param clk_neg_edge True if TX output at negedge, posedge otherwise
 */
FORCE_INLINE_ATTR void usb_wrap_ll_phy_set_tx_edge(usb_wrap_dev_t *hw, bool clk_neg_edge)
{
    // Not supported on ESP32-H4: no wrap_phy_tx_edge_sel field
    (void)hw;
    (void)clk_neg_edge;
}

/* ------------------------------ USB PHY Test ------------------------------ */

/**
 * @brief Enable the USB FSLS PHY's test mode
 *
 * @param hw Start address of the USB Wrap registers
 * @param enable Whether to enable the USB FSLS PHY's test mode
 */
FORCE_INLINE_ATTR void usb_wrap_ll_phy_enable_test_mode(usb_wrap_dev_t *hw, bool enable)
{
    /* Not supported on H4: test_conf not present in usb_wrap_dev_t */
    (void)hw; (void)enable; // Corrected to indicate test_conf is not present
}

/**
 * @brief Set the USB FSLS PHY's signal test values
 *
 * @param hw Start address of the USB Wrap registers
 * @param vals Test values to set
 */
FORCE_INLINE_ATTR void usb_wrap_ll_phy_test_mode_set_signals(usb_wrap_dev_t *hw, const usb_wrap_test_mode_vals_t *vals)
{
    /* Not supported on H4: test_conf not present in usb_wrap_dev_t */
    (void)hw; (void)vals; // Corrected to indicate test_conf is not present
}

/* ----------------------------- RCC Functions  ----------------------------- */

/**
 * Enable the bus clock for USB Wrap module
 * @param clk_en True if enable the clock of USB Wrap module
 */
FORCE_INLINE_ATTR void _usb_wrap_ll_enable_bus_clock(bool clk_en)
{
    PCR.usb_device_conf.usb_device_clk_en = clk_en;
}

// SYSTEM.perip_clk_enx are shared registers, so this function must be used in an atomic way
#define usb_wrap_ll_enable_bus_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _usb_wrap_ll_enable_bus_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Reset the USB Wrap module
 */
FORCE_INLINE_ATTR void _usb_wrap_ll_reset_register(void)
{
    PCR.usb_device_conf.usb_device_rst_en = 1;
    PCR.usb_device_conf.usb_device_rst_en = 0;
}

// SYSTEM.perip_rst_enx are shared registers, so this function must be used in an atomic way
#define usb_wrap_ll_reset_register(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _usb_wrap_ll_reset_register(__VA_ARGS__); \
    } while(0)

#ifdef __cplusplus
}
#endif
