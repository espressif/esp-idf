/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include "unity.h"
#include <string.h>
#include "utils/common.h"
#include "utils/includes.h"
#include "crypto/crypto.h"

#include "esp_timer.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/ecp.h"
#include "mbedtls/pk.h"
#include "test_utils.h"
#include "test_wpa_supplicant_common.h"

typedef struct crypto_bignum crypto_bignum;

static const uint8_t test_secp256r1_prime[32] = {
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

static const uint8_t test_p256_bignum_vals[][32] = {
    {
        0x00, 0x00, 0x00, 0x00, 0xde, 0xad, 0xbe, 0xef,
        0xca, 0xfe, 0xba, 0xbe, 0x88, 0x99, 0xaa, 0xbb,
        0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe,
        0x13, 0x57, 0x9b, 0xdf, 0x24, 0x68, 0xac, 0xe0
    },
    {
        0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0,
        0x0f, 0xed, 0xcb, 0xa9, 0x87, 0x65, 0x43, 0x21,
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    },
    {
        0xa5, 0x5a, 0xa5, 0x5a, 0xa5, 0x5a, 0xa5, 0x5a,
        0x5a, 0xa5, 0x5a, 0xa5, 0x5a, 0xa5, 0x5a, 0xa5,
        0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78,
        0x89, 0x9a, 0xab, 0xbc, 0xcd, 0xde, 0xef, 0xf0
    }
};

static const uint8_t test_p256_scalar_seeds[][32] = {
    {
        0xff, 0xff, 0xff, 0xff, 0xde, 0xad, 0xbe, 0xef,
        0xca, 0xfe, 0xba, 0xbe, 0x88, 0x99, 0xaa, 0xbb,
        0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe,
        0x13, 0x57, 0x9b, 0xdf, 0x24, 0x68, 0xac, 0xe0
    },
    {
        0xa5, 0x5a, 0xa5, 0x5a, 0xa5, 0x5a, 0xa5, 0x5a,
        0x5a, 0xa5, 0x5a, 0xa5, 0x5a, 0xa5, 0x5a, 0xa5,
        0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78,
        0x89, 0x9a, 0xab, 0xbc, 0xcd, 0xde, 0xef, 0xf0
    },
    {
        0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69, 0x78,
        0x87, 0x96, 0xa5, 0xb4, 0xc3, 0xd2, 0xe1, 0xf0,
        0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87,
        0x78, 0x69, 0x5a, 0x4b, 0x3c, 0x2d, 0x1e, 0x0f
    }
};

static const unsigned int test_p256_point_multipliers[] = { 7, 13 };
static const unsigned int test_small_exponents[] = { 0, 1, 2, 3 };

static int test_mbedtls_rng(void *ctx, unsigned char *buf, size_t len)
{
    (void) ctx;
    return os_get_random(buf, len) == 0 ? 0 : MBEDTLS_ERR_ECP_RANDOM_FAILED;
}

static void test_load_valid_p256_scalar(const mbedtls_ecp_group *grp,
                                        const uint8_t *seed, size_t seed_len,
                                        mbedtls_mpi *scalar)
{
    mbedtls_mpi range;

    mbedtls_mpi_init(&range);
    TEST_ASSERT_EQUAL(0, mbedtls_mpi_sub_int(&range, &grp->N, 1));
    TEST_ASSERT_EQUAL(0, mbedtls_mpi_read_binary(scalar, seed, seed_len));
    TEST_ASSERT_EQUAL(0, mbedtls_mpi_mod_mpi(scalar, scalar, &range));
    TEST_ASSERT_EQUAL(0, mbedtls_mpi_add_int(scalar, scalar, 1));
    mbedtls_mpi_free(&range);
}

static void test_make_p256_affine_point(mbedtls_ecp_group *grp,
                                        unsigned int multiplier,
                                        mbedtls_ecp_point *point)
{
    mbedtls_mpi k;

    mbedtls_mpi_init(&k);
    TEST_ASSERT_EQUAL(0, mbedtls_mpi_lset(&k, multiplier));
    TEST_ASSERT_EQUAL(0, mbedtls_ecp_mul(grp, point, &k, &grp->G,
                                         test_mbedtls_rng, NULL));
    TEST_ASSERT_EQUAL(0, mbedtls_mpi_cmp_int(&point->MBEDTLS_PRIVATE(Z), 1));
    mbedtls_mpi_free(&k);
}

static int test_legendre_reference(const mbedtls_mpi *a, const mbedtls_mpi *p)
{
    mbedtls_mpi a_mod, exp, res, one, pm1;
    int legendre = -2;

    mbedtls_mpi_init(&a_mod);
    mbedtls_mpi_init(&exp);
    mbedtls_mpi_init(&res);
    mbedtls_mpi_init(&one);
    mbedtls_mpi_init(&pm1);

    TEST_ASSERT_EQUAL(0, mbedtls_mpi_mod_mpi(&a_mod, a, p));
    if (mbedtls_mpi_cmp_int(&a_mod, 0) == 0) {
        legendre = 0;
        goto cleanup;
    }

    TEST_ASSERT_EQUAL(0, mbedtls_mpi_copy(&exp, p));
    TEST_ASSERT_EQUAL(0, mbedtls_mpi_sub_int(&exp, &exp, 1));
    TEST_ASSERT_EQUAL(0, mbedtls_mpi_shift_r(&exp, 1));
    TEST_ASSERT_EQUAL(0, mbedtls_mpi_exp_mod(&res, &a_mod, &exp, p, NULL));

    TEST_ASSERT_EQUAL(0, mbedtls_mpi_lset(&one, 1));
    if (mbedtls_mpi_cmp_mpi(&res, &one) == 0) {
        legendre = 1;
        goto cleanup;
    }

    TEST_ASSERT_EQUAL(0, mbedtls_mpi_copy(&pm1, p));
    TEST_ASSERT_EQUAL(0, mbedtls_mpi_sub_int(&pm1, &pm1, 1));
    if (mbedtls_mpi_cmp_mpi(&res, &pm1) == 0) {
        legendre = -1;
        goto cleanup;
    }

    TEST_FAIL_MESSAGE("Unexpected Legendre reference result");

cleanup:
    mbedtls_mpi_free(&a_mod);
    mbedtls_mpi_free(&exp);
    mbedtls_mpi_free(&res);
    mbedtls_mpi_free(&one);
    mbedtls_mpi_free(&pm1);
    return legendre;
}

static void test_print_crypto_timing(const char *label,
                                     int64_t generic_total_us, size_t generic_ops,
                                     int64_t api_total_us, size_t api_ops)
{
    long long generic_avg = generic_ops ? (long long)(generic_total_us / (int64_t) generic_ops) : 0;
    long long api_avg = api_ops ? (long long)(api_total_us / (int64_t) api_ops) : 0;

    printf("%s timing(us): generic_avg=%lld api_avg=%lld generic_total=%lld api_total=%lld ops=%u\n",
           label, generic_avg, api_avg,
           (long long) generic_total_us, (long long) api_total_us,
           (unsigned int) api_ops);
}

static int test_mbedtls_ecdh(const struct crypto_ec_key *key_own,
                             const struct crypto_ec_key *key_peer,
                             u8 *secret, size_t *secret_len)
{
    mbedtls_ecdh_context ctx;
    mbedtls_pk_context *own = (mbedtls_pk_context *) key_own;
    mbedtls_pk_context *peer = (mbedtls_pk_context *) key_peer;
    int ret = -1;

    mbedtls_ecdh_init(&ctx);

    if (mbedtls_ecdh_get_params(&ctx, mbedtls_pk_ec(*own),
                                MBEDTLS_ECDH_OURS) != 0) {
        goto out;
    }

    if (mbedtls_ecdh_get_params(&ctx, mbedtls_pk_ec(*peer),
                                MBEDTLS_ECDH_THEIRS) != 0) {
        goto out;
    }

    if (mbedtls_ecdh_calc_secret(&ctx, secret_len, secret, 66,
                                 test_mbedtls_rng, NULL) != 0) {
        goto out;
    }

    ret = 0;

out:
    mbedtls_ecdh_free(&ctx);
    return ret;
}

static int test_mbedtls_key_gen_p256(mbedtls_pk_context *kctx)
{
    mbedtls_pk_init(kctx);

    if (mbedtls_pk_setup(kctx,
                         mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY)) != 0) {
        return -1;
    }

    if (mbedtls_ecp_gen_key(MBEDTLS_ECP_DP_SECP256R1, mbedtls_pk_ec(*kctx),
                            test_mbedtls_rng, NULL) != 0) {
        mbedtls_pk_free(kctx);
        return -1;
    }

    return 0;
}

