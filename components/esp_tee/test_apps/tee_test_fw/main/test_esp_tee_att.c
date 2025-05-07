/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>

#include "esp_log.h"
#include "esp_heap_caps.h"

#include "mbedtls/ecp.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/sha256.h"

#include "esp_tee.h"
#include "esp_tee_attestation.h"
#include "secure_service_num.h"

#include "esp_tee_sec_storage.h"

#include "cJSON.h"
#include "unity.h"

/* Note: negative value here so that assert message prints a grep-able
   error hex value (mbedTLS uses -N for error codes) */
#define TEST_ASSERT_MBEDTLS_OK(X)  TEST_ASSERT_EQUAL_HEX32(0, -(X))

#define SHA256_DIGEST_SZ         (32)
#define ECDSA_SECP256R1_KEY_LEN  (32)

#define ESP_ATT_TK_BUF_SIZE      (1792)
#define ESP_ATT_TK_PSA_CERT_REF  ("0632793520245-10010")

#define ESP_ATT_TK_NONCE         (0xABCD1234)
#define ESP_ATT_TK_CLIENT_ID     (0x0FACADE0)

static const char *TAG = "test_esp_tee_att";

extern int verify_ecdsa_sign(const uint8_t *digest, size_t len, const esp_tee_sec_storage_ecdsa_pubkey_t *pubkey, const esp_tee_sec_storage_ecdsa_sign_t *sign, bool is_crv_p192);

static uint8_t hexchar_to_byte(char hex)
{
    if (hex >= '0' && hex <= '9') {
        return hex - '0';
    } else if (hex >= 'a' && hex <= 'f') {
        return hex - 'a' + 10;
    } else if (hex >= 'A' && hex <= 'F') {
        return hex - 'A' + 10;
    } else {
        // Handle invalid hex characters
        return UINT8_MAX;
    }
}

static void hexstr_to_bytes(const char *hex_str, uint8_t **hex_buf, size_t *buf_sz)
{
    size_t hex_len = strlen(hex_str);

    // Check if the hex string has an even number of characters
    TEST_ASSERT_EQUAL(0, hex_len % 2);

    // Calculate the size of the byte buffer
    *buf_sz = hex_len / 2;

    // Allocate memory for the byte buffer
    *hex_buf = calloc(*buf_sz, sizeof(uint8_t));
    TEST_ASSERT_NOT_NULL(hex_buf);

    // Convert each pair of hex characters to a byte
    for (size_t i = 0; i < *buf_sz; ++i) {
        (*hex_buf)[i] = (hexchar_to_byte(hex_str[2 * i]) << 4) | hexchar_to_byte(hex_str[2 * i + 1]);
    }
}

static int decompress_ecdsa_pubkey(const mbedtls_ecp_group *grp, const unsigned char *input, size_t ilen, esp_tee_sec_storage_ecdsa_pubkey_t *pubkey)
{
    int ret = -1;
    mbedtls_mpi r, x, n;

    size_t plen = mbedtls_mpi_size(&grp->P);

    if (ilen != plen + 1) {
        return (MBEDTLS_ERR_ECP_BAD_INPUT_DATA);
    }

    if (input[0] != 0x02 && input[0] != 0x03) {
        return (MBEDTLS_ERR_ECP_BAD_INPUT_DATA);
    }

    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&x);
    mbedtls_mpi_init(&n);

    // x <= input
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&x, input + 1, plen));

    // r = x^2
    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&r, &x, &x));

    // r = x^2 + a
    if (grp->A.MBEDTLS_PRIVATE(p) == NULL) {
        // Special case where a is -3
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_int(&r, &r, 3));
    } else {
        MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&r, &r, &grp->A));
    }

    // r = x^3 + ax
    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&r, &r, &x));

    // r = x^3 + ax + b
    MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&r, &r, &grp->B));

    // Calculate square root of r over finite field P:
    //   r = sqrt(x^3 + ax + b) = (x^3 + ax + b) ^ ((P + 1) / 4) (mod P)

    // n = P + 1
    MBEDTLS_MPI_CHK(mbedtls_mpi_add_int(&n, &grp->P, 1));

    // n = (P + 1) / 4
    MBEDTLS_MPI_CHK(mbedtls_mpi_shift_r(&n, 2));

    // r ^ ((P + 1) / 4) (mod p)
    MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(&r, &r, &n, &grp->P, NULL));

    // Select solution that has the correct "sign" (equals odd/even solution in finite group)
    if ((input[0] == 0x03) != mbedtls_mpi_get_bit(&r, 0)) {
        // r = p - r
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(&r, &grp->P, &r));
    }

    // y => output
    memcpy(pubkey->pub_x, input + 1, ilen - 1);
    ret = mbedtls_mpi_write_binary(&r, pubkey->pub_y, plen);

