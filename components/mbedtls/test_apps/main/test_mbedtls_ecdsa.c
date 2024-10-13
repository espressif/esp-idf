/* mbedTLS Elliptic Curve Digital Signature performance tests
 *
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <esp_log.h>

#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/ecdh.h>
#include <mbedtls/ecdsa.h>
#include <mbedtls/error.h>

#include "soc/soc_caps.h"
#include "test_utils.h"
#include "ccomp_timer.h"
#include "unity.h"
#include "esp_heap_caps.h"

#include "ecdsa/ecdsa_alt.h"
#if SOC_KEY_MANAGER_SUPPORTED
#include "esp_key_mgr.h"
#endif
#define TEST_ASSERT_MBEDTLS_OK(X) TEST_ASSERT_EQUAL_HEX32(0, -(X))

#if CONFIG_NEWLIB_NANO_FORMAT
#define NEWLIB_NANO_COMPAT_FORMAT             PRIu32
#define NEWLIB_NANO_COMPAT_CAST(int64_t_var)  (uint32_t)int64_t_var
#else
#define NEWLIB_NANO_COMPAT_FORMAT             PRId64
#define NEWLIB_NANO_COMPAT_CAST(int64_t_var)  int64_t_var
#endif

/*
 * All the following values are in big endian format, as required by the mbedTLS APIs
 */

const uint8_t sha[] = {
    0x0c, 0xaa, 0x08, 0xb4, 0xf0, 0x89, 0xd3, 0x45,
    0xbb, 0x55, 0x98, 0xd9, 0xc2, 0xe9, 0x65, 0x5d,
    0x7e, 0xa3, 0xa9, 0xc3, 0xcd, 0x69, 0xb1, 0xcf,
    0x91, 0xbe, 0x58, 0x10, 0xfe, 0x80, 0x65, 0x6e
};

#if CONFIG_MBEDTLS_HARDWARE_ECC || CONFIG_MBEDTLS_HARDWARE_ECDSA_VERIFY

const uint8_t ecdsa256_r[] = {
    0x26, 0x1a, 0x0f, 0xbd, 0xa5, 0xe5, 0x1e, 0xe7,
    0xb3, 0xc3, 0xb7, 0x09, 0xd1, 0x4a, 0x7a, 0x2a,
    0x16, 0x69, 0x4b, 0xaf, 0x76, 0x5c, 0xd4, 0x0e,
    0x93, 0x57, 0xb8, 0x67, 0xf9, 0xa1, 0xe5, 0xe8
};

const uint8_t ecdsa256_s[] = {
    0x63, 0x59, 0xc0, 0x3b, 0x6a, 0xc2, 0xc4, 0xc4,
    0xaf, 0x47, 0x5c, 0xe6, 0x6d, 0x43, 0x3b, 0xa7,
    0x91, 0x51, 0x15, 0x62, 0x7e, 0x46, 0x0e, 0x68,
    0x84, 0xce, 0x72, 0xa0, 0xd8, 0x8b, 0x69, 0xd5
};

const uint8_t ecdsa256_pub_x[] = {
    0xcb, 0x59, 0xde, 0x9c, 0xbb, 0x28, 0xaa, 0xac,
    0x72, 0x06, 0xc3, 0x43, 0x2a, 0x65, 0x82, 0xcc,
    0x68, 0x01, 0x76, 0x68, 0xfc, 0xec, 0xf5, 0x91,
    0xd1, 0x9e, 0xbf, 0xcf, 0x67, 0x7d, 0x7d, 0xbe
};

const uint8_t ecdsa256_pub_y[] = {
    0x00, 0x66, 0x14, 0x74, 0xe0, 0x06, 0x44, 0x66,
    0x6f, 0x3b, 0x8c, 0x3b, 0x2d, 0x05, 0xf6, 0xd5,
    0xb2, 0x5d, 0xe4, 0x85, 0x6c, 0x61, 0x38, 0xc5,
    0xb1, 0x21, 0xde, 0x2b, 0x44, 0xf5, 0x13, 0x62
};

