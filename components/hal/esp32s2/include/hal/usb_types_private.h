// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*
Note: This header file contains USB2.0 related types and macros that can be used by code specific to the DWC_OTG
controller (i.e., the HW specific layers of the USB host stack). Thus, this header is only meant to be used below (and
including) the HAL layer. For types and macros that are HW implementation agnostic (i.e., HCD layer and above), add them
to the "usb.h" header instead.
*/

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief USB speeds supported by the DWC OTG controller
 */
typedef enum {
    USB_PRIV_SPEED_FULL,
    USB_PRIV_SPEED_LOW,
} usb_priv_speed_t;

/**
 * @brief USB transfer types supported by the DWC OTG controller
 */
typedef enum {
    USB_PRIV_XFER_TYPE_CTRL,
    USB_PRIV_XFER_TYPE_ISOCHRONOUS,
    USB_PRIV_XFER_TYPE_BULK,
    USB_PRIV_XFER_TYPE_INTR,
} usb_priv_xfer_type_t;

#ifdef __cplusplus
}
#endif
