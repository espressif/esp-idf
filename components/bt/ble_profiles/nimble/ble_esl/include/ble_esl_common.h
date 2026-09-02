/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file ble_esl_common.h
 * @brief Electronic Shelf Label (ESL) Service v1.0.1 — Common Protocol Definitions
 *
 * Shared protocol artifacts used by both AP (Access Point) and ESL roles:
 * service/characteristic UUIDs, TLV wire format, command/response opcodes,
 * error codes, data format structures, and cross-layer encode/decode utilities.
 *
 * All multi-byte fields are little-endian. Bit fields within a byte use
 * LSB-first numbering unless stated otherwise.
 */

#ifndef BLE_ESL_COMMON_H
#define BLE_ESL_COMMON_H

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================== ESL States ========================== */

/**
 * @brief ESL state enumeration
 */
typedef enum {
    BLE_ESL_STATE_UNASSOCIATED  = 0, /*!< ESL not yet configured by an AP */
    BLE_ESL_STATE_CONFIGURING   = 1, /*!< ESL connected and being configured for the first time */
    BLE_ESL_STATE_SYNCHRONIZED  = 2, /*!< ESL receiving commands via PAwR periodic advertising */
    BLE_ESL_STATE_UPDATING      = 3, /*!< ESL connected for reconfiguration / image transfer */
    BLE_ESL_STATE_UNSYNCHRONIZED = 4, /*!< ESL associated but not connected and not synchronized */
} ble_esl_state_t;

/* ========================== Service & Characteristic UUIDs ========================== */

#define BLE_ESL_SVC_UUID                    0x1857  /*!< Electronic Shelf Label Service UUID */

#define BLE_ESL_CHR_UUID_ESL_ADDRESS        0x2BF6  /*!< ESL Address characteristic UUID */
#define BLE_ESL_CHR_UUID_AP_SYNC_KEY        0x2BF7  /*!< AP Sync Key Material characteristic UUID */
#define BLE_ESL_CHR_UUID_RESP_KEY           0x2BF8  /*!< ESL Response Key Material characteristic UUID */
#define BLE_ESL_CHR_UUID_CURRENT_ABS_TIME   0x2BF9  /*!< ESL Current Absolute Time characteristic UUID */
#define BLE_ESL_CHR_UUID_DISPLAY_INFO       0x2BFA  /*!< ESL Display Information characteristic UUID */
#define BLE_ESL_CHR_UUID_IMAGE_INFO         0x2BFB  /*!< ESL Image Information characteristic UUID */
#define BLE_ESL_CHR_UUID_SENSOR_INFO        0x2BFC  /*!< ESL Sensor Information characteristic UUID */
#define BLE_ESL_CHR_UUID_LED_INFO           0x2BFD  /*!< ESL LED Information characteristic UUID */
#define BLE_ESL_CHR_UUID_ECP                0x2BFE  /*!< ESL Control Point (ECP) characteristic UUID */

/* ========================== Command Opcodes ========================== */

#define BLE_ESL_CMD_PING                    0x00    /*!< Ping command */
#define BLE_ESL_CMD_UNASSOCIATE             0x01    /*!< Unassociate from AP command */
#define BLE_ESL_CMD_SERVICE_RESET           0x02    /*!< Service Reset command */
#define BLE_ESL_CMD_FACTORY_RESET           0x03    /*!< Factory Reset command */
#define BLE_ESL_CMD_UPDATE_COMPLETE         0x04    /*!< Update Complete command */
#define BLE_ESL_CMD_READ_SENSOR             0x10    /*!< Read Sensor Data command */
#define BLE_ESL_CMD_REFRESH_DISPLAY         0x11    /*!< Refresh Display command */
#define BLE_ESL_CMD_DISPLAY_IMAGE           0x20    /*!< Display Image command */
#define BLE_ESL_CMD_DISPLAY_TIMED_IMAGE     0x60    /*!< Display Timed Image command */
#define BLE_ESL_CMD_LED_CONTROL             0xB0    /*!< LED Control command */
#define BLE_ESL_CMD_LED_TIMED_CONTROL       0xF0    /*!< LED Timed Control command */

