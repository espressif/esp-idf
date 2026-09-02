/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file ble_esl_ap_pawr.c
 * @brief ESL AP PAwR broadcasting and AES-CCM encryption/decryption
 *
 * Implements PAwR broadcaster lifecycle (init/deinit/start/stop),
 * encrypted sync packet assembly (AP → ESL), and encrypted response
 * packet parsing (ESL → AP) using AES-CCM with per-direction key material.
 */

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "esp_log.h"
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "nimble/ble.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "os/os_mbuf.h"

#include "host/ble_ead.h"

#include "ble_esl_ap_int.h"
#include "ble_esl_common.h"

static const char *TAG = "esl_ap_pawr";

/** Mutex protecting the per-subevent pending TX buffers (g_esl_ap->pawr_pending) */
static SemaphoreHandle_t s_pawr_mutex;

/** Advertising instance used for PAwR */
#define PAWR_ADV_INSTANCE   0

/**
 * Number of consecutive PAwR events a queued sync packet is transmitted before
 * its pending slot is cleared. >1 improves delivery reliability at the cost of
 * the ESL seeing (and responding to) the same command multiple times.
 * 1 == transmit exactly once.
 */
#define PAWR_TX_REPEATS     3

/* ========================== Forward Declarations ========================== */

static int pawr_gap_event_cb(struct ble_gap_event *event, void *arg);
static void pawr_handle_subev_data_req(struct ble_gap_event *event);
static void pawr_handle_subev_response(struct ble_gap_event *event);

/* ========================== PAwR Init / Deinit ========================== */

esp_err_t ble_esl_ap_pawr_init(void)
{
    assert(g_esl_ap != NULL);

    ble_esl_ead_randomizer_init(g_esl_ap->randomizer, BLE_ESL_EAD_DIR_AP_TO_ESL);
    g_esl_ap->pawr_active = false;
    g_esl_ap->pawr_pending = NULL;

    if (s_pawr_mutex == NULL) {
        s_pawr_mutex = xSemaphoreCreateMutex();
        if (s_pawr_mutex == NULL) {
            ESP_LOGE(TAG, "Failed to create PAwR mutex");
            return ESP_ERR_NO_MEM;
        }
    }

    ESP_LOGI(TAG, "PAwR sub-module initialized");
    return ESP_OK;
}

void ble_esl_ap_pawr_deinit(void)
{
    assert(g_esl_ap != NULL);

    if (g_esl_ap->pawr_active) {
        ble_esl_ap_pawr_stop();
    }

    if (s_pawr_mutex != NULL) {
        vSemaphoreDelete(s_pawr_mutex);
        s_pawr_mutex = NULL;
    }

    ESP_LOGI(TAG, "PAwR sub-module deinitialized");
}

/* ========================== PAwR Start / Stop ========================== */

