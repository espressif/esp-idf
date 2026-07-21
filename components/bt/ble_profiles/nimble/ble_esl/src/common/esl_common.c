/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file ble_esl_common.c
 * @brief Implementation of ESL common protocol TLV and payload encode/decode utilities
 *
 * Protocol layer shared by both roles: TLV / ESL Payload encode and decode,
 * plus the Encrypted Advertising Data (AES-CCM) transmit and receive helpers
 * used by the AP sync and ESL response PAwR paths.
 */

#include <string.h>
#include <assert.h>
#include "esp_log.h"
#include "esp_random.h"
#include "host/ble_aes_ccm.h"
#include "ble_esl_common.h"

static const char *TAG = "ble_esl_common";

/** Bit position of the directionBit within the last Randomizer octet */
#define ESL_EAD_DIRECTION_BIT       7
/** Mask of the directionBit within the last Randomizer octet */
#define ESL_EAD_DIRECTION_BIT_MASK  (1U << ESL_EAD_DIRECTION_BIT)

/**
 * Additional authenticated data of the Encrypted Advertising Data AD type
 * (Supplement to the Bluetooth Core Specification v11, Part A, 1.23.3)
 */
static const uint8_t s_ead_aad[] = {0xEA};

/* ========================== Forward Declarations ========================== */

static void esl_ead_increment_randomizer(uint8_t *randomizer);

esp_err_t ble_esl_tlv_encode(uint8_t opcode, const uint8_t *params,
                             uint8_t params_len, uint8_t *out_buf,
                             uint8_t *out_len)
{
    /* Validate pointer arguments */
    if (params == NULL || out_buf == NULL || out_len == NULL) {
        ESP_LOGE(TAG, "tlv_encode: NULL argument");
        return ESP_ERR_INVALID_ARG;
    }

    /* Validate that params_len matches the Length nibble in the opcode */
    uint8_t expected_params_len = BLE_ESL_TLV_PARAMS_LEN(opcode);
    if (params_len != expected_params_len) {
        ESP_LOGE(TAG, "tlv_encode: params_len %u does not match opcode 0x%02X "
                 "(expected %u)", params_len, opcode, expected_params_len);
        return ESP_ERR_INVALID_ARG;
    }

    /* Write the opcode byte */
    out_buf[0] = opcode;

    /* Copy parameter bytes after the opcode */
    memcpy(&out_buf[1], params, params_len);

    /* Total TLV size = 1 (opcode) + params_len */
    *out_len = (uint8_t)(1 + params_len);

    return ESP_OK;
}

esp_err_t ble_esl_tlv_decode(const uint8_t *in_buf, uint8_t in_len,
                             uint8_t *opcode, const uint8_t **params,
                             uint8_t *params_len)
{
    /* Validate pointer arguments */
    if (in_buf == NULL || opcode == NULL || params == NULL || params_len == NULL) {
        ESP_LOGE(TAG, "tlv_decode: NULL argument");
        return ESP_ERR_INVALID_ARG;
    }

    /* Need at least the minimum TLV size (opcode + 1 param byte) */
    if (in_len < BLE_ESL_TLV_MIN_SIZE) {
        ESP_LOGE(TAG, "tlv_decode: buffer too short (%u < %u)",
                 in_len, BLE_ESL_TLV_MIN_SIZE);
        return ESP_ERR_INVALID_SIZE;
    }

    /* Extract the opcode */
    uint8_t opc = in_buf[0];
    uint8_t expected_total = BLE_ESL_TLV_TOTAL_LEN(opc);

    /* Verify the buffer contains enough data for the full TLV */
    if (in_len < expected_total) {
        ESP_LOGE(TAG, "tlv_decode: buffer too short for TLV "
                 "(have %u, need %u for opcode 0x%02X)",
                 in_len, expected_total, opc);
        return ESP_ERR_INVALID_SIZE;
    }

    *opcode = opc;
    *params = &in_buf[1];
    *params_len = BLE_ESL_TLV_PARAMS_LEN(opc);

    return ESP_OK;
}

