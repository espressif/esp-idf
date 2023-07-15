/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_check.h"
#include "esp_attr.h"
#include "esp_ieee802154_dev.h"
#include "esp_ieee802154_frame.h"

static const char *TAG = "ieee802154 frame";

IEEE802154_STATIC IEEE802154_INLINE bool is_security_enabled(const uint8_t *frame)
{
    return frame[IEEE802154_FRAME_SECURITY_OFFSET] & IEEE802154_FRAME_SECURITY_BIT;
}

IEEE802154_STATIC IEEE802154_INLINE bool is_ie_present(const uint8_t *frame)
{
    return frame[IEEE802154_FRAME_IE_OFFSET] & IEEE802154_FRAME_IE_BIT;
}

IEEE802154_STATIC IEEE802154_INLINE bool is_dsn_present(const uint8_t *frame)
{
    return ((ieee802154_frame_get_version(frame) != IEEE802154_FRAME_VERSION_2) ||
            !(frame[IEEE802154_FRAME_DSN_OFFSET] & IEEE802154_FRAME_DSN_BIT));
}

IEEE802154_STATIC IEEE802154_INLINE uint8_t dst_addr_mode(const uint8_t *frame)
{
    return frame[IEEE802154_FRAME_DST_MODE_OFFSET] & IEEE802154_FRAME_DST_MODE_MASK;
}

IEEE802154_STATIC IEEE802154_INLINE uint8_t src_addr_mode(const uint8_t *frame)
{
    return frame[IEEE802154_FRAME_SRC_MODE_OFFSET] & IEEE802154_FRAME_SRC_MODE_MASK;
}

IEEE802154_STATIC IEEE802154_INLINE bool is_panid_compression(const uint8_t *frame)
{
    return frame[IEEE802154_FRAME_PANID_COMP_OFFSET] & IEEE802154_FRAME_PANID_COMP_BIT;
}

IEEE802154_STATIC IEEE802154_INLINE bool is_suported_frame_type(uint8_t frame_type)
{
    // HW supports 4 kinds of frame type: Beacon, Ack, Data, Command.
    return (frame_type == IEEE802154_FRAME_TYPE_BEACON || frame_type == IEEE802154_FRAME_TYPE_DATA ||
            frame_type == IEEE802154_FRAME_TYPE_ACK || frame_type == IEEE802154_FRAME_TYPE_COMMAND);
}

IEEE802154_STATIC bool is_dst_panid_present(const uint8_t *frame)
{
    uint8_t dst_mode = dst_addr_mode(frame);
    bool dst_panid_present = false;

    if (ieee802154_frame_get_version(frame) == IEEE802154_FRAME_VERSION_2) {
        uint8_t src_mode = src_addr_mode(frame);
        bool panid_compression = is_panid_compression(frame);

        if (dst_mode != IEEE802154_FRAME_DST_MODE_NONE) { // dest address is present/short/extented
            if ((src_mode == IEEE802154_FRAME_SRC_MODE_NONE && panid_compression) ||
                    (dst_mode == IEEE802154_FRAME_DST_MODE_EXT && src_mode == IEEE802154_FRAME_SRC_MODE_EXT && panid_compression)) {
                dst_panid_present = false;
            } else {
                dst_panid_present = true;
            }
        } else { // dest address is not present
            if (src_mode == IEEE802154_FRAME_SRC_MODE_NONE && panid_compression) {
                dst_panid_present = true;
            } else {
                dst_panid_present = false;
            }
        }
    } else { // frame version: 0b00, 0b01
        dst_panid_present = (dst_mode != IEEE802154_FRAME_DST_MODE_NONE);
    }

    return dst_panid_present;
}

IEEE802154_STATIC bool is_src_panid_present(const uint8_t *frame)
{
    uint8_t src_mode = src_addr_mode(frame);
    bool panid_compression = is_panid_compression(frame);
    bool src_panid_present = false;

    if (ieee802154_frame_get_version(frame) == IEEE802154_FRAME_VERSION_2) {
        uint8_t dst_mode = dst_addr_mode(frame);

        if (src_mode != IEEE802154_FRAME_SRC_MODE_NONE) {
            if (dst_mode == IEEE802154_FRAME_DST_MODE_EXT && src_mode == IEEE802154_FRAME_SRC_MODE_EXT && !panid_compression) {
                src_panid_present = false;
            } else {
                src_panid_present = !panid_compression;
            }
        } else {
            src_panid_present = false;
        }
    } else { // frame version: 0b00, 0b01
        if (src_mode != IEEE802154_FRAME_SRC_MODE_NONE) {
            src_panid_present = !panid_compression;
        } else {
            src_panid_present = false;
        }
    }

    return src_panid_present;
}