esp_err_t ble_esl_ap_pawr_start(void)
{
    assert(g_esl_ap != NULL);

    int rc;

    if (g_esl_ap->pawr_active) {
        ESP_LOGW(TAG, "PAwR already active");
        return ESP_ERR_INVALID_STATE;
    }

    /* Step 1: Configure extended advertising (non-connectable, non-scannable) */
    struct ble_gap_ext_adv_params ext_params;
    memset(&ext_params, 0, sizeof(ext_params));
    ext_params.connectable = 0;
    ext_params.scannable = 0;
    ext_params.legacy_pdu = 0;
    ext_params.anonymous = 0;
    ext_params.include_tx_power = 0;
    ext_params.itvl_min = 0;
    ext_params.itvl_max = 0;
    ext_params.primary_phy = BLE_HCI_LE_PHY_1M;
    ext_params.secondary_phy = BLE_HCI_LE_PHY_1M;
    ext_params.own_addr_type = BLE_OWN_ADDR_PUBLIC;
    ext_params.sid = 0;
    ext_params.tx_power = 127; /* Host selects max */

    int8_t selected_tx_power = 0;
    rc = ble_gap_ext_adv_configure(PAWR_ADV_INSTANCE, &ext_params,
                                   &selected_tx_power,
                                   pawr_gap_event_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "ext_adv_configure failed: %d", rc);
        return ESP_FAIL;
    }

    /* Step 2: Configure periodic advertising with PAwR parameters */
    struct ble_gap_periodic_adv_params pparams;
    memset(&pparams, 0, sizeof(pparams));
    pparams.include_tx_power = 0;
    pparams.itvl_min = g_esl_ap->pawr_config.periodic_adv_interval_min;
    pparams.itvl_max = g_esl_ap->pawr_config.periodic_adv_interval_max;
    pparams.num_subevents = g_esl_ap->pawr_config.num_subevents;
    pparams.subevent_interval = g_esl_ap->pawr_config.subevent_interval;
    pparams.response_slot_delay = g_esl_ap->pawr_config.response_slot_delay;
    pparams.response_slot_spacing = g_esl_ap->pawr_config.response_slot_spacing;
    pparams.num_response_slots = g_esl_ap->pawr_config.num_response_slots;

    rc = ble_gap_periodic_adv_configure(PAWR_ADV_INSTANCE, &pparams);
    if (rc != 0) {
        ESP_LOGE(TAG, "periodic_adv_configure failed: %d", rc);
        /* Roll back the instance configured in Step 1, otherwise it stays
         * configured in the host and a retry may be rejected. */
        ble_gap_ext_adv_remove(PAWR_ADV_INSTANCE);
        return ESP_FAIL;
    }

    /* Step 3: Start periodic advertising */
    rc = ble_gap_periodic_adv_start(PAWR_ADV_INSTANCE);
    if (rc != 0) {
        ESP_LOGE(TAG, "periodic_adv_start failed: %d", rc);
        ble_gap_ext_adv_remove(PAWR_ADV_INSTANCE);
        return ESP_FAIL;
    }

    /* Step 4: Start extended advertising (required for periodic adv train) */
    rc = ble_gap_ext_adv_start(PAWR_ADV_INSTANCE, 0, 0);
    if (rc != 0) {
        ESP_LOGE(TAG, "ext_adv_start failed: %d", rc);
        ble_gap_periodic_adv_stop(PAWR_ADV_INSTANCE);
        ble_gap_ext_adv_remove(PAWR_ADV_INSTANCE);
        return ESP_FAIL;
    }

    /* Step 5: Allocate one pending TX slot per subevent. PAwR is pull-based:
     * ble_esl_ap_pawr_send() stores encrypted packets here and the
     * BLE_GAP_EVENT_PER_SUBEV_DATA_REQ callback drains them into subevents. */
    uint8_t num_subevents = g_esl_ap->pawr_config.num_subevents;
    g_esl_ap->pawr_pending = calloc(num_subevents,
                                    sizeof(ble_esl_ap_pawr_pending_t));
    if (g_esl_ap->pawr_pending == NULL) {
        ESP_LOGE(TAG, "Failed to allocate %u pending TX buffers", num_subevents);
        ble_gap_ext_adv_stop(PAWR_ADV_INSTANCE);
        ble_gap_periodic_adv_stop(PAWR_ADV_INSTANCE);
        ble_gap_ext_adv_remove(PAWR_ADV_INSTANCE);
        return ESP_ERR_NO_MEM;
    }

    g_esl_ap->pawr_active = true;
    ESP_LOGI(TAG, "PAwR broadcaster started (subevents=%u, resp_slots=%u)",
             g_esl_ap->pawr_config.num_subevents,
             g_esl_ap->pawr_config.num_response_slots);

    return ESP_OK;
}

esp_err_t ble_esl_ap_pawr_stop(void)
{
    assert(g_esl_ap != NULL);

    if (!g_esl_ap->pawr_active) {
        ESP_LOGW(TAG, "PAwR not active");
        return ESP_ERR_INVALID_STATE;
    }

    int rc;

    /* Stop periodic advertising */
    rc = ble_gap_periodic_adv_stop(PAWR_ADV_INSTANCE);
    if (rc != 0) {
        ESP_LOGW(TAG, "periodic_adv_stop failed: %d", rc);
    }

    /* Stop extended advertising */
    rc = ble_gap_ext_adv_stop(PAWR_ADV_INSTANCE);
    if (rc != 0) {
        ESP_LOGW(TAG, "ext_adv_stop failed: %d", rc);
    }

    /* Release pending TX buffers under the lock so the data-request callback
     * (host task) never dereferences a freed pointer. */
    xSemaphoreTake(s_pawr_mutex, portMAX_DELAY);
    g_esl_ap->pawr_active = false;
    free(g_esl_ap->pawr_pending);
    g_esl_ap->pawr_pending = NULL;
    xSemaphoreGive(s_pawr_mutex);

    ESP_LOGI(TAG, "PAwR broadcaster stopped");

    return ESP_OK;
}

