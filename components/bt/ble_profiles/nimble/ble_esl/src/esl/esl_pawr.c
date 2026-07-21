/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file esl_pawr.c
 * @brief ESL PAwR (Periodic Advertising with Responses) module
 *
 * Handles PAwR sync packet reception, command dispatch, response slot
 * calculation, response assembly with capacity-limit handling, AES-CCM
 * encryption, and response transmission via NimBLE PAwR response API.
 */

#include <string.h>
#include <assert.h>

#include "esp_log.h"
#include "esp_err.h"

#include "nimble/ble.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "os/os_mbuf.h"

#include "host/ble_ead.h"

#include "ble_esl.h"
#include "ble_esl_common.h"
#include "ble_esl_int.h"

static const char *TAG = "esl_pawr";

/* ========================== Constants ========================== */

/** Maximum response ESL Payload size in octets */
#define ESL_PAWR_MAX_RESPONSE_PAYLOAD   BLE_ESL_PAYLOAD_MAX_SIZE  /* 48 */

/** Error response TLV size: opcode (1) + error_code (1) = 2 bytes */
#define ESL_ERROR_RESP_SIZE             2

/** Error response opcode byte */
#define ESL_ERROR_RESP_OPCODE           BLE_ESL_RESP_ERROR  /* 0x00 */


/* ========================== Internal Context ========================== */

/**
 * @brief PAwR module internal state
 */
typedef struct {
    uint8_t resp_buf[ESL_PAWR_MAX_RESPONSE_PAYLOAD]; /*!< Response assembly buffer */
    uint8_t response_randomizer[BLE_ESL_RANDOMIZER_SIZE]; /*!< ESL -> AP EAD Randomizer */
    uint16_t sync_handle;   /*!< Sync handle for the current PAwR sync (set externally before processing) */
    uint16_t event_counter; /*!< Event counter for the current PAwR subevent report */
    uint8_t  subevent;      /*!< Subevent number for the current PAwR report */
} esl_pawr_ctx_t;

static esl_pawr_ctx_t *s_esl_pawr = NULL;

/* ========================== Forward Declarations ========================== */

static void build_error_response_tlv(uint8_t error_code, uint8_t *out_buf,
                                     uint8_t *out_len);
static bool is_forbidden_sync_opcode(uint8_t opcode);

/* ========================== Helper Functions ========================== */

/**
 * @brief Build a 2-byte Error response TLV
 *
 * @param[in]  error_code ESL error code
 * @param[out] out_buf    Output buffer (min 2 bytes)
 * @param[out] out_len    Set to 2 on output
 */
static void build_error_response_tlv(uint8_t error_code, uint8_t *out_buf,
                                     uint8_t *out_len)
{
    /* Error response: opcode = 0x00 (Tag=0, Length=0), param = error_code */
    out_buf[0] = ESL_ERROR_RESP_OPCODE;
    out_buf[1] = error_code;
    *out_len = ESL_ERROR_RESP_SIZE;
}

/**
 * @brief Check if an opcode is forbidden in the Synchronized state
 *
 * Factory Reset (0x03) and Update Complete (0x04) are not allowed via PAwR.
 *
 * @param[in] opcode Command opcode byte
 * @return true if the opcode is forbidden
 */
static bool is_forbidden_sync_opcode(uint8_t opcode)
{
    return (opcode == BLE_ESL_CMD_FACTORY_RESET ||
            opcode == BLE_ESL_CMD_UPDATE_COMPLETE);
}

/* ========================== Public Functions ========================== */

esp_err_t esl_pawr_init(void)
{
    if (s_esl_pawr != NULL) {
        ESP_LOGW(TAG, "PAwR module already initialized");
        return ESP_ERR_INVALID_STATE;
    }

    s_esl_pawr = calloc(1, sizeof(esl_pawr_ctx_t));
    if (s_esl_pawr == NULL) {
        ESP_LOGE(TAG, "Failed to allocate PAwR context");
        return ESP_ERR_NO_MEM;
    }

    ble_esl_ead_randomizer_init(s_esl_pawr->response_randomizer,
                                BLE_ESL_EAD_DIR_ESL_TO_AP);

    ESP_LOGI(TAG, "PAwR module initialized");
    return ESP_OK;
}

