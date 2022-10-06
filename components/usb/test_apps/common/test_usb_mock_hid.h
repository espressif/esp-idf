/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
This header contains bare-bone mock implementations of some device classes in order to test various layers of the USB
Host stack.
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_assert.h"
#include "usb/usb_types_ch9.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------- HID Mouse ------------------------------------------------------


/*
Note: The mock HID mouse tests require that USB low speed mouse be connected. The mouse should...

- Be implement the HID with standard report format used by mice
- It's configuration 1 should have the following endpoint

    ------------------ Configuration Descriptor -------------------
bLength                  : 0x09 (9 bytes)
bDescriptorType          : 0x02 (Configuration Descriptor)
wTotalLength             : 0x003B (59 bytes)
bNumInterfaces           : 0x02 (2 Interfaces)
bConfigurationValue      : 0x01 (Configuration 1)
iConfiguration           : 0x00 (No String Descriptor)
bmAttributes             : 0xA0
 D7: Reserved, set 1     : 0x01
 D6: Self Powered        : 0x00 (no)
 D5: Remote Wakeup       : 0x01 (yes)
 D4..0: Reserved, set 0  : 0x00
MaxPower                 : 0x32 (100 mA)
Data (HexDump)           : 09 02 3B 00 02 01 00 A0 32 09 04 00 00 01 03 01
                           02 00 09 21 00 02 00 01 22 4D 00 07 05 81 03 08
                           00 0A 09 04 01 00 01 03 01 01 00 09 21 00 02 00
                           01 22 31 00 07 05 82 03 08 00 0A

        ---------------- Interface Descriptor -----------------
bLength                  : 0x09 (9 bytes)
bDescriptorType          : 0x04 (Interface Descriptor)
bInterfaceNumber         : 0x00
bAlternateSetting        : 0x00
bNumEndpoints            : 0x01 (1 Endpoint)
bInterfaceClass          : 0x03 (HID - Human Interface Device)
bInterfaceSubClass       : 0x01 (Boot Interface)
bInterfaceProtocol       : 0x02 (Mouse)
iInterface               : 0x00 (No String Descriptor)
Data (HexDump)           : 09 04 00 00 01 03 01 02 00

        ------------------- HID Descriptor --------------------
bLength                  : 0x09 (9 bytes)
bDescriptorType          : 0x21 (HID Descriptor)
bcdHID                   : 0x0200 (HID Version 2.00)
bCountryCode             : 0x00 (00 = not localized)
bNumDescriptors          : 0x01
Data (HexDump)           : 09 21 00 02 00 01 22 4D 00
Descriptor 1:
bDescriptorType          : 0x22 (Class=Report)
wDescriptorLength        : 0x004D (77 bytes)
Error reading descriptor : ERROR_INVALID_PARAMETER (due to a obscure limitation of the Win32 USB API, see UsbTreeView.txt)

        ----------------- Endpoint Descriptor -----------------
bLength                  : 0x07 (7 bytes)
bDescriptorType          : 0x05 (Endpoint Descriptor)
bEndpointAddress         : 0x81 (Direction=IN EndpointID=1)
bmAttributes             : 0x03 (TransferType=Interrupt)
wMaxPacketSize           : 0x0008
bInterval                : 0x0A (10 ms)
Data (HexDump)           : 07 05 81 03 08 00 0A

If you're using another mice with different endpoints, modify the endpoint descriptor below
*/

extern  const usb_ep_desc_t mock_hid_mouse_in_ep_desc;

#define MOCK_HID_MOUSE_DEV_ID_VENDOR        0x03F0
#define MOCK_HID_MOUSE_DEV_ID_PRODUCT       0x1198
#define MOCK_HID_MOUSE_DEV_DFLT_EP_MPS      8
#define MOCK_HID_MOUSE_INTF_NUMBER          0
#define MOCK_HID_MOUSE_INTF_ALT_SETTING     0
#define MOCK_HID_MOUSE_INTR_IN_EP_ADDR      0x81
#define MOCK_HID_MOUSE_INTR_IN_MPS          8

typedef union {
    struct {
        uint32_t left_button: 1;
        uint32_t right_button: 1;
        uint32_t middle_button: 1;
        uint32_t reserved5: 5;
        uint8_t x_movement;
        uint8_t y_movement;
    } __attribute__((packed));
    uint8_t val[3];
} mock_hid_mouse_report_t;
ESP_STATIC_ASSERT(sizeof(mock_hid_mouse_report_t) == 3, "Size of HID mouse report incorrect");

void mock_hid_process_report(mock_hid_mouse_report_t *report, int iter);

#ifdef __cplusplus
}
#endif