const uint8_t ecdsa192_r[] = {
    0x2b, 0x8a, 0x18, 0x2f, 0xb2, 0x75, 0x26, 0xb7,
    0x1c, 0xe1, 0xe2, 0x6d, 0xaa, 0xe7, 0x74, 0x2c,
    0x42, 0xc8, 0xd5, 0x09, 0x4f, 0xb7, 0xee, 0x9f
};

const uint8_t ecdsa192_s[] = {
    0x1a, 0x74, 0xb4, 0x5, 0xf4, 0x28, 0xa5, 0xb6,
    0xce, 0xed, 0xa5, 0xff, 0xa8, 0x60, 0x06, 0x2f,
    0xf6, 0xeb, 0x24, 0x59, 0x24, 0x30, 0x5b, 0x12
};

const uint8_t ecdsa192_pub_x[] = {
    0xd0, 0x3f, 0x6f, 0xe7, 0x5d, 0xaa, 0xf4, 0xc0,
    0x1e, 0x63, 0x7b, 0x82, 0xab, 0x23, 0x33, 0x34,
    0x74, 0x59, 0x56, 0x5d, 0x21, 0x10, 0x9c, 0xb1
};

const uint8_t ecdsa192_pub_y[] = {
    0x85, 0xfc, 0x76, 0xcb, 0x65, 0xbc, 0xc4, 0xbe,
    0x74, 0x09, 0xfd, 0xf3, 0x74, 0xdc, 0xc2, 0xde,
    0x7e, 0x4b, 0x23, 0xad, 0x46, 0x5c, 0x87, 0xc2
};

void test_ecdsa_verify(mbedtls_ecp_group_id id, const uint8_t *hash, const uint8_t *r_comp, const uint8_t *s_comp,
                       const uint8_t *pub_x, const uint8_t *pub_y)
{
    int64_t elapsed_time;
    mbedtls_mpi r, s;

    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    mbedtls_ecdsa_context ecdsa_context;
    mbedtls_ecdsa_init(&ecdsa_context);

    mbedtls_ecp_group_load(&ecdsa_context.MBEDTLS_PRIVATE(grp), id);
    size_t plen = mbedtls_mpi_size(&ecdsa_context.MBEDTLS_PRIVATE(grp).P);

    TEST_ASSERT_MBEDTLS_OK(mbedtls_mpi_read_binary(&r, r_comp, plen));

    TEST_ASSERT_MBEDTLS_OK(mbedtls_mpi_read_binary(&s, s_comp, plen));

    TEST_ASSERT_MBEDTLS_OK(mbedtls_mpi_read_binary(&ecdsa_context.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(X), pub_x, plen));
    TEST_ASSERT_MBEDTLS_OK(mbedtls_mpi_read_binary(&ecdsa_context.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(Y), pub_y, plen));
    TEST_ASSERT_MBEDTLS_OK(mbedtls_mpi_lset(&ecdsa_context.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(Z), 1));

    ccomp_timer_start();
    TEST_ASSERT_MBEDTLS_OK(mbedtls_ecdsa_verify(&ecdsa_context.MBEDTLS_PRIVATE(grp), hash, 32, &ecdsa_context.MBEDTLS_PRIVATE(Q), &r, &s));
    elapsed_time = ccomp_timer_stop();

    if (id == MBEDTLS_ECP_DP_SECP192R1) {
        TEST_PERFORMANCE_CCOMP_LESS_THAN(ECDSA_P192_VERIFY_OP, "%" NEWLIB_NANO_COMPAT_FORMAT" us", NEWLIB_NANO_COMPAT_CAST(elapsed_time));
    } else if (id == MBEDTLS_ECP_DP_SECP256R1) {
        TEST_PERFORMANCE_CCOMP_LESS_THAN(ECDSA_P256_VERIFY_OP, "%" NEWLIB_NANO_COMPAT_FORMAT" us", NEWLIB_NANO_COMPAT_CAST(elapsed_time));
    }

    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);
    mbedtls_ecdsa_free(&ecdsa_context);
}

TEST_CASE("mbedtls ECDSA signature verification performance on SECP192R1", "[mbedtls]")
{
    test_ecdsa_verify(MBEDTLS_ECP_DP_SECP192R1, sha, ecdsa192_r, ecdsa192_s,
                 ecdsa192_pub_x, ecdsa192_pub_y);
}

TEST_CASE("mbedtls ECDSA signature verification performance on SECP256R1", "[mbedtls]")
{
    test_ecdsa_verify(MBEDTLS_ECP_DP_SECP256R1, sha, ecdsa256_r, ecdsa256_s,
                 ecdsa256_pub_x, ecdsa256_pub_y);
}

#endif /* CONFIG_MBEDTLS_HARDWARE_ECC */

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN

/*
 * This test assumes that ECDSA private key has been burnt in efuse.
 *
 * ecdsa_key_p192.pem must be burnt in efuse block 4
 * ecdsa_key_p256.pem must be burnt in efuse block 5
 */
#define SECP192R1_EFUSE_BLOCK             4         // EFUSE_BLK_KEY0
#define SECP256R1_EFUSE_BLOCK             5         // EFUSE_BLK_KEY1

#define MAX_ECDSA_COMPONENT_LEN           32
#define HASH_LEN                          32

const uint8_t ecdsa256_sign_pub_x[] = {
    0xa2, 0x8f, 0x52, 0x60, 0x20, 0x9b, 0x54, 0x3c,
    0x13, 0x2f, 0x51, 0xb1, 0x89, 0xbf, 0xc7, 0xfa,
    0x84, 0x5c, 0x56, 0x96, 0x2a, 0x00, 0x67, 0xdd,
    0x7c, 0x8c, 0x0f, 0x63, 0x8b, 0x76, 0x7f, 0xb9,
};

const uint8_t ecdsa256_sign_pub_y[] = {
    0xf6, 0x4c, 0x87, 0x5b, 0x5a, 0x9b, 0x59, 0x0a,
    0xc4, 0x53, 0x04, 0x72, 0x0d, 0x7c, 0xde, 0xac,
    0x7e, 0xad, 0x49, 0x8c, 0xf7, 0x5c, 0xc3, 0x1c,
    0x1e, 0x81, 0xf2, 0x47, 0x01, 0x74, 0x05, 0xd5
};

const uint8_t ecdsa192_sign_pub_x[] = {
    0x88, 0x47, 0x25, 0x3c, 0xb4, 0xb7, 0x87, 0x24,
    0x5e, 0x07, 0xe1, 0xc7, 0xfc, 0x76, 0x0f, 0x6b,
    0x83, 0xf6, 0x81, 0x7d, 0x9b, 0x5f, 0xc4, 0xb9,
};

const uint8_t ecdsa192_sign_pub_y[] = {
    0x9c, 0xfc, 0xaa, 0xed, 0xef, 0xba, 0x02, 0xc3,
    0x1c, 0x0a, 0x55, 0x17, 0xe0, 0x9d, 0x10, 0xcb,
    0x23, 0xae, 0x7e, 0x0f, 0x1f, 0x4d, 0x69, 0xd5
};

/* Big endian */
const uint8_t init_key[] = {
    0x4d, 0x21, 0x64, 0x21, 0x8f, 0xa2, 0xe3, 0xa0, 0xab, 0x74, 0xb5, 0xab, 0x17, 0x9a, 0x5d, 0x08, 0x58, 0xf4, 0x22, 0x03, 0xbd, 0x52, 0xe7, 0x88, 0x3c, 0x22, 0x0f, 0x95, 0x89, 0x70, 0xe1, 0x93
};

/* Big endian */
const uint8_t k2_info[] = {
    0xd8, 0xcd, 0x04, 0x45, 0xb4, 0x45, 0xc4, 0x15, 0xf6, 0x40, 0x1c, 0x7d, 0x90, 0x1b, 0x99, 0xa4, 0x79, 0x6b, 0xfb, 0x5b, 0x2a, 0x40, 0x60, 0xe1, 0xc1, 0xe1, 0x48, 0xcd, 0x46, 0x6b, 0x9b, 0x48, 0xda, 0x7a, 0x70, 0x0a, 0x78, 0x0b, 0x9d, 0xf9, 0x0e, 0xed, 0x91, 0xfc, 0xa5, 0xc2, 0x96, 0x05, 0x91, 0x76, 0xdb, 0x68, 0x84, 0x5d, 0x5e, 0x5b, 0xa6, 0xe9, 0x6b, 0x3b, 0x12, 0x50, 0x05, 0xc3
};

const uint8_t k1_ecdsa256_xts_encrypt[] = {
    0x9f, 0x64, 0x80, 0x16, 0xa3, 0xab, 0x26, 0x64, 0x9b, 0xe6, 0x86, 0xcd, 0xf5, 0x14, 0x11, 0xb9, 0xb0, 0xe9, 0x87, 0xf6, 0xfe, 0x1b, 0x98, 0x0f, 0x9c, 0x3e, 0x21, 0xa7, 0xfa, 0x53, 0x47, 0x60
};

const uint8_t k1_ecdsa192_xts_encrypt[] = {
   0x54, 0xf5, 0x97, 0xb8, 0xff, 0x1d, 0x34, 0x85, 0x8d, 0xf1, 0x43, 0xaa, 0xc0, 0x0f, 0xe2, 0x4d, 0x0b, 0xee, 0xdd, 0x89, 0x31, 0x39, 0x1b, 0xbe, 0x9b, 0x55, 0x53, 0xe0, 0xc7, 0xd9, 0x79, 0xaf
};

void test_ecdsa_sign(mbedtls_ecp_group_id id, const uint8_t *hash, const uint8_t *pub_x, const uint8_t *pub_y, bool is_deterministic, int efuse_key_block)
{
    uint8_t r_be[MAX_ECDSA_COMPONENT_LEN] = {0};
    uint8_t s_be[MAX_ECDSA_COMPONENT_LEN] = {0};

    mbedtls_mpi r, s;
    mbedtls_mpi key_mpi;

    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    mbedtls_ecdsa_context ecdsa_context;
    mbedtls_ecdsa_init(&ecdsa_context);

    if (id == MBEDTLS_ECP_DP_SECP192R1) {
        mbedtls_ecp_group_load(&ecdsa_context.MBEDTLS_PRIVATE(grp), MBEDTLS_ECP_DP_SECP192R1);
    } else if (id == MBEDTLS_ECP_DP_SECP256R1) {
        mbedtls_ecp_group_load(&ecdsa_context.MBEDTLS_PRIVATE(grp), MBEDTLS_ECP_DP_SECP256R1);
    }
    esp_ecdsa_privkey_load_mpi(&key_mpi, efuse_key_block);

    if (is_deterministic) {
        mbedtls_ecdsa_sign_det_ext(&ecdsa_context.MBEDTLS_PRIVATE(grp), &r, &s, &key_mpi, sha, HASH_LEN, 0, NULL, NULL);
    } else {
        mbedtls_ecdsa_sign(&ecdsa_context.MBEDTLS_PRIVATE(grp), &r, &s, &key_mpi, sha, HASH_LEN, NULL, NULL);
    }

    mbedtls_mpi_write_binary(&r, r_be, MAX_ECDSA_COMPONENT_LEN);
    mbedtls_mpi_write_binary(&s, s_be, MAX_ECDSA_COMPONENT_LEN);

    if (id == MBEDTLS_ECP_DP_SECP192R1) {
        // Skip the initial zeroes
        test_ecdsa_verify(id, sha, &r_be[8], &s_be[8], pub_x, pub_y);
    } else if (id == MBEDTLS_ECP_DP_SECP256R1) {
        test_ecdsa_verify(id, sha, r_be, s_be, pub_x, pub_y);
    }

    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);
    mbedtls_mpi_free(&key_mpi);
}

TEST_CASE("mbedtls ECDSA signature generation on SECP192R1", "[mbedtls][efuse_key]")
{
    test_ecdsa_sign(MBEDTLS_ECP_DP_SECP192R1, sha, ecdsa192_sign_pub_x, ecdsa192_sign_pub_y, false, SECP192R1_EFUSE_BLOCK);
}

TEST_CASE("mbedtls ECDSA signature generation on SECP256R1", "[mbedtls][efuse_key]")
{
    test_ecdsa_sign(MBEDTLS_ECP_DP_SECP256R1, sha, ecdsa256_sign_pub_x, ecdsa256_sign_pub_y, false, SECP256R1_EFUSE_BLOCK);
}

#ifdef SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE

TEST_CASE("mbedtls ECDSA deterministic signature generation on SECP192R1", "[mbedtls][efuse_key]")
{
    test_ecdsa_sign(MBEDTLS_ECP_DP_SECP192R1, sha, ecdsa192_sign_pub_x, ecdsa192_sign_pub_y, true, SECP192R1_EFUSE_BLOCK);
}

TEST_CASE("mbedtls ECDSA deterministic signature generation on SECP256R1", "[mbedtls][efuse_key]")
{
    test_ecdsa_sign(MBEDTLS_ECP_DP_SECP256R1, sha, ecdsa256_sign_pub_x, ecdsa256_sign_pub_y, true, SECP256R1_EFUSE_BLOCK);
}

#endif /* SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE */

#if SOC_KEY_MANAGER_SUPPORTED
void deploy_key_in_key_manager(const uint8_t *k1_encrypted) {
    esp_key_mgr_aes_key_config_t *key_config;

    key_config = heap_caps_calloc(1, sizeof(esp_key_mgr_aes_key_config_t), MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(key_config);
    memcpy(key_config->k2_info, (uint8_t*) k2_info, KEY_MGR_K2_INFO_SIZE);
    memcpy(key_config->k1_encrypted[0], (uint8_t*) k1_encrypted, KEY_MGR_K1_ENCRYPTED_SIZE);
    memcpy(key_config->sw_init_key, (uint8_t*) init_key, KEY_MGR_SW_INIT_KEY_SIZE);
    key_config->use_pre_generated_sw_init_key = 1;
    key_config->key_type = ESP_KEY_MGR_ECDSA_KEY;

    esp_key_mgr_key_recovery_info_t *key_info;
    key_info = heap_caps_calloc(1, sizeof(esp_key_mgr_key_recovery_info_t), MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(key_config);

    esp_key_mgr_deploy_key_in_aes_mode(key_config, key_info);
    printf("\nkey deployed successfully\n");
    esp_key_mgr_activate_key(key_info);
    free(key_info);
    free(key_config);
}

TEST_CASE("mbedtls ECDSA signature generation on SECP192R1", "[mbedtls][key_manager_key]")
{
    deploy_key_in_key_manager(k1_ecdsa192_xts_encrypt);
    test_ecdsa_sign(MBEDTLS_ECP_DP_SECP192R1, sha, ecdsa192_sign_pub_x, ecdsa192_sign_pub_y, false, USE_ECDSA_KEY_FROM_KEY_MANAGER);
    esp_key_mgr_deactivate_key(ESP_KEY_MGR_ECDSA_KEY);
}

TEST_CASE("mbedtls ECDSA signature generation on SECP256R1", "[mbedtls][key_manager_key]")
{
    deploy_key_in_key_manager(k1_ecdsa256_xts_encrypt);
    test_ecdsa_sign(MBEDTLS_ECP_DP_SECP256R1, sha, ecdsa256_sign_pub_x, ecdsa256_sign_pub_y, false, USE_ECDSA_KEY_FROM_KEY_MANAGER);
    esp_key_mgr_deactivate_key(ESP_KEY_MGR_ECDSA_KEY);
}
#endif

#ifdef SOC_ECDSA_SUPPORT_EXPORT_PUBKEY

void test_ecdsa_export_pubkey(mbedtls_ecp_group_id id, const uint8_t *pub_x, const uint8_t *pub_y, int efuse_key_block)
{
    uint8_t export_pub_x[32] = {0};
    uint8_t export_pub_y[32] = {0};
    int len = 0;

    esp_ecdsa_pk_conf_t pk_conf = {
        .grp_id = id,
        .load_pubkey = true,
        .efuse_block = efuse_key_block,
    };

    if (id == MBEDTLS_ECP_DP_SECP192R1) {
        len = 24;
    } else if (id == MBEDTLS_ECP_DP_SECP256R1) {
        len = 32;
    }

    mbedtls_pk_context key_ctx;

    int ret = esp_ecdsa_set_pk_context(&key_ctx, &pk_conf);
    TEST_ASSERT_EQUAL(0, ret);

    mbedtls_ecp_keypair *keypair = mbedtls_pk_ec(key_ctx);
    mbedtls_mpi_write_binary(&(keypair->MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(X)), export_pub_x, len);
    mbedtls_mpi_write_binary(&(keypair->MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(Y)), export_pub_y, len);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(pub_x, export_pub_x, len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(pub_y, export_pub_y, len);

    mbedtls_ecdsa_free(keypair);
    mbedtls_pk_free(&key_ctx);
}

TEST_CASE("mbedtls ECDSA export public key on SECP192R1", "[mbedtls][efuse_key]")
{
    test_ecdsa_export_pubkey(MBEDTLS_ECP_DP_SECP192R1, ecdsa192_sign_pub_x, ecdsa192_sign_pub_y, SECP192R1_EFUSE_BLOCK);
}

TEST_CASE("mbedtls ECDSA export public key on SECP256R1", "[mbedtls][efuse_key]")
{
    test_ecdsa_export_pubkey(MBEDTLS_ECP_DP_SECP256R1, ecdsa256_sign_pub_x, ecdsa256_sign_pub_y,  SECP256R1_EFUSE_BLOCK);
}

#if SOC_KEY_MANAGER_SUPPORTED
TEST_CASE("mbedtls ECDSA export public key on SECP192R1", "[mbedtls][key_manager_key]")
{
    deploy_key_in_key_manager(k1_ecdsa192_xts_encrypt);
    test_ecdsa_export_pubkey(MBEDTLS_ECP_DP_SECP192R1, ecdsa192_sign_pub_x, ecdsa192_sign_pub_y, USE_ECDSA_KEY_FROM_KEY_MANAGER);
    esp_key_mgr_deactivate_key(ESP_KEY_MGR_ECDSA_KEY);
}

TEST_CASE("mbedtls ECDSA export public key on SECP256R1", "[mbedtls][key_manager_key]")
{
    deploy_key_in_key_manager(k1_ecdsa256_xts_encrypt);
    test_ecdsa_export_pubkey(MBEDTLS_ECP_DP_SECP256R1, ecdsa256_sign_pub_x, ecdsa256_sign_pub_y,  USE_ECDSA_KEY_FROM_KEY_MANAGER);
    esp_key_mgr_deactivate_key(ESP_KEY_MGR_ECDSA_KEY);
}
#endif
#endif /* SOC_ECDSA_SUPPORT_EXPORT_PUBKEY */

#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN */
