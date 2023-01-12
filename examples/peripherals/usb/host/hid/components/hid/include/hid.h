/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief HID Subclass
 *
 * @see 4.2 Subclass, p.8 of Device Class Definition for Human Interface Devices (HID) Version 1.11
 */
typedef enum {
    HID_SUBCLASS_NO_SUBCLASS = 0x00,
    HID_SUBCLASS_BOOT_INTERFACE = 0x01
} __attribute__((packed)) hid_subclass_t;

/**
 * @brief HID Protocols
 *
 * @see 4.3 Protocols, p.9 of Device Class Definition for Human Interface Devices (HID) Version 1.11
 */
typedef enum {
    HID_PROTOCOL_NONE = 0x00,
    HID_PROTOCOL_KEYBOARD = 0x01,
    HID_PROTOCOL_MOUSE = 0x02,
    HID_PROTOCOL_MAX
} __attribute__((packed)) hid_protocol_t;

/**
 * @brief HID Descriptor
 *
 * @see 6.2.1 HID Descriptor, p.22 of Device Class Definition for Human Interface Devices (HID) Version 1.11
 */
typedef struct {
    uint8_t bLength;                        // Numeric expression that is the total size of the HID descriptor
    uint8_t bDescriptorType;                // Constant name specifying type of HID descriptor
    uint16_t bcdHID;                        // Numeric expression identifying the HIDClass Specification release
    int8_t bCountryCode;                    // Numeric expression identifying country code of the localized hardware
    uint8_t bNumDescriptors;                // Numeric expression specifying the number of class descriptors (always at least one i.e. Report descriptor.)
    uint8_t bReportDescriptorType;          // Constant name identifying type of class descriptor. See Section 7.1.2: Set_Descriptor Request for a table of class descriptor constants
    uint16_t wReportDescriptorLength;       // Numeric expression that is the total size of the Report descriptor
    // Optional descriptors may follow further
} __attribute__((packed)) hid_descriptor_t;

/**
 * @brief HID Country Codes
 *
 * @see 6.2.1 HID Descriptor, p.23 of Device Class Definition for Human Interface Devices (HID) Version 1.11
 */

typedef enum {
    HID_COUNTRY_CODE_NOT_SUPPORTED = 0x00,
    HID_COUNTRY_CODE_ARABIC = 0x01,
    HID_COUNTRY_CODE_BELGIAN = 0x02,
    HID_COUNTRY_CODE_CANADIAN_BILINGUAL = 0x03,
    HID_COUNTRY_CODE_CANADIAN_FRENCH = 0x04,
    HID_COUNTRY_CODE_CZECH = 0x05,
    HID_COUNTRY_CODE_DANISH = 0x06,
    HID_COUNTRY_CODE_FINNISH = 0x07,
    HID_COUNTRY_CODE_FRENCH = 0x08,
    HID_COUNTRY_CODE_GERMAN = 0x09,
    HID_COUNTRY_CODE_GREEK = 0x0A,
    HID_COUNTRY_CODE_HEBREW = 0x0B,
    HID_COUNTRY_CODE_HUNGARY = 0x0C,
    HID_COUNTRY_CODE_ISO = 0x0D,
    HID_COUNTRY_CODE_ITALIAN = 0x0E,
    HID_COUNTRY_CODE_JAPAN = 0x0F,
    HID_COUNTRY_CODE_KOREAN = 0x10,
    HID_COUNTRY_CODE_LATIN_AMERICAN = 0x11,
    HID_COUNTRY_CODE_NETHERLANDS = 0x12,
    HID_COUNTRY_CODE_NORWEGIAN = 0x13,
    HID_COUNTRY_CODE_PERSIAN = 0x14,
    HID_COUNTRY_CODE_POLAND = 0x15,
    HID_COUNTRY_CODE_PORTUGUESE = 0x16,
    HID_COUNTRY_CODE_RUSSIA = 0x17,
    HID_COUNTRY_CODE_SLOVAKIA = 0x18,
    HID_COUNTRY_CODE_SPANISH = 0x19,
    HID_COUNTRY_CODE_SWEDISH = 0x1A,
    HID_COUNTRY_CODE_SWISS_F = 0x1B,
    HID_COUNTRY_CODE_SWISS_G = 0x1C,
    HID_COUNTRY_CODE_SWITZERLAND = 0x1D,
    HID_COUNTRY_CODE_TAIWAN = 0x1E,
    HID_COUNTRY_CODE_TURKISH_Q = 0x1F,
    HID_COUNTRY_CODE_UK = 0x20,
    HID_COUNTRY_CODE_US = 0x21,
    HID_COUNTRY_CODE_YUGOSLAVIA = 0x22,
    HID_COUNTRY_CODE_TURKISH_F = 0x23
} __attribute__((packed)) hid_country_code_t;

/**
 * @brief HID Class Descriptor Types
 *
 * @see 7.1, p.49 of Device Class Definition for Human Interface Devices (HID) Version 1.11
 */
typedef enum {
    HID_CLASS_DESCRIPTOR_TYPE_HID = 0x21,
    HID_CLASS_DESCRIPTOR_TYPE_REPORT = 0x22,
    HID_CLASS_DESCRIPTOR_TYPE_PHYSICAL = 0x23
} __attribute__((packed)) hid_class_descritpor_type_t;

/**
 * @brief HID Class-Specific Requests
 *
 * @see 7.2, p.50 of Device Class Definition for Human Interface Devices (HID) Version 1.11
 */
typedef enum {
    HID_CLASS_SPECIFIC_REQ_GET_REPORT = 0x01,
    HID_CLASS_SPECIFIC_REQ_GET_IDLE = 0x02,
    HID_CLASS_SPECIFIC_REQ_GET_PROTOCOL = 0x03,
    HID_CLASS_SPECIFIC_REQ_SET_REPORT = 0x09,
    HID_CLASS_SPECIFIC_REQ_SET_IDLE = 0x0A,
    HID_CLASS_SPECIFIC_REQ_SET_PROTOCOL = 0x0B
} __attribute__((packed)) hid_class_specific_req_t;

/**
 * @brief HID Report Types
 *
 * @see 7.2.1, p.51 of Device Class Definition for Human Interface Devices (HID) Version 1.11
 */
typedef enum {
    HID_REPORT_TYPE_INPUT = 0x01,
    HID_REPORT_TYPE_OUTPUT = 0x02,
    HID_REPORT_TYPE_FEATURE = 0x03,
} __attribute__((packed)) hid_report_type_t;

/**
 * @brief HID Report protocol
 *
 * @see 7.2.5/7.2.6, p.54 of Device Class Definition for Human Interface Devices (HID) Version 1.11
 */
typedef enum {
    HID_REPORT_PROTOCOL_BOOT = 0x00,
    HID_REPORT_PROTOCOL_REPORT = 0x01,
    HID_REPORT_PROTOCOL_MAX
} __attribute__((packed)) hid_report_protocol_t;

#ifdef __cplusplus
}
#endif //__cplusplus