/** @brief Vendor-specific command tag nibble. Any opcode with Tag=0xF is vendor-specific. */
#define BLE_ESL_CMD_VENDOR_TAG              0x0F

/** @brief ESL Broadcast Address (addresses all ESLs in a group) */
#define BLE_ESL_BROADCAST_ADDRESS           0xFF

/* ========================== Response Opcodes ========================== */

#define BLE_ESL_RESP_ERROR                  0x00    /*!< Error Response */
#define BLE_ESL_RESP_LED_STATE              0x01    /*!< LED State Response */
#define BLE_ESL_RESP_BASIC_STATE            0x10    /*!< Basic State Response */
#define BLE_ESL_RESP_DISPLAY_STATE          0x11    /*!< Display State Response */

/** @brief Sensor Value Response tag nibble (0xE). Opcode = (Length << 4) | 0x0E */
#define BLE_ESL_RESP_SENSOR_VALUE_TAG       0x0E

/** @brief Vendor-specific Response tag nibble (0xF). Opcode = (Length << 4) | 0x0F */
#define BLE_ESL_RESP_VENDOR_TAG             0x0F

/* ========================== Error Codes ========================== */

#define BLE_ESL_ERR_UNSPECIFIED             0x01    /*!< Unspecified Error */
#define BLE_ESL_ERR_INVALID_OPCODE          0x02    /*!< Invalid Opcode */
#define BLE_ESL_ERR_INVALID_STATE           0x03    /*!< Invalid State */
#define BLE_ESL_ERR_INVALID_IMAGE_INDEX     0x04    /*!< Invalid Image_Index */
#define BLE_ESL_ERR_IMAGE_NOT_AVAILABLE     0x05    /*!< Image Not Available */
#define BLE_ESL_ERR_INVALID_PARAMS          0x06    /*!< Invalid Parameter(s) */
#define BLE_ESL_ERR_CAPACITY_LIMIT          0x07    /*!< Capacity Limit */
#define BLE_ESL_ERR_INSUFFICIENT_BATTERY    0x08    /*!< Insufficient Battery */
#define BLE_ESL_ERR_INSUFFICIENT_RESOURCES  0x09    /*!< Insufficient Resources */
#define BLE_ESL_ERR_RETRY                   0x0A    /*!< Retry */
#define BLE_ESL_ERR_QUEUE_FULL              0x0B    /*!< Queue Full */
#define BLE_ESL_ERR_IMPLAUSIBLE_ABS_TIME    0x0C    /*!< Implausible Absolute Time */

/* ========================== Basic State Bitmap Bits ========================== */

#define BLE_ESL_BASIC_STATE_SERVICE_NEEDED      (1 << 0)  /*!< Bit 0: Service Needed */
#define BLE_ESL_BASIC_STATE_SYNCHRONIZED        (1 << 1)  /*!< Bit 1: Synchronized */
#define BLE_ESL_BASIC_STATE_ACTIVE_LED          (1 << 2)  /*!< Bit 2: Active LED */
#define BLE_ESL_BASIC_STATE_PENDING_LED_UPDATE  (1 << 3)  /*!< Bit 3: Pending LED Update */
#define BLE_ESL_BASIC_STATE_PENDING_DISP_UPDATE (1 << 4)  /*!< Bit 4: Pending Display Update */

/* ========================== TLV Format Helpers ========================== */

/** @brief Extract the Tag nibble (bits [3:0]) from an opcode byte */
#define BLE_ESL_TLV_TAG(opcode)             ((opcode) & 0x0F)

/** @brief Extract the Length nibble (bits [7:4]) from an opcode byte */
#define BLE_ESL_TLV_LENGTH(opcode)          (((opcode) >> 4) & 0x0F)

/** @brief Compute the parameter byte count from an opcode: Length + 1 */
#define BLE_ESL_TLV_PARAMS_LEN(opcode)      (BLE_ESL_TLV_LENGTH(opcode) + 1)

/** @brief Compute the total TLV size from an opcode: Length + 2 */
#define BLE_ESL_TLV_TOTAL_LEN(opcode)       (BLE_ESL_TLV_LENGTH(opcode) + 2)

