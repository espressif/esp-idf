/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Smoke-test crypto accelerators after TOP power-domain light sleep.
 * DS / hardware HMAC need efuse keys and are not covered here.
 */
#include "sdkconfig.h"
#include "unity.h"
#include "test_utils.h"
#include "soc/soc_caps.h"

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_LIGHT_SLEEP_SUPPORTED && SOC_PM_SUPPORT_TOP_PD

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/esp_pmu.h"

#define MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS
#include "mbedtls/bignum.h"
#include "mbedtls/private/ecp.h"
#include "mbedtls/psa_util.h"
#include "psa/crypto.h"

#if CONFIG_MBEDTLS_HARDWARE_SHA
#include "sha/sha_parallel_engine.h"
#endif
#if CONFIG_MBEDTLS_HARDWARE_ECDSA_VERIFY && SOC_ECDSA_SUPPORTED
#include "hal/ecdsa_ll.h"
#endif

static const char *TAG = "crypto_pd_top";

static void enter_pd_top_lightsleep(void)
{
    esp_sleep_context_t sleep_ctx;
    TEST_ESP_OK(sleep_cpu_configure(true));
    esp_sleep_set_sleep_context(&sleep_ctx);
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(10 * 1000));
    fflush(stdout);
    fsync(fileno(stdout));
    TEST_ESP_OK(esp_light_sleep_start());
    TEST_ASSERT_EQUAL(PMU_SLEEP_PD_TOP, sleep_ctx.sleep_flags & PMU_SLEEP_PD_TOP);
    TEST_ASSERT_EQUAL(0, sleep_ctx.sleep_request_result);
    esp_sleep_set_sleep_context(NULL);
    TEST_ESP_OK(sleep_cpu_configure(false));
}

#if CONFIG_MBEDTLS_HARDWARE_AES
static void verify_aes_ecb(void)
{
    /* NIST AES-128-ECB sample */
    const uint8_t key[16] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
    };
    const uint8_t plaintext[16] = {
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    };
    const uint8_t expected[16] = {
        0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
        0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
    };
    uint8_t ciphertext[16] = {0};
    size_t out_len = 0;

    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_ECB_NO_PADDING);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);

    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_import_key(&attributes, key, sizeof(key), &key_id));
    TEST_ASSERT_EQUAL(PSA_SUCCESS,
                      psa_cipher_encrypt(key_id, PSA_ALG_ECB_NO_PADDING,
                                         plaintext, sizeof(plaintext),
                                         ciphertext, sizeof(ciphertext), &out_len));
    TEST_ASSERT_EQUAL(sizeof(expected), out_len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, ciphertext, sizeof(expected));
    psa_destroy_key(key_id);
    psa_reset_key_attributes(&attributes);
}
#endif

#if CONFIG_MBEDTLS_HARDWARE_SHA
static void verify_sha1(void)
{
    const uint8_t input[] = "abc";
    const uint8_t expected[20] = {
        0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a, 0xba, 0x3e,
        0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c, 0x9c, 0xd0, 0xd8, 0x9d,
    };
    uint8_t digest[20] = {0};

    esp_sha(SHA1, input, sizeof(input) - 1, digest);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, digest, sizeof(expected));
}
#endif

#if CONFIG_MBEDTLS_HARDWARE_MPI
static void verify_mpi_mod_mul(void)
{
    mbedtls_mpi A, B, M, Z;
    mbedtls_mpi_init(&A);
    mbedtls_mpi_init(&B);
    mbedtls_mpi_init(&M);
    mbedtls_mpi_init(&Z);

    TEST_ASSERT_EQUAL(0, mbedtls_mpi_lset(&A, 2));
    TEST_ASSERT_EQUAL(0, mbedtls_mpi_lset(&B, 3));
    /* M = 0xFFFFFFFF (32-bit all-ones) so 2*3 mod M == 6 */
    for (int i = 0; i < 32; i++) {
        TEST_ASSERT_EQUAL(0, mbedtls_mpi_set_bit(&M, i, 1));
    }

    TEST_ASSERT_EQUAL(0, esp_mpi_mul_mpi_mod(&Z, &A, &B, &M));
    TEST_ASSERT_EQUAL(0, mbedtls_mpi_cmp_int(&Z, 6));

    mbedtls_mpi_free(&A);
    mbedtls_mpi_free(&B);
    mbedtls_mpi_free(&M);
    mbedtls_mpi_free(&Z);
}
#endif

