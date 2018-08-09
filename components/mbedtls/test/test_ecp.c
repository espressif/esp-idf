/* mbedTLS Elliptic Curve functionality tests

   Focus on testing functionality where we use ESP32 hardware
   accelerated crypto features.

*/
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <esp_system.h>

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

    TEST_ASSERT_MBEDTLS_OK( mbedtls_ecp_group_load(&ctx.grp, MBEDTLS_ECP_DP_CURVE25519) );

    TEST_ASSERT_MBEDTLS_OK( mbedtls_ecdh_gen_public(&ctx.grp, &ctx.d, &ctx.Q,
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

