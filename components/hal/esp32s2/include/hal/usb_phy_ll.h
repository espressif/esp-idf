/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
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
static inline void usb_phy_ll_int_otg_enable(usb_wrap_dev_t *hw)
{
    //Enable internal PHY
    hw->otg_conf.pad_enable = 1;
    hw->otg_conf.phy_sel = 0;
}

/**
 * @brief Configures the external PHY for USB_OTG
 *
 * @param hw Start address of the USB Wrap registers
 */
static inline void usb_phy_ll_ext_otg_enable(usb_wrap_dev_t *hw)
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
static inline void usb_phy_ll_int_load_conf(usb_wrap_dev_t *hw, bool dp_pu, bool dp_pd, bool dm_pu, bool dm_pd)
{
    usb_wrap_otg_conf_reg_t conf = hw->otg_conf;
    conf.pad_pull_override = 1;
    conf.dp_pullup = dp_pu;
    conf.dp_pulldown = dp_pd;
    conf.dm_pullup = dm_pu;
    conf.dm_pulldown = dm_pd;
    hw->otg_conf = conf;
}

/**
 * @brief Enable the internal PHY's test mode
 *
 * @param hw Start address of the USB Wrap registers
 * @param en Whether to enable the internal PHY's test mode
 */
static inline void usb_phy_ll_int_enable_test_mode(usb_wrap_dev_t *hw, bool en)
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

#ifdef __cplusplus
}
#endif
