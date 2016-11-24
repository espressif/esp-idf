/* mbedTLS internal tests wrapped into Unity

   Focus on testing functionality where we use ESP32 hardware
   accelerated crypto features.

   See also test_hwcrypto.c
*/
#include <string.h>
#include <stdio.h>
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"
#include "mbedtls/aes.h"
#include "mbedtls/bignum.h"
#include "unity.h"

static int mbedtls_alt_sha256_self_test( int verbose );

TEST_CASE("mbedtls SHA self-tests", "[mbedtls]")
{
    TEST_ASSERT_FALSE_MESSAGE(mbedtls_sha1_self_test(1), "SHA1 self-tests should pass.");
    TEST_ASSERT_FALSE_MESSAGE(mbedtls_alt_sha256_self_test(1), "SHA256 self-tests should pass.");
    TEST_ASSERT_FALSE_MESSAGE(mbedtls_sha512_self_test(1), "SHA512 self-tests should pass.");
}

TEST_CASE("mbedtls AES self-tests", "[aes]")
{
    TEST_ASSERT_FALSE_MESSAGE(mbedtls_aes_self_test(1), "AES self-tests should pass.");
}

TEST_CASE("mbedtls MPI self-tests", "[bignum]")
{
    TEST_ASSERT_FALSE_MESSAGE(mbedtls_mpi_self_test(1), "MPI self-tests should pass.");
}


/* Following code is a copy of the mbedtls_sha256 test vectors,
   with the SHA-224 support removed as we don't currently support this hash.
*/

/*
 * FIPS-180-2 test vectors
 */
static const unsigned char sha256_test_buf[3][57] = {
    { "abc" },
    { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" },
    { "" }
};

static const int sha256_test_buflen[3] = {
    3, 56, 1000
};

static const unsigned char sha256_test_sum[6][32] = {
    /*
     * SHA-256 test vectors
     */
    {
        0xBA, 0x78, 0x16, 0xBF, 0x8F, 0x01, 0xCF, 0xEA,
        0x41, 0x41, 0x40, 0xDE, 0x5D, 0xAE, 0x22, 0x23,
        0xB0, 0x03, 0x61, 0xA3, 0x96, 0x17, 0x7A, 0x9C,
        0xB4, 0x10, 0xFF, 0x61, 0xF2, 0x00, 0x15, 0xAD
    },
    {
        0x24, 0x8D, 0x6A, 0x61, 0xD2, 0x06, 0x38, 0xB8,
        0xE5, 0xC0, 0x26, 0x93, 0x0C, 0x3E, 0x60, 0x39,
        0xA3, 0x3C, 0xE4, 0x59, 0x64, 0xFF, 0x21, 0x67,
        0xF6, 0xEC, 0xED, 0xD4, 0x19, 0xDB, 0x06, 0xC1
    },
    {
        0xCD, 0xC7, 0x6E, 0x5C, 0x99, 0x14, 0xFB, 0x92,
        0x81, 0xA1, 0xC7, 0xE2, 0x84, 0xD7, 0x3E, 0x67,
        0xF1, 0x80, 0x9A, 0x48, 0xA4, 0x97, 0x20, 0x0E,
        0x04, 0x6D, 0x39, 0xCC, 0xC7, 0x11, 0x2C, 0xD0
    }
};

/*
 * Checkup routine
 */
static int mbedtls_alt_sha256_self_test( int verbose )
{
    int j, n, buflen, ret = 0;
    unsigned char buf[1024];
    unsigned char sha256sum[32];
    mbedtls_sha256_context ctx;

    for ( j = 0; j < 3; j++ ) {
        mbedtls_sha256_init( &ctx );

        if ( verbose != 0 ) {
            printf( "  SHA-%d test #%d: ", 256, j + 1 );
        }

        mbedtls_sha256_starts( &ctx, 0 );

        if ( j == 2 ) {
            memset( buf, 'a', buflen = 1000 );

            for ( n = 0; n < 1000; n++ ) {
                mbedtls_sha256_update( &ctx, buf, buflen );
            }
        } else
            mbedtls_sha256_update( &ctx, sha256_test_buf[j],
                                   sha256_test_buflen[j] );

        mbedtls_sha256_finish( &ctx, sha256sum );

        if ( memcmp( sha256sum, sha256_test_sum[j], 32 ) != 0 ) {
            if ( verbose != 0 ) {
                printf( "failed\n" );
            }

            mbedtls_sha256_free( &ctx );

            ret = 1;
            goto exit;
        }

        if ( verbose != 0 ) {
            printf( "passed\n" );
        }

        mbedtls_sha256_free( &ctx );
    }

    if ( verbose != 0 ) {
        printf( "\n" );
    }

exit:

    return ( ret );
}