IEEE802154_STATIC uint8_t IEEE802154_INLINE ieee802154_frame_address_offset(const uint8_t *frame)
{
    return IEEE802154_FRAME_PHR_SIZE + IEEE802154_FRAME_FCF_SIZE + (is_dsn_present(frame) ? IEEE802154_FRAME_DSN_SIZE : 0);
}

IEEE802154_STATIC IRAM_ATTR uint8_t ieee802154_frame_address_size(const uint8_t *frame)
{
    uint8_t address_size = 0;

    if (is_dst_panid_present(frame)) {
        address_size += IEEE802154_FRAME_PANID_SIZE;
    }

    switch (dst_addr_mode(frame)) {
    case IEEE802154_FRAME_DST_MODE_NONE:
        break;

    case IEEE802154_FRAME_DST_MODE_SHORT:
        address_size += IEEE802154_FRAME_SHORT_ADDR_SIZE;
        break;

    case IEEE802154_FRAME_DST_MODE_EXT:
        address_size += IEEE802154_FRAME_EXT_ADDR_SIZE;
        break;

    default:
        return IEEE802154_FRAME_INVALID_OFFSET;
    }

    if (is_src_panid_present(frame)) {
        address_size += IEEE802154_FRAME_PANID_SIZE;
    }

    switch (src_addr_mode(frame)) {
    case IEEE802154_FRAME_SRC_MODE_NONE:
        break;

    case IEEE802154_FRAME_SRC_MODE_SHORT:
        address_size += IEEE802154_FRAME_SHORT_ADDR_SIZE;
        break;

    case IEEE802154_FRAME_SRC_MODE_EXT:
        address_size += IEEE802154_FRAME_EXT_ADDR_SIZE;
        break;

    default:
        return IEEE802154_FRAME_INVALID_OFFSET;
    }

    return address_size;
}

IEEE802154_STATIC uint8_t ieee802154_frame_security_header_offset(const uint8_t *frame)
{
    ESP_RETURN_ON_FALSE_ISR(is_suported_frame_type(ieee802154_frame_get_type(frame)), IEEE802154_FRAME_INVALID_ADDR_MODE, TAG, "invalid frame type");
    uint8_t offset = ieee802154_frame_address_offset(frame);
    uint8_t address_size = ieee802154_frame_address_size(frame);

    ESP_RETURN_ON_FALSE_ISR(offset != IEEE802154_FRAME_INVALID_OFFSET, IEEE802154_FRAME_INVALID_OFFSET, TAG, "invalid offset");
    ESP_RETURN_ON_FALSE_ISR(address_size != IEEE802154_FRAME_INVALID_OFFSET, IEEE802154_FRAME_INVALID_OFFSET, TAG, "invalid offset");

    offset += address_size;

    return offset;
}

IEEE802154_STATIC uint8_t ieee802154_frame_get_security_field_len(const uint8_t *frame)
{
    ESP_RETURN_ON_FALSE_ISR(is_suported_frame_type(ieee802154_frame_get_type(frame)), IEEE802154_FRAME_INVALID_OFFSET, TAG, "invalid frame type");

    uint8_t security_field_len = 0;
    uint8_t offset = ieee802154_frame_security_header_offset(frame);

    ESP_RETURN_ON_FALSE_ISR(offset != IEEE802154_FRAME_INVALID_OFFSET, IEEE802154_FRAME_INVALID_OFFSET, TAG, "invalid offset");

    security_field_len += IEEE802154_FRAME_SE_HEAD_SIZE;
    uint8_t security_header = frame[offset];

    if (security_header & IEEE802154_FRAME_COUNTER_SUPPRESS_BIT) {
        security_field_len += 0;
    } else {
        security_field_len += IEEE802154_FRAME_COUNTER_SIZE;
    }

    switch (security_header & IEEE802154_FRAME_KEY_ID_MODE_MASK) {
    case IEEE802154_FRAME_KEY_ID_MODE_1:
        security_field_len += IEEE802154_FRAME_KEY_ID_MODE_1_SIZE;
        break;
    case IEEE802154_FRAME_KEY_ID_MODE_2:
        security_field_len += IEEE802154_FRAME_KEY_ID_MODE_2_SIZE;
        break;
    case IEEE802154_FRAME_KEY_ID_MODE_3:
        security_field_len += IEEE802154_FRAME_KEY_ID_MODE_3_SIZE;
        break;
    default:
        security_field_len += 0;
        break;
    }

    return security_field_len;
}

