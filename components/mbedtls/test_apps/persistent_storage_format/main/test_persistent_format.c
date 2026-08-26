/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Shared scaffolding for persistent-storage format-stability tests.
 *
 * - One capture test that imports a persistent eFuse key for every PSA
 *   opaque driver compiled into the build (RSA-DS, HMAC, ECDSA), with
 *   fixed key ids. Run once on a chip that has all three drivers in the
 *   build; the resulting NVS partition is captured with esptool and
 *   committed as fixtures/nvs_efuse_v1.bin.
 *
 * - The committed fixture is then flashed by every CI runner; each runner
 *   only references its own driver's key id (see the per-driver consume
 *   files), so a single shared NVS image works across heterogeneous
 *   runners.
 */

#include <string.h>
#include "unity.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "esp_efuse.h"
#include "psa/crypto.h"

#ifdef CONFIG_MBEDTLS_HARDWARE_RSA_DS_PERIPHERAL
#include "esp_ds.h"
#include "hal/hmac_types.h"
#include "psa_crypto_driver_esp_rsa_ds.h"
#endif

#ifdef ESP_HMAC_OPAQUE_DRIVER_ENABLED
#include "psa_crypto_driver_esp_hmac_opaque.h"
#include "psa_crypto_driver_esp_hmac_opaque_contexts.h"
#endif

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
#include "psa_crypto_driver_esp_ecdsa.h"
#include "psa_crypto_driver_esp_ecdsa_contexts.h"
#endif

#include "test_persistent_format.h"

/* --- Dynamic-purpose wrap shared across all drivers' import validation. */
static volatile esp_efuse_purpose_t s_purpose_override = ESP_EFUSE_KEY_PURPOSE_USER;

extern esp_efuse_purpose_t __real_esp_efuse_get_key_purpose(esp_efuse_block_t block);

esp_efuse_purpose_t __wrap_esp_efuse_get_key_purpose(esp_efuse_block_t block)
{
    if (s_purpose_override != ESP_EFUSE_KEY_PURPOSE_USER) {
        return s_purpose_override;
    }
    return __real_esp_efuse_get_key_purpose(block);
}

/* --- DS hardware wraps. Without these the DS sign path on the consume
 *     side would touch the DS peripheral and fail for lack of a real
 *     eFuse HMAC key. Compiled only when DS is in the build. */