/** @brief Build an opcode byte from tag and length nibbles */
#define BLE_ESL_TLV_OPCODE(tag, length)     ((uint8_t)(((length) << 4) | ((tag) & 0x0F)))

/** @brief Maximum TLV total size (Length nibble = 0xF → 15+2 = 17) */
#define BLE_ESL_TLV_MAX_SIZE                17

/** @brief Minimum TLV total size (Length nibble = 0x0 → 0+2 = 2) */
#define BLE_ESL_TLV_MIN_SIZE                2

/* ========================== ESL Payload ========================== */

/** @brief Maximum ESL Payload size in octets */
#define BLE_ESL_PAYLOAD_MAX_SIZE            48

/** @brief Maximum Group_ID value (7-bit field) */
#define BLE_ESL_GROUP_ID_MAX                0x7F

/* ========================== Advertising Data Types ========================== */

#define BLE_ESL_AD_TYPE_ENCRYPTED_DATA      0x31    /*!< Encrypted Data AD type */
#define BLE_ESL_AD_TYPE_ESL                 0x34    /*!< ESL data AD type */

/* ========================== ECP Timeout ========================== */

/** @brief ECP Procedure Timeout in seconds */
#define BLE_ESL_ECP_TIMEOUT_SEC             30

/* ========================== Display Type Enumeration ========================== */

#define BLE_ESL_DISPLAY_TYPE_BLACK_WHITE            0x01  /*!< Black White */
#define BLE_ESL_DISPLAY_TYPE_THREE_GRAY             0x02  /*!< Three Gray Scale */
#define BLE_ESL_DISPLAY_TYPE_FOUR_GRAY              0x03  /*!< Four Gray Scale */
#define BLE_ESL_DISPLAY_TYPE_EIGHT_GRAY             0x04  /*!< Eight Gray Scale */
#define BLE_ESL_DISPLAY_TYPE_SIXTEEN_GRAY           0x05  /*!< Sixteen Gray Scale */
#define BLE_ESL_DISPLAY_TYPE_RED_BLACK_WHITE        0x06  /*!< Red Black White */
#define BLE_ESL_DISPLAY_TYPE_YELLOW_BLACK_WHITE     0x07  /*!< Yellow Black White */
#define BLE_ESL_DISPLAY_TYPE_RED_YELLOW_BLACK_WHITE 0x08  /*!< Red Yellow Black White */
#define BLE_ESL_DISPLAY_TYPE_SEVEN_COLOR            0x09  /*!< Seven Color */
#define BLE_ESL_DISPLAY_TYPE_SIXTEEN_COLOR          0x0A  /*!< Sixteen Color */
#define BLE_ESL_DISPLAY_TYPE_FULL_RGB               0x0B  /*!< Full RGB */

/* ========================== LED Information Bit Fields ========================== */

#define BLE_ESL_LED_INFO_RED_MASK           0x03    /*!< Bits [1:0]: Red component */
#define BLE_ESL_LED_INFO_RED_SHIFT          0
#define BLE_ESL_LED_INFO_GREEN_MASK         0x0C    /*!< Bits [3:2]: Green component */
#define BLE_ESL_LED_INFO_GREEN_SHIFT        2
#define BLE_ESL_LED_INFO_BLUE_MASK          0x30    /*!< Bits [5:4]: Blue component */
#define BLE_ESL_LED_INFO_BLUE_SHIFT         4
#define BLE_ESL_LED_INFO_TYPE_MASK          0xC0    /*!< Bits [7:6]: LED type */
#define BLE_ESL_LED_INFO_TYPE_SHIFT         6

/** @brief LED Type: sRGB multi-color */
#define BLE_ESL_LED_TYPE_SRGB               0x00
/** @brief LED Type: Monochrome single-color */
#define BLE_ESL_LED_TYPE_MONOCHROME         0x01

/* ========================== Sensor Information Sizes ========================== */

/** @brief Sensor Information short format size indicator */
#define BLE_ESL_SENSOR_INFO_SIZE_SHORT      0x00
/** @brief Sensor Information long format size indicator */
#define BLE_ESL_SENSOR_INFO_SIZE_LONG       0x01

