/* mbedTLS Elliptic Curve Digital Signature performance tests
 *
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <esp_log.h>

#include "psa/crypto.h"
#include "psa_crypto_driver_esp_ecdsa_contexts.h"
#include "psa_crypto_driver_esp_ecdsa.h"

#include "hal/efuse_ll.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "soc/soc_caps.h"
#include "test_utils.h"
#include "ccomp_timer.h"
#include "unity.h"
#include "esp_heap_caps.h"
#include "crypto_performance.h"

#if SOC_KEY_MANAGER_SUPPORTED
#include "esp_key_mgr.h"
#include "hal/key_mgr_ll.h"
#endif

#if SOC_ECDSA_SUPPORTED
#include "hal/ecdsa_ll.h"
#endif

#define TEST_ASSERT_MBEDTLS_OK(X) TEST_ASSERT_EQUAL_HEX32(0, -(X))

#if CONFIG_LIBC_NEWLIB_NANO_FORMAT
#define NEWLIB_NANO_COMPAT_FORMAT             PRIu32
#define NEWLIB_NANO_COMPAT_CAST(int64_t_var)  (uint32_t)int64_t_var
#else
#define NEWLIB_NANO_COMPAT_FORMAT             PRId64
#define NEWLIB_NANO_COMPAT_CAST(int64_t_var)  int64_t_var
#endif

#if SOC_ECDSA_SUPPORT_CURVE_P384
#define MAX_ECDSA_COMPONENT_LEN           48
#define MAX_HASH_LEN                      48
#else /* !SOC_ECDSA_SUPPORT_CURVE_P384 */
#define MAX_ECDSA_COMPONENT_LEN           32
#define MAX_HASH_LEN                      32
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */

#define HASH_LEN                         32
#if SOC_ECDSA_SUPPORT_CURVE_P384
#define HASH_LEN_P384                    48
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */

#define ECDSA_P192_HASH_COMPONENT_LEN    24
#define ECDSA_P256_HASH_COMPONENT_LEN    32

#define ECDSA_UNCOMPRESSED_POINT_FORMAT 0x04

__attribute__((unused)) static const char * TAG = "mbedtls_test";

/*
 * All the following values are in big endian format, as required by the mbedTLS APIs
 */

const uint8_t sha[] = {
    0x98, 0xca, 0xea, 0x85, 0x7b, 0x03, 0x5e, 0xc0,
    0xe3, 0xc3, 0x39, 0x29, 0xef, 0xf1, 0xf1, 0x25,
    0x00, 0x19, 0xe7, 0x11, 0xc3, 0x3d, 0x84, 0x42,
    0x38, 0x79, 0x10, 0xef, 0xb2, 0x9b, 0xd2, 0x63,
    0xed, 0xfe, 0x04, 0xce, 0x66, 0x89, 0xd0, 0xa4,
    0xb2, 0x60, 0xb2, 0x38, 0x93, 0xa6, 0x27, 0x14
};

#if CONFIG_MBEDTLS_HARDWARE_ECC || CONFIG_MBEDTLS_HARDWARE_ECDSA_VERIFY

/* Big endian */
uint8_t ecdsa384_r[] = {
    0x1a, 0x24, 0xe3, 0xaf, 0x08, 0x08, 0xb6, 0x81,
    0xe6, 0x18, 0xc6, 0xf8, 0x14, 0x75, 0x7f, 0x04,
    0x6b, 0x2d, 0x2e, 0xd7, 0xa9, 0xf0, 0x6f, 0xbe,
    0x9d, 0x3a, 0x20, 0x4a, 0xf0, 0x4b, 0x6a, 0x35,
    0xfb, 0x5f, 0xa7, 0x22, 0x82, 0x89, 0x0a, 0x59,
    0xc5, 0x31, 0x36, 0x09, 0x35, 0x80, 0x6e, 0xbb,
};

/* Big endian */
uint8_t ecdsa384_s[] = {
    0x38, 0x81, 0x30, 0x8c, 0x41, 0x94, 0x62, 0xe4,
    0x6e, 0x36, 0xf7, 0x4e, 0x20, 0xe4, 0xa7, 0x5f,
    0x47, 0x4e, 0x5f, 0x94, 0xa2, 0x89, 0x7c, 0x71,
    0x87, 0x90, 0xc2, 0xdc, 0xb9, 0x34, 0x5b, 0x85,
    0xe9, 0x9b, 0xa9, 0x29, 0x33, 0x3e, 0x12, 0xd8,
    0x23, 0x3c, 0x7a, 0xcd, 0xa8, 0xf9, 0x61, 0x94,
};
/* Big endian */
const uint8_t ecdsa384_pub_x[] = {
    0xae, 0xc2, 0x2b, 0xb3, 0xcd, 0xb2, 0x09, 0x29,
    0x8d, 0xe5, 0x1d, 0xf9, 0x6f, 0x5e, 0xc5, 0x19,
    0xc7, 0x5b, 0x71, 0xbb, 0x15, 0x06, 0x7e, 0xe7,
    0x73, 0x2d, 0x9e, 0x97, 0xd1, 0xd3, 0x34, 0x0c,
    0x07, 0xe1, 0x93, 0x7e, 0xcc, 0xc5, 0x71, 0xe5,
    0x8b, 0x7b, 0xde, 0x19, 0xe9, 0x5b, 0x91, 0x45,
};

/* Big endian */
const uint8_t ecdsa384_pub_y[] = {
    0xf7, 0xee, 0xac, 0x91, 0x84, 0x64, 0xa1, 0x3d,
    0xf3, 0x42, 0xce, 0x7c, 0x70, 0x81, 0xf1, 0xd1,
    0x4c, 0x1c, 0xc4, 0xba, 0xcd, 0x20, 0x47, 0x48,
    0x4c, 0xfc, 0xa0, 0xa3, 0x82, 0x2a, 0xf2, 0x3d,
    0x97, 0xe0, 0x23, 0x4b, 0x78, 0xd6, 0xc2, 0xce,
    0x7f, 0x86, 0xc9, 0xd6, 0xae, 0xad, 0xbf, 0x29,
};

/* Big endian */
uint8_t ecdsa256_r[] = {
    0xbf, 0x58, 0x35, 0xf2, 0xf4, 0x4d, 0x49, 0x0e,
    0xeb, 0x98, 0x66, 0x7e, 0x0c, 0x25, 0xd1, 0x1b,
    0x6a, 0xd0, 0x15, 0xc4, 0x68, 0x8b, 0x9f, 0x83,
    0x2f, 0xa1, 0x9e, 0xf2, 0x2f, 0x33, 0xc2, 0xf7,
};

/* Big endian */
uint8_t ecdsa256_s[] = {
    0x25, 0x63, 0xb1, 0x5f, 0xaa, 0xe4, 0xcf, 0xc1,
    0xd0, 0xfe, 0x43, 0xdf, 0xbb, 0x5f, 0xe7, 0x29,
    0xdf, 0xb4, 0xf9, 0xfa, 0xe4, 0xb1, 0xc1, 0x7a,
    0x7e, 0x0d, 0x73, 0xe2, 0x4b, 0x91, 0x7c, 0x79,
};

const uint8_t ecdsa256_pub_x[] = {
    0xa2, 0x8f, 0x52, 0x60, 0x20, 0x9b, 0x54, 0x3c,
    0x13, 0x2f, 0x51, 0xb1, 0x89, 0xbf, 0xc7, 0xfa,
    0x84, 0x5c, 0x56, 0x96, 0x2a, 0x00, 0x67, 0xdd,
    0x7c, 0x8c, 0x0f, 0x63, 0x8b, 0x76, 0x7f, 0xb9,
};

const uint8_t ecdsa256_pub_y[] = {
    0xf6, 0x4c, 0x87, 0x5b, 0x5a, 0x9b, 0x59, 0x0a,
    0xc4, 0x53, 0x04, 0x72, 0x0d, 0x7c, 0xde, 0xac,
    0x7e, 0xad, 0x49, 0x8c, 0xf7, 0x5c, 0xc3, 0x1c,
    0x1e, 0x81, 0xf2, 0x47, 0x01, 0x74, 0x05, 0xd5
};

uint8_t ecdsa192_r[] = {
    0x13, 0x94, 0xdb, 0x38, 0x14, 0xf3, 0x49, 0xc7,
    0x7a, 0x92, 0xd0, 0xfc, 0xcb, 0xa6, 0x6d, 0xbd,
    0xf0, 0x12, 0xdf, 0x5e, 0xb2, 0x43, 0xa6, 0x7e,
};

uint8_t ecdsa192_s[] = {
    0x1b, 0x6a, 0x71, 0xf7, 0x9e, 0x3c, 0xcb, 0x74,
    0x64, 0xf3, 0x01, 0x95, 0x50, 0xa5, 0x7b, 0xff,
    0x00, 0x19, 0x2c, 0xc1, 0xc2, 0x2c, 0x5a, 0x94,
};

const uint8_t ecdsa192_pub_x[] = {
    0x88, 0x47, 0x25, 0x3c, 0xb4, 0xb7, 0x87, 0x24,
    0x5e, 0x07, 0xe1, 0xc7, 0xfc, 0x76, 0x0f, 0x6b,
    0x83, 0xf6, 0x81, 0x7d, 0x9b, 0x5f, 0xc4, 0xb9,
};

const uint8_t ecdsa192_pub_y[] = {
    0x9c, 0xfc, 0xaa, 0xed, 0xef, 0xba, 0x02, 0xc3,
    0x1c, 0x0a, 0x55, 0x17, 0xe0, 0x9d, 0x10, 0xcb,
    0x23, 0xae, 0x7e, 0x0f, 0x1f, 0x4d, 0x69, 0xd5
};