esp_err_t ble_esl_payload_encode(uint8_t group_id, const uint8_t *tlvs[],
                                 const uint8_t tlv_lens[], uint8_t tlv_count,
                                 uint8_t *out_buf, uint8_t *out_len)
{
    /* Validate pointer arguments */
    if (out_buf == NULL || out_len == NULL) {
        ESP_LOGE(TAG, "payload_encode: NULL output argument");
        return ESP_ERR_INVALID_ARG;
    }

    if (tlv_count == 0) {
        ESP_LOGE(TAG, "payload_encode: tlv_count must be greater than 0");
        return ESP_ERR_INVALID_ARG;
    }

    if (tlvs == NULL || tlv_lens == NULL) {
        ESP_LOGE(TAG, "payload_encode: NULL TLV argument");
        return ESP_ERR_INVALID_ARG;
    }

    /* Validate Group_ID range (7-bit) */
    if (group_id > BLE_ESL_GROUP_ID_MAX) {
        ESP_LOGE(TAG, "payload_encode: group_id 0x%02X exceeds max 0x%02X",
                 group_id, BLE_ESL_GROUP_ID_MAX);
        return ESP_ERR_INVALID_ARG;
    }

    /* Calculate total payload size: 1 (Group_ID header) + sum of all TLV lengths */
    uint16_t total = 1; /* Group_ID header byte */
    for (uint8_t i = 0; i < tlv_count; i++) {
        total += tlv_lens[i];
    }

    if (total > BLE_ESL_PAYLOAD_MAX_SIZE) {
        ESP_LOGE(TAG, "payload_encode: total size %u exceeds max %u",
                 total, BLE_ESL_PAYLOAD_MAX_SIZE);
        return ESP_ERR_INVALID_SIZE;
    }

    /* Write Group_ID header byte: Group_ID in bits [6:0], RFU bit 7 = 0 */
    out_buf[0] = group_id & 0x7F;

    /* Concatenate all TLV buffers */
    uint8_t offset = 1;
    for (uint8_t i = 0; i < tlv_count; i++) {
        if (tlvs[i] == NULL) {
            ESP_LOGE(TAG, "payload_encode: tlvs[%u] is NULL", i);
            return ESP_ERR_INVALID_ARG;
        }
        if (tlv_lens[i] < BLE_ESL_TLV_MIN_SIZE || tlv_lens[i] > BLE_ESL_TLV_MAX_SIZE) {
            ESP_LOGE(TAG, "payload_encode: tlv_lens[%u] = %u out of valid range [%u, %u]",
                     i, tlv_lens[i], BLE_ESL_TLV_MIN_SIZE, BLE_ESL_TLV_MAX_SIZE);
            return ESP_ERR_INVALID_ARG;
        }
        memcpy(&out_buf[offset], tlvs[i], tlv_lens[i]);
        offset += tlv_lens[i];
    }

    *out_len = offset;

    return ESP_OK;
}

esp_err_t ble_esl_payload_decode(const uint8_t *in_buf, uint8_t in_len,
                                 uint8_t *group_id, uint8_t *tlv_count,
                                 uint8_t *tlv_offsets, uint8_t *tlv_lens,
                                 uint8_t max_tlvs)
{
    /* Validate pointer arguments */
    if (in_buf == NULL || group_id == NULL || tlv_count == NULL ||
        tlv_offsets == NULL || tlv_lens == NULL) {
        ESP_LOGE(TAG, "payload_decode: NULL argument");
        return ESP_ERR_INVALID_ARG;
    }

    /* Need at least 1 byte for Group_ID header + minimum TLV (2 bytes) */
    if (in_len < 1 + BLE_ESL_TLV_MIN_SIZE) {
        ESP_LOGE(TAG, "payload_decode: payload too short (%u bytes)", in_len);
        return ESP_ERR_INVALID_SIZE;
    }

    /* Extract Group_ID (bits [6:0] of first byte) */
    *group_id = in_buf[0] & 0x7F;

    /* Parse concatenated TLVs starting after the Group_ID header */
    uint8_t offset = 1;
    uint8_t count = 0;

    while (offset < in_len) {
        /* Need at least a minimum-size TLV in the remaining buffer */
        if ((uint16_t)offset + BLE_ESL_TLV_MIN_SIZE > in_len) {
            break;
        }

        uint8_t opc = in_buf[offset];
        uint8_t tlv_total = BLE_ESL_TLV_TOTAL_LEN(opc);

        /* Verify the TLV fits within the remaining buffer */
        if ((uint16_t)offset + tlv_total > in_len) {
            ESP_LOGE(TAG, "payload_decode: malformed TLV at offset %u "
                     "(need %u bytes, have %u remaining)",
                     offset, tlv_total, in_len - offset);
            return ESP_ERR_INVALID_SIZE;
        }

        /* Check if we have room to report this TLV */
        if (count >= max_tlvs) {
            ESP_LOGE(TAG, "payload_decode: found more TLVs than max_tlvs (%u)",
                     max_tlvs);
            *tlv_count = count;
            return ESP_ERR_NO_MEM;
        }

        /* Record this TLV's offset and length */
        tlv_offsets[count] = offset;
        tlv_lens[count] = tlv_total;
        count++;

        offset += tlv_total;
    }

    /* If no TLVs were found, the payload is malformed */
    if (count == 0) {
        ESP_LOGE(TAG, "payload_decode: no TLVs found in payload");
        return ESP_ERR_INVALID_SIZE;
    }

    *tlv_count = count;

    return ESP_OK;
}

void ble_esl_ead_randomizer_init(uint8_t *randomizer,
                                 ble_esl_ead_direction_t direction)
{
    assert(randomizer != NULL);

    esp_fill_random(randomizer, BLE_ESL_RANDOMIZER_SIZE);

    uint8_t *last = &randomizer[BLE_ESL_RANDOMIZER_SIZE - 1];
    if (direction == BLE_ESL_EAD_DIR_ESL_TO_AP) {
        *last |= ESL_EAD_DIRECTION_BIT_MASK;
    } else {
        *last &= (uint8_t)~ESL_EAD_DIRECTION_BIT_MASK;
    }
}

esp_err_t ble_esl_ead_encrypt(const uint8_t *session_key, const uint8_t *iv,
                              uint8_t *randomizer, const uint8_t *payload,
                              size_t payload_len, uint8_t *encrypted_payload)
{
    /* Validate arguments */
    if (session_key == NULL || iv == NULL || randomizer == NULL ||
        payload == NULL || payload_len == 0 || encrypted_payload == NULL) {
        ESP_LOGE(TAG, "ead_encrypt: invalid argument");
        return ESP_ERR_INVALID_ARG;
    }

    /*
     * Nonce is the concatenation of Randomizer and IV. The Randomizer is also
     * transmitted in clear ahead of the ciphertext so the peer can rebuild the
     * same nonce.
     */
    uint8_t nonce[BLE_ESL_CCM_NONCE_SIZE];

    memcpy(nonce, randomizer, BLE_ESL_RANDOMIZER_SIZE);
    memcpy(&nonce[BLE_ESL_RANDOMIZER_SIZE], iv, BLE_ESL_IV_SIZE);
    memcpy(encrypted_payload, randomizer, BLE_ESL_RANDOMIZER_SIZE);

    int rc = ble_aes_ccm_encrypt(session_key, nonce, payload, payload_len,
                                 s_ead_aad, sizeof(s_ead_aad),
                                 &encrypted_payload[BLE_ESL_RANDOMIZER_SIZE],
                                 BLE_ESL_MIC_SIZE);
    if (rc != 0) {
        ESP_LOGE(TAG, "ead_encrypt: AES-CCM failed: %d", rc);
        return ESP_FAIL;
    }

    esl_ead_increment_randomizer(randomizer);

    return ESP_OK;
}

esp_err_t ble_esl_ead_decrypt(const uint8_t *session_key, const uint8_t *iv,
                              const uint8_t *encrypted_payload,
                              size_t encrypted_payload_len, uint8_t *payload,
                              size_t payload_size, size_t *payload_len)
{
    /* Validate arguments */
    if (session_key == NULL || iv == NULL || encrypted_payload == NULL ||
        payload == NULL || payload_len == NULL) {
        ESP_LOGE(TAG, "ead_decrypt: invalid argument");
        return ESP_ERR_INVALID_ARG;
    }

    /*
     * The plaintext must hold at least one AD structure, so the encrypted
     * payload carries at least one ciphertext octet on top of the Randomizer
     * and the MIC (CSS v11, Part A, 1.23.2).
     */
    if (encrypted_payload_len < BLE_ESL_RANDOMIZER_SIZE + BLE_ESL_MIC_SIZE + 1) {
        ESP_LOGE(TAG, "ead_decrypt: encrypted payload too short: %u",
                 (unsigned)encrypted_payload_len);
        return ESP_ERR_INVALID_SIZE;
    }

    size_t plaintext_len = encrypted_payload_len -
                           (BLE_ESL_RANDOMIZER_SIZE + BLE_ESL_MIC_SIZE);
    if (plaintext_len > payload_size) {
        ESP_LOGE(TAG, "ead_decrypt: output buffer too small (%u < %u)",
                 (unsigned)payload_size, (unsigned)plaintext_len);
        return ESP_ERR_INVALID_SIZE;
    }

    /*
     * Rebuild the nonce from the Randomizer sent in clear ahead of the
     * ciphertext and the locally stored IV.
     */
    uint8_t nonce[BLE_ESL_CCM_NONCE_SIZE];

    memcpy(nonce, encrypted_payload, BLE_ESL_RANDOMIZER_SIZE);
    memcpy(&nonce[BLE_ESL_RANDOMIZER_SIZE], iv, BLE_ESL_IV_SIZE);

    int rc = ble_aes_ccm_decrypt(session_key, nonce,
                                 &encrypted_payload[BLE_ESL_RANDOMIZER_SIZE],
                                 plaintext_len, s_ead_aad, sizeof(s_ead_aad),
                                 payload, BLE_ESL_MIC_SIZE);
    if (rc != 0) {
        ESP_LOGW(TAG, "ead_decrypt: MIC authentication failed: %d", rc);
        return ESP_ERR_INVALID_RESPONSE;
    }

    *payload_len = plaintext_len;

    return ESP_OK;
}

static void esl_ead_increment_randomizer(uint8_t *randomizer)
{
    uint8_t direction_bit = randomizer[BLE_ESL_RANDOMIZER_SIZE - 1] &
                            ESL_EAD_DIRECTION_BIT_MASK;

    for (uint8_t i = 0; i < BLE_ESL_RANDOMIZER_SIZE; i++) {
        if (i == BLE_ESL_RANDOMIZER_SIZE - 1) {
            /* Last octet: 7-bit counter, directionBit restored */
            uint8_t counter = (uint8_t)((randomizer[i] + 1) & 0x7F);
            randomizer[i] = (uint8_t)(counter | direction_bit);
            break;
        }

        randomizer[i]++;
        if (randomizer[i] != 0) {
            break; /* No carry into the next octet */
        }
    }
}
