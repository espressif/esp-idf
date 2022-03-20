/* mbedTLS Elliptic Curve functionality tests
 *
 * Focus on testing functionality where we use ESP32 hardware
 * accelerated crypto features.
 *
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <esp_random.h>

/* ToDo - Remove this once appropriate solution is available.
We need to define this for the file as ssl_misc.h uses private structures from mbedtls,
which are undefined if the following flag is not defined */
/* Many APIs in the file make use of this flag instead of `MBEDTLS_PRIVATE` */
/* ToDo - Replace them with proper getter-setter once they are added */
#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/ecdh.h>
#include <mbedtls/ecdsa.h>
#include <mbedtls/error.h>

#include "unity.h"

/* Note: negative value here so that assert message prints a grep-able
   error hex value (mbedTLS uses -N for error codes) */
#define TEST_ASSERT_MBEDTLS_OK(X) TEST_ASSERT_EQUAL_HEX32(0, -(X))

TEST_CASE("mbedtls ECDH Generate Key", "[mbedtls]")
{
    mbedtls_ecdh_context ctx;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

    mbedtls_ecdh_init(&ctx);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    mbedtls_entropy_init(&entropy);
    TEST_ASSERT_MBEDTLS_OK( mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0) );

    TEST_ASSERT_MBEDTLS_OK( mbedtls_ecp_group_load(&ctx.ctx.mbed_ecdh.grp, MBEDTLS_ECP_DP_CURVE25519) );

    TEST_ASSERT_MBEDTLS_OK( mbedtls_ecdh_gen_public(&ctx.ctx.mbed_ecdh.grp, &ctx.ctx.mbed_ecdh.d, &ctx.ctx.mbed_ecdh.Q,
                                                    mbedtls_ctr_drbg_random, &ctr_drbg ) );

    mbedtls_ecdh_free(&ctx);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
}

TEST_CASE("mbedtls ECP self-tests", "[mbedtls]")
{
    TEST_ASSERT_EQUAL(0, mbedtls_ecp_self_test(1));
}

TEST_CASE("mbedtls ECP mul w/ koblitz", "[mbedtls]")
{
    /* Test case code via https://github.com/espressif/esp-idf/issues/1556 */
    mbedtls_entropy_context ctxEntropy;
    mbedtls_ctr_drbg_context ctxRandom;
    mbedtls_ecdsa_context ctxECDSA;
    const char* pers = "myecdsa";

    mbedtls_entropy_init(&ctxEntropy);
    mbedtls_ctr_drbg_init(&ctxRandom);
    TEST_ASSERT_MBEDTLS_OK( mbedtls_ctr_drbg_seed(&ctxRandom, mbedtls_entropy_func, &ctxEntropy,
                                                  (const unsigned char*) pers, strlen(pers)) );

    mbedtls_ecdsa_init(&ctxECDSA);

    TEST_ASSERT_MBEDTLS_OK( mbedtls_ecdsa_genkey(&ctxECDSA, MBEDTLS_ECP_DP_SECP256K1,
                                                 mbedtls_ctr_drbg_random, &ctxRandom) );


    TEST_ASSERT_MBEDTLS_OK(mbedtls_ecp_mul(&ctxECDSA.grp, &ctxECDSA.Q, &ctxECDSA.d, &ctxECDSA.grp.G,
                                           mbedtls_ctr_drbg_random, &ctxRandom) );

    mbedtls_ecdsa_free(&ctxECDSA);
    mbedtls_ctr_drbg_free(&ctxRandom);
    mbedtls_entropy_free(&ctxEntropy);
}

#if CONFIG_MBEDTLS_HARDWARE_ECC
/*
 * Coordinates and integers stored in big endian format
 */
const uint8_t ecc_p192_point_x[] = {
    0x18, 0x8D, 0xA8, 0x0E, 0xB0, 0x30, 0x90, 0xF6,
    0x7C, 0xBF, 0x20, 0xEB, 0x43, 0xA1, 0x88, 0x00,
    0xF4, 0xFF, 0x0A, 0xFD, 0x82, 0xFF, 0x10, 0x12
};