TEST_CASE("Test crypto lib bignum apis", "[wpa_crypto]")
{
    set_leak_threshold(300);
    {

        uint8_t buf[32], buf2[32];

        /* BN - Init & Deinit*/
        crypto_bignum *bn = crypto_bignum_init();
        crypto_bignum_deinit(bn, 1);

        /* BN - Binary to bignum & bignum to binary*/
        TEST_ASSERT(!os_get_random(buf, 32));

        bn = crypto_bignum_init_set(buf, 32);
        TEST_ASSERT_NOT_NULL(bn);

        TEST_ASSERT(crypto_bignum_to_bin(bn, buf2, 32, 0) == 32);

        TEST_ASSERT(!memcmp(buf, buf2, 32));
        crypto_bignum_deinit(bn, 1);

    }

    {   /** BN summation*/
        uint8_t buf1[32], buf2[32], buf3[32], buf4[32];
        crypto_bignum *bn1, *bn2, *sum;
        uint8_t  count;

        sum = crypto_bignum_init();

        for (count = 0; count < 32; count++) {
            buf1[count] = 0x11;
            buf2[count] = 0x22;
            buf3[count] = 0x33; //expected result
            buf4[count] = 0x0; //Calculated result
        }

        bn1 = crypto_bignum_init_set(buf1, 32);
        TEST_ASSERT_NOT_NULL(bn1);

        bn2 = crypto_bignum_init_set(buf2, 32);
        TEST_ASSERT_NOT_NULL(bn2);

        TEST_ASSERT(crypto_bignum_add(bn1, bn2, sum) == 0);

        TEST_ASSERT(crypto_bignum_to_bin(sum, buf4, 32, 0) == 32);

        TEST_ASSERT(!memcmp(buf3, buf4, 32));
        crypto_bignum_deinit(bn1, 1);
        crypto_bignum_deinit(bn2, 1);
        crypto_bignum_deinit(sum, 1);

    }

    {   /** BN mod*/
        uint8_t buf1[32], buf2[32], buf3[32], buf4[32];
        crypto_bignum *bn1, *bn2, *mod;
        uint8_t  count;

        mod = crypto_bignum_init();

        for (count = 0; count < 32; count++) {
            buf1[count] = 0x33;
            buf2[count] = 0x22;
            buf3[count] = 0x11; //expected result
            buf4[count] = 0x0; //Calculated result
        }

        bn1 = crypto_bignum_init_set(buf1, 32);
        TEST_ASSERT_NOT_NULL(bn1);

        bn2 = crypto_bignum_init_set(buf2, 32);
        TEST_ASSERT_NOT_NULL(bn2);

        TEST_ASSERT(crypto_bignum_mod(bn1, bn2, mod) == 0);

        TEST_ASSERT(crypto_bignum_to_bin(mod, buf4, 32, 0) == 32);

        TEST_ASSERT(!memcmp(buf3, buf4, 32));
        crypto_bignum_deinit(bn1, 1);
        crypto_bignum_deinit(bn2, 1);
        crypto_bignum_deinit(mod, 1);

    }

    {   /** BN sub*/
        uint8_t buf1[32], buf2[32], buf3[32], buf4[32];
        crypto_bignum *bn1, *bn2, *sub;
        uint8_t  count;

        sub = crypto_bignum_init();

        for (count = 0; count < 32; count++) {
            buf1[count] = 0x44;
            buf2[count] = 0x11;
            buf3[count] = 0x33; //expected result
            buf4[count] = 0x0; //Calculated result
        }

        bn1 = crypto_bignum_init_set(buf1, 32);
        TEST_ASSERT_NOT_NULL(bn1);

        bn2 = crypto_bignum_init_set(buf2, 32);
        TEST_ASSERT_NOT_NULL(bn2);

        TEST_ASSERT(crypto_bignum_sub(bn1, bn2, sub) == 0);

        TEST_ASSERT(crypto_bignum_to_bin(sub, buf4, 32, 0) == 32);

        TEST_ASSERT(!memcmp(buf3, buf4, 32));
        crypto_bignum_deinit(bn1, 1);
        crypto_bignum_deinit(bn2, 1);
        crypto_bignum_deinit(sub, 1);

    }

    {   /** BN div*/
        uint8_t buf1[32], buf2[32], buf3[32], buf4[32];
        crypto_bignum *bn1, *bn2, *div;
        uint8_t  count;

        div = crypto_bignum_init();

        for (count = 0; count < 32; count++) {
            buf1[count] = 0x44;
            buf2[count] = 0x22;
            buf3[count] = count ? 0 : 0x2; //expected result
            buf4[count] = 0x0; //Calculated result
        }

        bn1 = crypto_bignum_init_set(buf1, 32);
        TEST_ASSERT_NOT_NULL(bn1);

        bn2 = crypto_bignum_init_set(buf2, 32);
        TEST_ASSERT_NOT_NULL(bn2);

        TEST_ASSERT(crypto_bignum_div(bn1, bn2, div) == 0);

        TEST_ASSERT(crypto_bignum_to_bin(div, buf4, 32, 0) == 1);

        TEST_ASSERT(!memcmp(buf3, buf4, 1));
        crypto_bignum_deinit(bn1, 1);
        crypto_bignum_deinit(bn2, 1);
        crypto_bignum_deinit(div, 1);

    }

    {   /** BN mul mod*/
        uint8_t buf1[32], buf2[32], buf3[32], buf4[32], buf5[32];
        crypto_bignum *bn1, *bn2, *bn3, *mulmod;
        uint8_t  count;

        for (count = 0; count < 32; count++) {
            buf1[count] = 0x22;
            buf2[count] = 0x11;
            buf3[count] = (count < 4) ? 0x21 : 0;
            buf4[count] = (count < 4) ? 0x14 : 0;
            buf5[count] = 0;
        }
        mulmod = crypto_bignum_init();

        bn1 = crypto_bignum_init_set(buf1, 32);
        TEST_ASSERT_NOT_NULL(bn1);

        bn2 = crypto_bignum_init_set(buf2, 32);
        TEST_ASSERT_NOT_NULL(bn2);

        bn3 = crypto_bignum_init_set(buf3, 4);
        TEST_ASSERT_NOT_NULL(bn3);

        TEST_ASSERT(crypto_bignum_mulmod(bn1, bn2, bn3, mulmod) == 0);

        TEST_ASSERT(crypto_bignum_to_bin(mulmod, buf5, 32, 0) == 4);

        TEST_ASSERT(!memcmp(buf5, buf4, 4));
        crypto_bignum_deinit(bn1, 1);
        crypto_bignum_deinit(bn2, 1);
        crypto_bignum_deinit(bn3, 1);
        crypto_bignum_deinit(mulmod, 1);

    }

    {   /** BN mul mod on secp256r1 prime */
        uint8_t val[32];
        uint8_t one[32] = {0};
        crypto_bignum *bn1, *bn2, *bn3, *mulmod;

        one[0] = 1;
        os_memcpy(val, test_secp256r1_prime, sizeof(val));
        val[31]--;

        mulmod = crypto_bignum_init();
        TEST_ASSERT_NOT_NULL(mulmod);

        bn1 = crypto_bignum_init_set(val, sizeof(val));
        TEST_ASSERT_NOT_NULL(bn1);

        bn2 = crypto_bignum_init_set(val, sizeof(val));
        TEST_ASSERT_NOT_NULL(bn2);

        bn3 = crypto_bignum_init_set(test_secp256r1_prime,
                                     sizeof(test_secp256r1_prime));
        TEST_ASSERT_NOT_NULL(bn3);

        TEST_ASSERT(crypto_bignum_mulmod(bn1, bn2, bn3, mulmod) == 0);
        TEST_ASSERT(crypto_bignum_to_bin(mulmod, val, sizeof(val), 0) == 1);
        TEST_ASSERT_EQUAL_UINT8_ARRAY(one, val, 1);

        crypto_bignum_deinit(bn1, 1);
        crypto_bignum_deinit(bn2, 1);
        crypto_bignum_deinit(bn3, 1);
        crypto_bignum_deinit(mulmod, 1);
    }

    {   /** BN exp mod*/
        uint8_t buf1[32], buf2[32], buf3[32], buf4[32], buf5[32];

        crypto_bignum *bn1, *bn2, *bn3, *expmod;
        uint8_t  count;

        expmod = crypto_bignum_init();

        for (count = 0; count < 32; count++) {
            buf1[count] = 0x22;
            buf2[count] = (count >= 30) ? 0x11 : 0;
            buf3[count] = (count >= 31) ? 0xE9 : 0;
            buf4[count] = count ? 0 : 0x62;
            buf5[count] = 0;
        }

        bn1 = crypto_bignum_init_set(buf1, 32);
        TEST_ASSERT_NOT_NULL(bn1);

        bn2 = crypto_bignum_init_set(buf2, 32);
        TEST_ASSERT_NOT_NULL(bn2);

        bn3 = crypto_bignum_init_set(buf3, 32);
        TEST_ASSERT_NOT_NULL(bn3);

        TEST_ASSERT(crypto_bignum_exptmod(bn1, bn2, bn3, expmod) == 0);

        TEST_ASSERT(crypto_bignum_to_bin(expmod, buf5, 32, 0) == 1);

        TEST_ASSERT(!memcmp(buf5, buf4, 1));
        crypto_bignum_deinit(bn1, 1);
        crypto_bignum_deinit(bn2, 1);
        crypto_bignum_deinit(bn3, 1);
        crypto_bignum_deinit(expmod, 1);

    }

    {   /** BN Legendre symbol test*/
        uint8_t buf1[32], buf2[32];
        crypto_bignum *bn1, *bn2;

        buf1[0] = 0xf;
        buf2[0] = 0x11;

        bn1 = crypto_bignum_init_set(buf1, 1);
        TEST_ASSERT_NOT_NULL(bn1);

        bn2 = crypto_bignum_init_set(buf2, 1);
        TEST_ASSERT_NOT_NULL(bn2);

        TEST_ASSERT(crypto_bignum_legendre(bn1, bn2) == 1);

        crypto_bignum_deinit(bn1, 1);
        buf1[0] = 0xa;
        bn1 = crypto_bignum_init_set(buf1, 1);
        TEST_ASSERT_NOT_NULL(bn1);

        TEST_ASSERT(crypto_bignum_legendre(bn1, bn2) == -1);

        crypto_bignum_deinit(bn1, 1);
        buf1[0] = 0x11;
        bn1 = crypto_bignum_init_set(buf1, 1);
        TEST_ASSERT_NOT_NULL(bn1);

        TEST_ASSERT(crypto_bignum_legendre(bn1, bn2) == 0);

        crypto_bignum_deinit(bn1, 1);
        crypto_bignum_deinit(bn2, 1);

    }

    {   /** BN Legendre symbol test on secp256r1 prime */
        uint8_t val[32] = {0};
        crypto_bignum *bn_val, *bn_p;

        bn_p = crypto_bignum_init_set(test_secp256r1_prime,
                                      sizeof(test_secp256r1_prime));
        TEST_ASSERT_NOT_NULL(bn_p);

        val[31] = 1;
        bn_val = crypto_bignum_init_set(val, sizeof(val));
        TEST_ASSERT_NOT_NULL(bn_val);
        TEST_ASSERT(crypto_bignum_legendre(bn_val, bn_p) == 1);
        crypto_bignum_deinit(bn_val, 1);

        os_memset(val, 0, sizeof(val));
        val[31] = 3;
        bn_val = crypto_bignum_init_set(val, sizeof(val));
        TEST_ASSERT_NOT_NULL(bn_val);
        TEST_ASSERT(crypto_bignum_legendre(bn_val, bn_p) == -1);
        crypto_bignum_deinit(bn_val, 1);

        os_memset(val, 0, sizeof(val));
        bn_val = crypto_bignum_init_set(val, sizeof(val));
        TEST_ASSERT_NOT_NULL(bn_val);
        TEST_ASSERT(crypto_bignum_legendre(bn_val, bn_p) == 0);
        crypto_bignum_deinit(bn_val, 1);

        crypto_bignum_deinit(bn_p, 1);
    }
}

TEST_CASE("Test secp256r1 fast bignum paths against mbedtls reference", "[wpa_crypto]")
{
    crypto_bignum *bn_p;
    int i;

    set_leak_threshold(620);

    bn_p = crypto_bignum_init_set(test_secp256r1_prime,
                                  sizeof(test_secp256r1_prime));
    TEST_ASSERT_NOT_NULL(bn_p);

    for (i = 0; i < ARRAY_SIZE(test_p256_bignum_vals); i++) {
        crypto_bignum *bn_a = crypto_bignum_init_set(test_p256_bignum_vals[i], sizeof(test_p256_bignum_vals[i]));
        crypto_bignum *bn_b = crypto_bignum_init_set(
                                  test_p256_bignum_vals[(i + 1) % ARRAY_SIZE(test_p256_bignum_vals)],
                                  sizeof(test_p256_bignum_vals[0]));
        crypto_bignum *bn_mul = crypto_bignum_init();
        mbedtls_mpi ref_mul;
        int ref_legendre;

        TEST_ASSERT_NOT_NULL(bn_a);
        TEST_ASSERT_NOT_NULL(bn_b);
        TEST_ASSERT_NOT_NULL(bn_mul);

        mbedtls_mpi_init(&ref_mul);
        TEST_ASSERT_EQUAL(0, mbedtls_mpi_mul_mpi(&ref_mul,
                                                 (const mbedtls_mpi *) bn_a,
                                                 (const mbedtls_mpi *) bn_b));
        TEST_ASSERT_EQUAL(0, mbedtls_mpi_mod_mpi(&ref_mul, &ref_mul,
                                                 (const mbedtls_mpi *) bn_p));

        TEST_ASSERT_EQUAL(0, crypto_bignum_mulmod(bn_a, bn_b, bn_p, bn_mul));
        TEST_ASSERT_EQUAL(0, mbedtls_mpi_cmp_mpi((const mbedtls_mpi *) bn_mul,
                                                 &ref_mul));

        ref_legendre = test_legendre_reference((const mbedtls_mpi *) bn_a,
                                               (const mbedtls_mpi *) bn_p);
        TEST_ASSERT_EQUAL(ref_legendre, crypto_bignum_legendre(bn_a, bn_p));

        mbedtls_mpi_free(&ref_mul);
        crypto_bignum_deinit(bn_a, 1);
        crypto_bignum_deinit(bn_b, 1);
        crypto_bignum_deinit(bn_mul, 1);
    }

    crypto_bignum_deinit(bn_p, 1);
}

/*
 * Conversion macros for embedded constants:
 * build lists of mbedtls_mpi_uint's from lists of unsigned char's grouped by 8, 4 or 2
 */
#if defined(MBEDTLS_HAVE_INT32)

#define BYTES_TO_T_UINT_4( a, b, c, d )             \
    ( (mbedtls_mpi_uint) a <<  0 ) |                          \
    ( (mbedtls_mpi_uint) b <<  8 ) |                          \
    ( (mbedtls_mpi_uint) c << 16 ) |                          \
    ( (mbedtls_mpi_uint) d << 24 )

#define BYTES_TO_T_UINT_2( a, b )                   \
    BYTES_TO_T_UINT_4( a, b, 0, 0 )

#define BYTES_TO_T_UINT_8( a, b, c, d, e, f, g, h ) \
    BYTES_TO_T_UINT_4( a, b, c, d ),                \
    BYTES_TO_T_UINT_4( e, f, g, h )

#else /* 64-bits */

#define BYTES_TO_T_UINT_8( a, b, c, d, e, f, g, h ) \
    ( (mbedtls_mpi_uint) a <<  0 ) |                          \
    ( (mbedtls_mpi_uint) b <<  8 ) |                          \
    ( (mbedtls_mpi_uint) c << 16 ) |                          \
    ( (mbedtls_mpi_uint) d << 24 ) |                          \
    ( (mbedtls_mpi_uint) e << 32 ) |                          \
    ( (mbedtls_mpi_uint) f << 40 ) |                          \
    ( (mbedtls_mpi_uint) g << 48 ) |                          \
    ( (mbedtls_mpi_uint) h << 56 )

#define BYTES_TO_T_UINT_4( a, b, c, d )             \
    BYTES_TO_T_UINT_8( a, b, c, d, 0, 0, 0, 0 )

#define BYTES_TO_T_UINT_2( a, b )                   \
    BYTES_TO_T_UINT_8( a, b, 0, 0, 0, 0, 0, 0 )

#endif /* bits in mbedtls_mpi_uint */

/*
 * Create an MPI from embedded constants
 * (assumes len is an exact multiple of sizeof mbedtls_mpi_uint)
 * Allocate a new memory as well so that it can be freed.
 */
static inline void ecp_mpi_load(mbedtls_mpi *X, const mbedtls_mpi_uint *p, size_t len)
{
    X->MBEDTLS_PRIVATE(s) = 1;
    X->MBEDTLS_PRIVATE(n) = len / sizeof(mbedtls_mpi_uint);
    X->MBEDTLS_PRIVATE(p) = os_zalloc(len);
    memcpy(X->MBEDTLS_PRIVATE(p), (void *)p, len);
}

TEST_CASE("Test crypto lib ECC apis", "[wpa_crypto]")
{
    set_leak_threshold(620);

    static const mbedtls_mpi_uint secp256r1_gx[] = {
        BYTES_TO_T_UINT_8(0x96, 0xC2, 0x98, 0xD8, 0x45, 0x39, 0xA1, 0xF4),
        BYTES_TO_T_UINT_8(0xA0, 0x33, 0xEB, 0x2D, 0x81, 0x7D, 0x03, 0x77),
        BYTES_TO_T_UINT_8(0xF2, 0x40, 0xA4, 0x63, 0xE5, 0xE6, 0xBC, 0xF8),
        BYTES_TO_T_UINT_8(0x47, 0x42, 0x2C, 0xE1, 0xF2, 0xD1, 0x17, 0x6B),
    };
    static const mbedtls_mpi_uint secp256r1_gy[] = {
        BYTES_TO_T_UINT_8(0xF5, 0x51, 0xBF, 0x37, 0x68, 0x40, 0xB6, 0xCB),
        BYTES_TO_T_UINT_8(0xCE, 0x5E, 0x31, 0x6B, 0x57, 0x33, 0xCE, 0x2B),
        BYTES_TO_T_UINT_8(0x16, 0x9E, 0x0F, 0x7C, 0x4A, 0xEB, 0xE7, 0x8E),
        BYTES_TO_T_UINT_8(0x9B, 0x7F, 0x1A, 0xFE, 0xE2, 0x42, 0xE3, 0x4F),
    };

    {
        /* Check init and deinit APIs*/
        struct crypto_ec *e = crypto_ec_init(19);
        struct crypto_ec_point *pt = crypto_ec_point_init(e);
        crypto_ec_point_deinit(pt, 1);
        crypto_ec_deinit(e);
    }

    {
        uint8_t pt1[64], pt2[64];
        struct crypto_ec *e = crypto_ec_init(19);
        struct crypto_ec_point *p;

        TEST_ASSERT_NOT_NULL(e);

        /* Note this is just testing coversion & not whether point is
         * in the  group or not*/
        TEST_ASSERT(!os_get_random(pt1, 64));

        p = crypto_ec_point_from_bin(e, pt1);

        TEST_ASSERT(crypto_ec_prime_len(e) == 32);

        TEST_ASSERT(crypto_ec_point_to_bin(e, p, pt2, &pt2[32]) == 0);
        TEST_ASSERT(!memcmp(pt1, pt2, sizeof(pt1)));

        crypto_ec_point_deinit(p, 1);
        crypto_ec_deinit(e);
    }
    {
        /* Check addition and multiplication APIs
         * yield the same answer.
         */
        struct crypto_ec *e = crypto_ec_init(19);

        struct crypto_ec_point *p = crypto_ec_point_init(e);
        struct crypto_ec_point *q = crypto_ec_point_init(e);
        struct crypto_ec_point *r = crypto_ec_point_init(e);
        mbedtls_mpi num;

        TEST_ASSERT_NOT_NULL(e);
        TEST_ASSERT_NOT_NULL(p);
        TEST_ASSERT_NOT_NULL(q);
        TEST_ASSERT_NOT_NULL(r);

        mbedtls_mpi_init(&num);
        mbedtls_mpi_lset(&num, 3);

        ecp_mpi_load(& ((mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(X), secp256r1_gx, sizeof(secp256r1_gx));
        ecp_mpi_load(& ((mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(Y), secp256r1_gy, sizeof(secp256r1_gy));

        mbedtls_mpi_lset((&((mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(Z)), 1);

        TEST_ASSERT(crypto_ec_point_mul(e, p, (crypto_bignum *) &num, q) == 0); //q = 3p

        TEST_ASSERT(crypto_ec_point_add(e, p, p, r) == 0);
        TEST_ASSERT(crypto_ec_point_add(e, p, r, r) == 0);

        TEST_ASSERT(crypto_ec_point_cmp(e, q, r) == 0);

        mbedtls_mpi_free(&num);
        crypto_ec_point_deinit(p, 1);
        crypto_ec_point_deinit(q, 1);
        crypto_ec_point_deinit(r, 1);
        crypto_ec_deinit(e);

    }

    {
        /* Generate a point using generator and take its inverse
         * Check that adding point to inverse yields identity
         */
        struct crypto_ec *e = crypto_ec_init(19);

        struct crypto_ec_point *p = crypto_ec_point_init(e);
        struct crypto_ec_point *q = crypto_ec_point_init(e);
        struct crypto_ec_point *r = crypto_ec_point_init(e);
        mbedtls_mpi num;

        TEST_ASSERT_NOT_NULL(e);
        TEST_ASSERT_NOT_NULL(p);
        TEST_ASSERT_NOT_NULL(q);
        TEST_ASSERT_NOT_NULL(r);

        mbedtls_mpi_init(&num);
        mbedtls_mpi_lset(&num, 100);

        ecp_mpi_load(& ((mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(X), secp256r1_gx, sizeof(secp256r1_gx));
        ecp_mpi_load(& ((mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(Y), secp256r1_gy, sizeof(secp256r1_gy));

        mbedtls_mpi_lset((&((mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(Z)), 1);

        TEST_ASSERT(crypto_ec_point_mul(e, p, (crypto_bignum *) &num, q) == 0);
        TEST_ASSERT(crypto_ec_point_mul(e, p, (crypto_bignum *) &num, r) == 0);

        TEST_ASSERT(crypto_ec_point_invert(e, r) == 0);
        TEST_ASSERT(crypto_ec_point_add(e, q, r, r) == 0);

        TEST_ASSERT(crypto_ec_point_is_at_infinity(e, r));

        mbedtls_mpi_free(&num);
        crypto_ec_point_deinit(p, 1);
        crypto_ec_point_deinit(q, 1);
        crypto_ec_point_deinit(r, 1);
        crypto_ec_deinit(e);

    }
    {
        /* Check y_sqr calculations and other dependent APIs */

        struct crypto_ec *e = crypto_ec_init(19);

        struct crypto_ec_point *p = crypto_ec_point_init(e);
        struct crypto_ec_point *q = crypto_ec_point_init(e);
        mbedtls_mpi num;

        TEST_ASSERT_NOT_NULL(e);
        TEST_ASSERT_NOT_NULL(p);
        TEST_ASSERT_NOT_NULL(q);

        mbedtls_mpi_init(&num);
        mbedtls_mpi_lset(&num, 50);

        ecp_mpi_load(& ((mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(X), secp256r1_gx, sizeof(secp256r1_gx));
        ecp_mpi_load(& ((mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(Y), secp256r1_gy, sizeof(secp256r1_gy));

        mbedtls_mpi_lset((&((mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(Z)), 1);

        /* Generator should always be on the curve*/
        TEST_ASSERT(crypto_ec_point_is_on_curve(e, p));

        /* Any point generated using generated should also be on the same curve*/
        TEST_ASSERT(crypto_ec_point_mul(e, p, (crypto_bignum *) &num, q) == 0);
        TEST_ASSERT(crypto_ec_point_is_on_curve(e, q));

        mbedtls_mpi_free(&num);
        crypto_ec_point_deinit(p, 1);
        crypto_ec_point_deinit(q, 1);
        crypto_ec_deinit(e);

    }

    {
        /* crypto_ec_point_solve_y_coord APIs*/

        struct crypto_ec *e = crypto_ec_init(19);

        struct crypto_ec_point *p = crypto_ec_point_init(e);
        struct crypto_ec_point *q = crypto_ec_point_init(e);
        struct crypto_ec_point *r = crypto_ec_point_init(e);
        mbedtls_mpi num;

        TEST_ASSERT_NOT_NULL(e);
        TEST_ASSERT_NOT_NULL(p);
        TEST_ASSERT_NOT_NULL(q);
        TEST_ASSERT_NOT_NULL(r);

        mbedtls_mpi_init(&num);
        mbedtls_mpi_lset(&num, 50);

        ecp_mpi_load(& ((mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(X), secp256r1_gx, sizeof(secp256r1_gx));
        ecp_mpi_load(& ((mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(Y), secp256r1_gy, sizeof(secp256r1_gy));

        mbedtls_mpi_lset((&((mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(Z)), 1);

        mbedtls_mpi_copy(&((mbedtls_ecp_point *)q)->MBEDTLS_PRIVATE(X), &((mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(X));
        mbedtls_mpi_copy(&((mbedtls_ecp_point *)r)->MBEDTLS_PRIVATE(X), &((mbedtls_ecp_point *)p)->MBEDTLS_PRIVATE(X));

        mbedtls_mpi_lset((&((mbedtls_ecp_point *)q)->MBEDTLS_PRIVATE(Z)), 1);
        mbedtls_mpi_lset((&((mbedtls_ecp_point *)r)->MBEDTLS_PRIVATE(Z)), 1);

        TEST_ASSERT(crypto_ec_point_solve_y_coord(e, q, (crypto_bignum *) & ((mbedtls_ecp_point *)q)->MBEDTLS_PRIVATE(X), 0) == 0);
        TEST_ASSERT(crypto_ec_point_is_on_curve(e, q));

        TEST_ASSERT(crypto_ec_point_solve_y_coord(e, r, (crypto_bignum *) & ((mbedtls_ecp_point *)q)->MBEDTLS_PRIVATE(X), 1) == 0);
        TEST_ASSERT(crypto_ec_point_is_on_curve(e, r));

        TEST_ASSERT((crypto_ec_point_cmp(e, p, q) == 0) || (crypto_ec_point_cmp(e, p, r) == 0));

        /* The two roots should be inverse of one another*/
        TEST_ASSERT(crypto_ec_point_add(e, q, r, r) == 0);
        TEST_ASSERT(crypto_ec_point_is_at_infinity(e, r));

        mbedtls_mpi_free(&num);
        crypto_ec_point_deinit(p, 1);
        crypto_ec_point_deinit(q, 1);
        crypto_ec_point_deinit(r, 1);
        crypto_ec_deinit(e);

    }

}

TEST_CASE("Test secp256r1 point multiply against mbedtls reference", "[wpa_crypto]")
{
    struct crypto_ec *e;
    struct crypto_ec_point *p = NULL;
    struct crypto_ec_point *res = NULL;
    mbedtls_ecp_point ref;
    int i, j;

    set_leak_threshold(620);

    e = crypto_ec_init(19);
    TEST_ASSERT_NOT_NULL(e);

    p = crypto_ec_point_init(e);
    TEST_ASSERT_NOT_NULL(p);
    res = crypto_ec_point_init(e);
    TEST_ASSERT_NOT_NULL(res);
    mbedtls_ecp_point_init(&ref);

    for (i = 0; i < ARRAY_SIZE(test_p256_point_multipliers); i++) {
        test_make_p256_affine_point((mbedtls_ecp_group *) e,
                                    test_p256_point_multipliers[i],
                                    (mbedtls_ecp_point *) p);

        for (j = 0; j < ARRAY_SIZE(test_p256_scalar_seeds); j++) {
            mbedtls_mpi scalar;

            mbedtls_mpi_init(&scalar);
            test_load_valid_p256_scalar((const mbedtls_ecp_group *) e,
                                        test_p256_scalar_seeds[j],
                                        sizeof(test_p256_scalar_seeds[j]),
                                        &scalar);

            TEST_ASSERT_EQUAL(0, crypto_ec_point_mul(e, p,
                                                     (struct crypto_bignum *) &scalar,
                                                     res));
            TEST_ASSERT_EQUAL(0, mbedtls_ecp_mul((mbedtls_ecp_group *) e,
                                                 &ref, &scalar,
                                                 (const mbedtls_ecp_point *) p,
                                                 test_mbedtls_rng, NULL));
            TEST_ASSERT_EQUAL(0, crypto_ec_point_cmp(e, res,
                                                     (const struct crypto_ec_point *) &ref));

            mbedtls_mpi_free(&scalar);
        }
    }

    mbedtls_ecp_point_free(&ref);
    crypto_ec_point_deinit(p, 1);
    crypto_ec_point_deinit(res, 1);
    crypto_ec_deinit(e);
}

TEST_CASE("Measure secp256r1 bignum API timings against mbedtls reference", "[wpa_crypto]")
{
    const unsigned int loops = 64;
    crypto_bignum *bn_p;
    int64_t generic_total_us = 0;
    int64_t api_total_us = 0;
    size_t ops = 0;
    int i, loop;

    set_leak_threshold(700);

    bn_p = crypto_bignum_init_set(test_secp256r1_prime,
                                  sizeof(test_secp256r1_prime));
    TEST_ASSERT_NOT_NULL(bn_p);

    for (i = 0; i < ARRAY_SIZE(test_p256_bignum_vals); i++) {
        crypto_bignum *bn_a = crypto_bignum_init_set(test_p256_bignum_vals[i],
                                                     sizeof(test_p256_bignum_vals[i]));
        crypto_bignum *bn_b = crypto_bignum_init_set(
                                  test_p256_bignum_vals[(i + 1) % ARRAY_SIZE(test_p256_bignum_vals)],
                                  sizeof(test_p256_bignum_vals[0]));
        crypto_bignum *bn_mul = crypto_bignum_init();
        mbedtls_mpi ref_mul;
        int ref_legendre;

        TEST_ASSERT_NOT_NULL(bn_a);
        TEST_ASSERT_NOT_NULL(bn_b);
        TEST_ASSERT_NOT_NULL(bn_mul);

        mbedtls_mpi_init(&ref_mul);
        TEST_ASSERT_EQUAL(0, mbedtls_mpi_mul_mpi(&ref_mul,
                                                 (const mbedtls_mpi *) bn_a,
                                                 (const mbedtls_mpi *) bn_b));
        TEST_ASSERT_EQUAL(0, mbedtls_mpi_mod_mpi(&ref_mul, &ref_mul,
                                                 (const mbedtls_mpi *) bn_p));
        TEST_ASSERT_EQUAL(0, crypto_bignum_mulmod(bn_a, bn_b, bn_p, bn_mul));
        TEST_ASSERT_EQUAL(0, mbedtls_mpi_cmp_mpi((const mbedtls_mpi *) bn_mul,
                                                 &ref_mul));
        ref_legendre = test_legendre_reference((const mbedtls_mpi *) bn_a,
                                               (const mbedtls_mpi *) bn_p);
        TEST_ASSERT_EQUAL(ref_legendre, crypto_bignum_legendre(bn_a, bn_p));

        for (loop = 0; loop < loops; loop++) {
            int64_t start_us = esp_timer_get_time();

            TEST_ASSERT_EQUAL(0, mbedtls_mpi_mul_mpi(&ref_mul,
                                                     (const mbedtls_mpi *) bn_a,
                                                     (const mbedtls_mpi *) bn_b));
            TEST_ASSERT_EQUAL(0, mbedtls_mpi_mod_mpi(&ref_mul, &ref_mul,
                                                     (const mbedtls_mpi *) bn_p));
            generic_total_us += esp_timer_get_time() - start_us;
        }

        for (loop = 0; loop < loops; loop++) {
            int64_t start_us = esp_timer_get_time();

            TEST_ASSERT_EQUAL(0, crypto_bignum_mulmod(bn_a, bn_b, bn_p, bn_mul));
            api_total_us += esp_timer_get_time() - start_us;
        }

        ops += loops;
        mbedtls_mpi_free(&ref_mul);
        crypto_bignum_deinit(bn_a, 1);
        crypto_bignum_deinit(bn_b, 1);
        crypto_bignum_deinit(bn_mul, 1);
    }

    test_print_crypto_timing("secp256r1 mulmod", generic_total_us, ops,
                             api_total_us, ops);

    generic_total_us = 0;
    api_total_us = 0;
    ops = 0;

    for (i = 0; i < ARRAY_SIZE(test_small_exponents); i++) {
        crypto_bignum *bn_exp = crypto_bignum_init_uint(test_small_exponents[i]);
        char label[48];

        TEST_ASSERT_NOT_NULL(bn_exp);

        generic_total_us = 0;
        api_total_us = 0;
        ops = 0;

        for (loop = 0; loop < ARRAY_SIZE(test_p256_bignum_vals); loop++) {
            crypto_bignum *bn_a = crypto_bignum_init_set(test_p256_bignum_vals[loop],
                                                         sizeof(test_p256_bignum_vals[loop]));
            crypto_bignum *bn_res = crypto_bignum_init();
            mbedtls_mpi ref_res;
            int iter;

            TEST_ASSERT_NOT_NULL(bn_a);
            TEST_ASSERT_NOT_NULL(bn_res);

            mbedtls_mpi_init(&ref_res);
            TEST_ASSERT_EQUAL(0, mbedtls_mpi_exp_mod(&ref_res,
                                                     (const mbedtls_mpi *) bn_a,
                                                     (const mbedtls_mpi *) bn_exp,
                                                     (const mbedtls_mpi *) bn_p,
                                                     NULL));
            TEST_ASSERT_EQUAL(0, crypto_bignum_exptmod(bn_a, bn_exp, bn_p,
                                                       bn_res));
            TEST_ASSERT_EQUAL(0, mbedtls_mpi_cmp_mpi((const mbedtls_mpi *) bn_res,
                                                     &ref_res));

            for (iter = 0; iter < loops; iter++) {
                int64_t start_us = esp_timer_get_time();

                TEST_ASSERT_EQUAL(0, mbedtls_mpi_exp_mod(&ref_res,
                                                         (const mbedtls_mpi *) bn_a,
                                                         (const mbedtls_mpi *) bn_exp,
                                                         (const mbedtls_mpi *) bn_p,
                                                         NULL));
                generic_total_us += esp_timer_get_time() - start_us;
            }

            for (iter = 0; iter < loops; iter++) {
                int64_t start_us = esp_timer_get_time();

                TEST_ASSERT_EQUAL(0, crypto_bignum_exptmod(bn_a, bn_exp, bn_p,
                                                           bn_res));
                api_total_us += esp_timer_get_time() - start_us;
            }

            ops += loops;
            mbedtls_mpi_free(&ref_res);
            crypto_bignum_deinit(bn_a, 1);
            crypto_bignum_deinit(bn_res, 1);
        }

        snprintf(label, sizeof(label), "secp256r1 exptmod e=%u",
                 test_small_exponents[i]);
        test_print_crypto_timing(label, generic_total_us, ops,
                                 api_total_us, ops);
        crypto_bignum_deinit(bn_exp, 1);
    }

    generic_total_us = 0;
    api_total_us = 0;
    ops = 0;

    for (i = 0; i < ARRAY_SIZE(test_p256_bignum_vals); i++) {
        crypto_bignum *bn_a = crypto_bignum_init_set(test_p256_bignum_vals[i],
                                                     sizeof(test_p256_bignum_vals[i]));
        TEST_ASSERT_NOT_NULL(bn_a);

        TEST_ASSERT_EQUAL(test_legendre_reference((const mbedtls_mpi *) bn_a,
                                                  (const mbedtls_mpi *) bn_p),
                          crypto_bignum_legendre(bn_a, bn_p));

        for (loop = 0; loop < loops; loop++) {
            int64_t start_us = esp_timer_get_time();

            (void) test_legendre_reference((const mbedtls_mpi *) bn_a,
                                           (const mbedtls_mpi *) bn_p);
            generic_total_us += esp_timer_get_time() - start_us;
        }

        for (loop = 0; loop < loops; loop++) {
            int64_t start_us = esp_timer_get_time();

            (void) crypto_bignum_legendre(bn_a, bn_p);
            api_total_us += esp_timer_get_time() - start_us;
        }

        ops += loops;
        crypto_bignum_deinit(bn_a, 1);
    }

    test_print_crypto_timing("secp256r1 legendre", generic_total_us, ops,
                             api_total_us, ops);

    crypto_bignum_deinit(bn_p, 1);
}

TEST_CASE("Measure secp256r1 EC API timings against mbedtls reference", "[wpa_crypto]")
{
    const unsigned int point_mul_loops = 4;
    const unsigned int key_gen_loops = 4;
    const unsigned int ecdh_loops = 4;
    struct crypto_ec *e;
    struct crypto_ec_point *p = NULL;
    struct crypto_ec_point *res = NULL;
    mbedtls_ecp_point ref;
    int64_t generic_total_us = 0;
    int64_t api_total_us = 0;
    size_t ops = 0;
    int i, j, loop;

    set_leak_threshold(900);

    e = crypto_ec_init(19);
    TEST_ASSERT_NOT_NULL(e);

    p = crypto_ec_point_init(e);
    TEST_ASSERT_NOT_NULL(p);
    res = crypto_ec_point_init(e);
    TEST_ASSERT_NOT_NULL(res);
    mbedtls_ecp_point_init(&ref);

    for (i = 0; i < ARRAY_SIZE(test_p256_point_multipliers); i++) {
        test_make_p256_affine_point((mbedtls_ecp_group *) e,
                                    test_p256_point_multipliers[i],
                                    (mbedtls_ecp_point *) p);

        for (j = 0; j < ARRAY_SIZE(test_p256_scalar_seeds); j++) {
            mbedtls_mpi scalar;

            mbedtls_mpi_init(&scalar);
            test_load_valid_p256_scalar((const mbedtls_ecp_group *) e,
                                        test_p256_scalar_seeds[j],
                                        sizeof(test_p256_scalar_seeds[j]),
                                        &scalar);

            TEST_ASSERT_EQUAL(0, crypto_ec_point_mul(e, p,
                                                     (struct crypto_bignum *) &scalar,
                                                     res));
            TEST_ASSERT_EQUAL(0, mbedtls_ecp_mul((mbedtls_ecp_group *) e,
                                                 &ref, &scalar,
                                                 (const mbedtls_ecp_point *) p,
                                                 test_mbedtls_rng, NULL));
            TEST_ASSERT_EQUAL(0, crypto_ec_point_cmp(e, res,
                                                     (const struct crypto_ec_point *) &ref));

            for (loop = 0; loop < point_mul_loops; loop++) {
                int64_t start_us = esp_timer_get_time();

                TEST_ASSERT_EQUAL(0, mbedtls_ecp_mul((mbedtls_ecp_group *) e,
                                                     &ref, &scalar,
                                                     (const mbedtls_ecp_point *) p,
                                                     test_mbedtls_rng, NULL));
                generic_total_us += esp_timer_get_time() - start_us;
            }

            for (loop = 0; loop < point_mul_loops; loop++) {
                int64_t start_us = esp_timer_get_time();

                TEST_ASSERT_EQUAL(0, crypto_ec_point_mul(e, p,
                                                         (struct crypto_bignum *) &scalar,
                                                         res));
                api_total_us += esp_timer_get_time() - start_us;
            }

            ops += point_mul_loops;
            mbedtls_mpi_free(&scalar);
        }
    }

    test_print_crypto_timing("secp256r1 point_mul", generic_total_us, ops,
                             api_total_us, ops);

    generic_total_us = 0;
    api_total_us = 0;

    for (loop = 0; loop < key_gen_loops; loop++) {
        mbedtls_pk_context kctx;
        int64_t start_us = esp_timer_get_time();

        TEST_ASSERT_EQUAL(0, test_mbedtls_key_gen_p256(&kctx));
        generic_total_us += esp_timer_get_time() - start_us;
        mbedtls_pk_free(&kctx);
    }

    for (loop = 0; loop < key_gen_loops; loop++) {
        struct crypto_ec_key *key;
        int64_t start_us = esp_timer_get_time();

        key = (struct crypto_ec_key *) crypto_ec_gen_keypair(19);
        TEST_ASSERT_NOT_NULL(key);
        api_total_us += esp_timer_get_time() - start_us;
        crypto_ec_key_deinit(key);
    }

    test_print_crypto_timing("secp256r1 key_gen", generic_total_us, key_gen_loops,
                             api_total_us, key_gen_loops);

    {
        struct crypto_ec_key *key_own =
            (struct crypto_ec_key *) crypto_ec_gen_keypair(19);
        struct crypto_ec_key *key_peer =
            (struct crypto_ec_key *) crypto_ec_gen_keypair(19);
        u8 secret_generic[66];
        u8 secret_api[66];
        size_t secret_generic_len = 0;
        size_t secret_api_len = 0;

        TEST_ASSERT_NOT_NULL(key_own);
        TEST_ASSERT_NOT_NULL(key_peer);
        TEST_ASSERT_EQUAL(0, test_mbedtls_ecdh(key_own, key_peer,
                                               secret_generic,
                                               &secret_generic_len));
        TEST_ASSERT_EQUAL(0, crypto_ecdh((struct crypto_key *) key_own,
                                         (struct crypto_key *) key_peer,
                                         secret_api, &secret_api_len));
        TEST_ASSERT_EQUAL(secret_generic_len, secret_api_len);
        TEST_ASSERT_EQUAL_MEMORY(secret_generic, secret_api, secret_api_len);

        generic_total_us = 0;
        api_total_us = 0;

        for (loop = 0; loop < ecdh_loops; loop++) {
            int64_t start_us = esp_timer_get_time();
            size_t secret_len = 0;

            TEST_ASSERT_EQUAL(0, test_mbedtls_ecdh(key_own, key_peer,
                                                   secret_generic,
                                                   &secret_len));
            generic_total_us += esp_timer_get_time() - start_us;
        }

        for (loop = 0; loop < ecdh_loops; loop++) {
            int64_t start_us = esp_timer_get_time();
            size_t secret_len = 0;

            TEST_ASSERT_EQUAL(0, crypto_ecdh((struct crypto_key *) key_own,
                                             (struct crypto_key *) key_peer,
                                             secret_api, &secret_len));
            api_total_us += esp_timer_get_time() - start_us;
        }

        test_print_crypto_timing("secp256r1 ecdh", generic_total_us, ecdh_loops,
                                 api_total_us, ecdh_loops);

        crypto_ec_key_deinit(key_own);
        crypto_ec_key_deinit(key_peer);
    }

    mbedtls_ecp_point_free(&ref);
    crypto_ec_point_deinit(p, 1);
    crypto_ec_point_deinit(res, 1);
    crypto_ec_deinit(e);
}
