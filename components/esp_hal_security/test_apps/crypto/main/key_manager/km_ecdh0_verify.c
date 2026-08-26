/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#define MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS

#include <stdint.h>
#include <string.h>

#include "esp_log.h"
#include "unity.h"

#include "ecc_impl.h"
#include "psa/crypto.h"

#include "mbedtls/private/aes.h"
#include "mbedtls/ecp.h"
#include "mbedtls/bignum.h"

#include "km_ecdh0_verify.h"

/* P-192 generator hardcoded in esp_ecc's little-endian layout. We don't pull
 * P-192 from mbedtls because support for ECDSA P-192 is disabled by default in mbedtls.
 * P-256 and P-384 are loaded from mbedtls at use time -- see load_curve_generator_le below. */
static const uint8_t p192_G_x_le[P192_LEN] = {
    0x12, 0x10, 0xff, 0x82, 0xfd, 0x0a, 0xff, 0xf4, 0x00, 0x88, 0xa1, 0x43,
    0xeb, 0x20, 0xbf, 0x7c, 0xf6, 0x90, 0x30, 0xb0, 0x0e, 0xa8, 0x8d, 0x18,
};
static const uint8_t p192_G_y_le[P192_LEN] = {
    0x11, 0x48, 0x79, 0x1e, 0xa1, 0x77, 0xf9, 0x73, 0xd5, 0xcd, 0x24, 0x6b,
    0xed, 0x11, 0x10, 0x63, 0x78, 0xda, 0xc8, 0xff, 0x95, 0x2b, 0x19, 0x07,
};

static void load_curve_generator_le(mbedtls_ecp_group_id grp_id,
                                    uint8_t *gx_le, uint8_t *gy_le,
                                    size_t coord_len)
{
    mbedtls_ecp_group grp;
    mbedtls_ecp_group_init(&grp);
    TEST_ASSERT_EQUAL(0, mbedtls_ecp_group_load(&grp, grp_id));

    /* mbedtls_ecp_point_write_binary in uncompressed form gives
     * 0x04 || X_BE || Y_BE. Reverse each coord into the LE byte order
     * esp_ecc expects. */
    uint8_t buf[1 + 2 * P384_LEN];
    size_t olen = 0;
    TEST_ASSERT_EQUAL(0, mbedtls_ecp_point_write_binary(&grp, &grp.G,
                                                        MBEDTLS_ECP_PF_UNCOMPRESSED,
                                                        &olen, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL(1 + 2 * coord_len, olen);
    for (size_t i = 0; i < coord_len; ++i) {
        gx_le[i] = buf[1 + coord_len - 1 - i];
        gy_le[i] = buf[1 + 2 * coord_len - 1 - i];
    }

    mbedtls_ecp_group_free(&grp);
}

void km_verify_compute_x_be(const uint8_t *k1_be, size_t k1_len,
                            const uint8_t *k2_G_km, uint8_t x_be_out[32])
{
    uint8_t k1_le[P256_LEN] = { 0 };
    TEST_ASSERT_LESS_OR_EQUAL(P256_LEN, k1_len);
    for (size_t i = 0; i < k1_len; ++i) {
        k1_le[i] = k1_be[k1_len - 1 - i];
    }

    ecc_point_t k2_G = { .len = P256_LEN };
    memcpy(k2_G.x, k2_G_km, P256_LEN);
    memcpy(k2_G.y, k2_G_km + P256_LEN, P256_LEN);

    ecc_point_t result = { 0 };
    TEST_ASSERT_EQUAL(0, esp_ecc_point_multiply(&k2_G, k1_le, &result, true));

    for (int i = 0; i < P256_LEN; ++i) {
        x_be_out[i] = result.x[P256_LEN - 1 - i];
    }
}

void km_verify_ecdsa_pubkey_from_scalar(unsigned curve_len,
                                        const uint8_t *scalar_be,
                                        size_t scalar_len,
                                        uint8_t *pub_x_le_out,
                                        uint8_t *pub_y_le_out)
{
    uint8_t scalar_le[P384_LEN] = { 0 };
    TEST_ASSERT_LESS_OR_EQUAL(curve_len, scalar_len);
    for (size_t i = 0; i < scalar_len; ++i) {
        scalar_le[i] = scalar_be[scalar_len - 1 - i];
    }

    ecc_point_t G = { .len = curve_len };
    switch (curve_len) {
    case P192_LEN:
        memcpy(G.x, p192_G_x_le, P192_LEN);
        memcpy(G.y, p192_G_y_le, P192_LEN);
        break;
    case P256_LEN:
        load_curve_generator_le(MBEDTLS_ECP_DP_SECP256R1, G.x, G.y, P256_LEN);
        break;
#if SOC_ECC_SUPPORT_CURVE_P384
    case P384_LEN:
        load_curve_generator_le(MBEDTLS_ECP_DP_SECP384R1, G.x, G.y, P384_LEN);
        break;
#endif
    default:
        TEST_FAIL_MESSAGE("Unsupported curve length for ECDSA pubkey reconstruction");
    }

    ecc_point_t pub = { 0 };
    /* G is always a valid curve point, so verify_first=false. */
    TEST_ASSERT_EQUAL(0, esp_ecc_point_multiply(&G, scalar_le, &pub, false));

    memcpy(pub_x_le_out, pub.x, curve_len);
    memcpy(pub_y_le_out, pub.y, curve_len);
}

void km_verify_flash_xts_encrypt(const uint8_t *key, size_t key_bits,
                                 uint32_t flash_address,
                                 const uint8_t *pt, size_t pt_len,
                                 uint8_t *ct_out)
{
    TEST_ASSERT_LESS_OR_EQUAL(128, pt_len);
    TEST_ASSERT_EQUAL(0, flash_address & 0x7F);

    mbedtls_aes_xts_context ctx;
    mbedtls_aes_xts_init(&ctx);
    TEST_ASSERT_EQUAL(0, mbedtls_aes_xts_setkey_enc(&ctx, key, key_bits));

    uint8_t block_in[128]  = { 0 };
    uint8_t block_rev[128] = { 0 };
    uint8_t block_cipher_rev[128];
    uint8_t block_cipher[128];
    memcpy(block_in, pt, pt_len);
    for (int i = 0; i < 128; ++i) {
        block_rev[i] = block_in[127 - i];
    }

    uint8_t data_unit[16] = { 0 };
    uint32_t tweak_addr = flash_address & ~0x7FU;
    data_unit[0] = (uint8_t)(tweak_addr);
    data_unit[1] = (uint8_t)(tweak_addr >> 8);
    data_unit[2] = (uint8_t)(tweak_addr >> 16);
    data_unit[3] = (uint8_t)(tweak_addr >> 24);

    TEST_ASSERT_EQUAL(0, mbedtls_aes_crypt_xts(&ctx, MBEDTLS_AES_ENCRYPT, 128,
                                               data_unit, block_rev, block_cipher_rev));
    for (int i = 0; i < 128; ++i) {
        block_cipher[i] = block_cipher_rev[127 - i];
    }
    memcpy(ct_out, block_cipher, pt_len);

    mbedtls_aes_xts_free(&ctx);
}

void km_verify_hmac_sha256(const uint8_t *key, size_t key_len,
                           const uint8_t *msg, size_t msg_len,
                           uint8_t mac_out[32])
{
    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_SIGN_MESSAGE);
    psa_set_key_algorithm(&attr, PSA_ALG_HMAC(PSA_ALG_SHA_256));
    psa_set_key_type(&attr, PSA_KEY_TYPE_HMAC);
    psa_set_key_bits(&attr, key_len * 8);

    psa_key_id_t key_id = 0;
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_import_key(&attr, key, key_len, &key_id));

    size_t mac_len = 0;
    psa_status_t status = psa_mac_compute(key_id,
                                          PSA_ALG_HMAC(PSA_ALG_SHA_256),
                                          msg, msg_len,
                                          mac_out, 32, &mac_len);
    psa_destroy_key(key_id);

    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    TEST_ASSERT_EQUAL(32, mac_len);
}