const uint8_t ecc_p192_point_y[] = {
    0x07, 0x19, 0x2B, 0x95, 0xFF, 0xC8, 0xDA, 0x78,
    0x63, 0x10, 0x11, 0xED, 0x6B, 0x24, 0xCD, 0xD5,
    0x73, 0xF9, 0x77, 0xA1, 0x1E, 0x79, 0x48, 0x11
};

const uint8_t ecc_p192_scalar[] = {
    0x6f, 0x18, 0x34, 0xeb, 0x16, 0xb7, 0xac, 0x9f,
    0x3c, 0x77, 0x71, 0xb3, 0x02, 0x30, 0x70, 0x48,
    0x75, 0x87, 0xbb, 0x6f, 0x80, 0x34, 0x8d, 0x5e
};

const uint8_t ecc_p192_mul_res_x[] = {
    0x3F, 0xEE, 0x6F, 0x1F, 0x99, 0xDC, 0xCB, 0x78,
    0xB7, 0x47, 0x1C, 0x2A, 0xF5, 0xA0, 0xAC, 0xE6,
    0xEC, 0x24, 0x82, 0x37, 0x6C, 0xC0, 0x27, 0xC5,
};

const uint8_t ecc_p192_mul_res_y[] = {
    0xDF, 0xF3, 0x9E, 0x76, 0x24, 0xF4, 0xF6, 0xB4,
    0xF0, 0x0A, 0x18, 0xE1, 0x0B, 0xD2, 0xD9, 0x83,
    0xE8, 0x29, 0x5E, 0xD9, 0x46, 0x54, 0xC3, 0xE1
};

const uint8_t ecc_p256_point_x[] = {
    0x6B, 0x17, 0xD1, 0xF2, 0xE1, 0x2C, 0x42, 0x47,
    0xF8, 0xBC, 0xE6, 0xE5, 0x63, 0xA4, 0x40, 0xF2,
    0x77, 0x03, 0x7D, 0x81, 0x2D, 0xEB, 0x33, 0xA0,
    0xF4, 0xA1, 0x39, 0x45, 0xD8, 0x98, 0xC2, 0x96
};

const uint8_t ecc_p256_point_y[] = {
    0x4F, 0xE3, 0x42, 0xE2, 0xFE, 0x1A, 0x7F, 0x9B,
    0x8E, 0xE7, 0xEB, 0x4A, 0x7C, 0x0F, 0x9E, 0x16,
    0x2B, 0xCE, 0x33, 0x57, 0x6B, 0x31, 0x5E, 0xCE,
    0xCB, 0xB6, 0x40, 0x68, 0x37, 0xBF, 0x51, 0xF5
};

const uint8_t ecc_p256_scalar[] = {
   0xB2, 0xC5, 0x9E, 0x92, 0x64, 0xCD, 0x5F, 0x66,
   0x9E, 0xC8, 0x83, 0x6D, 0x99, 0x61, 0x18, 0x72,
   0xC8, 0x60, 0x83, 0x1E, 0xE5, 0x79, 0xCC, 0x73,
   0xA9, 0xB4, 0x74, 0x85, 0x70, 0x11, 0x2D, 0xA2,
};

const uint8_t ecc_p256_mul_res_x[] = {
    0x26, 0x1A, 0x0F, 0xBD, 0xA5, 0xE5, 0x1E, 0xE7,
    0xB3, 0xC3, 0xB7, 0x09, 0xD1, 0x4A, 0x7A, 0x2A,
    0x16, 0x69, 0x4B, 0xAF, 0x76, 0x5C, 0xD4, 0x0E,
    0x93, 0x57, 0xB8, 0x67, 0xF9, 0xA1, 0xE5, 0xE8
};

const uint8_t ecc_p256_mul_res_y[] = {
    0xA0, 0xF4, 0x2E, 0x62, 0x36, 0x25, 0x9F, 0xE0,
    0xF2, 0xA0, 0x41, 0x42, 0xD2, 0x95, 0x89, 0x41,
    0x38, 0xF0, 0xEB, 0x6E, 0xA7, 0x96, 0x29, 0x24,
    0xC7, 0xD4, 0x0C, 0x90, 0xA1, 0xC9, 0xD3, 0x3A
};