/** @brief Sensor Information short format total octets */
#define BLE_ESL_SENSOR_INFO_SHORT_LEN       3
/** @brief Sensor Information long format total octets */
#define BLE_ESL_SENSOR_INFO_LONG_LEN        5

/* ========================== Key Material Size ========================== */

/** @brief Key Material total size (Session_Key 16 + IV 8) */
#define BLE_ESL_KEY_MATERIAL_SIZE           24
/** @brief Session Key size in octets */
#define BLE_ESL_SESSION_KEY_SIZE            16
/** @brief IV size in octets */
#define BLE_ESL_IV_SIZE                     8

/* ========================== Encryption Parameters ========================== */

/** @brief CCM Nonce size (Randomizer 5 + IV 8) */
#define BLE_ESL_CCM_NONCE_SIZE              13
/** @brief Randomizer size in octets */
#define BLE_ESL_RANDOMIZER_SIZE             5
/** @brief MIC size in octets */
#define BLE_ESL_MIC_SIZE                    4

/* ========================== OTS Object ID ========================== */

/** @brief Base Object ID for ESL images (48-bit): Object_ID = base + Image_Index */
#define BLE_ESL_OTS_OBJECT_ID_BASE          0x000000000100ULL

/* ========================== Implausible Time Threshold ========================== */

/** @brief Maximum future time considered plausible: 48 days in ms */
#define BLE_ESL_IMPLAUSIBLE_TIME_MS         4147200000UL

/* ========================== Data Structures ========================== */

/**
 * @brief ESL Address (2 octets)
 *
 * ESL_ID (bits 0-7) + Group_ID (bits 8-14) + RFU (bit 15)
 */
typedef struct __attribute__((packed)) {
    uint8_t esl_id;       /*!< ESL identifier (0x00-0xFE valid, 0xFF = broadcast) */
    uint8_t group_id_rfu; /*!< Group_ID in bits [6:0], RFU in bit 7 (shall be 0) */
} ble_esl_address_t;

/**
 * @brief Key Material (24 octets)
 *
 * 128-bit AES session key + 64-bit initialization vector.
 * Shared layout of the AP Sync Key Material and the ESL Response Key Material.
 */
typedef struct __attribute__((packed)) {
    uint8_t session_key[BLE_ESL_SESSION_KEY_SIZE]; /*!< 128-bit AES session key */
    uint8_t iv[BLE_ESL_IV_SIZE];                   /*!< 64-bit initialization vector */
} ble_esl_key_material_t;

/**
 * @brief ESL Current Absolute Time (4 octets)
 */
typedef uint32_t ble_esl_abs_time_t;

/**
 * @brief Display Data Structure (5 octets per display)
 */
typedef struct __attribute__((packed)) {
    uint16_t width;        /*!< Display width in pixels */
    uint16_t height;       /*!< Display height in pixels */
    uint8_t  display_type; /*!< Display type enumeration */
} ble_esl_display_info_t;

/**
 * @brief Image Information (1 octet)
 */
typedef struct __attribute__((packed)) {
    uint8_t max_image_index; /*!< Numerically highest Image_Index supported */
} ble_esl_image_info_t;

/**
 * @brief Sensor Information — Short format (3 octets)
 */
typedef struct __attribute__((packed)) {
    uint8_t  size;        /*!< 0x00 = short format */
    uint16_t sensor_type; /*!< 16-bit Property ID from Mesh Device Properties */
} ble_esl_sensor_info_short_t;

/**
 * @brief Sensor Information — Long format (5 octets)
 */
typedef struct __attribute__((packed)) {
    uint8_t  size;        /*!< 0x01 = long format */
    uint16_t company_id;  /*!< Bluetooth SIG assigned Company ID (bits 0-15 of Sensor_Type) */
    uint16_t sensor_code; /*!< Vendor-assigned sensor type code (bits 16-31 of Sensor_Type) */
} ble_esl_sensor_info_long_t;

/**
 * @brief Flashing Pattern (56 bits = 7 octets)
 *
 * Pattern (40 bits) + Bit_Off_Period (8 bits) + Bit_On_Period (8 bits)
 * Stored in little-endian byte order.
 */
typedef struct __attribute__((packed)) {
    uint8_t pattern[5];     /*!< 40-bit on/off pattern (MSB examined first) */
    uint8_t bit_off_period; /*!< Off-time per bit = value × 2 ms (2-510 ms, 0 invalid) */
    uint8_t bit_on_period;  /*!< On-time per bit = value × 2 ms (2-510 ms, 0 invalid) */
} ble_esl_flashing_pattern_t;

/**
 * @brief LED Control Settings
 *
 * Contains color, brightness, flashing pattern, and repeat configuration.
 * Total: 10 octets (1 byte color+brightness + 7 bytes flashing + 2 bytes repeat)
 */
typedef struct __attribute__((packed)) {
    uint8_t color_brightness;                /*!< Red[1:0], Green[3:2], Blue[5:4], Brightness[7:6] */
    ble_esl_flashing_pattern_t flashing;     /*!< 56-bit flashing pattern (7 octets) */
    uint16_t repeat;                         /*!< Repeat_Type (bit 0) + Repeats_Duration (bits 15:1) */
} ble_esl_led_control_t;

/* ========================== LED Control Helpers ========================== */

/** @brief Extract Red component (2 bits) from color_brightness byte */
#define BLE_ESL_LED_CTRL_RED(cb)            ((cb) & 0x03)
/** @brief Extract Green component (2 bits) from color_brightness byte */
#define BLE_ESL_LED_CTRL_GREEN(cb)          (((cb) >> 2) & 0x03)
/** @brief Extract Blue component (2 bits) from color_brightness byte */
#define BLE_ESL_LED_CTRL_BLUE(cb)           (((cb) >> 4) & 0x03)
/** @brief Extract Brightness (2 bits) from color_brightness byte */
#define BLE_ESL_LED_CTRL_BRIGHTNESS(cb)     (((cb) >> 6) & 0x03)

/** @brief Build color_brightness byte from components */
#define BLE_ESL_LED_CTRL_COLOR_BRIGHTNESS(r, g, b, bright) \
    ((uint8_t)(((r) & 0x03) | (((g) & 0x03) << 2) | (((b) & 0x03) << 4) | (((bright) & 0x03) << 6)))

/** @brief Extract Repeat_Type (bit 0) from repeat field */
#define BLE_ESL_LED_CTRL_REPEAT_TYPE(rep)       ((rep) & 0x01)
/** @brief Extract Repeats_Duration (bits 15:1) from repeat field */
#define BLE_ESL_LED_CTRL_REPEATS_DURATION(rep)  (((rep) >> 1) & 0x7FFF)

/** @brief Build repeat field from type and duration */
#define BLE_ESL_LED_CTRL_REPEAT(type, duration) \
    ((uint16_t)(((type) & 0x01) | (((duration) & 0x7FFF) << 1)))

/* ========================== ESL Address Helpers ========================== */

/** @brief Extract Group_ID (7 bits) from group_id_rfu byte */
#define BLE_ESL_ADDR_GROUP_ID(addr)         ((addr).group_id_rfu & 0x7F)

/** @brief Set Group_ID in an ble_esl_address_t (clears RFU bit) */
#define BLE_ESL_ADDR_SET_GROUP_ID(addr, gid) \
    do { (addr).group_id_rfu = (uint8_t)((gid) & 0x7F); } while (0)

/**
 * @brief Build an ESL Address from its ESL_ID and Group_ID components
 *
 * @param esl_id   ESL identifier (0x00-0xFE valid, 0xFF = broadcast)
 * @param group_id Group identifier (0x00-0x7F); the RFU bit is always cleared
 * @return Assembled ESL Address
 */
static inline ble_esl_address_t ble_esl_addr_make(uint8_t esl_id, uint8_t group_id)
{
    ble_esl_address_t addr = {
        .esl_id = esl_id,
        .group_id_rfu = (uint8_t)(group_id & 0x7F),
    };
    return addr;
}

/* ========================== Cross-Layer Utility Functions ========================== */

