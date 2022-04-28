/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define USB_ESPRESSIF_VID 0x303A
#define USB_STRING_DESCRIPTOR_ARRAY_SIZE 7

typedef enum{
    TINYUSB_USBDEV_0,
} tinyusb_usbdev_t;

typedef const char *tusb_desc_strarray_device_t[USB_STRING_DESCRIPTOR_ARRAY_SIZE];

#ifdef __cplusplus
}
#endif