#if CONFIG_MBEDTLS_HARDWARE_ECC
static void verify_ecc_mul(void)
{
    /* Same flow as test_ecp.c: 127 * G */
    const uint8_t exp_x[32] = {
        0x53, 0x4D, 0x45, 0xDB, 0x6B, 0xAC, 0xA8, 0xE2,
        0xD2, 0xA5, 0xD0, 0xA7, 0x65, 0xF1, 0x60, 0x13,
        0xA8, 0xD4, 0xEB, 0x58, 0xC6, 0xAA, 0xAD, 0x35,
        0x67, 0xCE, 0xBD, 0xFA, 0xC4, 0x2D, 0x62, 0x3C,
    };
    const uint8_t exp_y[32] = {
        0xFA, 0xD6, 0x69, 0xC8, 0x9A, 0x2A, 0x54, 0xE4,
        0x41, 0x54, 0x35, 0x7F, 0x99, 0x2C, 0xCE, 0xC8,
        0xEE, 0xF0, 0x93, 0xE0, 0xF2, 0x3A, 0x63, 0x1D,
        0x17, 0xFD, 0xF6, 0x64, 0x41, 0x9E, 0x50, 0x0C,
    };
    mbedtls_ecp_group grp;
    mbedtls_ecp_point R;
    mbedtls_mpi m;
    uint8_t x[32] = {0};
    uint8_t y[32] = {0};

    mbedtls_ecp_group_init(&grp);
    mbedtls_ecp_point_init(&R);
    mbedtls_mpi_init(&m);
    TEST_ASSERT_EQUAL(0, mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1));
    TEST_ASSERT_EQUAL(0, mbedtls_mpi_lset(&m, 127));
    TEST_ASSERT_EQUAL(0, mbedtls_ecp_mul(&grp, &R, &m, &grp.G,
                                         mbedtls_psa_get_random, MBEDTLS_PSA_RANDOM_STATE));
    TEST_ASSERT_EQUAL(0, mbedtls_mpi_write_binary(&R.MBEDTLS_PRIVATE(X), x, sizeof(x)));
    TEST_ASSERT_EQUAL(0, mbedtls_mpi_write_binary(&R.MBEDTLS_PRIVATE(Y), y, sizeof(y)));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(exp_x, x, sizeof(exp_x));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(exp_y, y, sizeof(exp_y));
    mbedtls_ecp_point_free(&R);
    mbedtls_mpi_free(&m);
    mbedtls_ecp_group_free(&grp);
}
#endif

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_VERIFY && SOC_ECDSA_SUPPORTED
static void verify_ecdsa(void)
{
    if (!ecdsa_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA is not supported");
    }

    /* Known SECP256R1 verify vector (same as test_psa_ecdsa.c) */
    const uint8_t hash[32] = {
        0x98, 0xca, 0xea, 0x85, 0x7b, 0x03, 0x5e, 0xc0,
        0xe3, 0xc3, 0x39, 0x29, 0xef, 0xf1, 0xf1, 0x25,
        0x00, 0x19, 0xe7, 0x11, 0xc3, 0x3d, 0x84, 0x42,
        0x38, 0x79, 0x10, 0xef, 0xb2, 0x9b, 0xd2, 0x63,
    };
    const uint8_t r[32] = {
        0xbf, 0x58, 0x35, 0xf2, 0xf4, 0x4d, 0x49, 0x0e,
        0xeb, 0x98, 0x66, 0x7e, 0x0c, 0x25, 0xd1, 0x1b,
        0x6a, 0xd0, 0x15, 0xc4, 0x68, 0x8b, 0x9f, 0x83,
        0x2f, 0xa1, 0x9e, 0xf2, 0x2f, 0x33, 0xc2, 0xf7,
    };
    const uint8_t s[32] = {
        0x25, 0x63, 0xb1, 0x5f, 0xaa, 0xe4, 0xcf, 0xc1,
        0xd0, 0xfe, 0x43, 0xdf, 0xbb, 0x5f, 0xe7, 0x29,
        0xdf, 0xb4, 0xf9, 0xfa, 0xe4, 0xb1, 0xc1, 0x7a,
        0x7e, 0x0d, 0x73, 0xe2, 0x4b, 0x91, 0x7c, 0x79,
    };
    const uint8_t pub_x[32] = {
        0xa2, 0x8f, 0x52, 0x60, 0x20, 0x9b, 0x54, 0x3c,
        0x13, 0x2f, 0x51, 0xb1, 0x89, 0xbf, 0xc7, 0xfa,
        0x84, 0x5c, 0x56, 0x96, 0x2a, 0x00, 0x67, 0xdd,
        0x7c, 0x8c, 0x0f, 0x63, 0x8b, 0x76, 0x7f, 0xb9,
    };
    const uint8_t pub_y[32] = {
        0xf6, 0x4c, 0x87, 0x5b, 0x5a, 0x9b, 0x59, 0x0a,
        0xc4, 0x53, 0x04, 0x72, 0x0d, 0x7c, 0xde, 0xac,
        0x7e, 0xad, 0x49, 0x8c, 0xf7, 0x5c, 0xc3, 0x1c,
        0x1e, 0x81, 0xf2, 0x47, 0x01, 0x74, 0x05, 0xd5,
    };
    uint8_t pub[65] = {0x04};
    uint8_t signature[64];
    psa_key_id_t key_id;
    psa_key_attributes_t key_attr = PSA_KEY_ATTRIBUTES_INIT;


    memcpy(pub + 1, pub_x, sizeof(pub_x));
    memcpy(pub + 1 + sizeof(pub_x), pub_y, sizeof(pub_y));
    memcpy(signature, r, sizeof(r));
    memcpy(signature + sizeof(r), s, sizeof(s));

    psa_set_key_type(&key_attr, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_usage_flags(&key_attr, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&key_attr, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    psa_set_key_bits(&key_attr, 256);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_import_key(&key_attr, pub, sizeof(pub), &key_id));
    TEST_ASSERT_EQUAL(PSA_SUCCESS,
                      psa_verify_hash(key_id, PSA_ALG_ECDSA(PSA_ALG_SHA_256),
                                      hash, sizeof(hash), signature, sizeof(signature)));
    psa_destroy_key(key_id);
    psa_reset_key_attributes(&key_attr);
}
#endif

TEST_CASE("Crypto peripherals work after PD_TOP lightsleep", "[hw_crypto][pd_top]")
{
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_crypto_init());

    for (int i = 0; i < 5; ++i) {
#if CONFIG_MBEDTLS_HARDWARE_AES
        verify_aes_ecb();
#endif
#if CONFIG_MBEDTLS_HARDWARE_SHA
        verify_sha1();
#endif
#if CONFIG_MBEDTLS_HARDWARE_MPI
        verify_mpi_mod_mul();
#endif
#if CONFIG_MBEDTLS_HARDWARE_ECC
        verify_ecc_mul();
#endif
#if CONFIG_MBEDTLS_HARDWARE_ECDSA_VERIFY && SOC_ECDSA_SUPPORTED
        verify_ecdsa();
#endif
        enter_pd_top_lightsleep();
        ESP_LOGI(TAG, "loop %d/5: woke from PD_TOP lightsleep", i + 1);
    }
}

#endif /* CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_LIGHT_SLEEP_SUPPORTED && SOC_PM_SUPPORT_TOP_PD */
