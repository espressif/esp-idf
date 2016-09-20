/**
 * \brief  Multi-precision integer library, ESP32 hardware accelerated parts
 *
 *  based on mbedTLS implementation
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  Additions Copyright (C) 2016, Espressif Systems (Shanghai) PTE Ltd
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "mbedtls/bignum.h"
#include "mbedtls/bn_mul.h"
#include "rom/bigint.h"

#if defined(MBEDTLS_MPI_MUL_MPI_ALT) || defined(MBEDTLS_MPI_EXP_MOD_ALT)

/* Constants from mbedTLS bignum.c */
#define ciL    (sizeof(mbedtls_mpi_uint))         /* chars in limb  */
#define biL    (ciL << 3)               /* bits  in limb  */

static _lock_t mpi_lock;

/* At the moment these hardware locking functions aren't exposed publically
   for MPI. If you want to use the ROM bigint functions and co-exist with mbedTLS,
   please raise a feature request.
*/
static void esp_mpi_acquire_hardware( void )
{
    /* newlib locks lazy initialize on ESP-IDF */
    _lock_acquire(&mpi_lock);
    ets_bigint_enable();
}

static void esp_mpi_release_hardware( void )
{
    ets_bigint_disable();
    _lock_release(&mpi_lock);
}

/* Given a & b, determine u & v such that

   gcd(a,b) = d = au + bv

   Underlying algorithm comes from:
   http://www.ucl.ac.uk/~ucahcjm/combopt/ext_gcd_python_programs.pdf
   http://www.hackersdelight.org/hdcodetxt/mont64.c.txt
 */
static void extended_binary_gcd(const mbedtls_mpi *a, const mbedtls_mpi *b,
                                   mbedtls_mpi *u, mbedtls_mpi *v)
{
    mbedtls_mpi ta, tb;

    mbedtls_mpi_init(&ta);
    mbedtls_mpi_copy(&ta, a);
    mbedtls_mpi_init(&tb);
    mbedtls_mpi_copy(&tb, b);

    mbedtls_mpi_lset(u, 1);
    mbedtls_mpi_lset(v, 0);

    /* Loop invariant:
      ta = u*2*a - v*b. */
    while (mbedtls_mpi_cmp_int(&ta, 0) != 0) {
        mbedtls_mpi_shift_r(&ta, 1);
        if (mbedtls_mpi_get_bit(u, 0) == 0) {
            // Remove common factor of 2 in u & v
            mbedtls_mpi_shift_r(u, 1);
            mbedtls_mpi_shift_r(v, 1);
        }
        else {
            /* u = (u + b) >> 1 */
            mbedtls_mpi_add_mpi(u, u, b);
            mbedtls_mpi_shift_r(u, 1);
            /* v = (v >> 1) + a */
            mbedtls_mpi_shift_r(v, 1);
            mbedtls_mpi_add_mpi(v, v, a);
        }
    }
    mbedtls_mpi_free(&ta);
    mbedtls_mpi_free(&tb);

    /* u = u * 2, so 1 = u*a - v*b */
    mbedtls_mpi_shift_l(u, 1);
}

/* inner part of MPI modular multiply, after Rinv & Mprime are calculated */
static int mpi_mul_mpi_mod_inner(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *B, const mbedtls_mpi *M, mbedtls_mpi *Rinv, uint32_t Mprime, size_t num_words)
{
    int ret;
    mbedtls_mpi TA, TB;
    size_t num_bits = num_words * 32;

    mbedtls_mpi_grow(Rinv, num_words);

    /* TODO: fill memory blocks directly so this isn't needed */
    mbedtls_mpi_init(&TA);
    mbedtls_mpi_copy(&TA, A);
    mbedtls_mpi_grow(&TA, num_words);
    A = &TA;
    mbedtls_mpi_init(&TB);
    mbedtls_mpi_copy(&TB, B);
    mbedtls_mpi_grow(&TB, num_words);
    B = &TB;

    esp_mpi_acquire_hardware();

    if(ets_bigint_mod_mult_prepare(A->p, B->p, M->p, Mprime,
                                   Rinv->p, num_bits, false)) {
        mbedtls_mpi_grow(X, num_words);
        ets_bigint_wait_finish();
        if(ets_bigint_mod_mult_getz(M->p, X->p, num_bits)) {
            X->s = A->s * B->s;
            ret = 0;
        } else {
            printf("ets_bigint_mod_mult_getz failed\n");
            ret = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
        }
    } else {
            printf("ets_bigint_mod_mult_prepare failed\n");
            ret = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
    }
    esp_mpi_release_hardware();

    /* unclear why this is necessary, but the result seems
     to come back rotated 32 bits to the right... */
    uint32_t last_word = X->p[num_words-1];
    X->p[num_words-1] = 0;
    mbedtls_mpi_shift_l(X, 32);
    X->p[0] = last_word;

    mbedtls_mpi_free(&TA);
    mbedtls_mpi_free(&TB);

    return ret;
}

/* X = (A * B) mod M

   Not an mbedTLS function

   num_bits guaranteed to be a multiple of 512 already.

   TODO: ensure M is odd
 */
int esp_mpi_mul_mpi_mod(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *B, const mbedtls_mpi *M, size_t num_bits)
{
    int ret = 0;
    mbedtls_mpi RR, Rinv, Mprime;
    uint32_t Mprime_int;
    size_t num_words = num_bits / 32;

    /* Rinv & Mprime are calculated via extended binary gcd
       algorithm, see references on extended_binary_gcd above.
    */
    mbedtls_mpi_init(&Rinv);
    mbedtls_mpi_init(&RR);
    mbedtls_mpi_set_bit(&RR, num_bits+32, 1);
    mbedtls_mpi_init(&Mprime);
    extended_binary_gcd(&RR, M, &Rinv, &Mprime);

    /* M' is mod 2^32 */
    Mprime_int = Mprime.p[0];

    ret = mpi_mul_mpi_mod_inner(X, A, B, M, &Rinv, Mprime_int, num_words);

    mbedtls_mpi_free(&RR);
    mbedtls_mpi_free(&Mprime);
    mbedtls_mpi_free(&Rinv);

    return ret;
}


/*
 * Helper for mbedtls_mpi multiplication
 * copied/trimmed from mbedtls bignum.c
 */
static void mpi_mul_hlp( size_t i, mbedtls_mpi_uint *s, mbedtls_mpi_uint *d, mbedtls_mpi_uint b )
{
    mbedtls_mpi_uint c = 0, t = 0;

    for( ; i >= 16; i -= 16 )
    {
        MULADDC_INIT
        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE

        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE
        MULADDC_STOP
    }

    for( ; i >= 8; i -= 8 )
    {
        MULADDC_INIT
        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE

        MULADDC_CORE   MULADDC_CORE
        MULADDC_CORE   MULADDC_CORE
        MULADDC_STOP
    }


    for( ; i > 0; i-- )
    {
        MULADDC_INIT
        MULADDC_CORE
        MULADDC_STOP
    }

    t++;

    do {
        *d += c; c = ( *d < c ); d++;
    }
    while( c != 0 );
}


/*
 * Helper for mbedtls_mpi subtraction
 * Copied/adapter from mbedTLS bignum.c
 */
static void mpi_sub_hlp( size_t n, mbedtls_mpi_uint *s, mbedtls_mpi_uint *d )
{
    size_t i;
    mbedtls_mpi_uint c, z;

    for( i = c = 0; i < n; i++, s++, d++ )
    {
        z = ( *d <  c );     *d -=  c;
        c = ( *d < *s ) + z; *d -= *s;
    }

    while( c != 0 )
    {
        z = ( *d < c ); *d -= c;
        c = z; i++; d++;
    }
}


/* The following 3 Montgomery arithmetic function are
   copied from mbedTLS bigint.c verbatim as they are static.

   TODO: find a way to support making the versions in mbedtls
   non-static.
*/

/*
 * Fast Montgomery initialization (thanks to Tom St Denis)
 */
static void mpi_montg_init( mbedtls_mpi_uint *mm, const mbedtls_mpi *N )
{
    mbedtls_mpi_uint x, m0 = N->p[0];
    unsigned int i;

    x  = m0;
    x += ( ( m0 + 2 ) & 4 ) << 1;

    for( i = biL; i >= 8; i /= 2 )
        x *= ( 2 - ( m0 * x ) );

    *mm = ~x + 1;
}

/*
 * Montgomery multiplication: A = A * B * R^-1 mod N  (HAC 14.36)
 */
