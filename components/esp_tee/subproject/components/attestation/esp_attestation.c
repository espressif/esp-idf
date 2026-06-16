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

#include "mbedtls/sha256.h"

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

static esp_err_t fetch_ueids(esp_att_token_cfg_t *cfg)
{
    /* UEID: raw eFuse MAC */
    esp_err_t err = esp_efuse_read_field_blob(ESP_EFUSE_MAC, cfg->ueid_mac,
                                              ESP_ATT_EAT_UEID_MAC_SZ * 8);
    if (err != ESP_OK) {
        return err;
    }

    /* UEID: 128-bit OPTIONAL_UNIQUE_ID */
    err = esp_efuse_read_field_blob(ESP_EFUSE_OPTIONAL_UNIQUE_ID, cfg->ueid_opt_id,
                                    ESP_ATT_EAT_UEID_OPT_ID_SZ * 8);
    if (err != ESP_OK) {
        return err;
    }

    return ESP_OK;
}

static esp_err_t populate_att_token_cfg(esp_att_token_cfg_t *cfg, const esp_att_ecdsa_keypair_t *keypair)
{
    if (cfg == NULL || keypair == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = fetch_ueids(cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get the UEIDs!");
        return err;
    }

    /* Device ID = SHA-256 of the MAC */
    int ret = mbedtls_sha256(cfg->ueid_mac, sizeof(cfg->ueid_mac), cfg->device_id, 0);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to derive the device ID!");
        return ESP_FAIL;
    }

    err = esp_att_utils_ecdsa_get_pubkey_digest(keypair, cfg->instance_id, sizeof(cfg->instance_id));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get ECDSA public key hash!");
        return err;
    }

    /* Chip ID read from the ROM */
    extern const uint32_t _rom_chip_id;
    cfg->chip_id = _rom_chip_id;
    /* Chip revision read from eFuse */
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

esp_err_t esp_att_generate_token(const uint32_t nonce, const uint32_t client_id, const char *psa_cert_ref,
                                 uint8_t *token_buf, const size_t token_buf_size, uint32_t *token_len)
{
    if (token_buf == NULL || token_len == NULL || psa_cert_ref == NULL) {
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
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    char *hdr_json = NULL;
    char *eat_json = NULL;
    char *pubkey_json = NULL;
    char *sign_json = NULL;

    err = esp_att_utils_ecdsa_gen_keypair_secp256r1(&keypair);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to generate ECDSA key-pair!");
        goto exit;
    }

    esp_att_token_cfg_t cfg = {
        .nonce = nonce,
        .client_id = client_id,
    };
    memcpy(cfg.psa_cert_ref, psa_cert_ref, sizeof(cfg.psa_cert_ref));

    err = populate_att_token_cfg(&cfg, &keypair);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to populate token config data!");
        goto exit;
    }

    memset(token_buf, 0x00, token_buf_size);

    int ret = mbedtls_sha256_starts(&ctx, false);
    if (ret != 0) {
        err = ESP_FAIL;
        goto exit;
    }

    json_gen_str_t jstr;
    json_gen_str_start(&jstr, (char *)token_buf, token_buf_size, NULL, NULL);
    json_gen_start_object(&jstr);

    /* Pushing the Header object */
    const esp_att_token_hdr_t tk_hdr = {};
    int hdr_len = -1;
    /* NOTE: Token header is not yet configurable */
    err = esp_att_utils_header_to_json(&tk_hdr, &hdr_json, &hdr_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to format the token header as JSON!");
        goto exit;
    }
    json_gen_push_object_str(&jstr, "header", hdr_json);

    ret = mbedtls_sha256_update(&ctx, (const unsigned char *)hdr_json, hdr_len - 1);
    if (ret != 0) {
        err = ESP_FAIL;
        goto exit;
    }
    free(hdr_json);
    hdr_json = NULL;

    /* Pushing the EAT object */
    int eat_len = -1;
    err = esp_att_utils_eat_data_to_json(&sw_claim_data, &cfg, &eat_json, &eat_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to format the EAT data to JSON!");
        goto exit;
    }
    json_gen_push_object_str(&jstr, "eat", eat_json);

    ret = mbedtls_sha256_update(&ctx, (const unsigned char *)eat_json, eat_len - 1);
    if (ret != 0) {
        err = ESP_FAIL;
        goto exit;
    }
    free(eat_json);
    eat_json = NULL;

    int pubkey_len = -1;
    err = esp_att_utils_pubkey_to_json(&keypair, &pubkey_json, &pubkey_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to format the public key data to JSON!");
        goto exit;
    }
    json_gen_push_object_str(&jstr, "public_key", pubkey_json);

    ret = mbedtls_sha256_update(&ctx, (const unsigned char *)pubkey_json, pubkey_len - 1);
    if (ret != 0) {
        err = ESP_FAIL;
        goto exit;
    }
    free(pubkey_json);
    pubkey_json = NULL;

    uint8_t digest[SHA256_DIGEST_SZ] = {0};
    ret = mbedtls_sha256_finish(&ctx, digest);
    if (ret != 0) {
        err = ESP_FAIL;
        goto exit;
    }

    int sign_len = -1;
    err = esp_att_utils_sign_to_json(&keypair, digest, sizeof(digest), &sign_json, &sign_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to format the token signature to JSON!");
        goto exit;
    }
    json_gen_push_object_str(&jstr, "sign", sign_json);
    free(sign_json);
    sign_json = NULL;

    json_gen_end_object(&jstr);
    *token_len = json_gen_str_end(&jstr);
    err = ESP_OK;

exit:
    mbedtls_sha256_free(&ctx);
    free(hdr_json);
    free(eat_json);
    free(pubkey_json);
    free(sign_json);
    free_sw_claim_list();
    return err;
}
