// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include "unity.h"
#include <string.h>
#include "crypto/crypto.h"

#include "mbedtls/ecp.h"
typedef struct crypto_bignum crypto_bignum;

TEST_CASE_ESP32("Test crypto lib bignum apis", "[wpa_crypto]")
{
    {

        uint8_t buf[32], buf2[32];

        /* BN - Init & Deinit*/
        crypto_bignum *bn = crypto_bignum_init();
        crypto_bignum_deinit(bn, 1);

        /* BN - Binary to bignum & bignum to binary*/
        TEST_ASSERT(!crypto_get_random(buf, 32));

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
static inline void ecp_mpi_load( mbedtls_mpi *X, const mbedtls_mpi_uint *p, size_t len )
{
    X->s = 1;
    X->n = len / sizeof( mbedtls_mpi_uint );
    X->p = os_zalloc(len);
    memcpy(X->p, (void *)p, len);
}


TEST_CASE("Test crypto lib ECC apis", "[wpa_crypto]")
{

    static const mbedtls_mpi_uint secp256r1_gx[] = {
        BYTES_TO_T_UINT_8( 0x96, 0xC2, 0x98, 0xD8, 0x45, 0x39, 0xA1, 0xF4 ),
        BYTES_TO_T_UINT_8( 0xA0, 0x33, 0xEB, 0x2D, 0x81, 0x7D, 0x03, 0x77 ),
        BYTES_TO_T_UINT_8( 0xF2, 0x40, 0xA4, 0x63, 0xE5, 0xE6, 0xBC, 0xF8 ),
        BYTES_TO_T_UINT_8( 0x47, 0x42, 0x2C, 0xE1, 0xF2, 0xD1, 0x17, 0x6B ),
    };
    static const mbedtls_mpi_uint secp256r1_gy[] = {
        BYTES_TO_T_UINT_8( 0xF5, 0x51, 0xBF, 0x37, 0x68, 0x40, 0xB6, 0xCB ),
        BYTES_TO_T_UINT_8( 0xCE, 0x5E, 0x31, 0x6B, 0x57, 0x33, 0xCE, 0x2B ),
        BYTES_TO_T_UINT_8( 0x16, 0x9E, 0x0F, 0x7C, 0x4A, 0xEB, 0xE7, 0x8E ),
        BYTES_TO_T_UINT_8( 0x9B, 0x7F, 0x1A, 0xFE, 0xE2, 0x42, 0xE3, 0x4F ),
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
        TEST_ASSERT(!crypto_get_random(pt1, 64));

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

        mbedtls_mpi_init( &num );
        mbedtls_mpi_lset( &num, 3 );

        ecp_mpi_load(& ((mbedtls_ecp_point *)p)->X, secp256r1_gx, sizeof(secp256r1_gx));
        ecp_mpi_load(& ((mbedtls_ecp_point *)p)->Y, secp256r1_gy, sizeof(secp256r1_gy));

        mbedtls_mpi_lset((&((mbedtls_ecp_point *)p)->Z), 1);

        TEST_ASSERT(crypto_ec_point_mul(e, p, (crypto_bignum *) &num, q) == 0); //q = 3p

        TEST_ASSERT(crypto_ec_point_add(e, p, p, r) == 0);
        TEST_ASSERT(crypto_ec_point_add(e, p, r, r) == 0);

        TEST_ASSERT(crypto_ec_point_cmp(e, q, r) == 0);

        mbedtls_mpi_free( &num );
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

        mbedtls_mpi_init( &num );
        mbedtls_mpi_lset( &num, 100 );

        ecp_mpi_load(& ((mbedtls_ecp_point *)p)->X, secp256r1_gx, sizeof(secp256r1_gx));
        ecp_mpi_load(& ((mbedtls_ecp_point *)p)->Y, secp256r1_gy, sizeof(secp256r1_gy));

        mbedtls_mpi_lset((&((mbedtls_ecp_point *)p)->Z), 1);

        TEST_ASSERT(crypto_ec_point_mul(e, p, (crypto_bignum *) &num, q) == 0);
        TEST_ASSERT(crypto_ec_point_mul(e, p, (crypto_bignum *) &num, r) == 0);

        TEST_ASSERT(crypto_ec_point_invert(e, r) == 0);
        TEST_ASSERT(crypto_ec_point_add(e, q, r, r) == 0);

        TEST_ASSERT(crypto_ec_point_is_at_infinity(e, r));

        mbedtls_mpi_free( &num );
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

        mbedtls_mpi_init( &num );
        mbedtls_mpi_lset( &num, 50 );

        ecp_mpi_load(& ((mbedtls_ecp_point *)p)->X, secp256r1_gx, sizeof(secp256r1_gx));
        ecp_mpi_load(& ((mbedtls_ecp_point *)p)->Y, secp256r1_gy, sizeof(secp256r1_gy));

        mbedtls_mpi_lset((&((mbedtls_ecp_point *)p)->Z), 1);

        /* Generator should always be on the curve*/
        TEST_ASSERT(crypto_ec_point_is_on_curve(e, p));

        /* Any point generated using generated should also be on the same curve*/
        TEST_ASSERT(crypto_ec_point_mul(e, p, (crypto_bignum *) &num, q) == 0);
        TEST_ASSERT(crypto_ec_point_is_on_curve(e, q));


        mbedtls_mpi_free( &num );
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

        mbedtls_mpi_init( &num );
        mbedtls_mpi_lset( &num, 50 );

        ecp_mpi_load(& ((mbedtls_ecp_point *)p)->X, secp256r1_gx, sizeof(secp256r1_gx));
        ecp_mpi_load(& ((mbedtls_ecp_point *)p)->Y, secp256r1_gy, sizeof(secp256r1_gy));

        mbedtls_mpi_lset((&((mbedtls_ecp_point *)p)->Z), 1);

        mbedtls_mpi_copy(&((mbedtls_ecp_point *)q)->X, &((mbedtls_ecp_point *)p)->X);
        mbedtls_mpi_copy(&((mbedtls_ecp_point *)r)->X, &((mbedtls_ecp_point *)p)->X);

        mbedtls_mpi_lset((&((mbedtls_ecp_point *)q)->Z), 1);
        mbedtls_mpi_lset((&((mbedtls_ecp_point *)r)->Z), 1);

        TEST_ASSERT(crypto_ec_point_solve_y_coord(e, q, (crypto_bignum *) & ((mbedtls_ecp_point *)q)->X, 0) == 0);
        TEST_ASSERT(crypto_ec_point_is_on_curve(e, q));

        TEST_ASSERT(crypto_ec_point_solve_y_coord(e, r, (crypto_bignum *) & ((mbedtls_ecp_point *)q)->X, 1) == 0);
        TEST_ASSERT(crypto_ec_point_is_on_curve(e, r));

        TEST_ASSERT((crypto_ec_point_cmp(e, p, q) == 0) || (crypto_ec_point_cmp(e, p, r) == 0));

        /* The two roots should be inverse of one another*/
        TEST_ASSERT(crypto_ec_point_add(e, q, r, r) == 0);
        TEST_ASSERT(crypto_ec_point_is_at_infinity(e, r));

        mbedtls_mpi_free( &num );
        crypto_ec_point_deinit(p, 1);
        crypto_ec_point_deinit(q, 1);
        crypto_ec_point_deinit(r, 1);
        crypto_ec_deinit(e);

    }

}

