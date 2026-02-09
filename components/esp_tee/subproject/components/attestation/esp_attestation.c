/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

#include "esp_log.h"
#include "esp_err.h"

#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "hal/efuse_hal.h"
#include "psa/crypto.h"
#include "esp_attestation.h"
#include "esp_attestation_utils.h"

#include "json_generator.h"

#include "sdkconfig.h"

static const char *TAG = "esp_attestation";

/* ---------------------------------------------- Interface APIs ------------------------------------------------- */

static struct esp_att_sw_claim_list sw_claim_data = SLIST_HEAD_INITIALIZER(sw_claim_data);

static esp_att_sw_claim_list_t *create_sw_claim_entry(esp_att_part_metadata_t *metadata)
{
    esp_att_sw_claim_list_t *sw_claim_entry = calloc(1, sizeof(esp_att_sw_claim_list_t));
    if (sw_claim_entry == NULL) {
        ESP_LOGE(TAG, "Failed to allocate claim data!");
        return NULL;
    }
    memcpy(&sw_claim_entry->metadata, metadata, sizeof(esp_att_part_metadata_t));
    return sw_claim_entry;
}

static void free_sw_claim_list(void)
{
    while (!SLIST_EMPTY(&sw_claim_data)) {
        esp_att_sw_claim_list_t *claim = SLIST_FIRST(&sw_claim_data);
        SLIST_REMOVE_HEAD(&sw_claim_data, next);
        free(claim);
    }
}

static esp_err_t fetch_device_id(uint8_t *devid_buf)
{
    if (devid_buf == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t mac_addr[6] = {0};
    esp_err_t err = esp_efuse_read_field_blob(ESP_EFUSE_MAC, mac_addr, sizeof(mac_addr) * 8);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read MAC from eFuse!");
        goto exit;
    }

    psa_hash_operation_t hash_op = PSA_HASH_OPERATION_INIT;
    psa_status_t status = psa_hash_setup(&hash_op, PSA_ALG_SHA_256);
    if (status != PSA_SUCCESS) {
        return ESP_FAIL;
    }

    status = psa_hash_update(&hash_op, mac_addr, sizeof(mac_addr));
    if (status != PSA_SUCCESS) {
        return ESP_FAIL;
    }

    size_t digest_len = 0;
    status = psa_hash_finish(&hash_op, devid_buf, SHA256_DIGEST_SZ, &digest_len);
    if (status != PSA_SUCCESS) {
        return ESP_FAIL;
    }

    if (digest_len != SHA256_DIGEST_SZ) {
        return ESP_ERR_INVALID_SIZE;
    }

    return ESP_OK;

exit:
    return err;
}

static esp_err_t populate_att_token_cfg(esp_att_token_cfg_t *cfg, const esp_att_ecdsa_keypair_t *keypair)
{
    if (cfg == NULL || keypair == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = fetch_device_id(cfg->device_id);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get the device ID!");
        return err;
    }

    err = esp_att_utils_ecdsa_get_pubkey_digest(keypair, cfg->instance_id, sizeof(cfg->instance_id));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get ECDSA public key hash!");
        return err;
    }

    cfg->device_ver = efuse_hal_chip_revision();
    /* TODO: Decide what all fields we need here */
    cfg->device_stat = 0xA5;

    return ESP_OK;
}

static esp_err_t populate_sw_claim_data(void)
{
    esp_att_part_metadata_t metadata = {};
    esp_att_sw_claim_list_t *claim = NULL;

    esp_err_t err = esp_att_utils_get_btl_claim_data(&metadata);
    if (err != ESP_OK) {
        return err;
    }

    SLIST_INIT(&sw_claim_data);

    claim = create_sw_claim_entry(&metadata);
    if (claim == NULL) {
        return ESP_ERR_NO_MEM;
    }
    SLIST_INSERT_HEAD(&sw_claim_data, claim, next);

    err = esp_att_utils_get_app_claim_data(&metadata);
    if (err != ESP_OK) {
        goto exit;
    }

    claim = create_sw_claim_entry(&metadata);
    if (claim == NULL) {
        err = ESP_ERR_NO_MEM;
        goto exit;
    }
    SLIST_INSERT_HEAD(&sw_claim_data, claim, next);

    err = esp_att_utils_get_tee_claim_data(&metadata);
    if (err != ESP_OK) {
        if (err == ESP_ERR_NOT_SUPPORTED) {
            return ESP_OK;
        }
        return err;
    }

    claim = create_sw_claim_entry(&metadata);
    if (claim == NULL) {
        err = ESP_ERR_NO_MEM;
        goto exit;
    }
    SLIST_INSERT_HEAD(&sw_claim_data, claim, next);

    return ESP_OK;

exit:
    free_sw_claim_list();
    return err;
}

