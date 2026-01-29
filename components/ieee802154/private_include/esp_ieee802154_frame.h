/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IEEE802154_FRAME_MAX_LEN                127

#define IEEE802154_FRAME_INVALID_OFFSET         0xff
#define IEEE802154_FRAME_INVALID_ADDR_MODE      0xff

#define IEEE802154_FRAME_TYPE_OFFSET            1
#define IEEE802154_FRAME_TYPE_MASK              0x07
#define IEEE802154_FRAME_TYPE_BEACON            0x00
#define IEEE802154_FRAME_TYPE_DATA              0x01
#define IEEE802154_FRAME_TYPE_ACK               0x02
#define IEEE802154_FRAME_TYPE_COMMAND           0x03
#define IEEE802154_FRAME_TYPE_MULTIPURPOSE      0x05
#define IEEE802154_FRAME_TYPE_FRAGMENT          0x06
#define IEEE802154_FRAME_TYPE_EXTENDED          0x07

#define IEEE802154_FRAME_SECURITY_OFFSET        1
#define IEEE802154_FRAME_SECURITY_BIT           BIT(3)
#define IEEE802154_FRAME_SECURITY_MASK          0x07
#define IEEE802154_FRAME_SECURITY_MIC_32        0x01
#define IEEE802154_FRAME_SECURITY_MIC_64        0x02
#define IEEE802154_FRAME_SECURITY_MIC_128       0x03
#define IEEE802154_FRAME_SECURITY_ENC_MIC_32    0x05
#define IEEE802154_FRAME_SECURITY_ENC_MIC_64    0x06
#define IEEE802154_FRAME_SECURITY_ENC_MIC_128   0x07
#define IEEE802154_FRAME_SECURITY_MIC_32_SIZE   4
#define IEEE802154_FRAME_SECURITY_MIC_64_SIZE   8
#define IEEE802154_FRAME_SECURITY_MIC_128_SIZE  16

#define IEEE802154_FRAME_KEY_ID_MODE_MASK       0x18
#define IEEE802154_FRAME_KEY_ID_MODE_1          0x08
#define IEEE802154_FRAME_KEY_ID_MODE_2          0x10
#define IEEE802154_FRAME_KEY_ID_MODE_3          0x18
#define IEEE802154_FRAME_KEY_ID_MODE_1_SIZE     1
#define IEEE802154_FRAME_KEY_ID_MODE_2_SIZE     5
#define IEEE802154_FRAME_KEY_ID_MODE_3_SIZE     9

#define IEEE802154_FRAME_COUNTER_SUPPRESS_BIT   0x20
#define IEEE802154_FRAME_COUNTER_SIZE           4

#define IEEE802154_FRAME_AR_OFFSET              1
#define IEEE802154_FRAME_AR_BIT                 BIT(5)

#define IEEE802154_FRAME_PANID_COMP_OFFSET      1
#define IEEE802154_FRAME_PANID_COMP_BIT         BIT(6)

#define IEEE802154_FRAME_VERSION_OFFSET         2
#define IEEE802154_FRAME_VERSION_MASK           0x30
#define IEEE802154_FRAME_VERSION_0              0x00 // IEEE 802.15.4 - 2003
#define IEEE802154_FRAME_VERSION_1              0x10 // IEEE 802.15.4 - 2006 & 2011
#define IEEE802154_FRAME_VERSION_2              0x20 // IEEE 802.15.4 - 2015
#define IEEE802154_FRAME_VERSION_R              0x30 // Reserved

#define IEEE802154_FRAME_DSN_OFFSET             2
#define IEEE802154_FRAME_DSN_BIT                BIT(0)
#define IEEE802154_FRAME_IE_OFFSET              2
#define IEEE802154_FRAME_IE_BIT                 BIT(1)

#define IEEE802154_FRAME_IE_HEAD_ID_MASK        0x3f80
#define IEEE802154_IE_TYPE_HT2                  0x3f80
#define IEEE802154_FRAME_IE_SUBFIELD_LEN_MASK   0x007f
#define IEEE802154_FRAME_IE_HEAD_LEN            2

#define IEEE802154_FRAME_DST_MODE_OFFSET        2
#define IEEE802154_FRAME_DST_MODE_MASK          0x0C
#define IEEE802154_FRAME_DST_MODE_NONE          0x00
#define IEEE802154_FRAME_DST_MODE_SHORT         0x08
#define IEEE802154_FRAME_DST_MODE_EXT           0x0C

#define IEEE802154_FRAME_SRC_MODE_OFFSET        2
#define IEEE802154_FRAME_SRC_MODE_MASK          0xC0
#define IEEE802154_FRAME_SRC_MODE_NONE          0x00
#define IEEE802154_FRAME_SRC_MODE_SHORT         0x80
#define IEEE802154_FRAME_SRC_MODE_EXT           0xC0

