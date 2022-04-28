/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "usb_types_private.h"
#include "usb_phy_types.h"
#include "soc/soc_caps.h"
#include "soc/usb_wrap_struct.h"
#if SOC_USB_SERIAL_JTAG_SUPPORTED
#include "soc/usb_serial_jtag_struct.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Context that should be maintained by both the driver and the HAL
 */
typedef struct {
    usb_wrap_dev_t *wrap_dev;                   /**< Pointer to base address of USB Wrapper registers */
#if SOC_USB_SERIAL_JTAG_SUPPORTED
    usb_serial_jtag_dev_t *jtag_dev;            /**< Pointer to base address of USB Serial JTAG registers */
#endif
} usb_phy_hal_context_t;

/**
 * @brief Init the USB PHY hal. This function should be called first before other hal layer function is called
 *
 * @param hal Context of the HAL layer
 */
void usb_phy_hal_init(usb_phy_hal_context_t *hal);

/**
 * @brief Configure internal/external PHY for USB_OTG
 *
 * @param hal Context of the HAL layer
 * @param phy_target USB PHY target
 */
void usb_phy_hal_otg_conf(usb_phy_hal_context_t *hal, usb_phy_target_t phy_target);

#if SOC_USB_SERIAL_JTAG_SUPPORTED
/**
 * @brief Configure internal/external PHY for USB_Serial_JTAG
 *
 * @param hal Context of the HAL layer
 * @param phy_target USB PHY target
 */
void usb_phy_hal_jtag_conf(usb_phy_hal_context_t *hal, usb_phy_target_t phy_target);
#endif

/**
 * @brief Configure pullup/pulldown loads for the D+/D- as a host
 *
 * @param hal Context of the HAL layer
 */
void usb_phy_hal_int_load_conf_host(usb_phy_hal_context_t *hal);

/**
 * @brief Configure pullup/pulldown loads for the D+/D- as a device
 *
 * @param hal Context of the HAL layer
 * @param speed USB speed
 */
void usb_phy_hal_int_load_conf_dev(usb_phy_hal_context_t *hal, usb_priv_speed_t speed);

/**
 * @brief Enable/Disable test mode for internal PHY to mimick host-device disconnection
 *
 * @param hal Context of the HAL layer
 * @param disconn Whether to disconnect
 */
void usb_phy_hal_int_mimick_disconn(usb_phy_hal_context_t *hal, bool disconn);

#ifdef __cplusplus
}
#endif