esp_err_t esp_att_generate_token(const uint8_t *auth_challenge, size_t challenge_size,
                                 uint8_t *token_buf, size_t token_buf_size, size_t *token_size)
{
    if (auth_challenge == NULL || token_buf == NULL || token_buf_size == 0 || token_size == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (challenge_size != PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32 &&
            challenge_size != PSA_INITIAL_ATTEST_CHALLENGE_SIZE_48 &&
            challenge_size != PSA_INITIAL_ATTEST_CHALLENGE_SIZE_64) {
        ESP_LOGE(TAG, "Invalid challenge size");
        return ESP_ERR_INVALID_ARG;
    }

    if (token_buf_size < ESP_ATT_TK_MIN_SIZE) {
        ESP_LOGE(TAG, "EAT buffer too small: got %luB, need > %dB", token_buf_size, ESP_ATT_TK_MIN_SIZE);
        return ESP_ERR_INVALID_SIZE;
    }

    esp_err_t err = populate_sw_claim_data();
    if (err != ESP_OK || SLIST_EMPTY(&sw_claim_data)) {
        ESP_LOGE(TAG, "Failed to fetch S/W claim data!");
        return err;
    }

    esp_att_ecdsa_keypair_t keypair = {};
    err = esp_att_utils_ecdsa_gen_keypair_secp256r1(&keypair);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to generate ECDSA key-pair!");
        goto exit;
    }

    esp_att_token_cfg_t cfg = {
        .auth_challenge = (uint8_t *)auth_challenge,
        .challenge_size = challenge_size,
        /* TODO: client_id should point to the API caller (REE or TEE) */
        .client_id = 0x0FACADE0,
        /* TODO: PSA cert ref should be configurable or derived from system config */
        .psa_cert_ref = ESP_ATT_TK_PSA_CERT_REF,
    };

    err = populate_att_token_cfg(&cfg, &keypair);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to populate token config data!");
        goto exit;
    }

    memset(token_buf, 0x00, token_buf_size);

    psa_hash_operation_t hash_op = PSA_HASH_OPERATION_INIT;
    psa_status_t status = psa_hash_setup(&hash_op, PSA_ALG_SHA_256);
    if (status != PSA_SUCCESS) {
        return ESP_FAIL;
    }

    json_gen_str_t jstr;
    json_gen_str_start(&jstr, (char *)token_buf, token_buf_size, NULL, NULL);
    json_gen_start_object(&jstr);

    /* Pushing the Header object */
    const esp_att_token_hdr_t tk_hdr = {};
    char *hdr_json = NULL;
    int hdr_len = -1;
    /* NOTE: Token header is not yet configurable */
    err = esp_att_utils_header_to_json(&tk_hdr, &hdr_json, &hdr_len);
    if (err != ESP_OK || hdr_json == NULL || hdr_len <= 0) {
        ESP_LOGE(TAG, "Failed to format the token header as JSON!");
        return err;
    }
    json_gen_push_object_str(&jstr, "header", hdr_json);

    status = psa_hash_update(&hash_op, (const unsigned char *)hdr_json, hdr_len - 1);
    if (status != PSA_SUCCESS) {
        psa_hash_abort(&hash_op);
        return ESP_FAIL;
    }
    free(hdr_json);

    /* Pushing the EAT object */
    char *eat_json = NULL;
    int eat_len = -1;
    err = esp_att_utils_eat_data_to_json(&sw_claim_data, &cfg, &eat_json, &eat_len);
    if (err != ESP_OK || eat_json == NULL || eat_len <= 0) {
        ESP_LOGE(TAG, "Failed to format the EAT data to JSON!");
        return err;
    }
    json_gen_push_object_str(&jstr, "eat", eat_json);

    status = psa_hash_update(&hash_op, (const unsigned char *)eat_json, eat_len - 1);
    if (status != PSA_SUCCESS) {
        psa_hash_abort(&hash_op);
        return ESP_FAIL;
    }
    free(eat_json);

    char *pubkey_json = NULL;
    int pubkey_len = -1;
    err = esp_att_utils_pubkey_to_json(&keypair, &pubkey_json, &pubkey_len);
    if (err != ESP_OK || pubkey_json == NULL || pubkey_len <= 0) {
        ESP_LOGE(TAG, "Failed to format the public key data to JSON!");
        return err;
    }
    json_gen_push_object_str(&jstr, "public_key", pubkey_json);

    status = psa_hash_update(&hash_op, (const unsigned char *)pubkey_json, pubkey_len - 1);
    if (status != PSA_SUCCESS) {
        psa_hash_abort(&hash_op);
        return ESP_FAIL;
    }
    free(pubkey_json);

    uint8_t digest[SHA256_DIGEST_SZ] = {0};
    size_t digest_len = 0;
    status = psa_hash_finish(&hash_op, digest, sizeof(digest), &digest_len);
    if (status != PSA_SUCCESS) {
        psa_hash_abort(&hash_op);
        return ESP_FAIL;
    }

    char *sign_json = NULL;
    int sign_len = -1;
    err = esp_att_utils_sign_to_json(&keypair, digest, sizeof(digest), &sign_json, &sign_len);
    if (err != ESP_OK || sign_json == NULL || sign_len <= 0) {
        ESP_LOGE(TAG, "Failed to format the token signature to JSON!");
        return err;
    }
    json_gen_push_object_str(&jstr, "sign", sign_json);
    free(sign_json);

    json_gen_end_object(&jstr);
    *token_size = json_gen_str_end(&jstr);
    err = ESP_OK;

exit:
    free_sw_claim_list();
    return err;
}

esp_err_t esp_att_get_token_size(size_t challenge_size, size_t *token_size)
{
    if (token_size == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (challenge_size) {
    case PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32:
    case PSA_INITIAL_ATTEST_CHALLENGE_SIZE_48:
    case PSA_INITIAL_ATTEST_CHALLENGE_SIZE_64:
        *token_size = ESP_ATT_TK_MIN_SIZE;
        break;
    default:
        *token_size = UINT32_MAX;
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}