void esl_pawr_deinit(void)
{
    if (s_esl_pawr != NULL) {
        free(s_esl_pawr);
        s_esl_pawr = NULL;
    }
    ESP_LOGI(TAG, "PAwR module deinitialized");
}

void esl_pawr_set_sync_context(uint16_t sync_handle, uint16_t event_counter)
{
    if (s_esl_pawr == NULL) {
        ESP_LOGE(TAG, "PAwR module not initialized");
        return;
    }

    s_esl_pawr->sync_handle = sync_handle;
    s_esl_pawr->event_counter = event_counter;
}

int16_t esl_pawr_calc_response_slot(const uint8_t *data,
                                    const uint8_t *tlv_offsets,
                                    const uint8_t *tlv_lens,
                                    uint8_t tlv_count, uint8_t esl_id)
{
    /*
     * Response slot allocation algorithm:
     * 1. Number TLVs sequentially 1..N (all TLVs, including broadcast).
     * 2. Skip broadcast TLVs (ESL_ID = 0xFF) — they don't produce responses.
     * 3. Find the last individually-addressed TLV for this esl_id.
     * 4. If that TLV is number N (1-based), response slot = N - 1 (0-based).
     */
    if (data == NULL || tlv_offsets == NULL ||
        tlv_lens == NULL || tlv_count == 0) {
        return -1;
    }

    int16_t last_tlv_number = -1; /* 1-based TLV number of last match */

    for (uint8_t i = 0; i < tlv_count; i++) {
        /*
         * TLV layout in the payload: [opcode (1 byte)] [params (Length+1 bytes)]
         * ESL_ID is the first parameter byte, located at payload[tlv_offsets[i] + 1].
         */
        if (tlv_lens[i] < BLE_ESL_TLV_MIN_SIZE) {
            /* TLV too short to contain ESL_ID — skip */
            continue;
        }

        uint8_t cmd_esl_id = data[tlv_offsets[i] + 1];

        /* Skip broadcast TLVs — they don't count for slot allocation */
        if (cmd_esl_id == BLE_ESL_BROADCAST_ADDRESS) {
            continue;
        }

        /* Check if addressed to our ESL */
        if (cmd_esl_id == esl_id) {
            last_tlv_number = (int16_t)(i + 1); /* 1-based */
        }
    }

    if (last_tlv_number < 0) {
        return -1; /* Not individually addressed */
    }

    return (int16_t)(last_tlv_number - 1); /* 0-based slot */
}

esp_err_t esl_pawr_assemble_response(const uint8_t *resp_tlvs[],
                                     const uint8_t resp_tlv_lens[],
                                     uint8_t resp_count,
                                     uint8_t *out_buf, uint8_t *out_len)
{
    if (resp_tlvs == NULL || resp_tlv_lens == NULL ||
        out_buf == NULL || out_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (resp_count == 0) {
        *out_len = 0;
        return ESP_OK;
    }

    /*
     * First pass: calculate total size to determine if capacity limit
     * substitution is needed.
     */
    uint16_t total_size = 0;
    for (uint8_t i = 0; i < resp_count; i++) {
        total_size += resp_tlv_lens[i];
    }

    if (total_size <= ESL_PAWR_MAX_RESPONSE_PAYLOAD) {
        /* All responses fit — concatenate directly */
        uint8_t offset = 0;
        for (uint8_t i = 0; i < resp_count; i++) {
            memcpy(out_buf + offset, resp_tlvs[i], resp_tlv_lens[i]);
            offset += resp_tlv_lens[i];
        }
        *out_len = offset;
        return ESP_OK;
    }

    /*
     * Responses exceed 48 bytes. Substitute trailing TLVs with
     * Error: Capacity Limit (2 bytes: opcode 0x00, param 0x07).
     *
     * Strategy: keep as many leading TLVs as possible, then replace
     * the rest with 2-byte Capacity Limit error responses.
     */
    uint8_t offset = 0;
    uint8_t keep_count = 0;

    /*
     * Determine how many TLVs from the front we can keep while still
     * fitting the remaining substituted error TLVs within the limit.
     */
    for (uint8_t i = 0; i < resp_count; i++) {
        /* How many TLVs remain after this one (to be substituted) */
        uint8_t remaining_subs = resp_count - i - 1;
        uint16_t remaining_error_bytes = (uint16_t)remaining_subs * ESL_ERROR_RESP_SIZE;
        uint16_t projected = (uint16_t)offset + resp_tlv_lens[i] + remaining_error_bytes;

        if (projected <= ESL_PAWR_MAX_RESPONSE_PAYLOAD) {
            /* This TLV can be kept */
            memcpy(out_buf + offset, resp_tlvs[i], resp_tlv_lens[i]);
            offset += resp_tlv_lens[i];
            keep_count = i + 1;
        } else {
            /* This TLV and all subsequent must be replaced */
            break;
        }
    }

    /* Fill remaining slots with Capacity Limit error responses */
    for (uint8_t i = keep_count; i < resp_count; i++) {
        if ((uint16_t)offset + ESL_ERROR_RESP_SIZE > ESL_PAWR_MAX_RESPONSE_PAYLOAD) {
            /* Should not happen if algorithm is correct, but guard anyway */
            break;
        }
        out_buf[offset++] = ESL_ERROR_RESP_OPCODE;
        out_buf[offset++] = BLE_ESL_ERR_CAPACITY_LIMIT;
    }

    *out_len = offset;
    return ESP_OK;
}

esp_err_t esl_pawr_encrypt_and_send(uint8_t slot, const uint8_t *payload,
                                    uint8_t payload_len)
{
    if (s_esl_pawr == NULL) {
        ESP_LOGE(TAG, "PAwR module not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (payload == NULL || payload_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (payload_len > ESL_PAWR_MAX_RESPONSE_PAYLOAD) {
        ESP_LOGE(TAG, "payload_len %u exceeds max %u", payload_len, ESL_PAWR_MAX_RESPONSE_PAYLOAD);
        return ESP_ERR_INVALID_ARG;
    }

    /* Get the ESL Response Key Material */
    const ble_esl_key_material_t *resp_key = esl_get_resp_key();
    if (resp_key == NULL) {
        ESP_LOGE(TAG, "Response key material not configured");
        return ESP_ERR_INVALID_STATE;
    }

    /*
     * Step 1: Build inner AD structure (plaintext to encrypt)
     *   [inner_len (1)] [ESL Tag 0x34 (1)] [ESL Payload (payload_len)]
     *   inner_len = 1 (tag) + payload_len
     * The AP strips this wrapper after decryption (ble_esl_ap_pawr_parse_response),
     * mirroring the AP transmit path (ble_esl_ap_pawr_send). Without it the AP
     * reads the first response TLV bytes as the inner Len/Tag and rejects the
     * response (tag != 0x34).
     */
    uint8_t inner_len = 1 + payload_len;         /* ESL Tag + ESL Payload */
    uint8_t plaintext_len = 1 + inner_len;       /* Len octet + inner_len */
    uint8_t plaintext[1 + 1 + ESL_PAWR_MAX_RESPONSE_PAYLOAD];

    plaintext[0] = inner_len;
    plaintext[1] = BLE_ESL_AD_TYPE_ESL;          /* 0x34 */
    memcpy(&plaintext[2], payload, payload_len);

    /*
     * Step 2: Output packet layout (outer AD structure)
     *   [AD Length (1)] [AD Type 0x31 (1)] [Randomizer (5)] [Ciphertext (plaintext_len)] [MIC (4)]
     *
     * AD Length = 1 (type) + 5 (randomizer) + plaintext_len (ciphertext) + 4 (MIC)
     */
    uint16_t ad_length = 1 + BLE_EAD_RANDOMIZER_SIZE + plaintext_len + BLE_EAD_MIC_SIZE;
    uint16_t pkt_total = 1 + ad_length; /* 1 byte for AD Length field itself */
    uint8_t pkt_buf[1 + 1 + BLE_EAD_RANDOMIZER_SIZE + (1 + 1 + ESL_PAWR_MAX_RESPONSE_PAYLOAD) + BLE_EAD_MIC_SIZE];
    uint8_t pkt_len = 0;

    assert(pkt_total <= sizeof(pkt_buf));

    /* Outer AD structure header */
    pkt_buf[pkt_len++] = ad_length;
    pkt_buf[pkt_len++] = BLE_ESL_AD_TYPE_ENCRYPTED_DATA; /* 0x31 */

    /* Keep response timing tight by avoiding HCI RAND in this PAwR callback.
     * Only this host-task callback touches the Randomizer, so no lock needed. */
    esp_err_t enc_err = ble_esl_ead_encrypt(resp_key->session_key, resp_key->iv,
                                            s_esl_pawr->response_randomizer,
                                            plaintext, plaintext_len,
                                            &pkt_buf[pkt_len]);
    if (enc_err != ESP_OK) {
        ESP_LOGE(TAG, "PAwR response EAD encrypt failed: 0x%x", enc_err);
        return enc_err;
    }

    pkt_len += BLE_EAD_RANDOMIZER_SIZE + plaintext_len + BLE_EAD_MIC_SIZE;

    /* Transmit via NimBLE PAwR response API */
    struct ble_gap_periodic_adv_response_params params = {
        .request_event     = s_esl_pawr->event_counter,
        .request_subevent  = s_esl_pawr->subevent,
        .response_subevent = s_esl_pawr->subevent,
        .response_slot     = slot,
    };

    struct os_mbuf *om = os_msys_get_pkthdr(pkt_len, 0);
    if (om == NULL) {
        ESP_LOGE(TAG, "Failed to allocate mbuf for response");
        return ESP_ERR_NO_MEM;
    }

    int rc = os_mbuf_append(om, pkt_buf, pkt_len);
    if (rc != 0) {
        ESP_LOGE(TAG, "os_mbuf_append failed: %d", rc);
        os_mbuf_free_chain(om);
        return ESP_FAIL;
    }

    rc = ble_gap_periodic_adv_set_response_data(s_esl_pawr->sync_handle, &params, om);
    os_mbuf_free_chain(om);

    if (rc != 0) {
        ESP_LOGE(TAG, "ble_gap_periodic_adv_set_response_data failed: 0x%x", rc);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "PAwR response queued: event=%u subevent=%u slot=%u payload_len=%u pkt_len=%u",
             s_esl_pawr->event_counter, s_esl_pawr->subevent, slot, payload_len, pkt_len);

    return ESP_OK;
}

esp_err_t esl_pawr_process_sync_packet(uint8_t subevent, const uint8_t *data,
                                       uint8_t data_len)
{
    if (s_esl_pawr == NULL) {
        ESP_LOGE(TAG, "PAwR module not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (data == NULL || data_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    /*
     * --- Step 0: Strip the inner AD structure wrapping the ESL Payload ---
     *
     * The decrypted plaintext is an AD structure (CSS Encrypted Data type):
     *   [inner_len(1)] [ESL Tag 0x34(1)] [ESL Payload]
     * The ESL Payload (Group_ID + TLVs) begins after this 2-byte header.
     * This mirrors the AP transmit/parse paths (ble_esl_ap_pawr_send /
     * ble_esl_ap_pawr_parse_response); without it the header bytes get
     * misread as Group_ID / opcode and payload decode fails.
     */
    if (data_len < 2 || data[1] != BLE_ESL_AD_TYPE_ESL) {
        ESP_LOGW(TAG, "Invalid inner AD tag: 0x%02x (expected 0x%02x)",
                 data_len < 2 ? 0 : data[1], BLE_ESL_AD_TYPE_ESL);
        return ESP_ERR_INVALID_SIZE;
    }

    uint8_t inner_len = data[0]; /* length of ESL Tag + ESL Payload */
    if (inner_len < 1 || (uint16_t)(1 + inner_len) > data_len) {
        ESP_LOGW(TAG, "Inner AD length mismatch: inner_len=%u, data_len=%u",
                 inner_len, data_len);
        return ESP_ERR_INVALID_SIZE;
    }

    /* Advance past the [inner_len][0x34] header to the ESL Payload itself */
    data += 2;
    data_len = inner_len - 1; /* subtract the ESL Tag octet */

    /* --- Step 1: Decode the ESL Payload to extract Group_ID and TLVs --- */
    uint8_t group_id = 0;
    uint8_t tlv_count = 0;
    uint8_t tlv_offsets[ESL_MAX_TLVS_PER_PAYLOAD];
    uint8_t tlv_lens[ESL_MAX_TLVS_PER_PAYLOAD];

    ESP_LOG_BUFFER_HEX("PA data", data, data_len);

    esp_err_t err = ble_esl_payload_decode(data, data_len, &group_id,
                                           &tlv_count, tlv_offsets, tlv_lens,
                                           ESL_MAX_TLVS_PER_PAYLOAD);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Payload decode failed: 0x%x", err);
        return err;
    }

    /* --- Step 2: Validate Group_ID against ESL's configured address --- */
    uint8_t my_esl_id = 0;
    uint8_t my_group_id = 0;
    err = esl_get_address(&my_esl_id, &my_group_id);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ESL address not configured");
        return ESP_ERR_INVALID_STATE;
    }

    if (group_id != my_group_id) {
        ESP_LOGD(TAG, "Group_ID mismatch: received %u, expected %u",
                 group_id, my_group_id);
        return ESP_OK; /* Silently discard */
    }

    /* --- Step 3: Validate ESL is in Synchronized state --- */
    ble_esl_state_t state = ble_esl_get_state();
    if (state != BLE_ESL_STATE_SYNCHRONIZED) {
        ESP_LOGW(TAG, "Not in Synchronized state (current: %d), ignoring",
                 state);
        return ESP_ERR_INVALID_STATE;
    }

    /* Store subevent for use by encrypt_and_send */
    s_esl_pawr->subevent = subevent;

    /* --- Step 4: Iterate TLVs, dispatch commands, collect responses --- */

    /* Response collection arrays */
    uint8_t resp_bufs[ESL_MAX_TLVS_PER_PAYLOAD][BLE_ESL_TLV_MAX_SIZE];
    uint8_t resp_lens[ESL_MAX_TLVS_PER_PAYLOAD];
    const uint8_t *resp_ptrs[ESL_MAX_TLVS_PER_PAYLOAD];
    uint8_t resp_count = 0;
    bool has_individual = false;
    uint8_t individual_tlv_count = 0;
    uint8_t broadcast_tlv_count = 0;

    for (uint8_t i = 0; i < tlv_count; i++) {
        /* Decode each TLV */
        uint8_t opcode = 0;
        const uint8_t *params = NULL;
        uint8_t params_len = 0;

        err = ble_esl_tlv_decode(data + tlv_offsets[i], tlv_lens[i],
                                 &opcode, &params, &params_len);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "TLV %u decode failed: 0x%x, skipping", i, err);
            /* Use Length field for forward compatibility — skip this TLV */
            continue;
        }

        /* ESL_ID is the first parameter byte */
        if (params_len < 1) {
            ESP_LOGW(TAG, "TLV %u has no ESL_ID parameter, skipping", i);
            continue;
        }
        uint8_t cmd_esl_id = params[0];

        if (cmd_esl_id == my_esl_id) {
            /* ---- Individually addressed to this ESL ---- */
            has_individual = true;
            individual_tlv_count++;

            /* Forbidden over PAwR (Factory Reset / Update Complete): do NOT
             * execute the command, but since it is individually addressed to
             * this ESL we must still send an Error: Invalid State (0x03)
             * response in the assigned slot (per esl_pawr.md step 6c and
             * ESLS/SR/UPD/BV-01-C). Only broadcast forbidden commands are
             * dropped without a response (see the broadcast branch below). */
            if (is_forbidden_sync_opcode(opcode)) {
                ESP_LOGW(TAG, "Forbidden opcode 0x%02x (unicast) in Synchronized state, responding Invalid State",
                         opcode);
                uint8_t err_len = 0;
                build_error_response_tlv(BLE_ESL_ERR_INVALID_STATE,
                                         resp_bufs[resp_count], &err_len);
                resp_lens[resp_count] = err_len;
                resp_ptrs[resp_count] = resp_bufs[resp_count];
                resp_count++;
                continue;
            }

            /* Dispatch the command and collect response */
            ble_esl_cmd_result_t result;
            memset(&result, 0, sizeof(result));

            err = esl_cmd_dispatch(opcode, params, params_len, &result, ESL_TRANSPORT_PAWR);
            if (err != ESP_OK) {
                ESP_LOGW(TAG, "Command dispatch failed for TLV %u: 0x%x",
                         i, err);
                /* Build unspecified error response */
                uint8_t err_len = 0;
                build_error_response_tlv(BLE_ESL_ERR_UNSPECIFIED,
                                         resp_bufs[resp_count], &err_len);
                resp_lens[resp_count] = err_len;
                resp_ptrs[resp_count] = resp_bufs[resp_count];
                resp_count++;
                continue;
            }

            if (result.has_response) {
                /* Encode the response TLV */
                uint8_t encoded_len = 0;
                err = ble_esl_tlv_encode(result.resp_opcode,
                                         result.resp_params,
                                         result.resp_params_len,
                                         resp_bufs[resp_count],
                                         &encoded_len);
                if (err != ESP_OK) {
                    ESP_LOGW(TAG, "Response TLV encode failed for TLV %u", i);
                    uint8_t err_len = 0;
                    build_error_response_tlv(BLE_ESL_ERR_UNSPECIFIED,
                                             resp_bufs[resp_count], &err_len);
                    resp_lens[resp_count] = err_len;
                } else {
                    resp_lens[resp_count] = encoded_len;
                }
                resp_ptrs[resp_count] = resp_bufs[resp_count];
                resp_count++;
            }

        } else if (cmd_esl_id == BLE_ESL_BROADCAST_ADDRESS) {
            /* ---- Broadcast message ---- */
            broadcast_tlv_count++;

            /* Forbidden broadcast commands: just skip, no response */
            if (is_forbidden_sync_opcode(opcode)) {
                ESP_LOGW(TAG, "Forbidden broadcast opcode 0x%02x, skipping",
                         opcode);
                continue;
            }

            /* Dispatch broadcast command, discard result */
            ble_esl_cmd_result_t result;
            memset(&result, 0, sizeof(result));
            err = esl_cmd_dispatch(opcode, params, params_len, &result, ESL_TRANSPORT_PAWR);
            if (err != ESP_OK) {
                ESP_LOGW(TAG, "Broadcast dispatch failed for TLV %u: 0x%x",
                         i, err);
            }
            /* No response for broadcast */

        } else {
            /* ---- Command for a different ESL — skip ---- */
            individual_tlv_count++;
            ESP_LOGD(TAG, "TLV %u for ESL_ID 0x%02x, skipping", i, cmd_esl_id);
        }
    }

    /* --- Step 5: If no individually-addressed commands, do not transmit --- */
    if (!has_individual || resp_count == 0) {
        ESP_LOGD(TAG, "No individually-addressed commands for this ESL");
        return ESP_OK;
    }

    /* --- Step 6: Calculate response slot --- */
    int16_t slot = esl_pawr_calc_response_slot(data, tlv_offsets, tlv_lens,
                                               tlv_count, my_esl_id);
    if (slot < 0) {
        ESP_LOGW(TAG, "Response slot calculation returned -1, not transmitting");
        return ESP_OK;
    }

    /* Track slot info for logging/diagnostics */
    esl_pawr_slot_info_t slot_info = {
        .slot_number     = (uint8_t)slot,
        .tlv_count       = individual_tlv_count,
        .broadcast_count = broadcast_tlv_count,
    };
    ESP_LOGD(TAG, "Slot info: slot=%u, individual_tlvs=%u, broadcast_tlvs=%u",
             slot_info.slot_number, slot_info.tlv_count, slot_info.broadcast_count);

    /* --- Step 7: Assemble response payload --- */
    uint8_t assembled_payload[ESL_PAWR_MAX_RESPONSE_PAYLOAD];
    uint8_t assembled_len = 0;

    err = esl_pawr_assemble_response(resp_ptrs, resp_lens, resp_count,
                                     assembled_payload, &assembled_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Response assembly failed: 0x%x", err);
        return err;
    }

    if (assembled_len == 0) {
        ESP_LOGW(TAG, "Assembled response is empty");
        return ESP_OK;
    }

    /* --- Step 8: Encrypt and send --- */
    err = esl_pawr_encrypt_and_send((uint8_t)slot, assembled_payload,
                                    assembled_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Encrypt and send failed: 0x%x", err);
    }

    return err;
}