static int mpi_montmul( mbedtls_mpi *A, const mbedtls_mpi *B, const mbedtls_mpi *N, mbedtls_mpi_uint mm,
                         const mbedtls_mpi *T )
{
    size_t i, n, m;
    mbedtls_mpi_uint u0, u1, *d;

    if( T->n < N->n + 1 || T->p == NULL )
        return( MBEDTLS_ERR_MPI_BAD_INPUT_DATA );

    memset( T->p, 0, T->n * ciL );

    d = T->p;
    n = N->n;
    m = ( B->n < n ) ? B->n : n;

    for( i = 0; i < n; i++ )
    {
        /*
         * T = (T + u0*B + u1*N) / 2^biL
         */
        u0 = A->p[i];
        u1 = ( d[0] + u0 * B->p[0] ) * mm;

        mpi_mul_hlp( m, B->p, d, u0 );
        mpi_mul_hlp( n, N->p, d, u1 );

        *d++ = u0; d[n + 1] = 0;
    }

    memcpy( A->p, d, ( n + 1 ) * ciL );

    if( mbedtls_mpi_cmp_abs( A, N ) >= 0 )
        mpi_sub_hlp( n, N->p, A->p );
    else
        /* prevent timing attacks */
        mpi_sub_hlp( n, A->p, T->p );

    return( 0 );
}

/*
 * Montgomery reduction: A = A * R^-1 mod N
 */
static int mpi_montred( mbedtls_mpi *A, const mbedtls_mpi *N, mbedtls_mpi_uint mm, const mbedtls_mpi *T )
{
    mbedtls_mpi_uint z = 1;
    mbedtls_mpi U;

    U.n = U.s = (int) z;
    U.p = &z;

    return( mpi_montmul( A, &U, N, mm, T ) );
}

#if defined(MBEDTLS_MPI_MUL_MPI_ALT) /* MBEDTLS_MPI_MUL_MPI_ALT */

/* Number of words used to hold 'mpi', rounded up to nearest
   16 words (512 bits) to match hardware support
*/
static inline size_t hardware_words_needed(const mbedtls_mpi *mpi)
{
    size_t res;
    for(res = mpi->n; res > 0; res-- ) {
        if( mpi->p[res - 1] != 0 )
            break;
    }
    res = (res + 0xF) & ~0xF;
    return res;
}


/* Special-case multiply, where we use hardware montgomery mod
   multiplication to solve the case where A or B are >2048 bits so
   can't do standard multiplication.

   the modulus here is chosen with M=(2^num_bits-1)
   to guarantee the output isn't actually modulo anything. This means
   we don't need to calculate M' and Rinv, they are predictable
   as follows:
   M' = 1
   Rinv = (1 << (num_bits - 32)

   (See RSA Accelerator section in Technical Reference for derivation
   of M', Rinv)
*/
static int esp_mpi_mult_mpi_failover_mod_mult(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *B, size_t num_words)
 {
     mbedtls_mpi M, Rinv;
     int ret;
     size_t mprime;
     size_t num_bits = num_words * 32;

     mbedtls_mpi_init(&M);
     mbedtls_mpi_init(&Rinv);

     /* TODO: it may be faster to just use 4096-bit arithmetic every time,
        and make these constants rather than runtime derived
        derived. */
     /* M = (2^num_words)-1 */
     mbedtls_mpi_grow(&M, num_words);
     for(int i = 0; i < num_words*32; i++) {
         mbedtls_mpi_set_bit(&M, i, 1);
     }

     /* Rinv = (2^num_words-32) */
     mbedtls_mpi_grow(&Rinv, num_words);
     mbedtls_mpi_set_bit(&Rinv, num_bits - 32, 1);

     mprime = 1;

     ret = mpi_mul_mpi_mod_inner(X, A, B, &M, &Rinv, mprime, num_words);

     mbedtls_mpi_free(&M);
     mbedtls_mpi_free(&Rinv);
     return ret;
 }

int mbedtls_mpi_mul_mpi( mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *B )
{
    int ret = -1;
    size_t words_a, words_b, words_x, words_mult;

    mbedtls_mpi TA, TB;

    mbedtls_mpi_init( &TA ); mbedtls_mpi_init( &TB );

    /* Count words needed for A & B in hardware */
    words_a = hardware_words_needed(A);
    words_b = hardware_words_needed(B);

    words_mult = (words_a > words_b ? words_a : words_b);

    /* Take a copy of A if either X == A OR if A isn't long enough
       to hold the number of words needed for hardware.

       (can't grow A directly as it is const)

       TODO: growing the input operands is only necessary because the
       ROM functions only take one length argument. It should be
       possible for us to just copy the used data only into the
       hardware buffers, and set the remaining bits to zero - saving
       RAM. But we need to reimplement ets_bigint_mult_prepare() in
       software for this.
    */
    if( X == A || A->n < words_mult) {
        MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &TA, A ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_grow( &TA, words_mult) );
        A = &TA;
    }
    /* Same for B */
    if( X == B || B->n < words_mult ) {
        MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &TB, B ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_grow( &TB, words_mult) );
        B = &TB;
    }

    /* Result X has to have room for double the larger operand */
    words_x = words_mult * 2;
    MBEDTLS_MPI_CHK( mbedtls_mpi_grow( X, words_x ) );
    /* TODO: check if lset here is necessary, hardware should zero */
    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( X, 0 ) );

    /* If either operand is over 2048 bits, we can't use the standard hardware multiplier
       (it assumes result is double longest operand, and result is max 4096 bits.)

       However, we can fail over to mod_mult for up to 4096 bits.
    */
    if(words_mult * 32 > 2048) {
        /* TODO: check if there's an overflow condition if words_a & words_b are both
           the bit lengths of the operands, result could be 1 bit longer
        */
        if((words_a + words_b) * 32 > 4096) {
            printf("ERROR: %d bit operands (%d bits * %d bits) too large for hardware unit\n", words_mult * 32, mbedtls_mpi_bitlen(A), mbedtls_mpi_bitlen(B));
            ret = MBEDTLS_ERR_MPI_NOT_ACCEPTABLE;
        }
        else {
            ret = esp_mpi_mult_mpi_failover_mod_mult(X, A, B, words_a + words_b);
        }
    }
    else {

        /* normal mpi multiplication */
        esp_mpi_acquire_hardware();
        if (ets_bigint_mult_prepare(A->p, B->p, words_mult * 32)) {
            ets_bigint_wait_finish();
            /* NB: argument to bigint_mult_getz is length of inputs, double this number (words_x) is
               copied to output X->p.
            */
            if (ets_bigint_mult_getz(X->p, words_mult * 32) == true) {
                X->s = A->s * B->s;
                ret = 0;
            } else {
                printf("ets_bigint_mult_getz failed\n");
                ret = MBEDTLS_ERR_MPI_NOT_ACCEPTABLE;
            }
        } else{
            printf("Baseline multiplication failed\n");
            ret = MBEDTLS_ERR_MPI_NOT_ACCEPTABLE;
        }
        esp_mpi_release_hardware();
    }
cleanup:

    mbedtls_mpi_free( &TB ); mbedtls_mpi_free( &TA );

    return( ret );
}

#endif /* MBEDTLS_MPI_MUL_MPI_ALT */

#if defined(MBEDTLS_MPI_EXP_MOD_ALT)
/*
 * Sliding-window exponentiation: X = A^E mod N  (HAC 14.85)
 */
