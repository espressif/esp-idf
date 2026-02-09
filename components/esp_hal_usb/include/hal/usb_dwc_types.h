/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
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
 * @brief USB speeds supported by the DWC OTG controller
 *
 * @note usb_dwc_speed_t enum values must match the values of the DWC_OTG prtspd register field
 */
typedef enum {
    USB_DWC_SPEED_HIGH = 0,
    USB_DWC_SPEED_FULL = 1,
    USB_DWC_SPEED_LOW = 2,
} usb_dwc_speed_t;

/**
 * @brief USB transfer types supported by the DWC OTG controller
 *
 * @note usb_dwc_xfer_type_t enum values must match the values of the DWC_OTG hcchar register field
 */
typedef enum {
    USB_DWC_XFER_TYPE_CTRL = 0,
    USB_DWC_XFER_TYPE_ISOCHRONOUS = 1,
    USB_DWC_XFER_TYPE_BULK = 2,
    USB_DWC_XFER_TYPE_INTR = 3,
} usb_dwc_xfer_type_t;

/**
 * @brief Enumeration of different possible lengths of the periodic frame list
 */
typedef enum {
    USB_HAL_FRAME_LIST_LEN_8 = 8,
    USB_HAL_FRAME_LIST_LEN_16 = 16,
    USB_HAL_FRAME_LIST_LEN_32 = 32,
    USB_HAL_FRAME_LIST_LEN_64 = 64,
} usb_hal_frame_list_len_t;

#ifdef __cplusplus
}
#endif
