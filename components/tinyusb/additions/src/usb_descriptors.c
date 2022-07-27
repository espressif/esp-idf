/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "usb_descriptors.h"
#include "sdkconfig.h"

/*
 * A combination of interfaces must have a unique product id, since PC will save device driver after the first plug.
 * Same VID/PID with different interface e.g MSC (first), then CDC (later) will possibly cause system error on PC.
 *
 * Auto ProductID layout's Bitmap:
 *   [MSB]         HID | MSC | CDC          [LSB]
 */
#define USB_TUSB_PID (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | \
    _PID_MAP(MIDI, 3) ) //| _PID_MAP(AUDIO, 4) | _PID_MAP(VENDOR, 5) )

/**** TinyUSB default ****/
tusb_desc_device_t descriptor_tinyusb = {
    .bLength = sizeof(descriptor_tinyusb),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,

#if CFG_TUD_CDC
    // Use Interface Association Descriptor (IAD) for CDC
    // As required by USB Specs IAD's subclass must be common class (2) and protocol must be IAD (1)
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
#else
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
#endif

    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor = 0xCafe,
    .idProduct = USB_TUSB_PID,
    .bcdDevice = 0x0100,

    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,

    .bNumConfigurations = 0x01
};

tusb_desc_strarray_device_t descriptor_str_tinyusb = {
    // array of pointer to string descriptors
    (char[]){0x09, 0x04}, // 0: is supported language is English (0x0409)
    "TinyUSB",            // 1: Manufacturer
    "TinyUSB Device",     // 2: Product
    "123456",             // 3: Serials, should use chip ID
    "TinyUSB CDC",        // 4: CDC Interface
    "TinyUSB MSC",        // 5: MSC Interface
    "TinyUSB MIDI"        // 6: MIDI
};
/* End of TinyUSB default */

/**** Kconfig driven Descriptor ****/
const tusb_desc_device_t descriptor_dev_kconfig = {
    .bLength = sizeof(descriptor_dev_kconfig),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,

#if CFG_TUD_CDC
    // Use Interface Association Descriptor (IAD) for CDC
    // As required by USB Specs IAD's subclass must be common class (2) and protocol must be IAD (1)
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
#else
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
#endif

    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

#if CONFIG_TINYUSB_DESC_USE_ESPRESSIF_VID
    .idVendor = USB_ESPRESSIF_VID,
#else
    .idVendor = CONFIG_TINYUSB_DESC_CUSTOM_VID,
#endif

#if CONFIG_TINYUSB_DESC_USE_DEFAULT_PID
    .idProduct = USB_TUSB_PID,
#else
    .idProduct = CONFIG_TINYUSB_DESC_CUSTOM_PID,
#endif

    .bcdDevice = CONFIG_TINYUSB_DESC_BCD_DEVICE,

    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,

    .bNumConfigurations = 0x01
};

tusb_desc_strarray_device_t descriptor_str_kconfig = {
    // array of pointer to string descriptors
    (char[]){0x09, 0x04},                // 0: is supported language is English (0x0409)
    CONFIG_TINYUSB_DESC_MANUFACTURER_STRING, // 1: Manufacturer
    CONFIG_TINYUSB_DESC_PRODUCT_STRING,      // 2: Product
    CONFIG_TINYUSB_DESC_SERIAL_STRING,       // 3: Serials, should use chip ID

#if CONFIG_TINYUSB_CDC_ENABLED
    CONFIG_TINYUSB_DESC_CDC_STRING,          // 4: CDC Interface
#else
    "",
#endif

#if CONFIG_TINYUSB_MSC_ENABLED
    CONFIG_TINYUSB_DESC_MSC_STRING,          // 5: MSC Interface
#else
    "",
#endif

#if CONFIG_TINYUSB_MIDI_ENABLED
    CONFIG_TINYUSB_DESC_MIDI_STRING           // 6: MIDI
#else
    "",
#endif

};

//------------- Configuration Descriptor -------------//
enum {
#if CFG_TUD_CDC
    ITF_NUM_CDC = 0,
    ITF_NUM_CDC_DATA,
#endif

#if CFG_TUD_CDC > 1
    ITF_NUM_CDC1,
    ITF_NUM_CDC1_DATA,
#endif

#if CFG_TUD_MSC
    ITF_NUM_MSC,
#endif

#if CFG_TUD_MIDI
    ITF_NUM_MIDI,
    ITF_NUM_MIDI_STREAMING,
#endif

    ITF_NUM_TOTAL
};

enum {
    TUSB_DESC_TOTAL_LEN = TUD_CONFIG_DESC_LEN +
                        CFG_TUD_CDC * TUD_CDC_DESC_LEN +
                        CFG_TUD_MSC * TUD_MSC_DESC_LEN +
                        CFG_TUD_MIDI * TUD_MIDI_DESC_LEN
};

//------------- USB Endpoint numbers -------------//
enum {
    // Available USB Endpoints: 5 IN/OUT EPs and 1 IN EP
    EP_EMPTY = 0,
#if CFG_TUD_CDC
    EPNUM_0_CDC_NOTIF,
    EPNUM_0_CDC,
#endif

#if CFG_TUD_CDC > 1
    EPNUM_1_CDC_NOTIF,
    EPNUM_1_CDC,
#endif

#if CFG_TUD_MSC
    EPNUM_MSC,
#endif

#if CFG_TUD_MIDI
    EPNUM_MIDI,
#endif
};

uint8_t const descriptor_cfg_kconfig[] = {
    // Configuration number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

#if CFG_TUD_CDC
    // Interface number, string index, EP notification address and size, EP data address (out, in) and size.
    TUD_CDC_DESCRIPTOR(ITF_NUM_CDC, 4, 0x80 | EPNUM_0_CDC_NOTIF, 8, EPNUM_0_CDC, 0x80 | EPNUM_0_CDC, CFG_TUD_CDC_EP_BUFSIZE),
#endif

#if CFG_TUD_CDC > 1
    // Interface number, string index, EP notification address and size, EP data address (out, in) and size.
    TUD_CDC_DESCRIPTOR(ITF_NUM_CDC1, 4, 0x80 | EPNUM_1_CDC_NOTIF, 8, EPNUM_1_CDC, 0x80 | EPNUM_1_CDC, CFG_TUD_CDC_EP_BUFSIZE),
#endif

#if CFG_TUD_MSC
    // Interface number, string index, EP Out & EP In address, EP size
    TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 5, EPNUM_MSC, 0x80 | EPNUM_MSC, 64), // highspeed 512
#endif

#if CFG_TUD_MIDI
    // Interface number, string index, EP Out & EP In address, EP size
    TUD_MIDI_DESCRIPTOR(ITF_NUM_MIDI, 6, EPNUM_MIDI, 0x80 | EPNUM_MIDI, 64) // highspeed 512
#endif
};

/* End of Kconfig driven Descriptor */