#ifdef CONFIG_MBEDTLS_HARDWARE_RSA_DS_PERIPHERAL
int __wrap_esp_ds_start_sign(const void *message, const esp_ds_data_t *data,
                             hmac_key_id_t key_id, esp_ds_context_t **esp_ds_ctx)
{
    if (message == NULL || data == NULL || esp_ds_ctx == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    *esp_ds_ctx = malloc(sizeof(esp_ds_context_t));
    if (*esp_ds_ctx == NULL) {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

int __wrap_esp_ds_finish_sign(void *sig, esp_ds_context_t *ctx)
{
    free(ctx);
    return 0;
}

/* RSA-DS storage embeds the encrypted key blob; the capture test below
 * needs a syntactically-valid mock since the driver cross-validates
 * rsa_length_bits against ds_data.rsa_length. */
static esp_ds_data_ctx_t *mock_ds_data_ctx(void)
{
    esp_ds_data_ctx_t *ds = calloc(1, sizeof(esp_ds_data_ctx_t));
    if (!ds) {
        return NULL;
    }
    ds->esp_ds_data = calloc(1, sizeof(esp_ds_data_t));
    if (!ds->esp_ds_data) {
        free(ds);
        return NULL;
    }
    ds->rsa_length_bits = 2048;
    ds->efuse_key_id = ESP_PERSISTENT_FIXTURE_DS_EFUSE_KEY_ID;
    ds->esp_ds_data->rsa_length = (ds->rsa_length_bits / 32) - 1;
    return ds;
}

static void free_mock_ds_data_ctx(esp_ds_data_ctx_t *ds)
{
    if (ds) {
        free(ds->esp_ds_data);
        free(ds);
    }
}
#endif /* CONFIG_MBEDTLS_HARDWARE_RSA_DS_PERIPHERAL */

/* --- Capture test. Imports one persistent eFuse key per compiled driver,
 *     using fixed ids. Run once; capture NVS via esptool; commit. */
TEST_CASE("efuse persistent fixture capture v1 (one-shot, all drivers)",
          "[fixture_capture]")
{
    /* Clean any leftover from prior runs so we capture a deterministic NVS. */
    psa_destroy_key(ESP_PERSISTENT_FIXTURE_DS_KEY_ID);
    psa_destroy_key(ESP_PERSISTENT_FIXTURE_HMAC_KEY_ID);
    psa_destroy_key(ESP_PERSISTENT_FIXTURE_ECDSA_KEY_ID);

#ifdef CONFIG_MBEDTLS_HARDWARE_RSA_DS_PERIPHERAL
    {
        esp_rsa_ds_opaque_key_t key = {0};
        key.ds_data_ctx = mock_ds_data_ctx();
        TEST_ASSERT_NOT_NULL(key.ds_data_ctx);

        psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
        psa_set_key_type(&attr, PSA_KEY_TYPE_RSA_KEY_PAIR);
        psa_set_key_bits(&attr, key.ds_data_ctx->rsa_length_bits);
        psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_SIGN_HASH);
        psa_set_key_algorithm(&attr, PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256));
        psa_set_key_lifetime(&attr, PSA_KEY_LIFETIME_ESP_RSA_DS);
        psa_set_key_id(&attr, ESP_PERSISTENT_FIXTURE_DS_KEY_ID);

        s_purpose_override = ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_DIGITAL_SIGNATURE;
        psa_key_id_t kid;
        TEST_ASSERT_EQUAL(PSA_SUCCESS,
            psa_import_key(&attr, (const uint8_t *)&key, sizeof(key), &kid));
        s_purpose_override = ESP_EFUSE_KEY_PURPOSE_USER;
        TEST_ASSERT_EQUAL(ESP_PERSISTENT_FIXTURE_DS_KEY_ID, kid);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_purge_key(kid));

        free_mock_ds_data_ctx(key.ds_data_ctx);
        psa_reset_key_attributes(&attr);
    }
#endif

#ifdef ESP_HMAC_OPAQUE_DRIVER_ENABLED
    {
        esp_hmac_opaque_key_t key = {
            .efuse_key_id = ESP_PERSISTENT_FIXTURE_HMAC_EFUSE_KEY_ID,
        };

        psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
        psa_set_key_type(&attr, PSA_KEY_TYPE_HMAC);
        psa_set_key_bits(&attr, 256);
        psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE);
        psa_set_key_algorithm(&attr, PSA_ALG_HMAC(PSA_ALG_SHA_256));
        psa_set_key_lifetime(&attr, PSA_KEY_LIFETIME_ESP_HMAC);
        psa_set_key_id(&attr, ESP_PERSISTENT_FIXTURE_HMAC_KEY_ID);

        s_purpose_override = ESP_EFUSE_KEY_PURPOSE_HMAC_UP;
        psa_key_id_t kid;
        TEST_ASSERT_EQUAL(PSA_SUCCESS,
            psa_import_key(&attr, (const uint8_t *)&key, sizeof(key), &kid));
        s_purpose_override = ESP_EFUSE_KEY_PURPOSE_USER;
        TEST_ASSERT_EQUAL(ESP_PERSISTENT_FIXTURE_HMAC_KEY_ID, kid);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_purge_key(kid));

        psa_reset_key_attributes(&attr);
    }
#endif

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
    {
        esp_ecdsa_opaque_key_t key = {
            .curve = ESP_ECDSA_CURVE_SECP256R1,
            .efuse_block = ESP_PERSISTENT_FIXTURE_ECDSA_EFUSE_BLOCK,
        };

        psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
        psa_set_key_type(&attr, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
        psa_set_key_bits(&attr, 256);
        psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_SIGN_HASH);
        psa_set_key_algorithm(&attr, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
        psa_set_key_lifetime(&attr, PSA_KEY_LIFETIME_ESP_ECDSA);
        psa_set_key_id(&attr, ESP_PERSISTENT_FIXTURE_ECDSA_KEY_ID);

        s_purpose_override = ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY;
        psa_key_id_t kid;
        TEST_ASSERT_EQUAL(PSA_SUCCESS,
            psa_import_key(&attr, (const uint8_t *)&key, sizeof(key), &kid));
        s_purpose_override = ESP_EFUSE_KEY_PURPOSE_USER;
        TEST_ASSERT_EQUAL(ESP_PERSISTENT_FIXTURE_ECDSA_KEY_ID, kid);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_purge_key(kid));

        psa_reset_key_attributes(&attr);
    }
#endif

    printf("\n*** Fixture written; capture NVS partition with esptool now. ***\n\n");
}

/* ====================================================================== *
 * Per-driver consume tests.
 *
 * The CI runner flashes fixtures/nvs_efuse_v1.bin to the NVS partition.
 * The persistent keys are already there with their fixed ids; each
 * driver's consume test only references its own id and ignores the rest,
 * so a single shared NVS image works across heterogeneous runners.
 *
 * If anything in the on-NVS format has drifted (storage struct, PSA
 * framing, NVS encoding) the corresponding op call returns
 * INVALID_ARGUMENT/DATA_INVALID and the test fails. Each driver gates
 * its test on the same CONFIG flag that pulls its driver into the build.
 * ====================================================================== */

#if CONFIG_MBEDTLS_HARDWARE_RSA_DS_PERIPHERAL
TEST_CASE("rsa-ds persistent NVS fixture v1 sign",
          "[persistent_format][rsa_ds]")
{
    uint8_t hash[32] = {0};
    size_t hash_length = 0;
    uint8_t input[7] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    TEST_ASSERT_EQUAL(PSA_SUCCESS,
        psa_hash_compute(PSA_ALG_SHA_256, input, sizeof(input),
                         hash, sizeof(hash), &hash_length));

    uint8_t signature[256] = {0};
    size_t signature_length = 0;
    TEST_ASSERT_EQUAL_HEX32(PSA_SUCCESS,
        psa_sign_hash(ESP_PERSISTENT_FIXTURE_DS_KEY_ID,
                      PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256),
                      hash, hash_length,
                      signature, sizeof(signature), &signature_length));
    TEST_ASSERT_EQUAL(256, signature_length);

    /* v15 padding-shape sanity, same as the volatile sign test. */
    TEST_ASSERT_EQUAL(0, memcmp(hash, signature + (256 - hash_length), hash_length));
    TEST_ASSERT_EQUAL(hash_length, signature[256 - hash_length - 1]);
    TEST_ASSERT_EQUAL(0x04, signature[256 - hash_length - 2]);
    TEST_ASSERT_EQUAL(0x00, signature[0]);

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_purge_key(ESP_PERSISTENT_FIXTURE_DS_KEY_ID));
}
#endif /* CONFIG_MBEDTLS_HARDWARE_RSA_DS_PERIPHERAL */

#ifdef ESP_HMAC_OPAQUE_DRIVER_ENABLED
/* The runner has an eFuse HMAC key burned in
 * ESP_PERSISTENT_FIXTURE_HMAC_EFUSE_KEY_ID with HMAC_UP purpose; we do a
 * real mac_compute + mac_verify roundtrip. */
static const uint8_t hmac_test_data[] = "Pretty long input message";

TEST_CASE("hmac efuse persistent NVS fixture v1 mac",
          "[persistent_format][hmac_efuse_key]")
{
    uint8_t mac[32] = {0};
    size_t mac_length = 0;
    TEST_ASSERT_EQUAL_HEX32(PSA_SUCCESS,
        psa_mac_compute(ESP_PERSISTENT_FIXTURE_HMAC_KEY_ID,
                        PSA_ALG_HMAC(PSA_ALG_SHA_256),
                        hmac_test_data, sizeof(hmac_test_data) - 1,
                        mac, sizeof(mac), &mac_length));
    TEST_ASSERT_EQUAL(32, mac_length);

    TEST_ASSERT_EQUAL(PSA_SUCCESS,
        psa_mac_verify(ESP_PERSISTENT_FIXTURE_HMAC_KEY_ID,
                       PSA_ALG_HMAC(PSA_ALG_SHA_256),
                       hmac_test_data, sizeof(hmac_test_data) - 1,
                       mac, mac_length));

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_purge_key(ESP_PERSISTENT_FIXTURE_HMAC_KEY_ID));
}
#endif /* ESP_HMAC_OPAQUE_DRIVER_ENABLED */

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
/* SECP256R1 public key matching the SECP256R1 ECDSA key burned in
 * ESP_PERSISTENT_FIXTURE_ECDSA_EFUSE_BLOCK on the runner. Same constants
 * as mbedtls_ut/test_psa_ecdsa.c uses for its eFuse-key volatile tests. */
static const uint8_t ecdsa256_pub_x[] = {
    0xa2, 0x8f, 0x52, 0x60, 0x20, 0x9b, 0x54, 0x3c,
    0x13, 0x2f, 0x51, 0xb1, 0x89, 0xbf, 0xc7, 0xfa,
    0x84, 0x5c, 0x56, 0x96, 0x2a, 0x00, 0x67, 0xdd,
    0x7c, 0x8c, 0x0f, 0x63, 0x8b, 0x76, 0x7f, 0xb9,
};
static const uint8_t ecdsa256_pub_y[] = {
    0xf6, 0x4c, 0x87, 0x5b, 0x5a, 0x9b, 0x59, 0x0a,
    0xc4, 0x53, 0x04, 0x72, 0x0d, 0x7c, 0xde, 0xac,
    0x7e, 0xad, 0x49, 0x8c, 0xf7, 0x5c, 0xc3, 0x1c,
    0x1e, 0x81, 0xf2, 0x47, 0x01, 0x74, 0x05, 0xd5,
};

/* The runner has a SECP256R1 ECDSA key burned in
 * ESP_PERSISTENT_FIXTURE_ECDSA_EFUSE_BLOCK with ECDSA_KEY purpose. We
 * sign with the persistent key, then verify the resulting signature
 * with a freshly-imported transparent public key matching the burned
 * eFuse private key. The verify pass is the mathematical "compare the
 * signature" — it proves the persistent extract recovered the SAME
 * private key as the runner has burned. We don't rely on HW pubkey
 * export here, since some ECDSA-capable chips (e.g. esp32h2) lack it
 * for opaque keys. */
TEST_CASE("ecdsa efuse persistent NVS fixture v1 sign and verify",
          "[persistent_format][ecdsa_efuse_key]")
{
    psa_algorithm_t alg = PSA_ALG_ECDSA(PSA_ALG_SHA_256);
    uint8_t hash[32];
    memset(hash, 0xA5, sizeof(hash));

    /* Sign on the persistent eFuse key. */
    uint8_t signature[64];                  /* SECP256R1: r || s, 32 bytes each */
    size_t signature_length = 0;
    TEST_ASSERT_EQUAL_HEX32(PSA_SUCCESS,
        psa_sign_hash(ESP_PERSISTENT_FIXTURE_ECDSA_KEY_ID,
                      alg, hash, sizeof(hash),
                      signature, sizeof(signature), &signature_length));
    TEST_ASSERT_EQUAL(64, signature_length);

    /* Import the matching transparent public key and verify. */
    uint8_t pub[65];
    pub[0] = 0x04;                          /* uncompressed point format */
    memcpy(pub + 1, ecdsa256_pub_x, sizeof(ecdsa256_pub_x));
    memcpy(pub + 1 + sizeof(ecdsa256_pub_x), ecdsa256_pub_y, sizeof(ecdsa256_pub_y));

    psa_key_attributes_t pub_attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&pub_attr, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_usage_flags(&pub_attr, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&pub_attr, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    psa_set_key_bits(&pub_attr, 256);

    psa_key_id_t pub_kid = 0;
    TEST_ASSERT_EQUAL(PSA_SUCCESS,
        psa_import_key(&pub_attr, pub, sizeof(pub), &pub_kid));

    TEST_ASSERT_EQUAL_HEX32(PSA_SUCCESS,
        psa_verify_hash(pub_kid, PSA_ALG_ECDSA(PSA_ALG_SHA_256),
                        hash, sizeof(hash), signature, signature_length));

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_destroy_key(pub_kid));
    psa_reset_key_attributes(&pub_attr);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_purge_key(ESP_PERSISTENT_FIXTURE_ECDSA_KEY_ID));
}
#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN */
