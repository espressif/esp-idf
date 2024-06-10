/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "unity.h"
#include "usb/usb_host.h"

/*
Tests that check the configuration descriptor parsing functions provided in usb_host.h work by parsing a fixed
configuration descriptor. The fixed configuration descriptor used in this test is provided below (both in textual and
byte format), and is of a UVC device. Thus the configuration descriptor has a good set of scenarios that can be tested
(such as multiple interfaces, alternate settings, class specific descriptors, default endpoint only interfaces etc).
*/

/*
Configuration Descriptor:
    bLength                 9
    bDescriptorType         2
    wTotalLength       0x0185
    bNumInterfaces          2
    bConfigurationValue     1
    iConfiguration          0
    bmAttributes         0x80
        (Bus Powered)
    MaxPower              500mA
    Interface Association:
        bLength                 8
        bDescriptorType        11
        bFirstInterface         0
        bInterfaceCount         2
        bFunctionClass         14 Video
        bFunctionSubClass       3 Video Interface Collection
        bFunctionProtocol       0
        iFunction               5
    Interface Descriptor:
        bLength                 9
        bDescriptorType         4
        bInterfaceNumber        0
        bAlternateSetting       0
        bNumEndpoints           1
        bInterfaceClass        14 Video
        bInterfaceSubClass      1 Video Control
        bInterfaceProtocol      0
        iInterface              5
        VideoControl Interface Descriptor:
        bLength                13
        bDescriptorType        36
        bDescriptorSubtype      1 (HEADER)
        bcdUVC               1.00
        wTotalLength       0x004f
        dwClockFrequency       15.000000MHz
        bInCollection           1
        baInterfaceNr( 0)       1
        VideoControl Interface Descriptor:
            bLength                 9
            bDescriptorType        36
            bDescriptorSubtype      3 (OUTPUT_TERMINAL)
            bTerminalID             4
            wTerminalType      0x0101 USB Streaming
            bAssocTerminal          0
            bSourceID               3
            iTerminal               0
        VideoControl Interface Descriptor:
            bLength                28
            bDescriptorType        36
            bDescriptorSubtype      6 (EXTENSION_UNIT)
            bUnitID                 3
            guidExtensionCode         {4cf18db6-abd0-495c-9876-1fa3942ff9fa}
            bNumControl            24
            bNrPins                 1
            baSourceID( 0)          2
            bControlSize            3
            bmControls( 0)       0xff
            bmControls( 1)       0xff
            bmControls( 2)       0xff
            iExtension              0
        VideoControl Interface Descriptor:
            bLength                18
            bDescriptorType        36
            bDescriptorSubtype      2 (INPUT_TERMINAL)
            bTerminalID             1
            wTerminalType      0x0201 Camera Sensor
            bAssocTerminal          0
            iTerminal               0
            wObjectiveFocalLengthMin      0
            wObjectiveFocalLengthMax      0
            wOcularFocalLength            0
            bControlSize                  3
            bmControls           0x0000000e
                Auto-Exposure Mode
                Auto-Exposure Priority
                Exposure Time (Absolute)
        VideoControl Interface Descriptor:
            bLength                11
            bDescriptorType        36
            bDescriptorSubtype      5 (PROCESSING_UNIT)
            Warning: Descriptor too short
            bUnitID                 2
            bSourceID               1
            wMaxMultiplier          0
            bControlSize            2
            bmControls     0x0000177f
                Brightness
                Contrast
                Hue
                Saturation
                Sharpness
                Gamma
                White Balance Temperature
                Backlight Compensation
                Gain
                Power Line Frequency
                White Balance Temperature, Auto
            iProcessing             0
            bmVideoStandards     0x62
                NTSC - 525/60
                PAL - 525/60
            Endpoint Descriptor:
                bLength                 7
                bDescriptorType         5
                bEndpointAddress     0x83  EP 3 IN
                bmAttributes            3
                    Transfer Type            Interrupt
                    Synch Type               None
                    Usage Type               Data
                wMaxPacketSize     0x0010  1x 16 bytes
                bInterval               6
    Interface Descriptor:
        bLength                 9
        bDescriptorType         4
        bInterfaceNumber        1
        bAlternateSetting       0
        bNumEndpoints           0
        bInterfaceClass        14 Video
        bInterfaceSubClass      2 Video Streaming
        bInterfaceProtocol      0
        iInterface              0
        VideoStreaming Interface Descriptor:
            bLength                            15
            bDescriptorType                    36
            bDescriptorSubtype                  1 (INPUT_HEADER)
            bNumFormats                         2
            wTotalLength                   0x00f7
            bEndPointAddress                  129
            bmInfo                              0
            bTerminalLink                       4
            bStillCaptureMethod                 0
            bTriggerSupport                     0
            bTriggerUsage                       0
            bControlSize                        1
            bmaControls( 0)                     0
            bmaControls( 1)                     0
        VideoStreaming Interface Descriptor:
            bLength                            27
            bDescriptorType                    36
            bDescriptorSubtype                  4 (FORMAT_UNCOMPRESSED)
            bFormatIndex                        1
            bNumFrameDescriptors                2
            guidFormat                            {85f6cc1d-0c9f-44f5-9ce0-97c7dd8c98ab}
            bBitsPerPixel                      16
            bDefaultFrameIndex                  1
            bAspectRatioX                       0
            bAspectRatioY                       0
            bmInterlaceFlags                 0x00
                Interlaced stream or variable: No
                Fields per frame: 2 fields
                Field 1 first: No
                Field pattern: Field 1 only
            bCopyProtect                        0
        VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         1
            bmCapabilities                   0x00
                Still image unsupported
            wWidth                            480
            wHeight                           320
            dwMinBitRate                 12288000
            dwMaxBitRate                 12288000
            dwMaxVideoFrameBufferSize      307200
            dwDefaultFrameInterval        2000000
            bFrameIntervalType                  1
            dwFrameInterval( 0)           2000000
        VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
            bFrameIndex                         2
            bmCapabilities                   0x00
                Still image unsupported
            wWidth                            640
            wHeight                           480
            dwMinBitRate                 73728000
            dwMaxBitRate                 73728000
            dwMaxVideoFrameBufferSize      614400
            dwDefaultFrameInterval         666666
            bFrameIntervalType                  1
            dwFrameInterval( 0)            666666
        VideoStreaming Interface Descriptor:
            bLength                            11
            bDescriptorType                    36
            bDescriptorSubtype                  6 (FORMAT_MJPEG)
            bFormatIndex                        2
            bNumFrameDescriptors                4
            bFlags                              0
                Fixed-size samples: No
            bDefaultFrameIndex                  1
            bAspectRatioX                       0
            bAspectRatioY                       0
            bmInterlaceFlags                 0x00
                Interlaced stream or variable: No
                Fields per frame: 1 fields
                Field 1 first: No
                Field pattern: Field 1 only
            bCopyProtect                        0
        VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         1
            bmCapabilities                   0x00
                Still image unsupported
            wWidth                            640
            wHeight                           480
            dwMinBitRate                 36864000
            dwMaxBitRate                 36864000
            dwMaxVideoFrameBufferSize      307789
            dwDefaultFrameInterval         666666
            bFrameIntervalType                  1
            dwFrameInterval( 0)            666666
        VideoStreaming Interface Descriptor:
            bLength                            38
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         2
            bmCapabilities                   0x00
                Still image unsupported
            wWidth                            480
            wHeight                           320
            dwMinBitRate                  6144000
            dwMaxBitRate                 18432000
            dwMaxVideoFrameBufferSize      154189
            dwDefaultFrameInterval         666666
            bFrameIntervalType                  3
            dwFrameInterval( 0)            666666
            dwFrameInterval( 1)           1000000
            dwFrameInterval( 2)           2000000
        VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         3
            bmCapabilities                   0x00
                Still image unsupported
            wWidth                            352
            wHeight                           288
            dwMinBitRate                 12165120
            dwMaxBitRate                 12165120
            dwMaxVideoFrameBufferSize      101965
            dwDefaultFrameInterval         666666
            bFrameIntervalType                  1
            dwFrameInterval( 0)            666666
        VideoStreaming Interface Descriptor:
            bLength                            30
            bDescriptorType                    36
            bDescriptorSubtype                  7 (FRAME_MJPEG)
            bFrameIndex                         4
            bmCapabilities                   0x00
                Still image unsupported
            wWidth                            320
            wHeight                           240
            dwMinBitRate                  9216000
            dwMaxBitRate                  9216000
            dwMaxVideoFrameBufferSize       77389
            dwDefaultFrameInterval         666666
            bFrameIntervalType                  1
            dwFrameInterval( 0)            666666
        VideoStreaming Interface Descriptor:
            bLength                             6
            bDescriptorType                    36
            bDescriptorSubtype                 13 (COLORFORMAT)
            bColorPrimaries                     1 (BT.709,sRGB)
            bTransferCharacteristics            1 (BT.709)
            bMatrixCoefficients                 4 (SMPTE 170M (BT.601))
    Interface Descriptor:
        bLength                 9
        bDescriptorType         4
        bInterfaceNumber        1
        bAlternateSetting       1
        bNumEndpoints           1
        bInterfaceClass        14 Video
        bInterfaceSubClass      2 Video Streaming
        bInterfaceProtocol      0
        iInterface              0
        Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x81  EP 1 IN
            bmAttributes            5
                Transfer Type            Isochronous
                Synch Type               Asynchronous
                Usage Type               Data
            wMaxPacketSize     0x03bc  1x 956 bytes
            bInterval               1
*/

