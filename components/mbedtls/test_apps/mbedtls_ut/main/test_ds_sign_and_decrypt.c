/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "unity.h"
#include "mbedtls/private/rsa.h"
#include "esp_random.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "hal/hmac_types.h"
#include "esp_efuse.h"

#ifdef CONFIG_MBEDTLS_HARDWARE_RSA_DS_PERIPHERAL
#include "esp_ds.h"
#include "psa_crypto_driver_esp_rsa_ds.h"

esp_ds_data_ctx_t *esp_secure_cert_get_ds_ctx(void)
{
    // In real implementation, this function retrieves the DS key context from secure storage
    // For testing purposes, we will create a mock DS key context

    esp_ds_data_ctx_t *ds_key = calloc(1, sizeof(esp_ds_data_ctx_t));
    if (ds_key == NULL) {
        return NULL;
    }

    // Mock RSA key parameters
    ds_key->rsa_length_bits = 2048;
    ds_key->efuse_key_id = 0;
    ds_key->esp_ds_data = calloc(1, sizeof(esp_ds_data_t));
    if (ds_key->esp_ds_data != NULL) {
        /* rsa_length must match rsa_length_bits for driver validation */
        ds_key->esp_ds_data->rsa_length = (ds_key->rsa_length_bits / 32) - 1;
    }
    // Fill in other necessary fields as per esp_ds_data_ctx_t definition
    // For simplicity, we will leave them zeroed out

    return ds_key;
}

void esp_secure_cert_free_ds_ctx(esp_ds_data_ctx_t *ds_key)
{
    if (ds_key != NULL) {
        if (ds_key->esp_ds_data != NULL) {
            free(ds_key->esp_ds_data);
        }
        free(ds_key);
    }
}

TEST_CASE("ds sign test pkcs1_v15 PSA validation", "[ds_rsa_psa]")
{
    psa_key_id_t keyt_id;
    psa_status_t status;
    psa_algorithm_t alg = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256);

    esp_rsa_ds_opaque_key_t rsa_ds_opaque_key = {0};
    rsa_ds_opaque_key.ds_data_ctx = esp_secure_cert_get_ds_ctx();
    TEST_ASSERT_NOT_NULL(rsa_ds_opaque_key.ds_data_ctx);

    rsa_ds_opaque_key.ds_data_ctx->efuse_key_id = EFUSE_BLK_KEY_MAX; // Invalid efuse block to trigger validation failure

    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&attributes, PSA_KEY_TYPE_RSA_KEY_PAIR);
    psa_set_key_bits(&attributes, rsa_ds_opaque_key.ds_data_ctx->rsa_length_bits);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_ESP_RSA_DS_VOLATILE);
    status = psa_import_key(&attributes,
                            (const uint8_t *)&rsa_ds_opaque_key,
                            sizeof(rsa_ds_opaque_key),
                            &keyt_id);
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

    rsa_ds_opaque_key.ds_data_ctx->efuse_key_id = EFUSE_BLK_KEY0; // Reset to valid efuse block
    rsa_ds_opaque_key.ds_data_ctx->rsa_length_bits = 1000; // Invalid RSA length to trigger validation failure
    status = psa_import_key(&attributes,
                            (const uint8_t *)&rsa_ds_opaque_key,
                            sizeof(rsa_ds_opaque_key),
                            &keyt_id);
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

    rsa_ds_opaque_key.ds_data_ctx->rsa_length_bits = 2048; // Reset to valid RSA length
    esp_ds_data_t *ds_data_backup = rsa_ds_opaque_key.ds_data_ctx->esp_ds_data;
    rsa_ds_opaque_key.ds_data_ctx->esp_ds_data = NULL; // NULL esp_ds_data to trigger validation failure
    status = psa_import_key(&attributes,
                            (const uint8_t *)&rsa_ds_opaque_key,
                            sizeof(rsa_ds_opaque_key),
                            &keyt_id);
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

    rsa_ds_opaque_key.ds_data_ctx->esp_ds_data = ds_data_backup; // Restore esp_ds_data

    esp_secure_cert_free_ds_ctx(rsa_ds_opaque_key.ds_data_ctx);
}

TEST_CASE("ds sign test pkcs1_v15 PSA", "[ds_rsa_psa]")
{
    psa_key_id_t keyt_id;
    psa_status_t status;
    psa_algorithm_t alg = PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256);

    esp_rsa_ds_opaque_key_t rsa_ds_opaque_key = {0};
    rsa_ds_opaque_key.ds_data_ctx = esp_secure_cert_get_ds_ctx();
    TEST_ASSERT_NOT_NULL(rsa_ds_opaque_key.ds_data_ctx);

    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&attributes, PSA_KEY_TYPE_RSA_KEY_PAIR);
    psa_set_key_bits(&attributes, rsa_ds_opaque_key.ds_data_ctx->rsa_length_bits);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_ESP_RSA_DS_VOLATILE);
    status = psa_import_key(&attributes,
                            (const uint8_t *)&rsa_ds_opaque_key,
                            sizeof(rsa_ds_opaque_key),
                            &keyt_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    unsigned char hash[32] = {0};
    size_t hash_length = 0;
    uint8_t input[7] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    status = psa_hash_compute(PSA_ALG_SHA_256, input, 7, hash, sizeof(hash), &hash_length);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    TEST_ASSERT_EQUAL(32, hash_length);

    unsigned char signature[256] = {0};
    size_t signature_length = 0;
    status = psa_sign_hash(keyt_id, alg, hash, hash_length, signature, sizeof(signature), &signature_length);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    TEST_ASSERT_EQUAL(256, signature_length);

    status = psa_destroy_key(keyt_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    psa_reset_key_attributes(&attributes);

    // Free the DS context to prevent memory leak
    esp_secure_cert_free_ds_ctx(rsa_ds_opaque_key.ds_data_ctx);

    // Because we have wrapped around the ds_start_sign and ds_finish_sign functions,
    // we are not actually performing the real signing operation. That test is done in the
    // crypto test_apps, so here we just check that the surrounding code works as expected.
    // In this test, we have used v15 padding, so we expect the signature to be non-null
    // and the hash to be part of the signature.
    TEST_ASSERT_EQUAL(0, memcmp(hash, signature + (256 - hash_length), hash_length));

    // Let's also ensure that signature has correct encoding
    // Just before the hash start, it should have size of hash
    TEST_ASSERT_EQUAL(hash_length, signature[256 - hash_length - 1]);

    // One byte before should be MBEDTLS_ASN1_OCTET_STRING
    TEST_ASSERT_EQUAL(0x04, signature[256 - hash_length - 2]);

    // And the first byte should be 0x00, indicating that this is a valid PKCS#1 v1.5 signature
    TEST_ASSERT_EQUAL(0x00, signature[0]);
}

const unsigned char message[] = {
    0x62, 0x1c, 0xaa, 0x4a, 0xae, 0xf8, 0x1f, 0x4b, 0x59, 0x70, 0xee, 0xcb, 0x0c, 0x91, 0x35, 0xc9,
    0x4a, 0xe2, 0x85, 0xf4, 0xfc, 0x21, 0x18, 0x3e, 0xa6, 0xed, 0xa6, 0x71, 0xdb, 0xfe, 0x2b, 0x95,
    0x67, 0x45, 0xb7,
};
const size_t message_len = 35;

#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
TEST_CASE("ds sign test pkcs1_v21 PSA", "[ds_rsa_psa]")
{
    psa_key_id_t keyt_id;
    psa_status_t status;
    psa_algorithm_t alg = PSA_ALG_RSA_PSS(PSA_ALG_SHA_256);
    esp_rsa_ds_opaque_key_t rsa_ds_opaque_key = {0};
    rsa_ds_opaque_key.ds_data_ctx = esp_secure_cert_get_ds_ctx();
    TEST_ASSERT_NOT_NULL(rsa_ds_opaque_key.ds_data_ctx);

    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&attributes, PSA_KEY_TYPE_RSA_KEY_PAIR);
    psa_set_key_bits(&attributes, rsa_ds_opaque_key.ds_data_ctx->rsa_length_bits);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_ESP_RSA_DS_VOLATILE);
    status = psa_import_key(&attributes,
                            (const uint8_t *)&rsa_ds_opaque_key,
                            sizeof(rsa_ds_opaque_key),
                            &keyt_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    unsigned char hash[32] = {0};
    size_t hash_length = 0;
    uint8_t input[7] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    status = psa_hash_compute(PSA_ALG_SHA_256, input, 7, hash, sizeof(hash), &hash_length);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    TEST_ASSERT_EQUAL(32, hash_length);

    unsigned char signature[256] = {0};
    size_t signature_length = 0;
    status = psa_sign_hash(keyt_id, alg, hash, hash_length, signature, sizeof(signature), &signature_length);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    TEST_ASSERT_EQUAL(256, signature_length);

    status = psa_destroy_key(keyt_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    psa_reset_key_attributes(&attributes);

    // Free the DS context to prevent memory leak
    esp_secure_cert_free_ds_ctx(rsa_ds_opaque_key.ds_data_ctx);
}

/* Generated external data for OAEP padding */
const unsigned char oaep_padded_v21[] = {
    0x00, 0xb6, 0xd9, 0xe6, 0x62, 0xa5, 0xa0, 0xf7, 0xc7, 0xfe, 0xb9, 0x33, 0xdd, 0xdd, 0x67, 0xd7,
    0x35, 0xa5, 0x9d, 0xf7, 0x21, 0x55, 0xf9, 0x60, 0xa8, 0x38, 0x86, 0xb2, 0x73, 0x90, 0xf5, 0x40,
    0x06, 0x85, 0x51, 0x60, 0x10, 0x01, 0x91, 0x24, 0x47, 0x2e, 0x86, 0x16, 0xa6, 0xbc, 0x35, 0x5b,
    0xe4, 0xca, 0x74, 0x9c, 0xe2, 0xe0, 0x2b, 0x2d, 0x63, 0x56, 0x0f, 0x71, 0xe0, 0x8e, 0x1c, 0xa5,
    0x58, 0x72, 0x30, 0x2f, 0x93, 0xf7, 0xd9, 0x0f, 0x16, 0xed, 0x86, 0xd3, 0xbf, 0x42, 0x12, 0x0a,
    0xaa, 0x3b, 0x16, 0x43, 0x84, 0x0d, 0x6d, 0xc8, 0xf9, 0x22, 0xc7, 0x25, 0xf6, 0x61, 0xb4, 0xb8,
    0xd7, 0x07, 0x76, 0x1a, 0xfb, 0x01, 0x01, 0xd7, 0xd2, 0x9b, 0xc1, 0xec, 0x34, 0x53, 0x14, 0x78,
    0xc4, 0x10, 0xf5, 0xf1, 0x28, 0xf6, 0x30, 0x32, 0xe7, 0x8c, 0x27, 0x62, 0x17, 0xf0, 0x47, 0x10,
    0x87, 0xfa, 0xe6, 0x02, 0x90, 0x45, 0xf9, 0x20, 0x79, 0x42, 0xf9, 0x2a, 0x42, 0x06, 0xae, 0x37,
    0xf4, 0xae, 0x12, 0x6b, 0x9c, 0x7b, 0x7e, 0xed, 0x85, 0xdf, 0xdd, 0x27, 0x27, 0xf5, 0xac, 0xb6,
    0x9c, 0x7a, 0xd9, 0x1f, 0x45, 0xd8, 0xb3, 0xed, 0x73, 0xd7, 0x9e, 0xab, 0x68, 0xb8, 0x25, 0xeb,
    0xc5, 0xcc, 0x8a, 0x04, 0x03, 0xd3, 0xd8, 0x60, 0xcc, 0xab, 0xe9, 0xd1, 0xb1, 0x18, 0x28, 0x84,
    0xf9, 0x52, 0xd6, 0xe2, 0x3c, 0x2a, 0x19, 0x5c, 0xd8, 0x73, 0xff, 0x71, 0x94, 0xd6, 0x8b, 0x5f,
    0x69, 0x09, 0x2f, 0xd2, 0xb0, 0x23, 0xda, 0x1a, 0xe4, 0x47, 0x2d, 0xb6, 0xbf, 0x08, 0xbd, 0x5d,
    0x37, 0x9c, 0x81, 0xdd, 0x54, 0x42, 0xad, 0xf7, 0x65, 0xb2, 0x8e, 0xf7, 0x70, 0x7d, 0x62, 0x0a,
    0x3a, 0x1c, 0xf5, 0xe8, 0x9f, 0x17, 0x22, 0x66, 0x3d, 0xc5, 0xab, 0xf6, 0x51, 0xe9, 0x84, 0x73,
};

TEST_CASE("ds decrypt test pkcs1_v21 PSA", "[ds_rsa]")
{
    psa_key_id_t keyt_id;
    psa_status_t status;
    psa_algorithm_t alg = PSA_ALG_RSA_OAEP(PSA_ALG_SHA_256);

    esp_rsa_ds_opaque_key_t rsa_ds_opaque_key = {0};
    rsa_ds_opaque_key.ds_data_ctx = esp_secure_cert_get_ds_ctx();
    TEST_ASSERT_NOT_NULL(rsa_ds_opaque_key.ds_data_ctx);

    printf("DS Key RSA Length Bits: %d\n", rsa_ds_opaque_key.ds_data_ctx->rsa_length_bits);

    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&attributes, PSA_KEY_TYPE_RSA_KEY_PAIR);
    psa_set_key_bits(&attributes, rsa_ds_opaque_key.ds_data_ctx->rsa_length_bits);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_ESP_RSA_DS_VOLATILE);
    status = psa_import_key(&attributes,
                            (const uint8_t *)&rsa_ds_opaque_key,
                            sizeof(rsa_ds_opaque_key),
                            &keyt_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    unsigned char decrypted[256] = {0};
    size_t decrypted_len = 0;

    status = psa_asymmetric_decrypt(keyt_id, alg, oaep_padded_v21, sizeof(oaep_padded_v21),
                                   NULL, 0,
                                   decrypted, sizeof(decrypted),
                                   &decrypted_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    TEST_ASSERT_EQUAL(message_len, decrypted_len);
    TEST_ASSERT_EQUAL_MEMORY(decrypted, message, message_len);

    status = psa_destroy_key(keyt_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    psa_reset_key_attributes(&attributes);
    // Free the DS context to prevent memory leak
    esp_secure_cert_free_ds_ctx(rsa_ds_opaque_key.ds_data_ctx);
}
#endif /* CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 */

const unsigned char v15_padded[] = {
    0x00, 0x02, 0xdf, 0x36, 0xfc, 0x41, 0x57, 0x40, 0x87, 0x3f, 0x88, 0xa7, 0x7f, 0x7a, 0x33, 0xbe,
    0x2f, 0xf7, 0xdd, 0x8a, 0xb6, 0x6a, 0xb4, 0x6a, 0x96, 0xdb, 0x63, 0xbf, 0x50, 0xd8, 0x08, 0x5b,
    0x07, 0x9e, 0xdf, 0x4c, 0x1e, 0xb2, 0x6c, 0x83, 0x22, 0xff, 0x21, 0xc8, 0x99, 0xe1, 0x6c, 0x3d,
    0x31, 0x82, 0x8e, 0xb6, 0xb3, 0x3f, 0x77, 0xc6, 0x7d, 0xb1, 0x05, 0x97, 0xd0, 0x96, 0x49, 0x78,
    0x62, 0xd5, 0x7b, 0x5b, 0x49, 0x8c, 0xb0, 0x4e, 0x5f, 0xf2, 0x92, 0xc2, 0x9f, 0xd4, 0x77, 0x0a,
    0x77, 0x94, 0x9f, 0x80, 0xff, 0xde, 0x6a, 0xe7, 0x62, 0x36, 0x9f, 0x73, 0x78, 0xb3, 0xc0, 0x2f,
    0xd8, 0xf9, 0x06, 0x60, 0x78, 0xbc, 0x50, 0x06, 0x0d, 0x33, 0x6c, 0x5f, 0xcc, 0x4d, 0x40, 0x4e,
    0x12, 0x53, 0x39, 0x3b, 0x24, 0x4f, 0x9c, 0x14, 0x20, 0xbd, 0x71, 0x1d, 0xdc, 0xc5, 0xbc, 0x88,
    0xd1, 0x87, 0x4a, 0xac, 0x21, 0xb6, 0x06, 0x9d, 0x56, 0xe5, 0xb7, 0x05, 0x61, 0x32, 0x30, 0x97,
    0x7d, 0x72, 0x2f, 0x45, 0xf0, 0xc6, 0x55, 0x01, 0x87, 0x78, 0xbc, 0xa4, 0x9b, 0x4f, 0xe1, 0xc5,
    0x59, 0x8a, 0xaa, 0x3c, 0xd1, 0x0a, 0xe3, 0xbe, 0x0b, 0xde, 0x21, 0xa8, 0x3b, 0x89, 0x9f, 0x0a,
    0x30, 0x22, 0x64, 0x4e, 0x90, 0x71, 0x52, 0x27, 0x23, 0x7b, 0xe7, 0x0b, 0x07, 0xa9, 0x7e, 0x15,
    0xb1, 0xfe, 0x0e, 0x0e, 0x1b, 0x8e, 0xc3, 0xf0, 0x26, 0x66, 0xfb, 0xdf, 0x78, 0xf8, 0x03, 0xd5,
    0xf5, 0x90, 0x08, 0x04, 0x7c, 0x9f, 0x11, 0xa5, 0x5e, 0x5b, 0x2b, 0x01, 0x00, 0x62, 0x1c, 0xaa,
    0x4a, 0xae, 0xf8, 0x1f, 0x4b, 0x59, 0x70, 0xee, 0xcb, 0x0c, 0x91, 0x35, 0xc9, 0x4a, 0xe2, 0x85,
    0xf4, 0xfc, 0x21, 0x18, 0x3e, 0xa6, 0xed, 0xa6, 0x71, 0xdb, 0xfe, 0x2b, 0x95, 0x67, 0x45, 0xb7,
};

TEST_CASE("ds decrypt test pkcs1_v15 PSA", "[ds_rsa]")
{
    psa_key_id_t keyt_id;
    psa_status_t status;
    psa_algorithm_t alg = PSA_ALG_RSA_PKCS1V15_CRYPT;

    esp_rsa_ds_opaque_key_t rsa_ds_opaque_key = {0};
    rsa_ds_opaque_key.ds_data_ctx = esp_secure_cert_get_ds_ctx();
    TEST_ASSERT_NOT_NULL(rsa_ds_opaque_key.ds_data_ctx);

    printf("DS Key RSA Length Bits: %d\n", rsa_ds_opaque_key.ds_data_ctx->rsa_length_bits);

    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&attributes, PSA_KEY_TYPE_RSA_KEY_PAIR);
    psa_set_key_bits(&attributes, rsa_ds_opaque_key.ds_data_ctx->rsa_length_bits);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_ESP_RSA_DS_VOLATILE);
    status = psa_import_key(&attributes,
                            (const uint8_t *)&rsa_ds_opaque_key,
                            sizeof(rsa_ds_opaque_key),
                            &keyt_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    unsigned char decrypted[256] = {0};
    size_t decrypted_len = 0;

    status = psa_asymmetric_decrypt(keyt_id, alg, v15_padded, sizeof(v15_padded),
                                   NULL, 0,
                                   decrypted, sizeof(decrypted),
                                   &decrypted_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    TEST_ASSERT_EQUAL(message_len, decrypted_len);
    TEST_ASSERT_EQUAL_MEMORY(decrypted, message, message_len);

    status = psa_destroy_key(keyt_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    psa_reset_key_attributes(&attributes);
    // Free the DS context to prevent memory leak
    esp_secure_cert_free_ds_ctx(rsa_ds_opaque_key.ds_data_ctx);
}

int __wrap_esp_ds_start_sign(const void *message, const esp_ds_data_t *data, hmac_key_id_t key_id, esp_ds_context_t **esp_ds_ctx)
{
    // This function will be called instead of the original esp_ds_start_sign
    if (message == NULL || data == NULL || esp_ds_ctx == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *esp_ds_ctx = malloc(sizeof(esp_ds_context_t));
    if (*esp_ds_ctx == NULL) {
        return ESP_ERR_NO_MEM;
    }

    // Simulate successful start sign
    return ESP_OK;
}

// Test implementation using linker wrap
int __wrap_esp_ds_finish_sign(void *sig, esp_ds_context_t *ctx)
{
    // This function will be called instead of the original esp_ds_finish_sign
    free(ctx);
    return 0;

    // Or we can call the real implementation if needed:
    // return __real_esp_ds_finish_sign(sig, ctx);
}

#endif /* CONFIG_MBEDTLS_HARDWARE_RSA_DS_PERIPHERAL */

// Forward declaration for linker-wrapped function
extern esp_efuse_purpose_t __real_esp_efuse_get_key_purpose(esp_efuse_block_t block);

esp_efuse_purpose_t __wrap_esp_efuse_get_key_purpose(esp_efuse_block_t block)
{
#if CONFIG_MBEDTLS_HARDWARE_RSA_DS_PERIPHERAL
    if (block == EFUSE_BLK_KEY0 + 0) {
        printf("Mocked esp_efuse_get_key_purpose called for block %d\n", block);
        return ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_DIGITAL_SIGNATURE;
    } else
#endif /* CONFIG_MBEDTLS_HARDWARE_RSA_DS_PERIPHERAL */
    {
        printf("Mocked esp_efuse_get_key_purpose forwarding to real for block %d\n", block);
        return __real_esp_efuse_get_key_purpose(block);
    }
}