static int rng_wrapper(void *ctx, unsigned char *buf, size_t len)
{
    esp_fill_random(buf, len);
	return 0;
}

static void test_ecp_mul(mbedtls_ecp_group_id id, const uint8_t *x_coord, const uint8_t *y_coord, const uint8_t *scalar,
                        const uint8_t *result_x_coord, const uint8_t *result_y_coord)
{
    uint8_t x[32];
    uint8_t y[32];
    int size;
    int ret;

    mbedtls_ecp_group grp;
    mbedtls_ecp_point R;
    mbedtls_ecp_point P;
    mbedtls_mpi m;

    mbedtls_ecp_group_init(&grp);
    mbedtls_ecp_point_init(&R);
    mbedtls_ecp_point_init(&P);
    mbedtls_mpi_init(&m);

    mbedtls_ecp_group_load(&grp, id);

    size = grp.pbits / 8;

    mbedtls_mpi_read_binary(&m, scalar, size);

    mbedtls_mpi_read_binary(&P.X, x_coord, size);
    mbedtls_mpi_read_binary(&P.Y, y_coord, size);

    mbedtls_mpi_lset(&P.Z, 1);

    ret = mbedtls_ecp_mul(&grp, &R, &m, &P, rng_wrapper, NULL);

    TEST_ASSERT_EQUAL(0, ret);

    mbedtls_mpi_write_binary(&R.X, x, mbedtls_mpi_size(&R.X));
    mbedtls_mpi_write_binary(&R.Y, y, mbedtls_mpi_size(&R.Y));

    TEST_ASSERT_EQUAL(0, memcmp(x, result_x_coord, mbedtls_mpi_size(&R.X)));
    TEST_ASSERT_EQUAL(0, memcmp(y, result_y_coord, mbedtls_mpi_size(&R.Y)));

    mbedtls_ecp_point_free(&R);
    mbedtls_ecp_point_free(&P);
    mbedtls_mpi_free(&m);
    mbedtls_ecp_group_free(&grp);
}

TEST_CASE("mbedtls ECP point multiply with SECP192R1", "[mbedtls]")
{
    test_ecp_mul(MBEDTLS_ECP_DP_SECP192R1, ecc_p192_point_x, ecc_p192_point_y, ecc_p192_scalar,
                 ecc_p192_mul_res_x, ecc_p192_mul_res_y);
}

TEST_CASE("mbedtls ECP point multiply with SECP256R1", "[mbedtls]")
{
    test_ecp_mul(MBEDTLS_ECP_DP_SECP256R1, ecc_p256_point_x, ecc_p256_point_y, ecc_p256_scalar,
                 ecc_p256_mul_res_x, ecc_p256_mul_res_y);
}

static void test_ecp_verify(mbedtls_ecp_group_id id, const uint8_t *x_coord, const uint8_t *y_coord)
{
    int size;
    int ret;

    mbedtls_ecp_group grp;
    mbedtls_ecp_point P;

    mbedtls_ecp_group_init(&grp);
    mbedtls_ecp_point_init(&P);

    mbedtls_ecp_group_load(&grp, id);

    size = grp.pbits / 8;

    mbedtls_mpi_read_binary(&P.X, x_coord, size);
    mbedtls_mpi_read_binary(&P.Y, y_coord, size);
    mbedtls_mpi_lset(&P.Z, 1);

    ret = mbedtls_ecp_check_pubkey(&grp, &P);

    TEST_ASSERT_EQUAL(0, ret);

    mbedtls_ecp_point_free(&P);
    mbedtls_ecp_group_free(&grp);
}

TEST_CASE("mbedtls ECP point verify with SECP192R1", "[mbedtls]")
{
    test_ecp_verify(MBEDTLS_ECP_DP_SECP192R1, ecc_p192_mul_res_x, ecc_p192_mul_res_y);
}

TEST_CASE("mbedtls ECP point verify with SECP256R1", "[mbedtls]")
{
    test_ecp_verify(MBEDTLS_ECP_DP_SECP256R1, ecc_p256_mul_res_x, ecc_p256_mul_res_y);
}
#endif /* CONFIG_MBEDTLS_HARDWARE_ECC */
