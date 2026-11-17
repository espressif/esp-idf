/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_check.h"
#include "esp_attr.h"
#include "esp_ieee802154_dev.h"
#include "esp_ieee802154_frame.h"
#include "esp_ieee802154_util.h"
#include "esp_rom_sys.h"

bool ieee802154_frame_is_security_enabled(const uint8_t *frame)
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

IEEE802154_STATIC IEEE802154_INLINE bool is_valid_dst_mode(uint8_t dst_mode)
{
    return (dst_mode == IEEE802154_FRAME_DST_MODE_NONE ||
            dst_mode == IEEE802154_FRAME_DST_MODE_SHORT ||
            dst_mode == IEEE802154_FRAME_DST_MODE_EXT);
}

IEEE802154_STATIC IEEE802154_INLINE bool is_valid_src_mode(uint8_t src_mode)
{
    return (src_mode == IEEE802154_FRAME_SRC_MODE_NONE ||
            src_mode == IEEE802154_FRAME_SRC_MODE_SHORT ||
            src_mode == IEEE802154_FRAME_SRC_MODE_EXT);
}

/**
 * @brief Check whether a range is within the received MAC frame data.
 *
 * The PHR at frame[0] contains the PSDU length, including the FCS. On RX,
 * the hardware replaces the FCS with RSSI and LQI, so it must not be parsed
 * as MAC data. The valid MAC byte range is therefore
 * [IEEE802154_FRAME_PHR_SIZE, IEEE802154_FRAME_PHR_SIZE + frame_len - FCS).
 */
IEEE802154_STATIC IEEE802154_INLINE bool frame_has_mac_bytes(const uint8_t *frame, uint8_t offset, uint8_t count)
{
    const uint8_t frame_len = frame[0] & IEEE802154_FRAME_MAX_LEN;

    if (frame_len < IEEE802154_FRAME_FCS_SIZE) {
        return false;
    }

    const uint8_t mac_end = IEEE802154_FRAME_PHR_SIZE + frame_len - IEEE802154_FRAME_FCS_SIZE;

    return offset >= IEEE802154_FRAME_PHR_SIZE && offset <= mac_end && count <= mac_end - offset;
}

IEEE802154_STATIC IEEE802154_NOINLINE bool is_dst_panid_present(const uint8_t *frame)
{
    uint8_t dst_mode = dst_addr_mode(frame);
    uint8_t src_mode = src_addr_mode(frame);
    bool dst_panid_present = false;
    bool panid_compression = is_panid_compression(frame);
    IEEE802154_RETURN_ON_FALSE_SILENTLY(is_valid_dst_mode(dst_mode) && is_valid_src_mode(src_mode), false);

    if (ieee802154_frame_get_version(frame) == IEEE802154_FRAME_VERSION_2) {

        if (dst_mode != IEEE802154_FRAME_DST_MODE_NONE) { // dest address is present/short/extended
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

IEEE802154_STATIC IEEE802154_NOINLINE bool is_src_panid_present(const uint8_t *frame)
{
    uint8_t dst_mode = dst_addr_mode(frame);
    uint8_t src_mode = src_addr_mode(frame);
    bool src_panid_present = false;
    bool panid_compression = is_panid_compression(frame);
    IEEE802154_RETURN_ON_FALSE_SILENTLY(is_valid_dst_mode(dst_mode) && is_valid_src_mode(src_mode), false);

    if (ieee802154_frame_get_version(frame) == IEEE802154_FRAME_VERSION_2) {
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

uint8_t IEEE802154_INLINE ieee802154_frame_get_type(const uint8_t *frame)
{
    return frame[IEEE802154_FRAME_TYPE_OFFSET] & IEEE802154_FRAME_TYPE_MASK;
}

uint8_t IEEE802154_INLINE ieee802154_frame_get_version(const uint8_t *frame)
{
    return frame[IEEE802154_FRAME_VERSION_OFFSET] & IEEE802154_FRAME_VERSION_MASK;
}

IEEE802154_STATIC uint8_t IEEE802154_INLINE ieee802154_frame_address_offset(const uint8_t *frame)
{
    return IEEE802154_FRAME_PHR_SIZE + IEEE802154_FRAME_FCF_SIZE + (is_dsn_present(frame) ? IEEE802154_FRAME_DSN_SIZE : 0);
}

IEEE802154_STATIC IRAM_ATTR uint8_t ieee802154_frame_address_size(const uint8_t *frame)
{
    uint8_t address_size = 0;
    uint8_t dst_mode = dst_addr_mode(frame);
    uint8_t src_mode = src_addr_mode(frame);

    ESP_RETURN_ON_FALSE_ISR(is_valid_src_mode(src_mode) && is_valid_dst_mode(dst_mode), IEEE802154_FRAME_INVALID_VALUE, IEEE802154_TAG, "invalid address mode");


    if (is_dst_panid_present(frame)) {
        address_size += IEEE802154_FRAME_PANID_SIZE;
    }

    switch (dst_mode) {
    case IEEE802154_FRAME_DST_MODE_NONE:
        break;

    case IEEE802154_FRAME_DST_MODE_SHORT:
        address_size += IEEE802154_FRAME_SHORT_ADDR_SIZE;
        break;

    case IEEE802154_FRAME_DST_MODE_EXT:
        address_size += IEEE802154_FRAME_EXT_ADDR_SIZE;
        break;

    default:
        break;
    }

    if (is_src_panid_present(frame)) {
        address_size += IEEE802154_FRAME_PANID_SIZE;
    }

    switch (src_mode) {
    case IEEE802154_FRAME_SRC_MODE_NONE:
        break;

    case IEEE802154_FRAME_SRC_MODE_SHORT:
        address_size += IEEE802154_FRAME_SHORT_ADDR_SIZE;
        break;

    case IEEE802154_FRAME_SRC_MODE_EXT:
        address_size += IEEE802154_FRAME_EXT_ADDR_SIZE;
        break;

    default:
        break;
    }

    return address_size;
}

IEEE802154_STATIC IEEE802154_NOINLINE uint8_t ieee802154_frame_security_header_offset(const uint8_t *frame)
{
    IEEE802154_RETURN_ON_FALSE_SILENTLY(frame_has_mac_bytes(frame, IEEE802154_FRAME_PHR_SIZE, IEEE802154_FRAME_FCF_SIZE), IEEE802154_FRAME_INVALID_VALUE);
    IEEE802154_RETURN_ON_FALSE_SILENTLY(ieee802154_is_supported_frame_type(ieee802154_frame_get_type(frame)), IEEE802154_FRAME_INVALID_VALUE);
    uint8_t offset = ieee802154_frame_address_offset(frame);
    uint8_t address_size = ieee802154_frame_address_size(frame);
    IEEE802154_RETURN_ON_FALSE_SILENTLY(frame_has_mac_bytes(frame, offset, address_size), IEEE802154_FRAME_INVALID_VALUE);

    return offset + address_size;
}

// Security must be enabled, and offset must come from a successful ieee802154_frame_security_header_offset() call.
IEEE802154_STATIC IEEE802154_NOINLINE uint8_t ieee802154_frame_get_security_field_len(const uint8_t *frame, uint8_t offset)
{
    uint8_t security_field_len = 0;

    IEEE802154_RETURN_ON_FALSE_SILENTLY(frame_has_mac_bytes(frame, offset, IEEE802154_FRAME_SE_HEAD_SIZE), IEEE802154_FRAME_INVALID_VALUE);

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

    IEEE802154_RETURN_ON_FALSE_SILENTLY(frame_has_mac_bytes(frame, offset, security_field_len), IEEE802154_FRAME_INVALID_VALUE);

    return security_field_len;
}

IEEE802154_STATIC uint8_t ieee802154_frame_get_mic_len(uint8_t security_header)
{
    uint8_t mic_len = 0;

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

// offset is the validated IE-header offset; mic_len is decoded from the validated security header.
IEEE802154_STATIC uint8_t ieee802154_frame_get_ie_field_len(const uint8_t *frame, uint8_t offset, uint8_t mic_len)
{
    uint8_t frame_len = frame[0] & IEEE802154_FRAME_MAX_LEN;
    uint8_t ie_field_len = 0;
    uint8_t frame_footer_len = mic_len + IEEE802154_FRAME_FCS_SIZE;
    IEEE802154_RETURN_ON_FALSE_SILENTLY(offset + frame_footer_len <= IEEE802154_FRAME_PHR_SIZE + frame_len, IEEE802154_FRAME_INVALID_VALUE);

    const uint8_t ie_end = IEEE802154_FRAME_PHR_SIZE + frame_len - frame_footer_len;

    /* If the `offset + frame_footer_len == frame_len`, we exit the `while()`
       loop. This covers the case where frame contains one or more Header IEs
       but no data payload. In this case, 2015-spec does not
       require Header IE termination to be included (it is optional)
       since the end of frame can be determined from frame length and
       footer length. (for details, please reference 2015 - spec table 7 - 6 in page 169) */

    while (offset + ie_field_len < ie_end) {
        uint8_t ie_offset = offset + ie_field_len;
        IEEE802154_RETURN_ON_FALSE_SILENTLY(IEEE802154_FRAME_IE_HEAD_LEN <= ie_end - ie_offset, IEEE802154_FRAME_INVALID_VALUE);

        uint16_t ie_header = frame[ie_offset + 1] << 8 | frame[ie_offset];
        // Header Termination IE 2 is used in to signal end of the MHR and beginning of the MAC Payload.
        if ((ie_header & IEEE802154_FRAME_IE_HEAD_ID_MASK) == IEEE802154_IE_TYPE_HT2) {
            ie_field_len += IEEE802154_FRAME_IE_HEAD_LEN;
            break;
        } else {
            uint8_t ie_subfield_len = ie_header & IEEE802154_FRAME_IE_SUBFIELD_LEN_MASK;
            uint8_t ie_subfield_size = IEEE802154_FRAME_IE_HEAD_LEN + ie_subfield_len;

            IEEE802154_RETURN_ON_FALSE_SILENTLY(ie_subfield_size <= ie_end - ie_offset, IEEE802154_FRAME_INVALID_VALUE);
            ie_field_len += ie_subfield_size;
        }
    }

    return ie_field_len;
}

IEEE802154_STATIC IRAM_ATTR uint8_t ieee802154_frame_payload_offset(const uint8_t *frame)
{
    uint8_t security_offset = ieee802154_frame_security_header_offset(frame);
    IEEE802154_RETURN_ON_FALSE_SILENTLY(security_offset != IEEE802154_FRAME_INVALID_VALUE, IEEE802154_FRAME_INVALID_VALUE);
    uint8_t offset = security_offset;
    if (ieee802154_frame_is_security_enabled(frame)) {
        // skip security field.
        uint8_t security_field_len = ieee802154_frame_get_security_field_len(frame, security_offset);
        IEEE802154_RETURN_ON_FALSE_SILENTLY(security_field_len != IEEE802154_FRAME_INVALID_VALUE, IEEE802154_FRAME_INVALID_VALUE);
        offset += security_field_len;
    }

    if (ieee802154_frame_get_version(frame) == IEEE802154_FRAME_VERSION_2 && is_ie_present(frame)) {
        // skip IE fields.
        uint8_t mic_len = ieee802154_frame_is_security_enabled(frame) ? ieee802154_frame_get_mic_len(frame[security_offset]) : 0;
        uint8_t ie_field_len = ieee802154_frame_get_ie_field_len(frame, offset, mic_len);
        IEEE802154_RETURN_ON_FALSE_SILENTLY(ie_field_len != IEEE802154_FRAME_INVALID_VALUE, IEEE802154_FRAME_INVALID_VALUE);
        offset += ie_field_len;
    }
    if (ieee802154_frame_get_version(frame) == IEEE802154_FRAME_VERSION_0 || ieee802154_frame_get_version(frame) == IEEE802154_FRAME_VERSION_1) {
        if (ieee802154_frame_get_type(frame) == IEEE802154_FRAME_TYPE_BEACON) {
            // TODO: skip beacon payload.
        }
        if (ieee802154_frame_get_type(frame) == IEEE802154_FRAME_TYPE_COMMAND) {
            IEEE802154_RETURN_ON_FALSE_SILENTLY(frame_has_mac_bytes(frame, offset, IEEE802154_FRAME_COMMAND_ID_LEN), IEEE802154_FRAME_INVALID_VALUE);
            offset += IEEE802154_FRAME_COMMAND_ID_LEN;
        }
    }

    return offset - 1;
}

bool ieee802154_is_data_request(const uint8_t *frame)
{
    IEEE802154_RETURN_ON_FALSE_SILENTLY(frame_has_mac_bytes(frame, IEEE802154_FRAME_PHR_SIZE, IEEE802154_FRAME_FCF_SIZE), false);
    if (ieee802154_frame_get_type(frame) != IEEE802154_FRAME_TYPE_COMMAND) {
        return false;
    }
    uint8_t security_offset = ieee802154_frame_security_header_offset(frame);
    IEEE802154_RETURN_ON_FALSE_SILENTLY(security_offset != IEEE802154_FRAME_INVALID_VALUE, false);
    uint8_t offset = security_offset;
    if (ieee802154_frame_is_security_enabled(frame)) {
        // skip security field.
        uint8_t security_field_len = ieee802154_frame_get_security_field_len(frame, security_offset);
        IEEE802154_RETURN_ON_FALSE_SILENTLY(security_field_len != IEEE802154_FRAME_INVALID_VALUE, false);
        offset += security_field_len;
    }

    if (ieee802154_frame_get_version(frame) == IEEE802154_FRAME_VERSION_2 && is_ie_present(frame)) {
        // skip IE fields.
        uint8_t mic_len = ieee802154_frame_is_security_enabled(frame) ? ieee802154_frame_get_mic_len(frame[security_offset]) : 0;
        uint8_t ie_field_len = ieee802154_frame_get_ie_field_len(frame, offset, mic_len);
        IEEE802154_RETURN_ON_FALSE_SILENTLY(ie_field_len != IEEE802154_FRAME_INVALID_VALUE, false);
        offset += ie_field_len;
    }
    IEEE802154_RETURN_ON_FALSE_SILENTLY(frame_has_mac_bytes(frame, offset, IEEE802154_FRAME_COMMAND_ID_LEN), false);
    if (frame[offset] == IEEE802154_CMD_DATA_REQ) {
        return true;
    }

    return false;
}

bool IEEE802154_INLINE ieee802154_frame_is_ack_required(const uint8_t *frame)
{
    IEEE802154_RETURN_ON_FALSE_SILENTLY(frame_has_mac_bytes(frame, IEEE802154_FRAME_PHR_SIZE, IEEE802154_FRAME_FCF_SIZE), false);
    IEEE802154_RETURN_ON_FALSE_SILENTLY(ieee802154_is_supported_frame_type(ieee802154_frame_get_type(frame)), false);
    return (frame[IEEE802154_FRAME_AR_OFFSET] & IEEE802154_FRAME_AR_BIT);
}

uint8_t ieee802154_frame_get_dst_addr(const uint8_t *frame, uint8_t *addr)
{
    IEEE802154_RETURN_ON_FALSE_SILENTLY(frame_has_mac_bytes(frame, IEEE802154_FRAME_PHR_SIZE, IEEE802154_FRAME_FCF_SIZE), IEEE802154_FRAME_INVALID_VALUE);
    ESP_RETURN_ON_FALSE_ISR(ieee802154_is_supported_frame_type(ieee802154_frame_get_type(frame)), IEEE802154_FRAME_INVALID_VALUE, IEEE802154_TAG, "invalid frame type");

    uint8_t offset = ieee802154_frame_address_offset(frame);
    uint8_t dst_mode = dst_addr_mode(frame);
    uint8_t addr_size = 0;

    ESP_RETURN_ON_FALSE_ISR(is_valid_dst_mode(dst_mode), IEEE802154_FRAME_INVALID_VALUE, IEEE802154_TAG, "invalid destination address mode");
    IEEE802154_RETURN_ON_FALSE_SILENTLY(dst_mode != IEEE802154_FRAME_DST_MODE_NONE, IEEE802154_FRAME_DST_MODE_NONE);
    addr_size = (dst_mode == IEEE802154_FRAME_DST_MODE_SHORT) ? IEEE802154_FRAME_SHORT_ADDR_SIZE : IEEE802154_FRAME_EXT_ADDR_SIZE;

    if (is_dst_panid_present(frame)) {
        offset += IEEE802154_FRAME_PANID_SIZE;
    }

    IEEE802154_RETURN_ON_FALSE_SILENTLY(frame_has_mac_bytes(frame, offset, addr_size), IEEE802154_FRAME_INVALID_VALUE);
    memcpy(addr, frame + offset, addr_size);

    return dst_mode;
}

uint8_t ieee802154_frame_get_src_addr(const uint8_t *frame, uint8_t *addr)
{
    IEEE802154_RETURN_ON_FALSE_SILENTLY(frame_has_mac_bytes(frame, IEEE802154_FRAME_PHR_SIZE, IEEE802154_FRAME_FCF_SIZE), IEEE802154_FRAME_INVALID_VALUE);
    ESP_RETURN_ON_FALSE_ISR(ieee802154_is_supported_frame_type(ieee802154_frame_get_type(frame)), IEEE802154_FRAME_INVALID_VALUE, IEEE802154_TAG, "invalid frame type");

    uint8_t offset = ieee802154_frame_address_offset(frame);
    uint8_t dst_mode = dst_addr_mode(frame);
    uint8_t src_mode = src_addr_mode(frame);
    uint8_t addr_size = 0;

    ESP_RETURN_ON_FALSE_ISR(is_valid_src_mode(src_mode) && is_valid_dst_mode(dst_mode), IEEE802154_FRAME_INVALID_VALUE, IEEE802154_TAG, "invalid address mode");

    IEEE802154_RETURN_ON_FALSE_SILENTLY(src_mode != IEEE802154_FRAME_SRC_MODE_NONE, IEEE802154_FRAME_SRC_MODE_NONE);
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

    IEEE802154_RETURN_ON_FALSE_SILENTLY(frame_has_mac_bytes(frame, offset, addr_size), IEEE802154_FRAME_INVALID_VALUE);
    memcpy(addr, frame + offset, addr_size);

    return src_mode;
}

uint8_t ieee802154_frame_get_security_payload_offset(uint8_t *frame)
{
    uint8_t offset = ieee802154_frame_payload_offset(frame);
    ESP_RETURN_ON_FALSE_ISR(offset != IEEE802154_FRAME_INVALID_VALUE, IEEE802154_FRAME_INVALID_VALUE, IEEE802154_TAG, "invalid offset");
    return offset;
}

esp_err_t ieee802154_frame_get_dest_panid(const uint8_t *frame, uint8_t *panid)
{
    IEEE802154_RETURN_ON_FALSE_SILENTLY(frame_has_mac_bytes(frame, IEEE802154_FRAME_PHR_SIZE, IEEE802154_FRAME_FCF_SIZE), ESP_FAIL);
    uint8_t offset = ieee802154_frame_address_offset(frame);

    if (is_dst_panid_present(frame)) {
        IEEE802154_RETURN_ON_FALSE_SILENTLY(frame_has_mac_bytes(frame, offset, IEEE802154_FRAME_PANID_SIZE), ESP_FAIL);
        memcpy(panid, frame + offset, IEEE802154_FRAME_PANID_SIZE);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t ieee802154_frame_get_src_panid(const uint8_t *frame, uint8_t *panid)
{
    IEEE802154_RETURN_ON_FALSE_SILENTLY(frame_has_mac_bytes(frame, IEEE802154_FRAME_PHR_SIZE, IEEE802154_FRAME_FCF_SIZE), ESP_FAIL);
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
        IEEE802154_RETURN_ON_FALSE_SILENTLY(frame_has_mac_bytes(frame, offset, IEEE802154_FRAME_PANID_SIZE), ESP_FAIL);
        memcpy(panid, frame + offset, IEEE802154_FRAME_PANID_SIZE);
        return ESP_OK;
    }
    return ESP_FAIL;
}
