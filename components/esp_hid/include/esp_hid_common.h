// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

/* HID Report Map Values */
#define HID_RM_INPUT                        0x80
#define HID_RM_OUTPUT                       0x90
#define HID_RM_FEATURE                      0xb0
#define HID_RM_COLLECTION                   0xa0
#define HID_RM_END_COLLECTION               0xc0
#define HID_RM_USAGE_PAGE                   0x04
#define HID_RM_LOGICAL_MINIMUM              0x14
#define HID_RM_LOGICAL_MAXIMUM              0x24
#define HID_RM_PHYSICAL_MINIMUM             0x34
#define HID_RM_PHYSICAL_MAXIMUM             0x44
#define HID_RM_UNIT_EXPONENT                0x54
#define HID_RM_UNIT                         0x64
#define HID_RM_REPORT_SIZE                  0x74
#define HID_RM_REPORT_ID                    0x84
#define HID_RM_REPORT_COUNT                 0x94
#define HID_RM_PUSH                         0xa4
#define HID_RM_POP                          0xb4
#define HID_RM_USAGE                        0x08
#define HID_RM_USAGE_MINIMUM                0x18
#define HID_RM_USAGE_MAXIMUM                0x28
#define HID_RM_DESIGNATOR_INDEX             0x38
#define HID_RM_DESIGNATOR_MINIMUM           0x48
#define HID_RM_DESIGNATOR_MAXIMUM           0x58
#define HID_RM_STRING_INDEX                 0x78
#define HID_RM_STRING_MINIMUM               0x88
#define HID_RM_STRING_MAXIMUM               0x98
#define HID_RM_DELIMITER                    0xa8

/* HID Usage Pages and Usages */
#define HID_USAGE_PAGE_GENERIC_DESKTOP      0x01
#define HID_USAGE_KEYBOARD                  0x06
#define HID_USAGE_MOUSE                     0x02
#define HID_USAGE_JOYSTICK                  0x04
#define HID_USAGE_GAMEPAD                   0x05

#define HID_USAGE_PAGE_CONSUMER_DEVICE      0x0C
#define HID_USAGE_CONSUMER_CONTROL          0x01

/* HID BT COD Peripheral Min Values Main Role */
#define ESP_HID_COD_MIN_KEYBOARD            0x10
#define ESP_HID_COD_MIN_MOUSE               0x20

/* HID BLE Appearances */
#define ESP_HID_APPEARANCE_GENERIC          0x03C0
#define ESP_HID_APPEARANCE_KEYBOARD         0x03C1
#define ESP_HID_APPEARANCE_MOUSE            0x03C2
#define ESP_HID_APPEARANCE_JOYSTICK         0x03C3
#define ESP_HID_APPEARANCE_GAMEPAD          0x03C4

/* HID Report Types */
#define ESP_HID_REPORT_TYPE_INPUT           1
#define ESP_HID_REPORT_TYPE_OUTPUT          2
#define ESP_HID_REPORT_TYPE_FEATURE         3

/* HID Protocol Modes */
#define ESP_HID_PROTOCOL_MODE_BOOT          0x00      // Boot Protocol Mode
#define ESP_HID_PROTOCOL_MODE_REPORT        0x01      // Report Protocol Mode

/* HID information flags */
#define ESP_HID_FLAGS_REMOTE_WAKE           0x01      // RemoteWake
#define ESP_HID_FLAGS_NORMALLY_CONNECTABLE  0x02      // NormallyConnectable

/* Control point commands */
#define ESP_HID_CONTROL_SUSPEND             0x00      // Suspend
#define ESP_HID_CONTROL_EXIT_SUSPEND        0x01      // Exit Suspend

/* Client Characteristic Configuration values */
#define ESP_HID_CCC_NOTIFICATIONS_ENABLED   0x01      // Notifications enabled
#define ESP_HID_CCC_INDICATIONS_ENABLED     0x02      // Indications enabled

/* HID Transports */
typedef enum {
    ESP_HID_TRANSPORT_BT,
    ESP_HID_TRANSPORT_BLE,
    ESP_HID_TRANSPORT_USB,
    ESP_HID_TRANSPORT_MAX
} esp_hid_transport_t;

/* HID Usage Types */
typedef enum {
    ESP_HID_USAGE_GENERIC  = 0,
    ESP_HID_USAGE_KEYBOARD = 1,
    ESP_HID_USAGE_MOUSE    = 2,
    ESP_HID_USAGE_JOYSTICK = 4,
    ESP_HID_USAGE_GAMEPAD  = 8,
    ESP_HID_USAGE_TABLET   = 16,
    ESP_HID_USAGE_CCONTROL = 32,
    ESP_HID_USAGE_VENDOR   = 64
} esp_hid_usage_t;

/* HID BT COD Peripheral Min Values. Mask of (keyboard|mouse|ESP_HIDH_COD_*) */
typedef enum {
    ESP_HID_COD_MIN_GENERIC,
    ESP_HID_COD_MIN_JOYSTICK,
    ESP_HID_COD_MIN_GAMEPAD,
    ESP_HID_COD_MIN_REMOTE,
    ESP_HID_COD_MIN_SENSOR,
    ESP_HID_COD_MIN_TABLET,
    ESP_HID_COD_MIN_CARD_READER,
    ESP_HID_COD_MIN_MAX
} esp_hid_cod_min_t;

/**
 * @brief HID report item structure
 */
typedef struct {
    uint8_t map_index;              /*!< HID report map index */
    uint8_t report_id;              /*!< HID report id */
    uint8_t report_type;            /*!< HID report type */
    uint8_t protocol_mode;          /*!< HID protocol mode */
    esp_hid_usage_t usage;          /*!< HID usage type */
    uint16_t value_len;             /*!< HID report length in bytes */
} esp_hid_report_item_t;

/**
 * @brief HID parsed report map structure
 */
typedef struct {
    esp_hid_usage_t usage;              /*!< Dominant HID usage. (keyboard > mouse > joystick > gamepad > generic) */
    uint16_t appearance;                /*!< Calculated HID Appearance based on the dominant usage */
    uint8_t reports_len;                /*!< Number of reports discovered in the report map */
    esp_hid_report_item_t *reports;     /*!< Reports discovered in the report map */
} esp_hid_report_map_t;

/**
 * @brief HID raw report map structure
 */
typedef struct {
    const uint8_t *data;                /*!< Pointer to the HID report map data */
    uint16_t len;                       /*!< HID report map data length */
} esp_hid_raw_report_map_t;

/**
 * @brief HID device config structure
 */
typedef struct {
    uint16_t vendor_id;                     /*!< HID Vendor ID */
    uint16_t product_id;                    /*!< HID Product ID */
    uint16_t version;                       /*!< HID Product Version */
    const char *device_name;                /*!< HID Device Name */
    const char *manufacturer_name;          /*!< HID Manufacturer */
    const char *serial_number;              /*!< HID Serial Number */
    esp_hid_raw_report_map_t *report_maps;  /*!< Array of the raw HID report maps */
    uint8_t report_maps_len;                /*!< number of raw report maps in the array */
} esp_hid_device_config_t;

/*
 * @brief Parse RAW HID report map
 *        It is a responsibility of the user to free the parsed report map,
 *        when it's no longer needed. Use esp_hid_free_report_map
 * @param hid_rm      : pointer to the hid report map data
 * @param hid_rm_len  : length to the hid report map data
 *
 * @return: pointer to the parsed report map
 */
esp_hid_report_map_t *esp_hid_parse_report_map(const uint8_t *hid_rm, size_t hid_rm_len);

/*
 * @brief Free parsed HID report map
 * @param map      : pointer to the parsed hid report map
 */
void esp_hid_free_report_map(esp_hid_report_map_t *map);

/**
 * @brief Calculate the HID Device usage type from the BLE Apperance
 * @param appearance : BLE Apperance value
 *
 * @return: the hid usage type
 */
esp_hid_usage_t esp_hid_usage_from_appearance(uint16_t appearance);

/**
 * @brief Calculate the HID Device usage type from the BT CoD
 * @param cod : BT CoD value
 *
 * @return: the hid usage type
 */
esp_hid_usage_t esp_hid_usage_from_cod(uint32_t cod);

/**
 * @brief Convert device usage type to string
 * @param usage : The HID usage type to convert
 *
 * @return: a pointer to the string or NULL
 */
const char *esp_hid_usage_str(esp_hid_usage_t usage);

/**
 * @brief Convert HID protocol mode to string
 * @param protocol_mode : The HID protocol mode to convert
 *                        BOOT/REPORT
 *
 * @return: a pointer to the string or NULL
 */
const char *esp_hid_protocol_mode_str(uint8_t protocol_mode);

/**
 * @brief Convert HID report type to string
 * @param report_type : The HID report type to convert
 *                      INPUT/OUTPUT/FEATURE
 *
 * @return: a pointer to the string or NULL
 */
const char *esp_hid_report_type_str(uint8_t report_type);

/**
 * @brief Convert BT CoD major to string
 * @param cod_major : The CoD major value to convert
 *
 * @return: a pointer to the string or NULL
 */
const char *esp_hid_cod_major_str(uint8_t cod_major);

/**
 * @brief Print BT CoD minor value
 * @param cod_min : The CoD minor value to print
 * @param fp      : pointer to the output file
 */
void esp_hid_cod_minor_print(uint8_t cod_min, FILE *fp);

/**
 * @brief Convert BLE disconnect reason to string
 * @param reason : The value of the reason
 *
 * @return: a pointer to the string or NULL
 */
const char *esp_hid_disconnect_reason_str(esp_hid_transport_t transport, int reason);

#ifdef __cplusplus
}
#endif
