/*
 * SPDX-FileCopyrightText: 2019-2025, Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2024-2026 Espressif Systems (Shanghai) CO LTD
 */
#include <string.h>

#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_random.h"

#define MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS
#include "psa/crypto.h"
#include "psa/initial_attestation.h"

#include "esp_tee.h"
#include "secure_service_num.h"

#include "esp_tee_sec_storage.h"

#include "cJSON.h"
#include "unity.h"

#include "test_esp_tee_att_data.h"

/* Note: negative value here so that assert message prints a grep-able
   error hex value (mbedTLS uses -N for error codes) */
#define TEST_ASSERT_MBEDTLS_OK(X)  TEST_ASSERT_EQUAL_HEX32(0, -(X))
#define TEST_ASSERT_PSA_OK(X)  TEST_ASSERT_EQUAL_HEX32(0, -(X))

#define SHA256_DIGEST_SZ         (32)
#define ECDSA_SECP256R1_KEY_LEN  (32)

__attribute__((unused)) static const char *TAG = "test_esp_tee_att";

/* Helper functions */
extern int verify_ecdsa_sign(const esp_tee_sec_storage_type_t key_type, const uint8_t *digest, size_t len, const esp_tee_sec_storage_ecdsa_pubkey_t *pubkey, const esp_tee_sec_storage_ecdsa_sign_t *sign);

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

    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    TEST_ASSERT_PSA_OK(psa_hash_setup(&operation, PSA_ALG_SHA_256));
    size_t digest_len = 0;
    TEST_ASSERT_PSA_OK(psa_hash_update(&operation, (const unsigned char *)header_str, strlen(header_str)));
    TEST_ASSERT_PSA_OK(psa_hash_update(&operation, (const unsigned char *)eat_str, strlen(eat_str)));
    TEST_ASSERT_PSA_OK(psa_hash_update(&operation, (const unsigned char *)public_key_str, strlen(public_key_str)));
    TEST_ASSERT_PSA_OK(psa_hash_finish(&operation, digest, SHA256_DIGEST_SZ, &digest_len));

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
    memcpy(sign_ctx->signature, sign_r_buf, sign_r_buf_sz);
    free(sign_r_buf);

    uint8_t *sign_s_buf = NULL;
    size_t sign_s_buf_sz = 0;
    hexstr_to_bytes(sign_s->valuestring, &sign_s_buf, &sign_s_buf_sz);
    memcpy(sign_ctx->signature + sign_r_buf_sz, sign_s_buf, sign_s_buf_sz);
    free(sign_s_buf);

    cJSON_Delete(root);
}

static void verify_attestation_token(const uint8_t *token_buf, size_t token_len)
{
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
    TEST_ASSERT_EQUAL(0, verify_ecdsa_sign(ESP_SEC_STG_KEY_ECDSA_SECP256R1, digest, sizeof(digest), &pubkey_ctx, &sign_ctx));
}

/* Test-cases */
int32_t psa_initial_attestation_get_token_test(void)
{
    int num_checks = sizeof(check1) / sizeof(check1[0]);
    psa_status_t status;
    size_t token_buffer_size, token_size;
    uint8_t challenge[PSA_INITIAL_ATTEST_CHALLENGE_SIZE_64 + 1];
    uint8_t token_buffer[PSA_INITIAL_ATTEST_MAX_TOKEN_SIZE];

    for (int i = 0; i < num_checks; i++) {
        size_t challenge_size = check1[i].challenge_size;

        printf("Check %d: ", i);
        printf("%s", check1[i].test_desc);

        memset(challenge, 0x2a, sizeof(challenge));
        memset(token_buffer, 0, sizeof(token_buffer));

        status = psa_initial_attest_get_token_size(challenge_size, &token_buffer_size);
        if (status != PSA_SUCCESS) {
            if (challenge_size != PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32 &&
                    challenge_size != PSA_INITIAL_ATTEST_CHALLENGE_SIZE_48 &&
                    challenge_size != PSA_INITIAL_ATTEST_CHALLENGE_SIZE_64) {
                token_buffer_size = check1[i].token_size;
                challenge_size = check1[i].actual_challenge_size;
            } else {
                return status;
            }
        }

        if (token_buffer_size > PSA_INITIAL_ATTEST_MAX_TOKEN_SIZE) {
            printf("Insufficient token buffer size\n");
            return -1;
        }

        status = psa_initial_attest_get_token(challenge, challenge_size, token_buffer,
                                              token_buffer_size, &token_size);

        TEST_ASSERT_EQUAL_HEX32(check1[i].expected_status, status);

        if (check1[i].expected_status != PSA_SUCCESS) {
            continue;
        }

        /* Validate the token */
        verify_attestation_token(token_buffer, token_size);
    }

    return 0;
}

int32_t psa_initial_attestation_get_token_size_test(void)
{
    int num_checks = sizeof(check2) / sizeof(check2[0]);
    psa_status_t status;
    size_t token_size;

    for (int i = 0; i < num_checks; i++) {
        printf("Check %d: ", i);
        printf("%s", check2[i].test_desc);

        status = psa_initial_attest_get_token_size(check2[i].challenge_size, &token_size);

        TEST_ASSERT_EQUAL_HEX32(check2[i].expected_status, status);

        if (check2[i].expected_status != PSA_SUCCESS) {
            continue;
        }

        if (token_size < check2[i].challenge_size) {
            printf("Token size less than challenge size\n");
            return -1;
        }
    }

    return 0;
}

TEST_CASE("PSA Attestation: Test psa_initial_attestation_get_token", "[attestation]")
{
    TEST_ASSERT_PSA_OK(psa_initial_attestation_get_token_test());
}

TEST_CASE("PSA Attestation: Test psa_initial_attestation_get_token_size", "[attestation]")
{
    TEST_ASSERT_PSA_OK(psa_initial_attestation_get_token_size_test());
}
