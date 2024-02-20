/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_attr.h"
#include "soc/soc.h"
#include "soc/system_reg.h"
#include "soc/usb_wrap_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configures the internal PHY for USB_OTG
 *
 * @param hw Start address of the USB Wrap registers
 */
static inline void usb_fsls_phy_ll_int_otg_enable(usb_wrap_dev_t *hw)
{
    hw->otg_conf.phy_sel = 0;
}

/**
 * @brief Configures the external PHY for USB_OTG
 *
 * @param hw Start address of the USB Wrap registers
 */
static inline void usb_fsls_phy_ll_ext_otg_enable(usb_wrap_dev_t *hw)
{
    //Enable external PHY
    hw->otg_conf.phy_sel = 1;
}

/**
 * @brief Configures port loads for the internal PHY
 *
 * @param hw Start address of the USB Wrap registers
 * @param dp_pu D+ pullup load
 * @param dp_pd D+ pulldown load
 * @param dm_pu D- pullup load
 * @param dm_pd D- pulldown load
 */
static inline void usb_fsls_phy_ll_int_load_conf(usb_wrap_dev_t *hw, bool dp_pu, bool dp_pd, bool dm_pu, bool dm_pd)
{
    usb_wrap_otg_conf_reg_t conf;
    conf.val = hw->otg_conf.val;
    conf.pad_pull_override = 1;
    conf.dp_pullup = dp_pu;
    conf.dp_pulldown = dp_pd;
    conf.dm_pullup = dm_pu;
    conf.dm_pulldown = dm_pd;
    hw->otg_conf.val = conf.val;
}

/**
 * @brief Enable the internal PHY control to D+/D- pad
 * @param hw     Start address of the USB Wrap registers
 * @param pad_en Enable the PHY control to D+/D- pad
 */
static inline void usb_fsls_phy_ll_usb_wrap_pad_enable(usb_wrap_dev_t *hw, bool pad_en)
{
    hw->otg_conf.pad_enable = pad_en;
}

/**
 * @brief Enable the internal PHY's test mode
 *
 * @param hw Start address of the USB Wrap registers
 * @param en Whether to enable the internal PHY's test mode
 */
static inline void usb_fsls_phy_ll_int_enable_test_mode(usb_wrap_dev_t *hw, bool en)
{
    if (en) {
        // Clear USB_WRAP_TEST_CONF_REG
        hw->test_conf.val = 0;
        // Set USB test pad oen
        hw->test_conf.test_usb_wrap_oe = 1;
        // Enable USB test mode
        hw->test_conf.test_enable = 1;
    } else {
        hw->test_conf.test_enable = 0;
    }
}

/**
 * Enable the bus clock for USB Wrap module
 * @param clk_en True if enable the clock of USB Wrap module
 */
FORCE_INLINE_ATTR void usb_fsls_phy_ll_usb_wrap_enable_bus_clock(bool clk_en)
{
    REG_SET_FIELD(DPORT_PERIP_CLK_EN0_REG, DPORT_USB_CLK_EN, clk_en);
}

// SYSTEM.perip_clk_enx are shared registers, so this function must be used in an atomic way
#define usb_fsls_phy_ll_usb_wrap_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; usb_fsls_phy_ll_usb_wrap_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the USB Wrap module
 */
FORCE_INLINE_ATTR void usb_fsls_phy_ll_usb_wrap_reset_register(void)
{
    REG_SET_FIELD(DPORT_PERIP_RST_EN0_REG, DPORT_USB_RST, 1);
    REG_SET_FIELD(DPORT_PERIP_RST_EN0_REG, DPORT_USB_RST, 0);
}

// SYSTEM.perip_clk_enx are shared registers, so this function must be used in an atomic way
#define usb_fsls_phy_ll_usb_wrap_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; usb_fsls_phy_ll_usb_wrap_reset_register(__VA_ARGS__)

#ifdef __cplusplus
}
#endif