/**
 * @brief Encode a single command or response into TLV wire format
 *
 * @param[in]  opcode     TLV opcode byte (Tag in bits [3:0], Length in bits [7:4])
 * @param[in]  params     Pointer to parameter bytes (Length+1 octets)
 * @param[in]  params_len Length of params in octets (must equal (opcode >> 4) + 1)
 * @param[out] out_buf    Output buffer to receive the encoded TLV (min size: params_len + 1)
 * @param[out] out_len    On output, set to total TLV size written (params_len + 1)
 *
 * @return
 *  - ESP_OK on success
 *  - ESP_ERR_INVALID_ARG if params_len does not match opcode Length nibble,
 *    or if any pointer argument is NULL
 */
esp_err_t ble_esl_tlv_encode(uint8_t opcode, const uint8_t *params,
                             uint8_t params_len, uint8_t *out_buf,
                             uint8_t *out_len);

/**
 * @brief Decode a single TLV element from a byte buffer
 *
 * @param[in]  in_buf     Input buffer containing the TLV
 * @param[in]  in_len     Length of input buffer
 * @param[out] opcode     On output, the opcode byte
 * @param[out] params     On output, pointer into in_buf at the start of parameters
 * @param[out] params_len On output, parameter length in octets
 *
 * @return
 *  - ESP_OK on success
 *  - ESP_ERR_INVALID_SIZE if buffer too short for the TLV
 *  - ESP_ERR_INVALID_ARG if any pointer argument is NULL or TLV is malformed
 */
esp_err_t ble_esl_tlv_decode(const uint8_t *in_buf, uint8_t in_len,
                             uint8_t *opcode, const uint8_t **params,
                             uint8_t *params_len);

/**
 * @brief Pack Group_ID header and pre-encoded TLVs into an ESL Payload buffer
 *
 * Used by the AP to build command sync packets (max 48 octets).
 *
 * @param[in]  group_id  7-bit Group_ID value (0x00–0x7F)
 * @param[in]  tlvs      Array of pointers to pre-encoded TLV buffers
 * @param[in]  tlv_lens  Array of TLV lengths (one per TLV)
 * @param[in]  tlv_count Number of TLVs to pack
 * @param[out] out_buf   Output buffer (min 48 octets recommended)
 * @param[out] out_len   On output, total payload size written
 *
 * @return
 *  - ESP_OK on success
 *  - ESP_ERR_INVALID_SIZE if total exceeds 48 octets
 *  - ESP_ERR_INVALID_ARG if group_id > 0x7F or any pointer argument is NULL
 */
esp_err_t ble_esl_payload_encode(uint8_t group_id, const uint8_t *tlvs[],
                                 const uint8_t tlv_lens[], uint8_t tlv_count,
                                 uint8_t *out_buf, uint8_t *out_len);

/**
 * @brief Unpack an ESL Payload into Group_ID and individual TLV elements
 *
 * Used by the ESL to parse received command payloads.
 *
 * @param[in]  in_buf      Input ESL Payload buffer
 * @param[in]  in_len      Input buffer length
 * @param[out] group_id    On output, the 7-bit Group_ID
 * @param[out] tlv_count   On output, number of TLVs found
 * @param[out] tlv_offsets Array of offsets into in_buf where each TLV starts
 * @param[out] tlv_lens    Array of total lengths of each TLV
 * @param[in]  max_tlvs    Maximum number of TLVs that can be reported (array size)
 *
 * @return
 *  - ESP_OK on success
 *  - ESP_ERR_INVALID_SIZE if payload too short or malformed
 *  - ESP_ERR_INVALID_ARG if any pointer argument is NULL
 *  - ESP_ERR_NO_MEM if more TLVs found than max_tlvs
 */
esp_err_t ble_esl_payload_decode(const uint8_t *in_buf, uint8_t in_len,
                                 uint8_t *group_id, uint8_t *tlv_count,
                                 uint8_t *tlv_offsets, uint8_t *tlv_lens,
                                 uint8_t max_tlvs);

/* ========================== Encrypted Advertising Data ========================== */

/**
 * @brief PAwR traffic direction, selects the CCM nonce directionBit
 *
 * The directionBit is the MSB of the last Randomizer octet (Supplement to the
 * Bluetooth Core Specification v11, Part A, 1.23.3).
 */
