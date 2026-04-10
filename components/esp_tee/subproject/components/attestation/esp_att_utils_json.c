/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "esp_log.h"
#include "esp_err.h"

#if ESP_TEE_BUILD
#include "bootloader_sha.h"
#include "esp_tee_sec_storage.h"
#endif
#include "esp_random.h"

#include "json_generator.h"
#include "esp_attestation_utils.h"

#define DIGEST_HEXSTR_LEN           (MAX_DIGEST_SZ * 2 + 1)

static const char *TAG = "esp_att_utils_json";

static const char *str_from_fw_type(esp_att_part_type_t fw, size_t *length)
{
    if (fw >= ESP_ATT_PART_TYPE_MAX) {
        if (length) {
            *length = SIZE_MAX;
        }
        return NULL;
    }
    static const char *fw_type_str[] = {"bootloader", "tee", "app", "other"};
    if (length) {
        *length = strlen(fw_type_str[fw]);
    }
    return fw_type_str[fw];
}

static esp_err_t part_metadata_to_json(const esp_att_part_metadata_t *metadata, char **claim_json)
{
    if (metadata == NULL || claim_json == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    char *json_buf = calloc(ESP_ATT_CLAIM_JSON_MAX_SZ, sizeof(char));
    if (json_buf == NULL) {
        return ESP_ERR_NO_MEM;
    }

    json_gen_str_t json_gen;

    // Initialize the JSON string generator
    json_gen_str_start(&json_gen, json_buf, ESP_ATT_CLAIM_JSON_MAX_SZ, NULL, NULL);

    // Start the top-level JSON object
    json_gen_start_object(&json_gen);

    // Add the properties within the "app" object
    json_gen_obj_set_int(&json_gen, "type", metadata->type);
    json_gen_obj_set_string(&json_gen, "ver", (char *)metadata->ver);
    json_gen_obj_set_string(&json_gen, "idf_ver", (char *)metadata->idf_ver);
    json_gen_obj_set_int(&json_gen, "secure_ver", metadata->secure_ver);

    // Add "part_chip_rev" object
    json_gen_push_object(&json_gen, "part_chip_rev");
    json_gen_obj_set_int(&json_gen, "min", metadata->part_chip_rev.min_chip_rev);
    json_gen_obj_set_int(&json_gen, "max", metadata->part_chip_rev.max_chip_rev);
    json_gen_pop_object(&json_gen);

    // Add "part_digest" object
    json_gen_push_object(&json_gen, "part_digest");
    json_gen_obj_set_int(&json_gen, "type", metadata->part_digest.type);

    char calc_digest_hexstr[DIGEST_HEXSTR_LEN];
    esp_err_t err = esp_att_utils_hexbuf_to_hexstr(metadata->part_digest.calc_digest, sizeof(metadata->part_digest.calc_digest),
                                                   calc_digest_hexstr, sizeof(calc_digest_hexstr));
    if (err != ESP_OK) {
        return err;
    }
    json_gen_obj_set_string(&json_gen, "calc_digest", calc_digest_hexstr);

    json_gen_obj_set_bool(&json_gen, "digest_validated", metadata->part_digest.digest_validated);
    json_gen_obj_set_bool(&json_gen, "sign_verified", metadata->part_digest.sign_verified);
    if (metadata->type == ESP_ATT_PART_TYPE_TEE || metadata->type == ESP_ATT_PART_TYPE_APP) {
        json_gen_obj_set_bool(&json_gen, "secure_padding", metadata->part_digest.secure_padding);
    }
    json_gen_pop_object(&json_gen);

    // End the top-level JSON object
    json_gen_end_object(&json_gen);

    // Finalize the JSON string generation
    json_gen_str_end(&json_gen);

    *claim_json = json_buf;
    return ESP_OK;
}

esp_err_t esp_att_utils_header_to_json(const esp_att_token_hdr_t *tk_hdr, char **header_json, int *len)
{
    /* NOTE: Token header is not yet configurable, thus will be left empty for now */
    if (tk_hdr == NULL || header_json == NULL || len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    char *json_buf = calloc(ESP_ATT_HDR_JSON_MAX_SZ, sizeof(char));
    if (json_buf == NULL) {
        return ESP_ERR_NO_MEM;
    }

    json_gen_str_t json_gen;

    // Initialize the JSON string generator
    json_gen_str_start(&json_gen, json_buf, ESP_ATT_HDR_JSON_MAX_SZ, NULL, NULL);

    // Start the top-level JSON object
    json_gen_start_object(&json_gen);

    json_gen_obj_set_string(&json_gen, "magic", ESP_ATT_TK_MAGIC_STR);
    json_gen_obj_set_string(&json_gen, "encr_alg", NULL);
    json_gen_obj_set_string(&json_gen, "sign_alg", ESP_ATT_TK_SIGN_ALG);

    json_gen_obj_set_string(&json_gen, "key_id", ESP_ATT_TK_KEY_ID);

    // End the top-level JSON object
    json_gen_end_object(&json_gen);

    // Finalize the JSON string generation
    *len = json_gen_str_end(&json_gen);

    *header_json = json_buf;
    return ESP_OK;
}

esp_err_t esp_att_utils_eat_data_to_json(struct esp_att_sw_claim_list *head, const esp_att_token_cfg_t *cfg, char **eat_json, int *len)
{
    if (head == NULL || eat_json == NULL || len == NULL || cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    char *json_buf = calloc(ESP_ATT_EAT_JSON_MAX_SZ, sizeof(char));
    if (json_buf == NULL) {
        return ESP_ERR_NO_MEM;
    }

    json_gen_str_t json_gen;

    // Initialize the JSON string generator
    json_gen_str_start(&json_gen, json_buf, ESP_ATT_EAT_JSON_MAX_SZ, NULL, NULL);

    // Start the top-level JSON object
    json_gen_start_object(&json_gen);
    esp_att_sw_claim_list_t *claim = NULL;
    char *claim_json = NULL;

    // Convert auth_challenge to hex string - allocate based on actual challenge size
    size_t auth_challenge_hexstr_size = cfg->challenge_size * 2 + 1;
    char *auth_challenge_hexstr = calloc(auth_challenge_hexstr_size, sizeof(char));
    if (auth_challenge_hexstr == NULL) {
        free(json_buf);
        return ESP_ERR_NO_MEM;
    }

    esp_err_t err = esp_att_utils_hexbuf_to_hexstr(cfg->auth_challenge, cfg->challenge_size,
                                                   auth_challenge_hexstr, auth_challenge_hexstr_size);
    if (err != ESP_OK) {
        free(auth_challenge_hexstr);
        free(json_buf);
        return err;
    }
    json_gen_obj_set_string(&json_gen, "auth_challenge", auth_challenge_hexstr);
    free(auth_challenge_hexstr);

    json_gen_obj_set_int(&json_gen, "client_id", cfg->client_id);
    json_gen_obj_set_int(&json_gen, "device_ver", cfg->device_ver);

    char dev_id_hexstr[ESP_ATT_EAT_DEV_ID_SZ * 2 + 1] = {0};
    err = esp_att_utils_hexbuf_to_hexstr(cfg->device_id, sizeof(cfg->device_id), dev_id_hexstr, sizeof(dev_id_hexstr));
    if (err != ESP_OK) {
        free(json_buf);
        return err;
    }
    json_gen_obj_set_string(&json_gen, "device_id", dev_id_hexstr);

    /* NOTE: Instance ID is the SHA256 of the ECDSA public key in usage */
    char inst_id_hexstr[DIGEST_HEXSTR_LEN] = {0};
    err = esp_att_utils_hexbuf_to_hexstr(cfg->instance_id, sizeof(cfg->instance_id), inst_id_hexstr, sizeof(inst_id_hexstr));
    if (err != ESP_OK) {
        free(json_buf);
        return err;
    }
    json_gen_obj_set_string(&json_gen, "instance_id", inst_id_hexstr);

    json_gen_obj_set_string(&json_gen, "psa_cert_ref", cfg->psa_cert_ref);
    json_gen_obj_set_int(&json_gen, "device_status", cfg->device_stat);

    json_gen_push_object(&json_gen, "sw_claims");
    SLIST_FOREACH(claim, head, next) {
        esp_err_t err = part_metadata_to_json(&claim->metadata, &claim_json);
        if (err != ESP_OK || claim_json == NULL) {
            ESP_LOGE(TAG, "Failed to format the FW metadata to JSON!");
            return err;
        }

        json_gen_push_object_str(&json_gen, (char *)str_from_fw_type(claim->metadata.type, NULL), claim_json);
        free(claim_json);
    }
    json_gen_pop_object(&json_gen);

    // End the top-level JSON object
    json_gen_end_object(&json_gen);

    // Finalize the JSON string generation
    *len = json_gen_str_end(&json_gen);
    *eat_json = json_buf;

    return ESP_OK;
}

esp_err_t esp_att_utils_pubkey_to_json(const esp_att_ecdsa_keypair_t *keypair, char **pubkey_json, int *len)
{
    if (keypair == NULL || pubkey_json == NULL || len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    char *json_buf = calloc(ESP_ATT_PUBKEY_JSON_MAX_SZ, sizeof(char));
    if (json_buf == NULL) {
        return ESP_ERR_NO_MEM;
    }

    json_gen_str_t json_gen;

    // Initialize the JSON string generator
    json_gen_str_start(&json_gen, json_buf, ESP_ATT_PUBKEY_JSON_MAX_SZ, NULL, NULL);

    // Start the top-level JSON object
    json_gen_start_object(&json_gen);

    char *pubkey_hexstr = NULL;
    esp_err_t err = esp_att_utils_ecdsa_get_pubkey(keypair, &pubkey_hexstr);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get public key!");
        return err;
    }

    json_gen_obj_set_string(&json_gen, "compressed", pubkey_hexstr);

    // End the top-level JSON object
    json_gen_end_object(&json_gen);

    // Finalize the JSON string generation
    *len = json_gen_str_end(&json_gen);
    *pubkey_json = json_buf;

    free(pubkey_hexstr);

    return ESP_OK;
}

esp_err_t esp_att_utils_sign_to_json(const esp_att_ecdsa_keypair_t *keypair, const uint8_t *digest, const size_t digest_len,
                                     char **sign_json, int *len)
{
    if (keypair == NULL || digest == NULL || sign_json == NULL || len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    char *json_buf = calloc(ESP_ATT_SIGN_JSON_MAX_SZ, sizeof(char));
    if (json_buf == NULL) {
        return ESP_ERR_NO_MEM;
    }

    json_gen_str_t json_gen;

    // Initialize the JSON string generator
    json_gen_str_start(&json_gen, json_buf, ESP_ATT_SIGN_JSON_MAX_SZ, NULL, NULL);

    // Start the top-level JSON object
    json_gen_start_object(&json_gen);

    char *sign_r_hexstr = NULL, *sign_s_hexstr = NULL;
    esp_err_t err = esp_att_utils_ecdsa_get_sign(keypair, digest, digest_len, &sign_r_hexstr, &sign_s_hexstr);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to sign token!");
        return err;
    }

    json_gen_obj_set_string(&json_gen, "r", sign_r_hexstr);
    json_gen_obj_set_string(&json_gen, "s", sign_s_hexstr);

    // End the top-level JSON object
    json_gen_end_object(&json_gen);

    // Finalize the JSON string generation
    *len = json_gen_str_end(&json_gen);
    *sign_json = json_buf;

    free(sign_r_hexstr);
    free(sign_s_hexstr);

    return ESP_OK;
}