#define IEEE802154_CMD_INVALID                  0x00
#define IEEE802154_CMD_DATA_REQ                 0x04

#define IEEE802154_FRAME_PHR_SIZE               1
#define IEEE802154_FRAME_FCF_SIZE               2
#define IEEE802154_FRAME_FCS_SIZE               2
#define IEEE802154_FRAME_DSN_SIZE               1
#define IEEE802154_FRAME_PANID_SIZE             2
#define IEEE802154_FRAME_SHORT_ADDR_SIZE        2
#define IEEE802154_FRAME_EXT_ADDR_SIZE          8
#define IEEE802154_FRAME_SE_HEAD_SIZE           1
#define IEEE802154_FRAME_COMMAND_ID_LEN         1

static inline bool ieee802154_is_supported_frame_type(uint8_t frame_type)
{
    // HW supports 4 kinds of frame type: Beacon, Ack, Data, Command.
    return (frame_type == IEEE802154_FRAME_TYPE_BEACON || frame_type == IEEE802154_FRAME_TYPE_DATA ||
            frame_type == IEEE802154_FRAME_TYPE_ACK || frame_type == IEEE802154_FRAME_TYPE_COMMAND);
}

/**
 * @brief  Get the frame type.
 *
 * @param[in]  frame  The pointer to the frame.
 *
 * @return
 *      - The type of the frame.
 *
 */
uint8_t ieee802154_frame_get_type(const uint8_t *frame);

/**
 * @brief  Get the frame version.
 *
 * @param[in]  frame  The pointer to the frame.
 *
 * @return
 *      - The version of the frame.
 *
 */
uint8_t ieee802154_frame_get_version(const uint8_t *frame);

/**
 * @brief  Is the frame ack required.
 *
 * @param[in]  frame  The pointer to the frame.
 *
 * @return
 *      - True if the frame is ack required, otherwise false.
 *
 */
bool ieee802154_frame_is_ack_required(const uint8_t *frame);

/**
 * @brief  Is the frame security enabled.
 *
 * @param[in]  frame  The pointer to the frame.
 *
 * @return
 *      - True if the frame is security enabled, otherwise false.
 *
 */
bool ieee802154_frame_is_security_enabled(const uint8_t *frame);
/**
 * @brief  Get the destination address of the frame.
 *
 * @param[in]  frame  The pointer to the frame.
 * @param[out] addr   The pointer to the address.
 *
 * @return
 *      - IEEE802154_FRAME_DST_MODE_NONE    if destination address mode is none.
 *      - IEEE802154_FRAME_DST_MODE_SHORT   if destination address mode is short.
 *      - IEEE802154_FRAME_DST_MODE_EXT     if destination address mode is extended.
 *
 */
uint8_t ieee802154_frame_get_dst_addr(const uint8_t *frame, uint8_t *addr);

/**
 * @brief  Get the source address of the frame.
 *
 * @param[in]  frame  The pointer to the frame.
 * @param[out] addr   The pointer to the address.
 *
 * @return
 *      - IEEE802154_FRAME_SRC_MODE_NONE    if source address mode is none.
 *      - IEEE802154_FRAME_SRC_MODE_SHORT   if source address mode is short.
 *      - IEEE802154_FRAME_SRC_MODE_EXT     if source address mode is extended.
 *
 */
uint8_t ieee802154_frame_get_src_addr(const uint8_t *frame, uint8_t *addr);

/**
 * @brief  Get the offset of the private payload.
 *
 * @param[in]  frame  The pointer to the frame.
 *
 * @return
 *      - The offset of the private payload
 *
 */
uint8_t ieee802154_frame_get_security_payload_offset(uint8_t *frame);

/**
 * @brief  Get the destination PAN ID of the frame.
 *
 * @param[in]  frame  The pointer to the frame.
 * @param[out] panid  The pointer to the destination PAN ID.
 *
 * @return
 *      - ESP_OK if destination PAN ID is present, otherwise ESP_FAIL.
 *
 */
esp_err_t ieee802154_frame_get_dest_panid(const uint8_t *frame, uint8_t *panid);

/**
 * @brief  Get the source PAN ID of the frame.
 *
 * @param[in]  frame  The pointer to the frame.
 * @param[out] panid  The pointer to the source PAN ID.
 *
 * @return
 *      - ESP_OK if source PAN ID is present, otherwise ESP_FAIL.
 *
 */
esp_err_t ieee802154_frame_get_src_panid(const uint8_t *frame, uint8_t *panid);

/**
 * @brief Check whether the given frame is a MAC Data Request command.
 *
 * @param[in] frame Pointer to the raw MAC frame buffer.
 *
 * @return true if the frame is a Data Request command, false otherwise.
 */
bool ieee802154_is_data_request(const uint8_t *frame);
#ifdef __cplusplus
}
#endif