static uint8_t config_desc_bytes [] = {
    0x09, 0x02, 0x85, 0x01, 0x02, 0x01, 0x00, 0x80, 0xFA, 0x08, 0x0B, 0x00, 0x02, 0x0E, 0x03, 0x00, 0x05, 0x09, 0x04,
    0x00, 0x00, 0x01, 0x0E, 0x01, 0x00, 0x05, 0x0D, 0x24, 0x01, 0x00, 0x01, 0x4F, 0x00, 0xC0, 0xE1, 0xE4, 0x00, 0x01,
    0x01, 0x09, 0x24, 0x03, 0x04, 0x01, 0x01, 0x00, 0x03, 0x00, 0x1C, 0x24, 0x06, 0x03, 0xB6, 0x8D, 0xF1, 0x4C, 0xD0,
    0xAB, 0x5C, 0x49, 0x76, 0x98, 0xFA, 0xF9, 0x2F, 0x94, 0xA3, 0x1F, 0x18, 0x01, 0x02, 0x03, 0xFF, 0xFF, 0xFF, 0x00,
    0x12, 0x24, 0x02, 0x01, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0E, 0x00, 0x00, 0x0B,
    0x24, 0x05, 0x02, 0x01, 0x00, 0x00, 0x02, 0x7F, 0x17, 0x00, 0x07, 0x05, 0x83, 0x03, 0x10, 0x00, 0x06, 0x05, 0x25,
    0x03, 0x80, 0x00, 0x09, 0x04, 0x01, 0x00, 0x00, 0x0E, 0x02, 0x00, 0x00, 0x0F, 0x24, 0x01, 0x02, 0xF7, 0x00, 0x81,
    0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x1B, 0x24, 0x04, 0x01, 0x02, 0x1D, 0xCC, 0xF6, 0x85, 0x9F, 0x0C,
    0xF5, 0x44, 0xE0, 0x9C, 0xAB, 0x98, 0x8C, 0xDD, 0xC7, 0x97, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x24, 0x05,
    0x01, 0x00, 0xE0, 0x01, 0x40, 0x01, 0x00, 0x80, 0xBB, 0x00, 0x00, 0x80, 0xBB, 0x00, 0x00, 0xB0, 0x04, 0x00, 0x80,
    0x84, 0x1E, 0x00, 0x01, 0x80, 0x84, 0x1E, 0x00, 0x1E, 0x24, 0x05, 0x02, 0x00, 0x80, 0x02, 0xE0, 0x01, 0x00, 0x00,
    0x65, 0x04, 0x00, 0x00, 0x65, 0x04, 0x00, 0x60, 0x09, 0x00, 0x2A, 0x2C, 0x0A, 0x00, 0x01, 0x2A, 0x2C, 0x0A, 0x00,
    0x0B, 0x24, 0x06, 0x02, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x24, 0x07, 0x01, 0x00, 0x80, 0x02, 0xE0,
    0x01, 0x00, 0x80, 0x32, 0x02, 0x00, 0x80, 0x32, 0x02, 0x4D, 0xB2, 0x04, 0x00, 0x2A, 0x2C, 0x0A, 0x00, 0x01, 0x2A,
    0x2C, 0x0A, 0x00, 0x26, 0x24, 0x07, 0x02, 0x00, 0xE0, 0x01, 0x40, 0x01, 0x00, 0xC0, 0x5D, 0x00, 0x00, 0x40, 0x19,
    0x01, 0x4D, 0x5A, 0x02, 0x00, 0x2A, 0x2C, 0x0A, 0x00, 0x03, 0x2A, 0x2C, 0x0A, 0x00, 0x40, 0x42, 0x0F, 0x00, 0x80,
    0x84, 0x1E, 0x00, 0x1E, 0x24, 0x07, 0x03, 0x00, 0x60, 0x01, 0x20, 0x01, 0x00, 0xA0, 0xB9, 0x00, 0x00, 0xA0, 0xB9,
    0x00, 0x4D, 0x8E, 0x01, 0x00, 0x2A, 0x2C, 0x0A, 0x00, 0x01, 0x2A, 0x2C, 0x0A, 0x00, 0x1E, 0x24, 0x07, 0x04, 0x00,
    0x40, 0x01, 0xF0, 0x00, 0x00, 0xA0, 0x8C, 0x00, 0x00, 0xA0, 0x8C, 0x00, 0x4D, 0x2E, 0x01, 0x00, 0x2A, 0x2C, 0x0A,
    0x00, 0x01, 0x2A, 0x2C, 0x0A, 0x00, 0x06, 0x24, 0x0D, 0x01, 0x01, 0x04, 0x09, 0x04, 0x01, 0x01, 0x01, 0x0E, 0x02,
    0x00, 0x00, 0x07, 0x05, 0x81, 0x05, 0xBC, 0x03, 0x01,
};
_Static_assert(sizeof(config_desc_bytes) == 0x0185, "Configuration Descriptor size does not match");