/* ========================== Key Material ========================== */

void ble_esl_ap_pawr_set_sync_key(const ble_esl_key_material_t *key_mat)
{
    assert(g_esl_ap != NULL);
    assert(key_mat != NULL);

    g_esl_ap->ap_sync_key = *key_mat;

    ESP_LOGI(TAG, "AP Sync Key Material set");
}

esp_err_t ble_esl_ap_pawr_set_response_key(uint16_t esl_addr,
                                           const ble_esl_key_material_t *key_mat)
{
    assert(g_esl_ap != NULL);

    if (key_mat == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ble_esl_ap_esl_entry_t *entry = ble_esl_ap_find_esl(esl_addr);
    if (entry == NULL) {
        ESP_LOGE(TAG, "ESL 0x%04x not found for response key", esl_addr);
        return ESP_ERR_NOT_FOUND;
    }

    entry->resp_key = *key_mat;

    ESP_LOGI(TAG, "Response Key Material set for ESL 0x%04x", esl_addr);
    return ESP_OK;
}

/* ========================== PAwR Send (Encrypt & Transmit) ========================== */

esp_err_t ble_esl_ap_pawr_send(uint8_t group_id, const uint8_t *payload,
                               uint8_t payload_len)
{
    assert(g_esl_ap != NULL);

    /* Validate arguments */
    if (group_id > BLE_ESL_GROUP_ID_MAX) {
        ESP_LOGE(TAG, "Invalid group_id: 0x%02x", group_id);
        return ESP_ERR_INVALID_ARG;
    }
    if (payload == NULL || payload_len == 0 || payload_len > BLE_ESL_PAYLOAD_MAX_SIZE) {
        ESP_LOGE(TAG, "Invalid payload (ptr=%p, len=%u)", payload, payload_len);
        return ESP_ERR_INVALID_ARG;
    }
    if (!g_esl_ap->pawr_active) {
        ESP_LOGE(TAG, "PAwR broadcaster not active");
        return ESP_ERR_INVALID_STATE;
    }

    /*
     * Step 1: Build inner AD structure (plaintext)
     *   [len_byte] [0x34] [ESL Payload]
     *   len_byte = 1 (tag) + payload_len
     */
    uint8_t inner_ad_len = 1 + payload_len; /* ESL Tag + ESL Payload */
    uint8_t plaintext_len = 1 + inner_ad_len; /* Len byte + inner_ad_len */
    uint8_t plaintext[1 + 1 + BLE_ESL_PAYLOAD_MAX_SIZE]; /* len + tag + payload */

    plaintext[0] = inner_ad_len;                /* Len byte */
    plaintext[1] = BLE_ESL_AD_TYPE_ESL;         /* ESL Tag 0x34 */
    memcpy(&plaintext[2], payload, payload_len); /* ESL Payload */

    /*
     * Step 2: Encrypt with the AP Sync Key Material (AES-CCM)
     *   Input:  plaintext (inner AD)
     *   Output: encrypted_payload = [Randomizer(5)] [Ciphertext(N)] [MIC(4)]
     */
    uint8_t enc_payload_len = BLE_EAD_ENCRYPTED_PAYLOAD_SIZE(plaintext_len);
    uint8_t encrypted_payload[BLE_EAD_ENCRYPTED_PAYLOAD_SIZE(1 + 1 + BLE_ESL_PAYLOAD_MAX_SIZE)];

    /* The Randomizer is read and advanced under the lock: this function is a
     * public command path and may be called from any application task. */
    xSemaphoreTake(s_pawr_mutex, portMAX_DELAY);
    esp_err_t enc_err = ble_esl_ead_encrypt(g_esl_ap->ap_sync_key.session_key,
                                            g_esl_ap->ap_sync_key.iv,
                                            g_esl_ap->randomizer,
                                            plaintext,
                                            plaintext_len,
                                            encrypted_payload);
    xSemaphoreGive(s_pawr_mutex);
    if (enc_err != ESP_OK) {
        ESP_LOGE(TAG, "EAD encrypt failed: 0x%x", enc_err);
        return enc_err;
    }

    /*
     * Step 3: Build outer AD structure (Encrypted Data)
     *   [total_len] [0x31] [encrypted_payload]
     *   total_len = 1 (ED tag) + enc_payload_len
     */
    uint8_t ed_content_len = 1 + enc_payload_len;
    uint8_t outer_ad[BLE_ESL_AP_PAWR_MAX_AD_BUF_SIZE];
    uint8_t pos = 0;

    outer_ad[pos++] = ed_content_len;                                   /* Len */
    outer_ad[pos++] = BLE_ESL_AD_TYPE_ENCRYPTED_DATA;                   /* ED Tag 0x31 */
    memcpy(&outer_ad[pos], encrypted_payload, enc_payload_len);         /* Randomizer + Ciphertext + MIC */
    pos += enc_payload_len;

    /*
     * Step 4: Queue the encrypted packet for the target subevent (= group_id).
     *
     * PAwR subevent data can only be handed to the controller in response to a
     * BLE_GAP_EVENT_PER_SUBEV_DATA_REQ event; pushing it here directly makes the
     * controller reject the HCI command with Command Disallowed (0x0C). So we
     * store it in the pending slot and let pawr_handle_subev_data_req() drain it.
     */
    if (group_id >= g_esl_ap->pawr_config.num_subevents) {
        ESP_LOGE(TAG, "group_id %u exceeds configured subevents (%u)",
                 group_id, g_esl_ap->pawr_config.num_subevents);
        return ESP_ERR_INVALID_ARG;
    }
    if (pos > BLE_ESL_AP_PAWR_MAX_AD_BUF_SIZE) {
        ESP_LOGE(TAG, "Outer AD too large: %u", pos);
        return ESP_ERR_INVALID_SIZE;
    }

    xSemaphoreTake(s_pawr_mutex, portMAX_DELAY);
    if (g_esl_ap->pawr_pending == NULL) {
        xSemaphoreGive(s_pawr_mutex);
        ESP_LOGE(TAG, "PAwR pending buffers not allocated");
        return ESP_ERR_INVALID_STATE;
    }
    ble_esl_ap_pawr_pending_t *slot = &g_esl_ap->pawr_pending[group_id];
    memcpy(slot->data, outer_ad, pos);
    slot->len = pos;
    slot->repeats_left = PAWR_TX_REPEATS;
    slot->valid = true;
    xSemaphoreGive(s_pawr_mutex);

    ESP_LOGD(TAG, "PAwR sync packet queued for group %u (payload_len=%u, ad_len=%u)",
             group_id, payload_len, pos);
    return ESP_OK;
}

/* ========================== PAwR Parse Response (Decrypt) ========================== */

esp_err_t ble_esl_ap_pawr_parse_response(uint16_t esl_addr,
                                         uint8_t response_slot,
                                         const uint8_t *enc_data,
                                         uint8_t enc_data_len,
                                         ble_esl_ap_parsed_response_t *out_response)
{
    assert(g_esl_ap != NULL);

    /* Validate arguments */
    if (enc_data == NULL || out_response == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /*
     * PAwR response data is a raw AD structure:
     *   [AD Len(1)] [AD Type 0x31(1)] [Randomizer(5)] [Ciphertext(N)] [MIC(4)]
     * ble_esl_ead_decrypt() expects only the AD value (Randomizer..MIC), so the
     * outer length + type octets must be parsed off first (mirrors the ESL
     * receive path in handle_gap_periodic_report).
     *
     * Minimum enc_data_len:
     *   2 (AD hdr) + 5 (randomizer) + 4 (MIC) + 1 (min ciphertext) = 12
     */
    if (enc_data_len < (2 + BLE_ESL_RANDOMIZER_SIZE + BLE_ESL_MIC_SIZE + 1)) {
        ESP_LOGE(TAG, "enc_data too short: %u", enc_data_len);
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t outer_len  = enc_data[0];   /* AD type octet + encrypted payload */
    uint8_t outer_type = enc_data[1];
    if (outer_type != BLE_ESL_AD_TYPE_ENCRYPTED_DATA ||
        outer_len < 1 + BLE_ESL_RANDOMIZER_SIZE + BLE_ESL_MIC_SIZE ||
        (uint16_t)(outer_len + 1) > enc_data_len) {
        ESP_LOGE(TAG, "Not a valid Encrypted Data AD (type=0x%02x len=%u)",
                 outer_type, outer_len);
        return ESP_ERR_INVALID_ARG;
    }

    /* Strip the outer AD length + type octets to get the EAD payload. */
    const uint8_t *enc_payload = &enc_data[2];
    uint8_t enc_payload_len = outer_len - 1;

    /* Step 1: Look up ESL entry by esl_addr */
    ble_esl_ap_esl_entry_t *entry = ble_esl_ap_find_esl(esl_addr);
    if (entry == NULL) {
        ESP_LOGE(TAG, "ESL 0x%04x not found for response decryption", esl_addr);
        return ESP_ERR_NOT_FOUND;
    }

    /* Step 2: Decrypt and verify MIC via ble_esl_ead_decrypt
     *   Input:  enc_payload = [Randomizer(5)] [Ciphertext(N)] [MIC(4)]
     *   Output: plaintext (inner AD), ciphertext_len octets
     */
    uint8_t plaintext[BLE_ESL_PAYLOAD_MAX_SIZE + 2]; /* inner AD max */
    size_t decrypted_len = 0;
    esp_err_t err = ble_esl_ead_decrypt(entry->resp_key.session_key,
                                        entry->resp_key.iv,
                                        enc_payload,
                                        enc_payload_len,
                                        plaintext, sizeof(plaintext),
                                        &decrypted_len);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Response decryption failed for ESL 0x%04x (err=%d)",
                 esl_addr, (int)err);
        return err;
    }

    uint8_t ciphertext_len = (uint8_t)decrypted_len;

    /*
     * Step 3: Strip inner AD wrapper
     *   plaintext[0] = Len (length of ESL Tag + response payload)
     *   plaintext[1] = ESL Tag (must be 0x34)
     *   plaintext[2..] = response TLVs
     */
    if (ciphertext_len < 2) {
        ESP_LOGE(TAG, "Decrypted data too short for inner AD");
        return ESP_ERR_INVALID_SIZE;
    }

    uint8_t inner_len = plaintext[0];
    uint8_t inner_tag = plaintext[1];

    if (inner_tag != BLE_ESL_AD_TYPE_ESL) {
        ESP_LOGE(TAG, "Invalid inner AD tag: 0x%02x (expected 0x34)", inner_tag);
        return ESP_ERR_INVALID_SIZE;
    }

    /* inner_len = 1 (tag) + response_payload_len */
    if (inner_len < 1 || (1 + inner_len) > ciphertext_len) {
        ESP_LOGE(TAG, "Inner AD length mismatch: inner_len=%u, ciphertext_len=%u",
                 inner_len, ciphertext_len);
        return ESP_ERR_INVALID_SIZE;
    }

    uint8_t response_payload_len = inner_len - 1; /* subtract ESL Tag */

    /* Step 4: Copy decrypted data into owned buffer and populate output structure */
    memcpy(out_response->payload_buf, plaintext, ciphertext_len);
    out_response->payload = &out_response->payload_buf[2];
    uint8_t *response_payload = out_response->payload;

    out_response->esl_addr = esl_addr;
    out_response->response_slot = response_slot;
    out_response->payload_len = response_payload_len;
    out_response->auth_success = true;

    /* Step 5: Count TLVs by iterating with ble_esl_tlv_decode */
    uint8_t tlv_count = 0;
    uint8_t offset = 0;

    while (offset < response_payload_len) {
        uint8_t opcode;
        const uint8_t *params;
        uint8_t params_len;

        esp_err_t dec_rc = ble_esl_tlv_decode(&response_payload[offset],
                                              response_payload_len - offset,
                                              &opcode, &params, &params_len);
        if (dec_rc != ESP_OK) {
            ESP_LOGW(TAG, "TLV decode failed at offset %u: 0x%x", offset, dec_rc);
            break;
        }

        uint8_t tlv_total = BLE_ESL_TLV_TOTAL_LEN(opcode);
        offset += tlv_total;
        tlv_count++;
    }

    out_response->tlv_count = tlv_count;

    ESP_LOGD(TAG, "Parsed response from ESL 0x%04x: slot=%u, tlvs=%u, len=%u",
             esl_addr, response_slot, tlv_count, response_payload_len);

    return ESP_OK;
}

/* ========================== GAP Event Handling ========================== */

/**
 * @brief GAP event callback for PAwR-related events
 *
 * Handles subevent data requests and subevent responses.
 */
static int pawr_gap_event_cb(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_PER_SUBEV_DATA_REQ:
        pawr_handle_subev_data_req(event);
        return 0;

    case BLE_GAP_EVENT_PER_SUBEV_RESP:
        pawr_handle_subev_response(event);
        return 0;

    default:
        return 0;
    }
}