void test_ecdsa_verify(esp_ecdsa_curve_t curve, const uint8_t *hash, const uint8_t *r_comp, const uint8_t *s_comp,
                       const uint8_t *pub_x, const uint8_t *pub_y)
{
    size_t hash_len = 0;
    int64_t elapsed_time;
    size_t plen;
    psa_key_id_t key_id;
    psa_key_attributes_t key_attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&key_attr, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_usage_flags(&key_attr, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&key_attr, PSA_ALG_ECDSA(PSA_ALG_SHA_256));

    switch (curve) {
        case ESP_ECDSA_CURVE_SECP192R1:
            plen = 192;
            hash_len = HASH_LEN;
            break;
        case ESP_ECDSA_CURVE_SECP256R1:
            plen = 256;
            hash_len = HASH_LEN;
            break;
#if SOC_ECDSA_SUPPORT_CURVE_P384
        case ESP_ECDSA_CURVE_SECP384R1:
            plen = 384;
            hash_len = HASH_LEN_P384;
            break;
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */
        default:
            TEST_FAIL_MESSAGE("Invalid curve");
            break;
    }

    psa_set_key_bits(&key_attr, plen);
    size_t plen_bytes = plen / 8;

    uint8_t psa_key[2 * MAX_ECDSA_COMPONENT_LEN + 1];
    size_t psa_key_len = 2 * plen_bytes + 1;

    psa_key[0] = ECDSA_UNCOMPRESSED_POINT_FORMAT;

    memcpy(psa_key + 1, pub_x, plen_bytes);
    memcpy(psa_key + 1 + plen_bytes, pub_y, plen_bytes);

    psa_status_t status = psa_import_key(&key_attr, psa_key, psa_key_len, &key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    uint8_t signature[2 * MAX_ECDSA_COMPONENT_LEN];
    memcpy(signature, r_comp, plen_bytes);
    memcpy(signature + plen_bytes, s_comp, plen_bytes);

    ccomp_timer_start();
    status = psa_verify_hash(key_id, PSA_ALG_ECDSA(PSA_ALG_SHA_256), hash, hash_len, signature, 2 * plen_bytes);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    elapsed_time = ccomp_timer_stop();

    if (curve == ESP_ECDSA_CURVE_SECP192R1) {
        TEST_PERFORMANCE_CCOMP_LESS_THAN(ECDSA_P192_VERIFY_OP, "%" NEWLIB_NANO_COMPAT_FORMAT" us", NEWLIB_NANO_COMPAT_CAST(elapsed_time));
    } else if (curve == ESP_ECDSA_CURVE_SECP256R1) {
        TEST_PERFORMANCE_CCOMP_LESS_THAN(ECDSA_P256_VERIFY_OP, "%" NEWLIB_NANO_COMPAT_FORMAT" us", NEWLIB_NANO_COMPAT_CAST(elapsed_time));
    }
#if SOC_ECDSA_SUPPORT_CURVE_P384
    else if (curve == ESP_ECDSA_CURVE_SECP384R1) {
        TEST_PERFORMANCE_CCOMP_LESS_THAN(ECDSA_P384_VERIFY_OP, "%" NEWLIB_NANO_COMPAT_FORMAT" us", NEWLIB_NANO_COMPAT_CAST(elapsed_time));
    }
#endif
    psa_destroy_key(key_id);
    psa_reset_key_attributes(&key_attr);
}

TEST_CASE("mbedtls ECDSA signature verification performance on SECP192R1", "[mbedtls]")
{
#if SOC_ECDSA_SUPPORTED
    if (!ecdsa_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA is not supported");
    }
#endif
    test_ecdsa_verify(ESP_ECDSA_CURVE_SECP192R1, sha, ecdsa192_r, ecdsa192_s, ecdsa192_pub_x, ecdsa192_pub_y);
}

TEST_CASE("mbedtls ECDSA signature verification performance on SECP256R1", "[mbedtls]")
{
#if SOC_ECDSA_SUPPORTED
    if (!ecdsa_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA is not supported");
    }
#endif
    test_ecdsa_verify(ESP_ECDSA_CURVE_SECP256R1, sha, ecdsa256_r, ecdsa256_s, ecdsa256_pub_x, ecdsa256_pub_y);
}

#ifdef SOC_ECDSA_SUPPORT_CURVE_P384
TEST_CASE("mbedtls ECDSA signature verification performance on SECP384R1", "[mbedtls]")
{
#if SOC_ECDSA_SUPPORTED
    if (!ecdsa_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA is not supported");
    }
#endif
    test_ecdsa_verify(ESP_ECDSA_CURVE_SECP384R1, sha, ecdsa384_r, ecdsa384_s, ecdsa384_pub_x, ecdsa384_pub_y);
}
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */

#endif /* CONFIG_MBEDTLS_HARDWARE_ECC */

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
#define USE_ECDSA_KEY_FROM_KEY_MANAGER INT_MAX

/*
 * This test assumes that ECDSA private key has been burnt in efuse.
 *
 * ecdsa_key_p192.pem must be burnt in efuse block 4
 * ecdsa_key_p256.pem must be burnt in efuse block 5
 * ecdsa_key_p384.pem must be burnt in efuse block 6 and 7
 */
#define SECP192R1_EFUSE_BLOCK             4         // EFUSE_BLK_KEY0
#define SECP256R1_EFUSE_BLOCK             5         // EFUSE_BLK_KEY1
#define SECP384R1_EFUSE_BLOCK_HIGH        6         // EFUSE_BLK_KEY2
#define SECP384R1_EFUSE_BLOCK_LOW         7         // EFUSE_BLK_KEY3

/* Big endian */
const uint8_t init_key[] = {
    0xee, 0x89, 0x95, 0xda, 0x3c, 0x8a, 0x43, 0x83, 0xa9, 0x4b, 0x25, 0x5b, 0x04, 0x7e, 0xf1, 0x57, 0xb8, 0xe8, 0x06, 0x45, 0x87, 0x76, 0xee, 0x1b, 0x4e, 0x2e, 0x55, 0xa7, 0x1f, 0x25, 0xe1, 0x94,
};

/* Big endian */
const uint8_t k2_info[] = {
    0x8f, 0x96, 0x33, 0x47, 0xe1, 0xa5, 0x57, 0xe9, 0x2a, 0x51, 0xa9, 0xbe, 0x48, 0x84, 0x25, 0x4e, 0x6f, 0x50, 0x1c, 0x45, 0xdb, 0xb6, 0xfa, 0xeb, 0x35, 0xd2, 0x27, 0x91, 0x3f, 0x67, 0x57, 0xd9, 0xcb, 0x55, 0xe4, 0x2b, 0x18, 0x16, 0xe7, 0xce, 0x6c, 0xf2, 0x58, 0x71, 0x17, 0x76, 0x2a, 0x86, 0x05, 0xe7, 0x37, 0x45, 0x71, 0x34, 0xca, 0xaf, 0x60, 0x07, 0xdf, 0xf4, 0xd2, 0xee, 0x3d, 0x4b,
};

const uint8_t k1_ecdsa256_encrypt[] = {
    0xcb, 0x8b, 0x74, 0xfb, 0xdf, 0x8f, 0x52, 0x0a, 0xff, 0x00, 0xf2, 0x83, 0xfa, 0xdb, 0x34, 0x18, 0xbe, 0xae, 0xe2, 0x58, 0x75, 0x94, 0x69, 0x89, 0xdd, 0x72, 0xdb, 0x04, 0x2c, 0xad, 0x4e, 0x3a,
};

const uint8_t k1_ecdsa192_encrypt[] = {
   0xde, 0xe9, 0x9c, 0x89, 0xf2, 0x3b, 0x29, 0xb7, 0x9e, 0x33, 0xec, 0x76, 0x75, 0x2f, 0x3e, 0xab, 0x61, 0x06, 0x4d, 0xea, 0x05, 0x2c, 0xc3, 0x29, 0x1c, 0x7f, 0xb7, 0x3d, 0xb8, 0x1c, 0xb2, 0x17,
};

void test_ecdsa_sign(esp_ecdsa_curve_t curve, const uint8_t *hash, const uint8_t *pub_x, const uint8_t *pub_y, bool is_deterministic, int efuse_key_block)
{
    size_t hash_len = HASH_LEN;
    uint8_t signature[2 * MAX_ECDSA_COMPONENT_LEN];
    size_t signature_len = 0;
    size_t plen = 0;
    psa_algorithm_t sha_alg = 0;

    switch (curve) {
        case ESP_ECDSA_CURVE_SECP192R1:
            hash_len = HASH_LEN;
            plen = 192;
            sha_alg = PSA_ALG_SHA_256;
            break;
        case ESP_ECDSA_CURVE_SECP256R1:
            hash_len = HASH_LEN;
            plen = 256;
            sha_alg = PSA_ALG_SHA_256;
            break;
#if SOC_ECDSA_SUPPORT_CURVE_P384
        case ESP_ECDSA_CURVE_SECP384R1:
            hash_len = HASH_LEN_P384;
            plen = 384;
            sha_alg = PSA_ALG_SHA_384;
            break;
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */
        default:
            TEST_FAIL_MESSAGE("Invalid curve");
            break;
    }

    size_t plen_bytes = plen / 8;
    psa_key_id_t priv_key_id = 0;
    psa_key_attributes_t priv_attr = PSA_KEY_ATTRIBUTES_INIT;

    esp_ecdsa_opaque_key_t opaque_key = {
        .curve = curve,
    };

    if (efuse_key_block == USE_ECDSA_KEY_FROM_KEY_MANAGER) {
        opaque_key.use_km_key = true;
    } else {
        opaque_key.efuse_block = efuse_key_block;
    }

    psa_algorithm_t alg = (is_deterministic ? PSA_ALG_DETERMINISTIC_ECDSA(sha_alg) : PSA_ALG_ECDSA(sha_alg));

    // Set attributes for opaque private key
    psa_set_key_type(&priv_attr, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&priv_attr, plen);
    psa_set_key_usage_flags(&priv_attr, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&priv_attr, alg);
    psa_set_key_lifetime(&priv_attr, PSA_KEY_LIFETIME_ESP_ECDSA_VOLATILE);  // Opaque key

    // Import opaque key reference
    psa_status_t status = psa_import_key(&priv_attr, (uint8_t*) &opaque_key, sizeof(opaque_key), &priv_key_id);
    TEST_ASSERT_EQUAL_HEX32(PSA_SUCCESS, status);
    TEST_ASSERT_NOT_EQUAL(0, priv_key_id);

    status = psa_sign_hash(priv_key_id,
                          alg,
                          hash, hash_len,
                          signature, 2 * plen_bytes,
                          &signature_len);

    TEST_ASSERT_EQUAL_HEX32(PSA_SUCCESS, status);
    TEST_ASSERT_TRUE(signature_len == 2 * plen_bytes);
    test_ecdsa_verify(curve, sha, signature, signature + plen_bytes, pub_x, pub_y);
    psa_destroy_key(priv_key_id);
    psa_reset_key_attributes(&priv_attr);
}

TEST_CASE("mbedtls ECDSA signature generation on SECP192R1", "[mbedtls][efuse_key]")
{
    if (!ecdsa_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA is not supported");
    }
    test_ecdsa_sign(ESP_ECDSA_CURVE_SECP192R1, sha, ecdsa192_pub_x, ecdsa192_pub_y, false, SECP192R1_EFUSE_BLOCK);
}

TEST_CASE("mbedtls ECDSA signature generation on SECP256R1", "[mbedtls][efuse_key]")
{
    if (!ecdsa_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA is not supported");
    }
    test_ecdsa_sign(ESP_ECDSA_CURVE_SECP256R1, sha, ecdsa256_pub_x, ecdsa256_pub_y, false, SECP256R1_EFUSE_BLOCK);
}

#ifdef SOC_ECDSA_SUPPORT_CURVE_P384
TEST_CASE("mbedtls ECDSA signature generation on SECP384R1", "[mbedtls][efuse_key]")
{
    if (!ecdsa_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA is not supported");
    }
    uint8_t efuse_key_block = HAL_ECDSA_COMBINE_KEY_BLOCKS(SECP384R1_EFUSE_BLOCK_HIGH, SECP384R1_EFUSE_BLOCK_LOW);
    test_ecdsa_sign(ESP_ECDSA_CURVE_SECP384R1, sha, ecdsa384_pub_x, ecdsa384_pub_y, false, efuse_key_block);
}
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */



#if SOC_KEY_MANAGER_SUPPORTED
static void deploy_key_in_key_manager(const uint8_t *k1_encrypted, esp_key_mgr_key_type_t key_type, esp_key_mgr_key_len_t key_len) {
    esp_key_mgr_aes_key_config_t *key_config = NULL;
    key_config = heap_caps_calloc(1, sizeof(esp_key_mgr_aes_key_config_t), MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(key_config);

    key_config->key_type = key_type;
    key_config->key_len = key_len;
    key_config->use_pre_generated_sw_init_key = 1;
    memcpy(key_config->k2_info, (uint8_t*) k2_info, KEY_MGR_K2_INFO_SIZE);
    memcpy(key_config->k1_encrypted[0], (uint8_t*) k1_encrypted, KEY_MGR_K1_ENCRYPTED_SIZE);
    memcpy(key_config->sw_init_key, (uint8_t*) init_key, KEY_MGR_SW_INIT_KEY_SIZE);

    esp_key_mgr_key_recovery_info_t *key_info = NULL;
    key_info = heap_caps_calloc(1, sizeof(esp_key_mgr_key_recovery_info_t), MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(key_info);

    esp_key_mgr_deploy_key_in_aes_mode(key_config, key_info);

    ESP_LOGI(TAG, "Key deployed successfully");
    esp_key_mgr_activate_key(key_info);

    free(key_info);
    free(key_config);
}

TEST_CASE("mbedtls ECDSA signature generation on SECP192R1", "[mbedtls][key_manager_key]")
{
    if (!ecdsa_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA is not supported");
    }
    if (!key_mgr_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("Key manager is not supported");
    }

    deploy_key_in_key_manager(k1_ecdsa192_encrypt, ESP_KEY_MGR_ECDSA_KEY, ESP_KEY_MGR_ECDSA_LEN_192);
    test_ecdsa_sign(ESP_ECDSA_CURVE_SECP192R1, sha, ecdsa192_pub_x, ecdsa192_pub_y, false, USE_ECDSA_KEY_FROM_KEY_MANAGER);
    esp_key_mgr_deactivate_key(ESP_KEY_MGR_ECDSA_KEY);
}

TEST_CASE("mbedtls ECDSA signature generation on SECP256R1", "[mbedtls][key_manager_key]")
{
    if (!ecdsa_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA is not supported");
    }
    if (!key_mgr_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("Key manager is not supported");
    }

    deploy_key_in_key_manager(k1_ecdsa256_encrypt, ESP_KEY_MGR_ECDSA_KEY, ESP_KEY_MGR_ECDSA_LEN_256);
    test_ecdsa_sign(ESP_ECDSA_CURVE_SECP256R1, sha, ecdsa256_pub_x, ecdsa256_pub_y, false, USE_ECDSA_KEY_FROM_KEY_MANAGER);
    esp_key_mgr_deactivate_key(ESP_KEY_MGR_ECDSA_KEY);
}
#endif /* SOC_KEY_MANAGER_SUPPORTED */

#ifdef SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
TEST_CASE("mbedtls ECDSA deterministic signature generation on SECP192R1", "[mbedtls][efuse_key]")
{
    if (!ecdsa_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA is not supported");
    }
    if (!ecdsa_ll_is_deterministic_mode_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA deterministic mode is not supported.");
    } else {
        test_ecdsa_sign(ESP_ECDSA_CURVE_SECP192R1, sha, ecdsa192_pub_x, ecdsa192_pub_y, true, SECP192R1_EFUSE_BLOCK);
    }
}

TEST_CASE("mbedtls ECDSA deterministic signature generation on SECP256R1", "[mbedtls][efuse_key]")
{
    if (!ecdsa_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA is not supported");
    }
    if (!ecdsa_ll_is_deterministic_mode_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA deterministic mode is not supported.");
    } else {
        test_ecdsa_sign(ESP_ECDSA_CURVE_SECP256R1, sha, ecdsa256_pub_x, ecdsa256_pub_y, true, SECP256R1_EFUSE_BLOCK);
    }
}

#ifdef SOC_ECDSA_SUPPORT_CURVE_P384
TEST_CASE("mbedtls ECDSA deterministic signature generation on SECP384R1", "[mbedtls][efuse_key]")
{
    if (!ecdsa_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA is not supported");
    }
    uint8_t efuse_key_block = HAL_ECDSA_COMBINE_KEY_BLOCKS(SECP384R1_EFUSE_BLOCK_HIGH, SECP384R1_EFUSE_BLOCK_LOW);
    test_ecdsa_sign(ESP_ECDSA_CURVE_SECP384R1, sha, ecdsa384_pub_x, ecdsa384_pub_y, true, efuse_key_block);
}
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */

#if SOC_KEY_MANAGER_SUPPORTED
TEST_CASE("mbedtls ECDSA deterministic signature generation on SECP192R1", "[mbedtls][key_manager_key]")
{
    if (!ecdsa_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA is not supported");
    }
    if (!key_mgr_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("Key manager is not supported");
    }

    if (!ecdsa_ll_is_deterministic_mode_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA deterministic mode is not supported.");
    } else {
        deploy_key_in_key_manager(k1_ecdsa192_encrypt, ESP_KEY_MGR_ECDSA_KEY, ESP_KEY_MGR_ECDSA_LEN_192);
        test_ecdsa_sign(ESP_ECDSA_CURVE_SECP192R1, sha, ecdsa192_pub_x, ecdsa192_pub_y, true, USE_ECDSA_KEY_FROM_KEY_MANAGER);
        esp_key_mgr_deactivate_key(ESP_KEY_MGR_ECDSA_KEY);
    }
}

TEST_CASE("mbedtls ECDSA deterministic signature generation on SECP256R1", "[mbedtls][key_manager_key]")
{
    if (!ecdsa_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA is not supported");
    }
    if (!key_mgr_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("Key manager is not supported");
    }

    if (!ecdsa_ll_is_deterministic_mode_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA deterministic mode is not supported.");
    } else {
        deploy_key_in_key_manager(k1_ecdsa256_encrypt, ESP_KEY_MGR_ECDSA_KEY, ESP_KEY_MGR_ECDSA_LEN_256);
        test_ecdsa_sign(ESP_ECDSA_CURVE_SECP256R1, sha, ecdsa256_pub_x, ecdsa256_pub_y, true, USE_ECDSA_KEY_FROM_KEY_MANAGER);
        esp_key_mgr_deactivate_key(ESP_KEY_MGR_ECDSA_KEY);
    }
}
#endif /* SOC_KEY_MANAGER_SUPPORTED */
#endif /* SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE */

#ifdef SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
void test_ecdsa_export_pubkey(esp_ecdsa_curve_t curve, const uint8_t *pub_x, const uint8_t *pub_y, int efuse_key_block)
{
    uint8_t export_pub_key[1 + 2 * MAX_ECDSA_COMPONENT_LEN] = {0};
    size_t len = 0;

    psa_key_id_t priv_key_id = 0;
    psa_key_attributes_t key_attr = PSA_KEY_ATTRIBUTES_INIT;

    esp_ecdsa_opaque_key_t opaque_key = {
        .curve = curve,
    };

    size_t plen = 0;
    psa_algorithm_t sha_alg = 0;

    switch (curve) {
        case ESP_ECDSA_CURVE_SECP192R1:
            plen = 192;
            sha_alg = PSA_ALG_SHA_256;
            break;
        case ESP_ECDSA_CURVE_SECP256R1:
            plen = 256;
            sha_alg = PSA_ALG_SHA_256;
            break;
#if SOC_ECDSA_SUPPORT_CURVE_P384
        case ESP_ECDSA_CURVE_SECP384R1:
            plen = 384;
            sha_alg = PSA_ALG_SHA_384;
            break;
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */
        default:
            TEST_FAIL_MESSAGE("Invalid curve");
            break;
    }
    size_t plen_bytes = plen / 8;

    if (efuse_key_block == USE_ECDSA_KEY_FROM_KEY_MANAGER) {
        opaque_key.use_km_key = true;
    } else {
        opaque_key.efuse_block = efuse_key_block;
    }
    // Set attributes for opaque private key
    psa_set_key_type(&key_attr, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&key_attr, plen);
    psa_set_key_usage_flags(&key_attr, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&key_attr, PSA_ALG_ECDSA(sha_alg));
    psa_set_key_lifetime(&key_attr, PSA_KEY_LIFETIME_ESP_ECDSA_VOLATILE);  // Opaque key

    // Import opaque key reference
    psa_status_t status = psa_import_key(&key_attr, (uint8_t*) &opaque_key, sizeof(opaque_key), &priv_key_id);
    TEST_ASSERT_EQUAL_HEX32(PSA_SUCCESS, status);
    TEST_ASSERT_NOT_EQUAL(0, priv_key_id);

    status = psa_export_public_key(priv_key_id, export_pub_key, sizeof(export_pub_key), &len);

    TEST_ASSERT_EQUAL_HEX32(PSA_SUCCESS, status);
    TEST_ASSERT_TRUE(len == 1 + 2 * plen_bytes);

    TEST_ASSERT_EQUAL_HEX8(ECDSA_UNCOMPRESSED_POINT_FORMAT, export_pub_key[0]);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pub_x, export_pub_key + 1, plen_bytes);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pub_y, export_pub_key + 1 + plen_bytes, plen_bytes);

    psa_destroy_key(priv_key_id);
    psa_reset_key_attributes(&key_attr);
}

TEST_CASE("mbedtls ECDSA export public key on SECP192R1", "[mbedtls][efuse_key]")
{
    if (!ecdsa_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA is not supported");
    }
    test_ecdsa_export_pubkey(ESP_ECDSA_CURVE_SECP192R1, ecdsa192_pub_x, ecdsa192_pub_y, SECP192R1_EFUSE_BLOCK);
}

TEST_CASE("mbedtls ECDSA export public key on SECP256R1", "[mbedtls][efuse_key]")
{
    if (!ecdsa_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA is not supported");
    }
    test_ecdsa_export_pubkey(ESP_ECDSA_CURVE_SECP256R1, ecdsa256_pub_x, ecdsa256_pub_y,  SECP256R1_EFUSE_BLOCK);
}

#ifdef SOC_ECDSA_SUPPORT_CURVE_P384
TEST_CASE("mbedtls ECDSA export public key on SECP384R1", "[mbedtls][efuse_key]")
{
    if (!ecdsa_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA is not supported");
    }
    uint8_t efuse_key_block = HAL_ECDSA_COMBINE_KEY_BLOCKS(SECP384R1_EFUSE_BLOCK_HIGH, SECP384R1_EFUSE_BLOCK_LOW);
    test_ecdsa_export_pubkey(ESP_ECDSA_CURVE_SECP384R1, ecdsa384_pub_x, ecdsa384_pub_y, efuse_key_block);
}
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */

#if SOC_KEY_MANAGER_SUPPORTED
TEST_CASE("mbedtls ECDSA export public key on SECP192R1", "[mbedtls][key_manager_key]")
{
    if (!ecdsa_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA is not supported");
    }
    if (!key_mgr_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("Key manager is not supported");
    }

    deploy_key_in_key_manager(k1_ecdsa192_encrypt, ESP_KEY_MGR_ECDSA_KEY, ESP_KEY_MGR_ECDSA_LEN_192);
    test_ecdsa_export_pubkey(ESP_ECDSA_CURVE_SECP192R1, ecdsa192_pub_x, ecdsa192_pub_y, USE_ECDSA_KEY_FROM_KEY_MANAGER);
    esp_key_mgr_deactivate_key(ESP_KEY_MGR_ECDSA_KEY);
}

TEST_CASE("mbedtls ECDSA export public key on SECP256R1", "[mbedtls][key_manager_key]")
{
    if (!ecdsa_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA is not supported");
    }
    if (!key_mgr_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("Key manager is not supported");
    }

    deploy_key_in_key_manager(k1_ecdsa256_encrypt, ESP_KEY_MGR_ECDSA_KEY, ESP_KEY_MGR_ECDSA_LEN_256);
    test_ecdsa_export_pubkey(ESP_ECDSA_CURVE_SECP256R1, ecdsa256_pub_x, ecdsa256_pub_y,  USE_ECDSA_KEY_FROM_KEY_MANAGER);
    esp_key_mgr_deactivate_key(ESP_KEY_MGR_ECDSA_KEY);
}
#endif

void test_ecdsa_sign_verify_import_export_error_codes(esp_ecdsa_curve_t curve, const uint8_t *hash, const uint8_t *r_comp, const uint8_t *s_comp, const uint8_t *pub_x, const uint8_t *pub_y, int efuse_key_block)
{
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;
    size_t hash_len = 0;
    size_t plen = 0;
    psa_algorithm_t sha_alg = 0;

    switch (curve) {
        case ESP_ECDSA_CURVE_SECP192R1:
            hash_len = HASH_LEN;
            plen = 192;
            sha_alg = PSA_ALG_SHA_256;
            break;
        case ESP_ECDSA_CURVE_SECP256R1:
            hash_len = HASH_LEN;
            plen = 256;
            sha_alg = PSA_ALG_SHA_256;
            break;
#if SOC_ECDSA_SUPPORT_CURVE_P384
        case ESP_ECDSA_CURVE_SECP384R1:
            hash_len = HASH_LEN_P384;
            plen = 384;
            sha_alg = PSA_ALG_SHA_384;
            break;
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */
        default:
            TEST_FAIL_MESSAGE("Invalid curve");
            break;
    }

    size_t plen_bytes = plen / 8;

    // 1. Signature Verification
    psa_key_id_t pub_key_id;
    psa_key_attributes_t pub_key_attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&pub_key_attr, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_usage_flags(&pub_key_attr, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&pub_key_attr, PSA_ALG_ECDSA(sha_alg));
    psa_set_key_bits(&pub_key_attr, plen);

    uint8_t psa_key[2 * MAX_ECDSA_COMPONENT_LEN + 1];
    size_t psa_key_len = 2 * plen_bytes + 1;

    psa_key[0] = ECDSA_UNCOMPRESSED_POINT_FORMAT;

    memcpy(psa_key + 1, pub_x, plen_bytes);
    memcpy(psa_key + 1 + plen_bytes, pub_y, plen_bytes);

    status = psa_import_key(&pub_key_attr, psa_key, psa_key_len, &pub_key_id);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    uint8_t signature[2 * MAX_ECDSA_COMPONENT_LEN];
    memcpy(signature, r_comp, plen_bytes);
    memcpy(signature + plen_bytes, s_comp, plen_bytes);

    // 1.1 Incorrect signature buffer length
    status = psa_verify_hash(pub_key_id, PSA_ALG_ECDSA(sha_alg), hash, hash_len, signature, 2 * plen_bytes - 1);
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_SIGNATURE, status);

    psa_destroy_key(pub_key_id);
    psa_reset_key_attributes(&pub_key_attr);

    // 2. Import Opaque Private Key
    psa_key_id_t priv_key_id = 0;
    psa_key_attributes_t priv_attr = PSA_KEY_ATTRIBUTES_INIT;
    esp_ecdsa_opaque_key_t opaque_key = { 0 };
    opaque_key.curve = curve;
    opaque_key.efuse_block = efuse_key_block;

    // Set attributes for opaque private key
    psa_set_key_type(&priv_attr, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&priv_attr, plen);
    psa_set_key_usage_flags(&priv_attr, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&priv_attr, PSA_ALG_ECDSA(sha_alg));
    psa_set_key_lifetime(&priv_attr, PSA_KEY_LIFETIME_ESP_ECDSA_VOLATILE);  // Opaque

    // incorrect opaque key buffer length
    status = psa_import_key(&priv_attr, (uint8_t*) &opaque_key, sizeof(opaque_key) - 1, &priv_key_id);
    TEST_ASSERT_EQUAL_HEX32(PSA_ERROR_BUFFER_TOO_SMALL, status);

    // incorrect curve
    opaque_key.curve = curve + 1;
    status = psa_import_key(&priv_attr, (uint8_t*) &opaque_key, sizeof(opaque_key), &priv_key_id);
    TEST_ASSERT_EQUAL_HEX32(PSA_ERROR_INVALID_ARGUMENT, status);

    // incorrect efuse block
    opaque_key.efuse_block = 9; // EFUSE_BLK_KEY5 [EFUSE_BLK_KEY0 to EFUSE_BLK_KEY4 are used for the ECDSA tests]
    status = psa_import_key(&priv_attr, (uint8_t*) &opaque_key, sizeof(opaque_key), &priv_key_id);
    TEST_ASSERT_EQUAL_HEX32(PSA_ERROR_INVALID_ARGUMENT, status);

    opaque_key.efuse_block = efuse_key_block;
    opaque_key.curve = curve;
    status = psa_import_key(&priv_attr, (uint8_t*) &opaque_key, sizeof(opaque_key), &priv_key_id);
    TEST_ASSERT_EQUAL_HEX32(PSA_SUCCESS, status);
    TEST_ASSERT_NOT_EQUAL(0, priv_key_id);

    // Signature Generation
    psa_algorithm_t alg = PSA_ALG_ECDSA(sha_alg);

    memset(signature, 0, sizeof(signature));
    size_t signature_len = 0;

    // Invalid hash length
    status = psa_sign_hash(priv_key_id,
                            alg,
                            hash, hash_len - 1,
                            signature, 2 * plen_bytes,
                            &signature_len);
    TEST_ASSERT_EQUAL_HEX32(PSA_ERROR_INVALID_ARGUMENT, status);

    // Invalid signature buffer length
    status = psa_sign_hash(priv_key_id,
                            alg,
                            hash, hash_len,
                            signature, 2 * plen_bytes - 1,
                            &signature_len);
    TEST_ASSERT_EQUAL_HEX32(PSA_ERROR_BUFFER_TOO_SMALL, status);

    status = psa_sign_hash(priv_key_id,
                            alg,
                            hash, hash_len,
                            signature, 2 * plen_bytes,
                            &signature_len);

    TEST_ASSERT_EQUAL_HEX32(PSA_SUCCESS, status);
    TEST_ASSERT_TRUE(signature_len == 2 * plen_bytes);


    // Export Public Key
    uint8_t export_pub_key[1 + 2 * MAX_ECDSA_COMPONENT_LEN] = {0};
    size_t export_pub_key_len = 0;

    // incorrect export public key buffer length
    status = psa_export_public_key(priv_key_id, export_pub_key, (2 * plen_bytes), &export_pub_key_len);
    TEST_ASSERT_EQUAL_HEX32(PSA_ERROR_BUFFER_TOO_SMALL, status);

    // correct export public key buffer length
    status = psa_export_public_key(priv_key_id, export_pub_key, sizeof(export_pub_key), &export_pub_key_len);
    TEST_ASSERT_EQUAL_HEX32(PSA_SUCCESS, status);
    psa_destroy_key(priv_key_id);
    psa_reset_key_attributes(&priv_attr);
}

TEST_CASE("mbedtls ECDSA signature generation verification, import and export error codes", "[mbedtls][efuse_key]")
{
    if (!ecdsa_ll_is_supported()) {
        TEST_IGNORE_MESSAGE("ECDSA is not supported");
    }
    test_ecdsa_sign_verify_import_export_error_codes(ESP_ECDSA_CURVE_SECP256R1, sha, ecdsa256_r, ecdsa256_s, ecdsa256_pub_x, ecdsa256_pub_y, SECP256R1_EFUSE_BLOCK);
}

#endif /* SOC_ECDSA_SUPPORT_EXPORT_PUBKEY */
#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN */
