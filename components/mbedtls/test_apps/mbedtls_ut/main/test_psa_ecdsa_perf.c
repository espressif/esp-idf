/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/*
 * ECDSA performance: the ECDSA peripheral with a software key, against the
 * builtin software implementation.
 *
 * PSA selects the driver at build time, so both drivers are called here through
 * their transparent driver entry points with the same key buffer, hash and
 * algorithm. The builtin path still uses the ECC and MPI accelerators when
 * they are enabled in the configuration.
 */
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include "esp_timer.h"
#include "esp_log.h"
#include "unity.h"
#include "psa/crypto.h"
#include "psa_crypto_ecp.h"
#include "psa_crypto_driver_esp_ecdsa.h"
#include "mbedtls/platform_util.h"
#include "esp_efuse.h"
#include "sdkconfig.h"

#if defined(ESP_ECDSA_TRANSPARENT_SIGN_DRIVER_ENABLED)
#include "hal/ecdsa_ll.h"

#define PERF_ITERATIONS         8
#define PERF_MAX_COMPONENT_LEN  48

static const char *TAG = "ecdsa_perf";

/* The transparent driver entry points of both drivers share these signatures */
typedef psa_status_t (*sign_fn_t)(const psa_key_attributes_t *attributes,
                                  const uint8_t *key_buffer, size_t key_buffer_size,
                                  psa_algorithm_t alg, const uint8_t *hash, size_t hash_length,
                                  uint8_t *signature, size_t signature_size, size_t *signature_length);
typedef psa_status_t (*verify_fn_t)(const psa_key_attributes_t *attributes,
                                    const uint8_t *key_buffer, size_t key_buffer_size,
                                    psa_algorithm_t alg, const uint8_t *hash, size_t hash_length,
                                    const uint8_t *signature, size_t signature_length);

typedef struct {
    psa_key_attributes_t priv_attr;
    psa_key_attributes_t pub_attr;
    psa_algorithm_t alg;
    size_t len;                                     /* Curve component length in bytes */
    uint8_t priv[PERF_MAX_COMPONENT_LEN];           /* Raw private key, big-endian */
    uint8_t pub[2 * PERF_MAX_COMPONENT_LEN + 1];    /* 0x04 || X || Y */
    uint8_t hash[PERF_MAX_COMPONENT_LEN];
} perf_ctx_t;

/* Generate a fresh key pair and a random hash, and export the raw key buffers */
static void perf_ctx_init(perf_ctx_t *ctx, size_t bits, bool deterministic)
{
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_crypto_init());

    psa_algorithm_t hash_alg = (bits == 384) ? PSA_ALG_SHA_384 : PSA_ALG_SHA_256;
    ctx->alg = deterministic ? PSA_ALG_DETERMINISTIC_ECDSA(hash_alg) : PSA_ALG_ECDSA(hash_alg);
    ctx->len = PSA_BITS_TO_BYTES(bits);

    psa_key_attributes_t gen_attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&gen_attr, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&gen_attr, bits);
    psa_set_key_usage_flags(&gen_attr, PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_EXPORT);
    psa_set_key_algorithm(&gen_attr, ctx->alg);

    psa_key_id_t key_id = 0;
    size_t out_len = 0;
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_generate_key(&gen_attr, &key_id));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_export_key(key_id, ctx->priv, sizeof(ctx->priv), &out_len));
    TEST_ASSERT_EQUAL(ctx->len, out_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_export_public_key(key_id, ctx->pub, sizeof(ctx->pub), &out_len));
    TEST_ASSERT_EQUAL(2 * ctx->len + 1, out_len);
    psa_destroy_key(key_id);

    ctx->priv_attr = gen_attr;
    ctx->pub_attr = gen_attr;
    psa_set_key_type(&ctx->pub_attr, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_usage_flags(&ctx->pub_attr, PSA_KEY_USAGE_VERIFY_HASH);

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_generate_random(ctx->hash, ctx->len));
}

/* Sign PERF_ITERATIONS times and return the average time in microseconds */
static int64_t time_sign(const perf_ctx_t *ctx, sign_fn_t sign, uint8_t *signature)
{
    size_t signature_length = 0;
    int64_t start = esp_timer_get_time();

    for (int i = 0; i < PERF_ITERATIONS; i++) {
        TEST_ASSERT_EQUAL(PSA_SUCCESS, sign(&ctx->priv_attr, ctx->priv, ctx->len, ctx->alg,
                                            ctx->hash, ctx->len, signature,
                                            2 * PERF_MAX_COMPONENT_LEN, &signature_length));
    }

    int64_t elapsed = (esp_timer_get_time() - start) / PERF_ITERATIONS;
    TEST_ASSERT_EQUAL(2 * ctx->len, signature_length);
    return elapsed;
}

/* Verify PERF_ITERATIONS times and return the average time in microseconds */
static int64_t time_verify(const perf_ctx_t *ctx, verify_fn_t verify, const uint8_t *signature)
{
    int64_t start = esp_timer_get_time();

    for (int i = 0; i < PERF_ITERATIONS; i++) {
        TEST_ASSERT_EQUAL(PSA_SUCCESS, verify(&ctx->pub_attr, ctx->pub, 2 * ctx->len + 1, ctx->alg,
                                              ctx->hash, ctx->len, signature, 2 * ctx->len));
    }

    return (esp_timer_get_time() - start) / PERF_ITERATIONS;
}

static void log_result(const char *op, size_t bits, int64_t hw_us, int64_t sw_us)
{
    /* The speedup divides by the peripheral time */
    TEST_ASSERT_TRUE(hw_us > 0);
    ESP_LOGI(TAG, "P-%u %s: peripheral %" PRId32 " us, software %" PRId32 " us, speedup x%" PRId32 ".%02" PRId32,
             (unsigned)bits, op, (int32_t)hw_us, (int32_t)sw_us,
             (int32_t)(sw_us / hw_us), (int32_t)((sw_us * 100 / hw_us) % 100));
}

static void run_ecdsa_perf(size_t bits, bool deterministic)
{
    perf_ctx_t ctx = { 0 };
    uint8_t hw_sig[2 * PERF_MAX_COMPONENT_LEN] = { 0 };
    uint8_t sw_sig[2 * PERF_MAX_COMPONENT_LEN] = { 0 };

    perf_ctx_init(&ctx, bits, deterministic);

    int64_t hw_us = time_sign(&ctx, esp_ecdsa_transparent_sign_hash, hw_sig);
    int64_t sw_us = time_sign(&ctx, mbedtls_psa_ecdsa_sign_hash, sw_sig);
    log_result(deterministic ? "deterministic sign" : "sign", bits, hw_us, sw_us);

    /* Deterministic signatures are not compared byte for byte: on P-384 the
     * peripheral derives the nonce with a variant of RFC 6979.
     * Each implementation must accept the signature of the other one. */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, mbedtls_psa_ecdsa_verify_hash(&ctx.pub_attr, ctx.pub, 2 * ctx.len + 1,
                                                                 ctx.alg, ctx.hash, ctx.len,
                                                                 hw_sig, 2 * ctx.len));
#if defined(ESP_ECDSA_VERIFY_DRIVER_ENABLED)
    if (!deterministic) {
        hw_us = time_verify(&ctx, esp_ecdsa_transparent_verify_hash, sw_sig);
        sw_us = time_verify(&ctx, mbedtls_psa_ecdsa_verify_hash, hw_sig);
        log_result("verify", bits, hw_us, sw_us);
    }
#endif /* ESP_ECDSA_VERIFY_DRIVER_ENABLED */

    mbedtls_platform_zeroize(&ctx, sizeof(ctx));
}

static void run_ecdsa_perf_on_curve(size_t bits)
{
    if (!ecdsa_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA is not supported");
    }
    if (!esp_efuse_is_ecdsa_software_key_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA software key is disabled by eFuse");
    }

    run_ecdsa_perf(bits, false);
#if SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
    run_ecdsa_perf(bits, true);
#endif /* SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE */
}

TEST_CASE("mbedtls ECDSA peripheral and software performance on SECP256R1", "[mbedtls][ecdsa_sw_key][timeout=60]")
{
    run_ecdsa_perf_on_curve(256);
}

#if SOC_ECDSA_SUPPORT_CURVE_P384
TEST_CASE("mbedtls ECDSA peripheral and software performance on SECP384R1", "[mbedtls][ecdsa_sw_key][timeout=60]")
{
    run_ecdsa_perf_on_curve(384);
}
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */

#endif /* ESP_ECDSA_TRANSPARENT_SIGN_DRIVER_ENABLED */
