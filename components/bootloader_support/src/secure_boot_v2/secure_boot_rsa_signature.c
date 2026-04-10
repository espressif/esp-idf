/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_log.h"
#include "esp_secure_boot.h"
#include "psa/crypto.h"
#include "mbedtls/asn1.h"
#include "mbedtls/asn1write.h"
#include "mbedtls/x509.h"

#include "secure_boot_signature_priv.h"

ESP_LOG_ATTR_TAG(TAG, "secure_boot_v2_rsa");

/*
 * Helper function to encode RSA public key (N, e) into DER format manually
 * This creates a PKCS#1 RSAPublicKey structure:
 *
 * RSAPublicKey ::= SEQUENCE {
 *     modulus           INTEGER,  -- n
 *     publicExponent    INTEGER   -- e
 * }
 */
static int encode_rsa_pubkey_der(const uint8_t *modulus, size_t modulus_len,
                                  const uint8_t *exponent, size_t exponent_len,
                                  uint8_t *der_buf, size_t der_buf_size,
                                  uint8_t **der_start, size_t *der_len)
{
    if (!der_buf || !der_start || !der_len || der_buf_size == 0) {
        return MBEDTLS_ERR_X509_BAD_INPUT_DATA;
    }

    int ret;
    unsigned char *c = der_buf + der_buf_size;
    size_t len = 0;

    /* Write the exponent (e) as an INTEGER */
    /* Skip leading zeros in exponent */
    while (exponent_len > 0 && *exponent == 0) {
        exponent++;
        exponent_len--;
    }

    /* Write exponent */
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_raw_buffer(&c, der_buf, exponent, exponent_len));

    /* Add padding byte if MSB is set (to keep it positive) */
    if (exponent_len > 0 && (exponent[0] & 0x80)) {
        MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_raw_buffer(&c, der_buf, (const unsigned char *)"\x00", 1));
    }

    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_len(&c, der_buf, exponent_len + ((exponent[0] & 0x80) ? 1 : 0)));
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_tag(&c, der_buf, MBEDTLS_ASN1_INTEGER));

    /* Write the modulus (N) as an INTEGER */
    /* Skip leading zeros in modulus */
    const uint8_t *mod_ptr = modulus;
    size_t mod_len = modulus_len;
    while (mod_len > 0 && *mod_ptr == 0) {
        mod_ptr++;
        mod_len--;
    }

    /* Write modulus */
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_raw_buffer(&c, der_buf, mod_ptr, mod_len));

    /* Add padding byte if MSB is set */
    if (mod_len > 0 && (mod_ptr[0] & 0x80)) {
        MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_raw_buffer(&c, der_buf, (const unsigned char *)"\x00", 1));
    }

    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_len(&c, der_buf, mod_len + ((mod_ptr[0] & 0x80) ? 1 : 0)));
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_tag(&c, der_buf, MBEDTLS_ASN1_INTEGER));

    /* Write SEQUENCE header */
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_len(&c, der_buf, len));
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_tag(&c, der_buf,
                                                       MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE));

    *der_start = c;
    *der_len = len;

    return 0;
}

esp_err_t verify_rsa_signature_block(const ets_secure_boot_signature_t *sig_block, const uint8_t *image_digest, const ets_secure_boot_sig_block_t *trusted_block)
{
    if (!sig_block || !image_digest || !trusted_block) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;
    psa_status_t status;
    const unsigned rsa_key_size = sizeof(sig_block->block[0].signature);
    unsigned char *sig_be = NULL;
    unsigned char *pubkey_der_buf = NULL;

    sig_be = calloc(1, rsa_key_size);
    if (sig_be == NULL) {
        return ESP_ERR_NO_MEM;
    }

    /* Create key attributes for RSA public key */
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;

    /* Allocate buffer for DER-encoded public key */
    size_t pubkey_der_buf_size = PSA_KEY_EXPORT_RSA_PUBLIC_KEY_MAX_SIZE(3072);
    pubkey_der_buf = calloc(1, pubkey_der_buf_size);
    if (pubkey_der_buf == NULL) {
        free(sig_be);
        return ESP_ERR_NO_MEM;
    }

    /* Convert raw N and e to DER format manually */
    uint8_t *der_start = NULL;
    size_t der_len = 0;

    /* Convert modulus from little-endian to big-endian */
    uint8_t *n_be = calloc(1, rsa_key_size);
    if (n_be == NULL) {
        free(sig_be);
        free(pubkey_der_buf);
        return ESP_ERR_NO_MEM;
    }
    for (size_t i = 0; i < rsa_key_size; i++) {
        n_be[i] = trusted_block->key.n[rsa_key_size - 1 - i];
    }

    /* Convert e from uint32_t to byte array (big-endian) */
    uint8_t e_bytes[4];
    e_bytes[0] = (trusted_block->key.e >> 24) & 0xFF;
    e_bytes[1] = (trusted_block->key.e >> 16) & 0xFF;
    e_bytes[2] = (trusted_block->key.e >> 8) & 0xFF;
    e_bytes[3] = trusted_block->key.e & 0xFF;

    ret = encode_rsa_pubkey_der(
        n_be, rsa_key_size,
        e_bytes, sizeof(e_bytes),
        pubkey_der_buf, pubkey_der_buf_size,
        &der_start, &der_len
    );

    free(n_be);

    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to encode RSA public key to DER, err: %d", ret);
        goto cleanup;
    }

    /* Set key attributes */
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_RSA_PSS(PSA_ALG_SHA_256));
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_RSA_PUBLIC_KEY);

    /* Import DER-encoded public key into PSA */
    status = psa_import_key(&key_attributes, der_start, der_len, &key_id);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to import key into PSA, err: %d", status);
        ret = ESP_FAIL;
        goto cleanup;
    }

    /* Signature needs to be byte swapped into BE representation */
    for (int j = 0; j < rsa_key_size; j++) {
        sig_be[rsa_key_size - j - 1] = trusted_block->signature[j];
    }

    /* Verify the signature using PSA APIs */
    status = psa_verify_hash(key_id, PSA_ALG_RSA_PSS(PSA_ALG_SHA_256),
                            image_digest, ESP_SECURE_BOOT_DIGEST_LEN,
                            sig_be, rsa_key_size);

    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Signature verification failed, err: %d", status);
        ret = ESP_FAIL;
    } else {
        ESP_LOGI(TAG, "Signature verified successfully!");
        ret = ESP_OK;
    }

cleanup:
    /* Clean up resources */
    if (key_id != 0) {
        psa_destroy_key(key_id);
    }
    psa_reset_key_attributes(&key_attributes);
    free(sig_be);
    free(pubkey_der_buf);

    return ret;
}