IEEE802154_STATIC uint8_t ieee802154_frame_ie_header_offset(const uint8_t *frame)
{
    uint8_t offset = ieee802154_frame_security_header_offset(frame);
    uint8_t security_field_len = ieee802154_frame_get_security_field_len(frame);

    offset += security_field_len;

    return offset;
}

IEEE802154_STATIC uint8_t ieee802154_frame_get_mic_len(const uint8_t *frame)
{
    uint8_t offset = ieee802154_frame_security_header_offset(frame);
    uint8_t mic_len = 0;
    uint8_t security_header = frame[offset];

    switch (security_header & IEEE802154_FRAME_SECURITY_MASK) {
    case IEEE802154_FRAME_SECURITY_MIC_32:
    case IEEE802154_FRAME_SECURITY_ENC_MIC_32:
        mic_len = IEEE802154_FRAME_SECURITY_MIC_32_SIZE;
        break;
    case IEEE802154_FRAME_SECURITY_MIC_64:
    case IEEE802154_FRAME_SECURITY_ENC_MIC_64:
        mic_len = IEEE802154_FRAME_SECURITY_MIC_64_SIZE;
        break;
    case IEEE802154_FRAME_SECURITY_MIC_128:
    case IEEE802154_FRAME_SECURITY_ENC_MIC_128:
        mic_len = IEEE802154_FRAME_SECURITY_MIC_128_SIZE;
        break;
    default:
        mic_len = 0;
        break;
    }
    return mic_len;
}

IEEE802154_STATIC uint8_t ieee802154_frame_get_ie_field_len(const uint8_t *frame)
{
    uint8_t offset = ieee802154_frame_ie_header_offset(frame);
    uint8_t ie_field_len = 0;
    uint8_t frame_footer_len = ieee802154_frame_get_mic_len(frame) + IEEE802154_FRAME_FCS_SIZE;

    /* If the `offset + frame_footer_len == frame_len`, we exit the `while()`
       loop. This covers the case where frame contains one or more Header IEs
       but no data payload. In this case, 2015-spec does not
       require Header IE termination to be included (it is optional)
       since the end of frame can be determined from frame length and
       footer length. (for details, please reference 2015 - spec table 7 - 6 in page 169) */

    while (frame[0] > offset + ie_field_len + frame_footer_len) {
        uint16_t ie_header = frame[offset + ie_field_len + 1] << 8 | frame[offset + ie_field_len];
        // Header Termination IE 2 is used in to signal end of the MHR and beginning of the MAC Payload.
        if ((ie_header & IEEE802154_FRAME_IE_HEAD_ID_MASK) == IEEE802154_IE_TYPE_HT2) {
            ie_field_len += IEEE802154_FRAME_IE_HEAD_LEN;
            break;
        } else {
            uint8_t ie_subfield_len = (ie_header & IEEE802154_FRAME_IE_SUBFIELD_LEN_MASK);
            ie_field_len += IEEE802154_FRAME_IE_HEAD_LEN + ie_subfield_len;
        }
    }
    return ie_field_len;
}

IEEE802154_STATIC IRAM_ATTR uint8_t ieee802154_frame_payload_offset(const uint8_t *frame)
{
    uint8_t offset = ieee802154_frame_security_header_offset(frame);
    if (is_security_enabled(frame)) {
        // skip security field.
        offset += ieee802154_frame_get_security_field_len(frame);
    }

    if (ieee802154_frame_get_version(frame) == IEEE802154_FRAME_VERSION_2 && is_ie_present(frame)) {
        // skip IE fields.
        offset += ieee802154_frame_get_ie_field_len(frame);
    }
    if (ieee802154_frame_get_version(frame) == IEEE802154_FRAME_VERSION_0 || ieee802154_frame_get_version(frame) == IEEE802154_FRAME_VERSION_1) {
        if (ieee802154_frame_get_type(frame) == IEEE802154_FRAME_TYPE_BEACON) {
            // TODO: skip beacon payload.
        }
        if (ieee802154_frame_get_type(frame) == IEEE802154_FRAME_TYPE_COMMAND) {
            offset += IEEE802154_FRAME_COMMAND_ID_LEN;
        }
    }

    return offset - 1;
}

uint8_t IEEE802154_INLINE ieee802154_frame_get_type(const uint8_t *frame)
{
    return frame[IEEE802154_FRAME_TYPE_OFFSET] & IEEE802154_FRAME_TYPE_MASK;
}

uint8_t IEEE802154_INLINE ieee802154_frame_get_version(const uint8_t *frame)
{
    return frame[IEEE802154_FRAME_VERSION_OFFSET] & IEEE802154_FRAME_VERSION_MASK;
}