int mbedtls_mpi_exp_mod( mbedtls_mpi* X, const mbedtls_mpi* A, const mbedtls_mpi* E, const mbedtls_mpi* N, mbedtls_mpi* _RR )
{
    int ret;
    size_t wbits, wsize, one = 1;
    size_t i, j, nblimbs;
    size_t bufsize, nbits;
    mbedtls_mpi_uint ei, mm, state;
    mbedtls_mpi RR, T, W[ 2 << MBEDTLS_MPI_WINDOW_SIZE ], Apos;
    int neg;

    if( mbedtls_mpi_cmp_int( N, 0 ) < 0 || ( N->p[0] & 1 ) == 0 )
        return( MBEDTLS_ERR_MPI_BAD_INPUT_DATA );

    if( mbedtls_mpi_cmp_int( E, 0 ) < 0 )
        return( MBEDTLS_ERR_MPI_BAD_INPUT_DATA );

    /*
     * Init temps and window size
     */
    mpi_montg_init( &mm, N );
    mbedtls_mpi_init( &RR ); mbedtls_mpi_init( &T );
    mbedtls_mpi_init( &Apos );
    memset( W, 0, sizeof( W ) );

    i = mbedtls_mpi_bitlen( E );

    wsize = ( i > 671 ) ? 6 : ( i > 239 ) ? 5 :
            ( i >  79 ) ? 4 : ( i >  23 ) ? 3 : 1;

    if( wsize > MBEDTLS_MPI_WINDOW_SIZE )
        wsize = MBEDTLS_MPI_WINDOW_SIZE;

    j = N->n + 1;
    MBEDTLS_MPI_CHK( mbedtls_mpi_grow( X, j ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_grow( &W[1],  j ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_grow( &T, j * 2 ) );

    /*
     * Compensate for negative A (and correct at the end)
     */
    neg = ( A->s == -1 );
    if( neg )
    {
        MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &Apos, A ) );
        Apos.s = 1;
        A = &Apos;
    }

    /*
     * If 1st call, pre-compute R^2 mod N
     */
    if( _RR == NULL || _RR->p == NULL )
    {
        MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &RR, 1 ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_shift_l( &RR, N->n * 2 * biL ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &RR, &RR, N ) );

        if( _RR != NULL )
            memcpy( _RR, &RR, sizeof( mbedtls_mpi) );
    }
    else
        memcpy( &RR, _RR, sizeof( mbedtls_mpi) );

    /*
     * W[1] = A * R^2 * R^-1 mod N = A * R mod N
     */
    if( mbedtls_mpi_cmp_mpi( A, N ) >= 0 )
        MBEDTLS_MPI_CHK( mbedtls_mpi_mod_mpi( &W[1], A, N ) );
    else
        MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &W[1], A ) );

    mpi_montmul( &W[1], &RR, N, mm, &T );

    /*
     * X = R^2 * R^-1 mod N = R mod N
     */
    MBEDTLS_MPI_CHK( mbedtls_mpi_copy( X, &RR ) );
    mpi_montred( X, N, mm, &T );

    if( wsize > 1 )
    {
        /*
         * W[1 << (wsize - 1)] = W[1] ^ (wsize - 1)
         */
        j =  one << ( wsize - 1 );

        MBEDTLS_MPI_CHK( mbedtls_mpi_grow( &W[j], N->n + 1 ) );
        MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &W[j], &W[1]    ) );

        for( i = 0; i < wsize - 1; i++ )
            mpi_montmul( &W[j], &W[j], N, mm, &T );

        /*
         * W[i] = W[i - 1] * W[1]
         */
        for( i = j + 1; i < ( one << wsize ); i++ )
        {
            MBEDTLS_MPI_CHK( mbedtls_mpi_grow( &W[i], N->n + 1 ) );
            MBEDTLS_MPI_CHK( mbedtls_mpi_copy( &W[i], &W[i - 1] ) );

            mpi_montmul( &W[i], &W[1], N, mm, &T );
        }
    }

    nblimbs = E->n;
    bufsize = 0;
    nbits   = 0;
    wbits   = 0;
    state   = 0;

    while( 1 )
    {
        if( bufsize == 0 )
        {
            if( nblimbs == 0 )
                break;

            nblimbs--;

            bufsize = sizeof( mbedtls_mpi_uint ) << 3;
        }

        bufsize--;

        ei = (E->p[nblimbs] >> bufsize) & 1;

        /*
         * skip leading 0s
         */
        if( ei == 0 && state == 0 )
            continue;

        if( ei == 0 && state == 1 )
        {
            /*
             * out of window, square X
             */
            mpi_montmul( X, X, N, mm, &T );
            continue;
        }

        /*
         * add ei to current window
         */
        state = 2;

        nbits++;
        wbits |= ( ei << ( wsize - nbits ) );

        if( nbits == wsize )
        {
            /*
             * X = X^wsize R^-1 mod N
             */
            for( i = 0; i < wsize; i++ )
                mpi_montmul( X, X, N, mm, &T );

            /*
             * X = X * W[wbits] R^-1 mod N
             */
            mpi_montmul( X, &W[wbits], N, mm, &T );

            state--;
            nbits = 0;
            wbits = 0;
        }
    }

    /*
     * process the remaining bits
     */
    for( i = 0; i < nbits; i++ )
    {
        mpi_montmul( X, X, N, mm, &T );

        wbits <<= 1;

        if( ( wbits & ( one << wsize ) ) != 0 )
            mpi_montmul( X, &W[1], N, mm, &T );
    }

    /*
     * X = A^E * R * R^-1 mod N = A^E mod N
     */
    mpi_montred( X, N, mm, &T );

    if( neg )
    {
        X->s = -1;
        MBEDTLS_MPI_CHK( mbedtls_mpi_add_mpi( X, N, X ) );
    }

cleanup:

    for( i = ( one << ( wsize - 1 ) ); i < ( one << wsize ); i++ )
        mbedtls_mpi_free( &W[i] );

    mbedtls_mpi_free( &W[1] ); mbedtls_mpi_free( &T ); mbedtls_mpi_free( &Apos );

    if( _RR == NULL || _RR->p == NULL )
        mbedtls_mpi_free( &RR );

    return( ret );
}

#endif /* MBEDTLS_MPI_EXP_MOD_ALT */

#endif /* MBEDTLS_MPI_MUL_MPI_ALT || MBEDTLS_MPI_EXP_MOD_ALT */

