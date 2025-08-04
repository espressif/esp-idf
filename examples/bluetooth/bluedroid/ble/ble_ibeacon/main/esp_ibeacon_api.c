/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */



/****************************************************************************
*
* This file is for iBeacon APIs. It supports both iBeacon encode and decode.
*
* iBeacon is a trademark of Apple Inc. Before building devices which use iBeacon technology,
* visit https://developer.apple.com/ibeacon/ to obtain a license.
*
****************************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "esp_gap_ble_api.h"
#include "esp_ibeacon_api.h"

// Ensure structure sizes are as expected
_Static_assert(sizeof(esp_ble_ibeacon_head_t) == 9, "Unexpected esp_ble_ibeacon_head_t size");
_Static_assert(sizeof(esp_ble_ibeacon_vendor_t) == 21, "Unexpected esp_ble_ibeacon_vendor_t size");


const uint8_t uuid_zeros[ESP_UUID_LEN_128] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* For iBeacon packet format, please refer to Apple "Proximity Beacon Specification" doc */
/* Constant part of iBeacon data */
esp_ble_ibeacon_head_t ibeacon_common_head = {
    .flags = {0x02, 0x01, 0x06},
    .length = 0x1A,
    .type = 0xFF,
    .company_id = 0x004C,
    .beacon_type = 0x1502
};

/* Vendor part of iBeacon data*/
esp_ble_ibeacon_vendor_t vendor_config = {
    .proximity_uuid = ESP_UUID,
    .major = ENDIAN_CHANGE_U16(ESP_MAJOR), //Major=ESP_MAJOR
    .minor = ENDIAN_CHANGE_U16(ESP_MINOR), //Minor=ESP_MINOR
    .measured_power = 0xC5
};

bool esp_ble_is_ibeacon_packet (const uint8_t *adv_data, uint8_t adv_data_len){
    bool result = false;

    /* iBeacon payload should at least fit the header + vendor fields */
    const size_t min_len = sizeof(esp_ble_ibeacon_t);

    if (adv_data && adv_data_len >= min_len) {
        /*
         * The first 3 bytes (flags) can vary based on stack config, so skip them.
         * ibeaon_common_head.flags = {0x02, 0x01, 0x06}
         */
        const size_t skip = sizeof(ibeacon_common_head.flags);
        if (!memcmp(adv_data + skip, ((const uint8_t *)&ibeacon_common_head) + skip, sizeof(ibeacon_common_head) - skip)) {
            result = true;
        }
    }

    return result;
}

esp_err_t esp_ble_config_ibeacon_data (const esp_ble_ibeacon_vendor_t *vendor_config, esp_ble_ibeacon_t *ibeacon_adv_data){
    if ((vendor_config == NULL) || (ibeacon_adv_data == NULL) || (!(!memcmp(vendor_config->proximity_uuid, uuid_zeros, ESP_UUID_LEN_128)))){
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(&ibeacon_adv_data->ibeacon_head, &ibeacon_common_head, sizeof(esp_ble_ibeacon_head_t));
    memcpy(&ibeacon_adv_data->ibeacon_vendor, vendor_config, sizeof(esp_ble_ibeacon_vendor_t));

    return ESP_OK;
}