/**
 * @brief Handle subevent data request from the controller
 *
 * The controller requests subevent data before transmission. For PAwR with ESL,
 * the actual data is set via ble_esl_ap_pawr_send() when commands are queued.
 * Here we provide empty data for subevents that have no pending commands.
 */
static void pawr_handle_subev_data_req(struct ble_gap_event *event)
{
    uint8_t start = event->periodic_adv_subev_data_req.subevent_start;
    uint8_t count = event->periodic_adv_subev_data_req.subevent_data_count;
    uint8_t adv_handle = event->periodic_adv_subev_data_req.adv_handle;

    if (count == 0) {
        return;
    }

    /*
     * Allocate params array on stack. For each requested subevent we supply the
     * queued sync packet (if any) or an empty mbuf; the NimBLE stack rejects
     * NULL data pointers. Note: ble_gap_set_periodic_adv_subev_data always frees
     * the mbufs.
     */
    struct ble_gap_set_periodic_adv_subev_data_params sub_params[count];
    memset(sub_params, 0, sizeof(sub_params));

    for (uint8_t i = 0; i < count; i++) {
        uint8_t sub = (start + i) % g_esl_ap->pawr_config.num_subevents;
        sub_params[i].subevent = sub;
        sub_params[i].response_slot_start = 0;
        sub_params[i].response_slot_count = g_esl_ap->pawr_config.num_response_slots;

        /* Snapshot this subevent's pending slot under the lock, then release it
         * before touching the mbuf pool / HCI. */
        uint8_t pkt[BLE_ESL_AP_PAWR_MAX_AD_BUF_SIZE];
        uint8_t pkt_len = 0;

        xSemaphoreTake(s_pawr_mutex, portMAX_DELAY);
        if (g_esl_ap->pawr_pending != NULL) {
            ble_esl_ap_pawr_pending_t *slot = &g_esl_ap->pawr_pending[sub];
            if (slot->valid && slot->len > 0) {
                pkt_len = slot->len;
                memcpy(pkt, slot->data, pkt_len);
                if (--slot->repeats_left == 0) {
                    slot->valid = false;
                    slot->len = 0;
                }
            }
        }
        xSemaphoreGive(s_pawr_mutex);

        /* Allocate an mbuf (sized to the packet, or empty for idle subevents) */
        struct os_mbuf *mbuf = os_msys_get_pkthdr(pkt_len, 0);
        if (mbuf == NULL) {
            ESP_LOGE(TAG, "Failed to allocate mbuf for subevent %u", sub);
            /* Free already-allocated mbufs before returning */
            for (uint8_t j = 0; j < i; j++) {
                os_mbuf_free_chain(sub_params[j].data);
            }
            return;
        }
        if (pkt_len > 0 && os_mbuf_append(mbuf, pkt, pkt_len) != 0) {
            ESP_LOGE(TAG, "os_mbuf_append failed for subevent %u", sub);
            os_mbuf_free_chain(mbuf);
            for (uint8_t j = 0; j < i; j++) {
                os_mbuf_free_chain(sub_params[j].data);
            }
            return;
        }
        sub_params[i].data = mbuf;
    }

    /* ble_gap_set_periodic_adv_subev_data always frees the mbufs */
    int rc = ble_gap_set_periodic_adv_subev_data(adv_handle, count, sub_params);
    if (rc != 0) {
        ESP_LOGW(TAG, "set_periodic_adv_subev_data failed: %d", rc);
    }
}

/**
 * @brief Handle a PAwR response received in a response slot
 *
 * Extracts the response data and forwards it to the command module
 * for decryption and dispatch.
 */
static void pawr_handle_subev_response(struct ble_gap_event *event)
{
    const struct ble_gap_periodic_adv_response *resp = &event->periodic_adv_response;

    /* Only process complete data */
    if (resp->data_status != BLE_GAP_PER_ADV_DATA_STATUS_COMPLETE) {
        return;
    }

    if (resp->data == NULL || resp->data_length == 0) {
        return;
    }

    uint8_t group_id = resp->subevent;
    uint8_t response_slot = resp->response_slot;

    ESP_LOGD(TAG, "PAwR response: subevent=%u, slot=%u, len=%u",
             group_id, response_slot, resp->data_length);

    /* Forward to command module for decryption and dispatch */
    ble_esl_ap_command_handle_pawr_response(group_id, response_slot,
                                            resp->data, resp->data_length);
}