cleanup:
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&x);
    mbedtls_mpi_free(&n);

    return (ret);
}

static void prehash_token_data(const char *token_json, uint8_t *digest, size_t len)
{
    TEST_ASSERT_NOT_NULL(token_json);
    TEST_ASSERT_NOT_NULL(digest);
    TEST_ASSERT_NOT_EQUAL(0, len);

    // Parse JSON string
    cJSON *root = cJSON_Parse(token_json);
    TEST_ASSERT_NOT_NULL(root);

    // Fetching the data to be verified
    cJSON *header = cJSON_GetObjectItemCaseSensitive(root, "header");
    TEST_ASSERT_NOT_NULL(header);

    cJSON *eat = cJSON_GetObjectItemCaseSensitive(root, "eat");
    TEST_ASSERT_NOT_NULL(eat);

    cJSON *public_key = cJSON_GetObjectItemCaseSensitive(root, "public_key");
    TEST_ASSERT_NOT_NULL(public_key);

    char *header_str = cJSON_PrintUnformatted(header);
    char *eat_str = cJSON_PrintUnformatted(eat);
    char *public_key_str = cJSON_PrintUnformatted(public_key);

    mbedtls_sha256_context sha256_ctx;

    mbedtls_sha256_init(&sha256_ctx);

    TEST_ASSERT_MBEDTLS_OK(mbedtls_sha256_starts(&sha256_ctx, false));

    TEST_ASSERT_MBEDTLS_OK(mbedtls_sha256_update(&sha256_ctx, (const unsigned char *)header_str, strlen(header_str)));
    TEST_ASSERT_MBEDTLS_OK(mbedtls_sha256_update(&sha256_ctx, (const unsigned char *)eat_str, strlen(eat_str)));
    TEST_ASSERT_MBEDTLS_OK(mbedtls_sha256_update(&sha256_ctx, (const unsigned char *)public_key_str, strlen(public_key_str)));

    TEST_ASSERT_MBEDTLS_OK(mbedtls_sha256_finish(&sha256_ctx, digest));

    mbedtls_sha256_free(&sha256_ctx);

    free(public_key_str);
    free(eat_str);
    free(header_str);

    cJSON_Delete(root);
}

static void fetch_pubkey(const char *token_json, esp_tee_sec_storage_ecdsa_pubkey_t *pubkey_ctx)
{
    TEST_ASSERT_NOT_NULL(token_json);
    TEST_ASSERT_NOT_NULL(pubkey_ctx);

    // Parse JSON string
    cJSON *root = cJSON_Parse(token_json);
    TEST_ASSERT_NOT_NULL(root);

    cJSON *public_key = cJSON_GetObjectItemCaseSensitive(root, "public_key");
    TEST_ASSERT_NOT_NULL(token_json);

    cJSON *compressed = cJSON_GetObjectItemCaseSensitive(public_key, "compressed");
    TEST_ASSERT_NOT_NULL(compressed);

    uint8_t *pubkey_buf = NULL;
    size_t pubkey_buf_sz = 0;
    hexstr_to_bytes(compressed->valuestring, &pubkey_buf, &pubkey_buf_sz);

    mbedtls_ecp_keypair keypair;
    mbedtls_ecp_keypair_init(&keypair);
    mbedtls_ecp_point_init(&keypair.MBEDTLS_PRIVATE(Q));
    mbedtls_ecp_group_init(&keypair.MBEDTLS_PRIVATE(grp));
    TEST_ASSERT_MBEDTLS_OK(mbedtls_ecp_group_load(&keypair.MBEDTLS_PRIVATE(grp), MBEDTLS_ECP_DP_SECP256R1));

    TEST_ASSERT_EQUAL(0, decompress_ecdsa_pubkey(&keypair.MBEDTLS_PRIVATE(grp), pubkey_buf, pubkey_buf_sz, pubkey_ctx));
    mbedtls_ecp_keypair_free(&keypair);

    free(pubkey_buf);
    cJSON_Delete(root);
}

static void fetch_signature(const char *token_json, esp_tee_sec_storage_ecdsa_sign_t *sign_ctx)
{
    TEST_ASSERT_NOT_NULL(token_json);
    TEST_ASSERT_NOT_NULL(sign_ctx);

    // Parse JSON string
    cJSON *root = cJSON_Parse(token_json);
    TEST_ASSERT_NOT_NULL(root);

    cJSON *sign = cJSON_GetObjectItemCaseSensitive(root, "sign");
    TEST_ASSERT_NOT_NULL(sign);

    cJSON *sign_r = cJSON_GetObjectItemCaseSensitive(sign, "r");
    TEST_ASSERT_NOT_NULL(sign_r);

    cJSON *sign_s = cJSON_GetObjectItemCaseSensitive(sign, "s");
    TEST_ASSERT_NOT_NULL(sign_s);

    uint8_t *sign_r_buf = NULL;
    size_t sign_r_buf_sz = 0;
    hexstr_to_bytes(sign_r->valuestring, &sign_r_buf, &sign_r_buf_sz);
    memcpy(sign_ctx->sign_r, sign_r_buf, sign_r_buf_sz);
    free(sign_r_buf);

    uint8_t *sign_s_buf = NULL;
    size_t sign_s_buf_sz = 0;
    hexstr_to_bytes(sign_s->valuestring, &sign_s_buf, &sign_s_buf_sz);
    memcpy(sign_ctx->sign_s, sign_s_buf, sign_s_buf_sz);
    free(sign_s_buf);

    cJSON_Delete(root);
}

TEST_CASE("Test TEE Attestation - Generate and verify the EAT", "[attestation]")
{
    uint8_t *token_buf = heap_caps_calloc(ESP_ATT_TK_BUF_SIZE, sizeof(uint8_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(token_buf);

    // Generating the attestation token
    uint32_t token_len = 0;
    TEST_ESP_OK(esp_tee_att_generate_token(0xA1B2C3D4, 0x0FACADE0, (const char *)ESP_ATT_TK_PSA_CERT_REF,
                                           token_buf, ESP_ATT_TK_BUF_SIZE, &token_len));
    ESP_LOGI(TAG, "EAT generated - length: %"PRIu32"", token_len);

    // Pre-hashing the data
    uint8_t digest[SHA256_DIGEST_SZ] = {};
    prehash_token_data((const char *)token_buf, digest, sizeof(digest));

    // Fetching and decompressing the public key
    esp_tee_sec_storage_ecdsa_pubkey_t pubkey_ctx = {};
    fetch_pubkey((const char *)token_buf, &pubkey_ctx);

    // Fetching the signature components
    esp_tee_sec_storage_ecdsa_sign_t sign_ctx = {};
    fetch_signature((const char *)token_buf, &sign_ctx);

    // Verifying the generated token
    TEST_ASSERT_EQUAL(0, verify_ecdsa_sign(digest, sizeof(digest), &pubkey_ctx, &sign_ctx, false));
    free(token_buf);
}

TEST_CASE("Test TEE Attestation - Invalid token buffer", "[attestation]")
{
    esp_err_t err;
    uint32_t token_len = 0;

    uint8_t *token_buf = heap_caps_calloc(4, sizeof(uint8_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(token_buf);

    err = esp_tee_att_generate_token(ESP_ATT_TK_NONCE, ESP_ATT_TK_CLIENT_ID, (const char *)ESP_ATT_TK_PSA_CERT_REF,
                                     token_buf, 0, &token_len);
    TEST_ESP_ERR(ESP_ERR_INVALID_SIZE, err);

    err = esp_tee_att_generate_token(ESP_ATT_TK_NONCE, ESP_ATT_TK_CLIENT_ID, (const char *)ESP_ATT_TK_PSA_CERT_REF,
                                     NULL, 0, &token_len);
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, err);

    free(token_buf);
}