#define TEST_NUM_INTF_DESC      3   // Total number of interface descriptors (including alternate)

// --------------------- Sub-Test 1 ------------------------

/*
Test if we can walk the configuration descriptor to find each interface descriptor
*/
static void test_walk_desc(const usb_config_desc_t *config_desc)
{
    int offset = 0;
    const usb_standard_desc_t *cur_desc = (usb_standard_desc_t *)config_desc;
    for (int i = 0; i < TEST_NUM_INTF_DESC; i++) {
        cur_desc = usb_parse_next_descriptor_of_type(cur_desc, config_desc->wTotalLength, USB_B_DESCRIPTOR_TYPE_INTERFACE, &offset);
        TEST_ASSERT_NOT_NULL(cur_desc);
    }
    // Attempting to look for another interface descriptor should result in NULL
    cur_desc = usb_parse_next_descriptor_of_type(cur_desc, config_desc->wTotalLength, USB_B_DESCRIPTOR_TYPE_INTERFACE, &offset);
    TEST_ASSERT_NULL(cur_desc);
}

/*
Test if the count of number of alternate descriptors is correct
*/
static void test_alt_intf_desc_count(const usb_config_desc_t *config_desc)
{
    // bInterface 0 has no alternate interfaces
    TEST_ASSERT_EQUAL(0, usb_parse_interface_number_of_alternate(config_desc, 0));
    // bInterface 1 has 1 alternate interface
    TEST_ASSERT_EQUAL(1, usb_parse_interface_number_of_alternate(config_desc, 1));
    // Non existent bInterface 2 should return -1
    TEST_ASSERT_EQUAL(-1, usb_parse_interface_number_of_alternate(config_desc, 2));
}

static void test_parse_intf_and_ep(const usb_config_desc_t *config_desc)
{
    int offset_intf = 0;

    // Get bInterfaceNumber 0 (index 0)
    const usb_intf_desc_t *intf_desc = usb_parse_interface_descriptor(config_desc, 0, 0, &offset_intf);
    TEST_ASSERT_NOT_NULL(intf_desc);
    // Should only have one endpoint
    int offset_ep = offset_intf;
    const usb_ep_desc_t *ep_desc = usb_parse_endpoint_descriptor_by_index(intf_desc, 0, config_desc->wTotalLength, &offset_ep);
    TEST_ASSERT_NOT_NULL(ep_desc);
    TEST_ASSERT_EQUAL(0x83, ep_desc->bEndpointAddress);
    offset_ep = offset_intf;
    ep_desc = usb_parse_endpoint_descriptor_by_index(intf_desc, 1, config_desc->wTotalLength, &offset_ep);
    TEST_ASSERT_NULL(ep_desc);

    // Get bInterfaceNumber 1 alternate setting 0
    offset_intf = 0;
    intf_desc = usb_parse_interface_descriptor(config_desc, 1, 0, &offset_intf);
    TEST_ASSERT_NOT_NULL(intf_desc);
    // Should have no endpoints
    offset_ep = offset_intf;
    ep_desc = usb_parse_endpoint_descriptor_by_index(intf_desc, 0, config_desc->wTotalLength, &offset_ep);
    TEST_ASSERT_NULL(ep_desc);

    // Get bInterfaceNumber 1 alternate setting 1
    offset_intf = 0;
    intf_desc = usb_parse_interface_descriptor(config_desc, 1, 1, &offset_intf);
    TEST_ASSERT_NOT_NULL(intf_desc);
    // Should only have one endpoint
    offset_ep = offset_intf;
    ep_desc = usb_parse_endpoint_descriptor_by_index(intf_desc, 0, config_desc->wTotalLength, &offset_ep);
    TEST_ASSERT_NOT_NULL(ep_desc);
    TEST_ASSERT_EQUAL(0x81, ep_desc->bEndpointAddress);
    offset_ep = offset_intf;
    ep_desc = usb_parse_endpoint_descriptor_by_index(intf_desc, 1, config_desc->wTotalLength, &offset_ep);
    TEST_ASSERT_NULL(ep_desc);
}