typedef enum {
    BLE_ESL_EAD_DIR_AP_TO_ESL = 0, /*!< Sync packets sent by the AP (directionBit = 0) */
    BLE_ESL_EAD_DIR_ESL_TO_AP = 1, /*!< Responses sent by the ESL (directionBit = 1) */
} ble_esl_ead_direction_t;

/**
 * @brief Seed a Randomizer for one PAwR direction
 *
 * Fills the Randomizer with random octets and sets its directionBit to match
 * @p direction. Call once per key material lifetime; afterwards
 * ble_esl_ead_encrypt() increments the Randomizer for every packet, so it must
 * not be re-seeded while the same session key and IV remain in use.
 *
 * @param[out] randomizer Randomizer buffer of BLE_ESL_RANDOMIZER_SIZE octets
 * @param[in]  direction  Direction the Randomizer is used for
 */
void ble_esl_ead_randomizer_init(uint8_t *randomizer,
                                 ble_esl_ead_direction_t direction);

/**
 * @brief Encrypt an ESL payload as Encrypted Advertising Data (AES-CCM)
 *
 * On success the Randomizer is incremented as a little-endian counter with its
 * directionBit preserved, so every packet encrypted with the same key material
 * uses a fresh nonce.
 *
 * @param[in]     session_key       Session key (BLE_ESL_SESSION_KEY_SIZE octets)
 * @param[in]     iv                Initialization vector (BLE_ESL_IV_SIZE octets)
 * @param[in,out] randomizer        Randomizer (BLE_ESL_RANDOMIZER_SIZE octets),
 *                                  seeded by ble_esl_ead_randomizer_init()
 * @param[in]     payload           Plaintext (inner AD structure) to encrypt
 * @param[in]     payload_len       Size of @p payload in octets
 * @param[out]    encrypted_payload Output buffer receiving
 *                                  [Randomizer(5)][Ciphertext(payload_len)][MIC(4)]
 *
 * @return
 *  - ESP_OK on success
 *  - ESP_ERR_INVALID_ARG if any pointer argument is NULL or payload_len is 0
 *  - ESP_FAIL if the AES-CCM operation fails
 */
esp_err_t ble_esl_ead_encrypt(const uint8_t *session_key, const uint8_t *iv,
                              uint8_t *randomizer, const uint8_t *payload,
                              size_t payload_len, uint8_t *encrypted_payload);

/**
 * @brief Decrypt Encrypted Advertising Data into an ESL payload (AES-CCM)
 *
 * Counterpart of ble_esl_ead_encrypt(). The Randomizer is taken from the first
 * BLE_ESL_RANDOMIZER_SIZE octets of @p encrypted_payload, so no Randomizer
 * state has to be kept for the receive direction. The MIC is verified before
 * the plaintext is reported as valid.
 *
 * @param[in]  session_key           Session key (BLE_ESL_SESSION_KEY_SIZE octets)
 * @param[in]  iv                    Initialization vector (BLE_ESL_IV_SIZE octets)
 * @param[in]  encrypted_payload     Encrypted Advertising Data value, i.e.
 *                                   [Randomizer(5)][Ciphertext(N)][MIC(4)],
 *                                   without the outer AD length and type octets
 * @param[in]  encrypted_payload_len Size of @p encrypted_payload in octets
 * @param[out] payload               Buffer receiving the plaintext
 *                                   (inner AD structure)
 * @param[in]  payload_size          Capacity of @p payload in octets
 * @param[out] payload_len           Number of plaintext octets written
 *
 * @return
 *  - ESP_OK on success
 *  - ESP_ERR_INVALID_ARG if any pointer argument is NULL
 *  - ESP_ERR_INVALID_SIZE if @p encrypted_payload_len is too short to hold a
 *    Randomizer, a MIC and at least one ciphertext octet, or if @p payload
 *    is too small for the plaintext
 *  - ESP_ERR_INVALID_RESPONSE if MIC verification fails
 */
esp_err_t ble_esl_ead_decrypt(const uint8_t *session_key, const uint8_t *iv,
                              const uint8_t *encrypted_payload,
                              size_t encrypted_payload_len, uint8_t *payload,
                              size_t payload_size, size_t *payload_len);

#ifdef __cplusplus
}
#endif

#endif /* BLE_ESL_COMMON_H */