bool IEEE802154_INLINE ieee802154_frame_is_ack_required(const uint8_t *frame)
{
    return frame[IEEE802154_FRAME_AR_OFFSET] & IEEE802154_FRAME_AR_BIT;
}

uint8_t ieee802154_frame_get_dst_addr(const uint8_t *frame, uint8_t *addr)
{
    ESP_RETURN_ON_FALSE_ISR(is_suported_frame_type(ieee802154_frame_get_type(frame)), IEEE802154_FRAME_INVALID_ADDR_MODE, TAG, "invalid frame type");

    uint8_t offset = ieee802154_frame_address_offset(frame);
    uint8_t dst_mode = dst_addr_mode(frame);
    uint8_t addr_size;

    ESP_RETURN_ON_FALSE_ISR(dst_mode == IEEE802154_FRAME_DST_MODE_SHORT || dst_mode == IEEE802154_FRAME_DST_MODE_EXT, dst_mode, TAG, "invalid address mode");

    addr_size = (dst_mode == IEEE802154_FRAME_DST_MODE_SHORT) ? IEEE802154_FRAME_SHORT_ADDR_SIZE : IEEE802154_FRAME_EXT_ADDR_SIZE;

    if (is_dst_panid_present(frame)) {
        offset += IEEE802154_FRAME_PANID_SIZE;
    }

    memcpy(addr, frame + offset, addr_size);

    return dst_mode;
}

uint8_t ieee802154_frame_get_src_addr(const uint8_t *frame, uint8_t *addr)
{
    ESP_RETURN_ON_FALSE_ISR(is_suported_frame_type(ieee802154_frame_get_type(frame)), IEEE802154_FRAME_INVALID_ADDR_MODE, TAG, "invalid frame type");

    uint8_t offset = ieee802154_frame_address_offset(frame);
    uint8_t dst_mode = dst_addr_mode(frame);
    uint8_t src_mode = src_addr_mode(frame);
    uint8_t addr_size;

    ESP_RETURN_ON_FALSE_ISR(src_mode == IEEE802154_FRAME_SRC_MODE_SHORT || src_mode == IEEE802154_FRAME_SRC_MODE_EXT, src_mode, TAG, "invalid address mode");

    addr_size = (src_mode == IEEE802154_FRAME_SRC_MODE_SHORT) ? IEEE802154_FRAME_SHORT_ADDR_SIZE : IEEE802154_FRAME_EXT_ADDR_SIZE;

    if (is_dst_panid_present(frame)) {
        offset += IEEE802154_FRAME_PANID_SIZE;
    }

    switch (dst_mode) {
    case IEEE802154_FRAME_DST_MODE_SHORT:
        offset += IEEE802154_FRAME_SHORT_ADDR_SIZE;
        break;

    case IEEE802154_FRAME_DST_MODE_EXT:
        offset += IEEE802154_FRAME_EXT_ADDR_SIZE;
        break;

    default:
        break;
    }

    if (is_src_panid_present(frame)) {
        offset += IEEE802154_FRAME_PANID_SIZE;
    }

    memcpy(addr, frame + offset, addr_size);

    return src_mode;
}

uint8_t ieee802154_frame_get_security_payload_offset(uint8_t *frame)
{
    return ieee802154_frame_payload_offset(frame);
}

esp_err_t ieee802154_frame_get_dest_panid(const uint8_t *frame, uint8_t *panid)
{
    uint8_t offset = ieee802154_frame_address_offset(frame);

    if (is_dst_panid_present(frame)) {
        memcpy(panid, frame + offset, IEEE802154_FRAME_PANID_SIZE);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t ieee802154_frame_get_src_panid(const uint8_t *frame, uint8_t *panid)
{
    uint8_t offset = ieee802154_frame_address_offset(frame);
    uint8_t dst_mode = dst_addr_mode(frame);

    if (is_src_panid_present(frame)) {
        if (is_dst_panid_present(frame)) {
            offset += IEEE802154_FRAME_PANID_SIZE;
        }

        switch (dst_mode) {
        case IEEE802154_FRAME_DST_MODE_SHORT:
            offset += IEEE802154_FRAME_SHORT_ADDR_SIZE;
            break;

        case IEEE802154_FRAME_DST_MODE_EXT:
            offset += IEEE802154_FRAME_EXT_ADDR_SIZE;
            break;

        default:
            break;
        }
        memcpy(panid, frame + offset, IEEE802154_FRAME_PANID_SIZE);
        return ESP_OK;
    }
    return ESP_FAIL;
}
