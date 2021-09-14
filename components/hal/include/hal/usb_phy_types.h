/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
Note: This header file contains USB2.0 related types and macros that can be used by code specific to the DWC_OTG
controller (i.e., the HW specific layers of the USB host stack). Thus, this header is only meant to be used below (and
including) the HAL layer. For types and macros that are HW implementation agnostic (i.e., HCD layer and above), add them
to the "usb/usb_types_ch9.h" header instead.
*/

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief USB PHY target
 */
typedef enum {
    USB_PHY_TARGET_INT,              /**< USB target is internal PHY */
    USB_PHY_TARGET_EXT,              /**< USB target is external PHY */
    USB_PHY_TARGET_MAX,
} usb_phy_target_t;

/**
 * @brief USB PHY source
 */
typedef enum {
    USB_PHY_CTRL_OTG,                /**< PHY controller is USB OTG */
#if SOC_USB_SERIAL_JTAG_SUPPORTED
    USB_PHY_CTRL_SERIAL_JTAG,        /**< PHY controller is USB Serial JTAG */
#endif
    USB_PHY_CTRL_MAX,
} usb_phy_controller_t;

/**
 * @brief USB OTG mode
 */
typedef enum {
    USB_PHY_MODE_DEFAULT,            /**< USB OTG default mode */
    USB_OTG_MODE_HOST,               /**< USB OTG host mode */
    USB_OTG_MODE_DEVICE,             /**< USB OTG device mode */
    USB_OTG_MODE_MAX,
} usb_otg_mode_t;

/**
 * @brief USB speed
 */
typedef enum {
    USB_PHY_SPEED_UNDEFINED,
    USB_PHY_SPEED_LOW,              /**< USB Low Speed (1.5 Mbit/s)  */
    USB_PHY_SPEED_FULL,             /**< USB Full Speed (12 Mbit/s)  */
    USB_PHY_SPEED_MAX,
} usb_phy_speed_t;

#ifdef __cplusplus
}
#endif