static void test_parse_ep_by_address(const usb_config_desc_t *config_desc)
{
    int offset_ep = 0;
    // Get bInterface 0 bAlternateSetting 0 EP 0x83
    const usb_ep_desc_t *ep_desc = usb_parse_endpoint_descriptor_by_address(config_desc, 0, 0, 0x83, &offset_ep);
    TEST_ASSERT_NOT_NULL(ep_desc);
    TEST_ASSERT_EQUAL(0x83, ep_desc->bEndpointAddress);
    // Getting same EP address under different interface should return NULL
    offset_ep = 0;
    ep_desc = usb_parse_endpoint_descriptor_by_address(config_desc, 1, 0, 0x83, &offset_ep);
    TEST_ASSERT_NULL(ep_desc);

    // Get bInterface 1 bAlternateSetting 1 EP 0x81
    offset_ep = 0;
    ep_desc = usb_parse_endpoint_descriptor_by_address(config_desc, 1, 1, 0x81, &offset_ep);
    TEST_ASSERT_NOT_NULL(ep_desc);
    TEST_ASSERT_EQUAL(0x81, ep_desc->bEndpointAddress);
    // Getting same EP address under different interface should return NULL
    offset_ep = 0;
    ep_desc = usb_parse_endpoint_descriptor_by_address(config_desc, 1, 0, 0x81, &offset_ep);
    TEST_ASSERT_NULL(ep_desc);
}

TEST_CASE("Test USB Helpers descriptor parsing", "[helpers][full_speed][high_speed]")
{
    const usb_config_desc_t *config_desc = (const usb_config_desc_t *)config_desc_bytes;
    test_walk_desc(config_desc);
    test_alt_intf_desc_count(config_desc);
    test_parse_intf_and_ep(config_desc);
    test_parse_ep_by_address(config_desc);
}
