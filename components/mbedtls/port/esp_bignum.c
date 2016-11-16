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
#include <limits.h>
#include <assert.h>
#include "mbedtls/bignum.h"
#include "mbedtls/bn_mul.h"
#include "rom/bigint.h"
#include "soc/hwcrypto_reg.h"
#include "esp_system.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "bignum";

#if defined(MBEDTLS_MPI_MUL_MPI_ALT) || defined(MBEDTLS_MPI_EXP_MOD_ALT)

/* Constants from mbedTLS bignum.c */
#define ciL    (sizeof(mbedtls_mpi_uint))         /* chars in limb  */
#define biL    (ciL << 3)               /* bits  in limb  */

static _lock_t mpi_lock;

/* Temporary debugging function to print an MPI number to
   stdout. Happens to be in a format compatible with Python.
*/
void mbedtls_mpi_printf(const char *name, const mbedtls_mpi *X)
{
    static char buf[1024];
    size_t n;
    memset(buf, 0, sizeof(buf));
    mbedtls_mpi_write_string(X, 16, buf, sizeof(buf)-1, &n);
    if(n) {
        ESP_LOGI(TAG, "%s = 0x%s", name, buf);
    } else {
        ESP_LOGI(TAG, "TOOLONG");
    }
}

/* Temporary debug function to dump a memory block's contents to stdout
   TODO remove
 */
static void __attribute__((unused)) dump_memory_block(const char *label, uint32_t addr)
{
    printf("Dumping %s @ %08x\n", label, addr);
    for(int i = 0; i < (4096 / 8); i += 4) {
        if(i % 32 == 0) {
            printf("\n %04x:", i);
        }
        printf("%08x ", REG_READ(addr + i));
    }
    printf("Done\n");
}

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

/* Number of words used to hold 'mpi', rounded up to nearest
   16 words (512 bits) to match hardware support.

   Note that mpi->n (size of memory buffer) may be higher than this
   number, if the high bits are mostly zeroes.

   This implementation may cause the caller to leak a small amount of
   timing information when an operation is performed (length of a
   given mpi value, rounded to nearest 512 bits), but not all mbedTLS
   RSA operations succeed if we use mpi->N as-is (buffers are too long).
*/
static inline size_t hardware_words_needed(const mbedtls_mpi *mpi)
{
    size_t res = 1;
    for(size_t i = 0; i < mpi->n; i++) {
        if( mpi->p[i] != 0 ) {
            res = i + 1;
        }
    }
    res = (res + 0xF) & ~0xF;
    return res;
}

/* Copy mbedTLS MPI bignum 'mpi' to hardware memory block at 'mem_base'.

   If num_words is higher than the number of words in the bignum then
   these additional words will be zeroed in the memory buffer.
*/
static inline void mpi_to_mem_block(uint32_t mem_base, const mbedtls_mpi *mpi, size_t num_words)
{
    for(size_t i = 0; i < mpi->n && i < num_words; i++) {
        REG_WRITE(mem_base + i * 4, mpi->p[i]);
    }
    for(size_t i = mpi->n; i < num_words; i++) {
        REG_WRITE(mem_base + i * 4, 0);
    }
}

/* Read mbedTLS MPI bignum back from hardware memory block.

   Reads num_words words from block.

   Can return a failure result if fails to grow the MPI result.
*/
static inline int mem_block_to_mpi(mbedtls_mpi *x, uint32_t mem_base, int num_words)
{
    int ret = 0;

    MBEDTLS_MPI_CHK( mbedtls_mpi_grow(x, num_words) );

    for(int i = 0; i < num_words; i++) {
        x->p[i] = REG_READ(mem_base + i * 4);
    }
    /* Zero any remaining limbs in the bignum, if the buffer is bigger
       than num_words */
    for(size_t i = num_words; i < x->n; i++) {
        x->p[i] = 0;
    }

 cleanup:
    return ret;
}

/* Given a & b, determine u & v such that

   gcd(a,b) = d = au - bv

   This is suitable for calculating values for montgomery multiplication:

   gcd(R, M) = R * Rinv - M * Mprime = 1

   Conditions which must be true:
   - argument 'a' (R) is a power of 2.
   - argument 'b' (M) is odd.

   Underlying algorithm comes from:
   http://www.hackersdelight.org/hdcodetxt/mont64.c.txt
   http://www.ucl.ac.uk/~ucahcjm/combopt/ext_gcd_python_programs.pdf
 */
static void extended_binary_gcd(const mbedtls_mpi *a, const mbedtls_mpi *b,
                                   mbedtls_mpi *u, mbedtls_mpi *v)
{
    mbedtls_mpi a_, ta;

    /* These checks degrade performance, TODO remove them... */
    assert(b->p[0] & 1);
    assert(mbedtls_mpi_bitlen(a) == mbedtls_mpi_lsb(a)+1);
    assert(mbedtls_mpi_cmp_mpi(a, b) > 0);

    mbedtls_mpi_lset(u, 1);
    mbedtls_mpi_lset(v, 0);

    /* 'a' needs to be half its real value for this algorithm
       TODO see if we can halve the number in the caller to avoid
       allocating a bignum here.
     */
    mbedtls_mpi_init(&a_);
    mbedtls_mpi_copy(&a_, a);
    mbedtls_mpi_shift_r(&a_, 1);

    mbedtls_mpi_init(&ta);
    mbedtls_mpi_copy(&ta, &a_);

    //mbedtls_mpi_printf("a", &a_);
    //mbedtls_mpi_printf("b", b);

    /* Loop invariant:
      2*ta = u*2*a - v*b.

      Loop until ta == 0
    */
    while (mbedtls_mpi_cmp_int(&ta, 0) != 0) {
        //mbedtls_mpi_printf("ta", &ta);
        //mbedtls_mpi_printf("u", u);
        //mbedtls_mpi_printf("v", v);
        //printf("2*ta == u*2*a - v*b\n");

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
            /* v = (v - a) >> 1 */
            mbedtls_mpi_shift_r(v, 1);
            mbedtls_mpi_add_mpi(v, v, &a_);
        }
    }
    mbedtls_mpi_free(&ta);
    mbedtls_mpi_free(&a_);
}

/* Execute RSA operation. op_reg specifies which 'START' register
   to write to.
*/
static inline void execute_op(uint32_t op_reg)
{
    /* Clear interrupt status, start operation */
    REG_WRITE(RSA_INTERRUPT_REG, 1);
    REG_WRITE(op_reg, 1);

    /* TODO: use interrupt instead of busywaiting */
    while(REG_READ(RSA_INTERRUPT_REG) != 1)
       { }

    /* clear the interrupt */
    REG_WRITE(RSA_INTERRUPT_REG, 1);
}

/* Sub-stages of modulo multiplication/exponentiation operations */
static int modular_op_prepare(const mbedtls_mpi *X, const mbedtls_mpi *M, size_t num_words);
inline static int modular_multiply_finish(mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, size_t num_words);

/* Z = (X * Y) mod M

   Not an mbedTLS function
 */
int esp_mpi_mul_mpi_mod(mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, const mbedtls_mpi *M)
{
    int ret;
    size_t num_words = hardware_words_needed(M);

    /* Calculate and load the first stage montgomery multiplication */
    MBEDTLS_MPI_CHK( modular_op_prepare(X, M, num_words) );

    execute_op(RSA_MULT_START_REG);

    MBEDTLS_MPI_CHK( modular_multiply_finish(Z, X, Y, num_words) );

    esp_mpi_release_hardware();

 cleanup:
    return ret;
}

#if defined(MBEDTLS_MPI_EXP_MOD_ALT)

/*
 * Sliding-window exponentiation: Z = X^Y mod M  (HAC 14.85)
 */
 #if 0
int mbedtls_mpi_exp_mod( mbedtls_mpi* Z, const mbedtls_mpi* X, const mbedtls_mpi* Y, const mbedtls_mpi* M, mbedtls_mpi* _RR )
{
    int ret;
    size_t z_words = hardware_words_needed(Z);
    size_t x_words = hardware_words_needed(X);
    size_t y_words = hardware_words_needed(Y);
    size_t m_words = hardware_words_needed(M);
    size_t num_words;

    mbedtls_mpi_printf("X",X);
    mbedtls_mpi_printf("Y",Y);
    mbedtls_mpi_printf("M",M);

    /* "all numbers must be the same length", so choose longest number
       as cardinal length of operation...
    */
    num_words = z_words;
    if (x_words > num_words) {
        num_words = x_words;
    }
    if (y_words > num_words) {
        num_words = y_words;
    }
    if (m_words > num_words) {
        num_words = m_words;
    }
    printf("num_words = %d  # %d, %d, %d\n", num_words, x_words, y_words, m_words);

    /* TODO: _RR parameter currently ignored */

    ret = modular_op_prepare(X, M, num_words);
    if (ret != 0) {
        return ret;
    }

    mpi_to_mem_block(RSA_MEM_Y_BLOCK_BASE, Y, num_words);

    //dump_memory_block("X_BLOCK", RSA_MEM_X_BLOCK_BASE);
    //dump_memory_block("Y_BLOCK", RSA_MEM_Y_BLOCK_BASE);
    //dump_memory_block("M_BLOCK", RSA_MEM_M_BLOCK_BASE);

    REG_WRITE(RSA_MODEXP_MODE_REG, (num_words / 16) - 1);

    execute_op(RSA_START_MODEXP_REG);

    //dump_memory_block("Z_BLOCK", RSA_MEM_Z_BLOCK_BASE);

    /* TODO: only need to read m_words not num_words, provided result is correct... */
    ret = mem_block_to_mpi(Z, RSA_MEM_Z_BLOCK_BASE, num_words);

    esp_mpi_release_hardware();

    mbedtls_mpi_printf("Z",Z);
    printf("print (Z == (X ** Y) %% M)\n");

    return ret;
}

#else

/**
 * There is a need for the value of integer N' such that B^-1(B-1)-N^-1N'=1, 
 * where B^-1(B-1) mod N=1. Actually, only the least significant part of 
 * N' is needed, hence the definition N0'=N' mod b. We reproduce below the 
 * simple algorithm from an article by Dusse and Kaliski to efficiently 
 * find N0' from N0 and b 
 */
static mbedtls_mpi_uint modular_inverse(const mbedtls_mpi *M)
{
    int i;
    uint64_t t = 1;
    uint64_t two_2_i_minus_1 = 2;   /* 2^(i-1) */
    uint64_t two_2_i = 4;           /* 2^i */
    uint64_t N = M->p[0];

    for (i = 2; i <= 32; i++) {
        if ((mbedtls_mpi_uint) N * t % two_2_i >= two_2_i_minus_1) {
            t += two_2_i_minus_1;
        }

        two_2_i_minus_1 <<= 1;
        two_2_i <<= 1;
    }

    return (mbedtls_mpi_uint)(UINT32_MAX - t + 1);
}

static int bignum_param_init(const mbedtls_mpi *M, mbedtls_mpi *_RR, mbedtls_mpi *r, mbedtls_mpi_uint *Mi, size_t num_words)
{
    int ret = 0;
    size_t num_bits;
    mbedtls_mpi RR;

    /* Calculate number of bits */
    num_bits = num_words * 32;
    ESP_LOGI(TAG, "num_bits = %d\n", num_bits);

    /* 
     *  R = b^n where b = 2^32, n=num_words,
     *  R = 2^N (where N=num_bits)
     *  RR(R^2) = 2^(2*N) (where N=num_bits)
     *
     *  r = RR(R^2) mod M
     *
     *  Get the RR(RR == r) value from up level if RR and RR->p is not NULL
     */
    ESP_LOGI(TAG, "r = RR(R^2) mod M\n");
    if (_RR == NULL || _RR->p == NULL) {
        ESP_LOGI(TAG, "RR(R^2) = 2^(2*N) (where N=num_bits)\n");
        mbedtls_mpi_init(&RR);
        MBEDTLS_MPI_CHK(mbedtls_mpi_set_bit(&RR, num_bits * 2, 1));
        mbedtls_mpi_printf("RR", &RR);
        
        MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(r, &RR, M));
        
        if (_RR != NULL)
            memcpy(_RR, r, sizeof( mbedtls_mpi ) );
    } else {
        memcpy(r, _RR, sizeof( mbedtls_mpi ) );
    }
    mbedtls_mpi_printf("r", r);

    *Mi = modular_inverse(M);

cleanup:
    mbedtls_mpi_free(&RR);

    return ret;
}

static void bignum_param_deinit(mbedtls_mpi *_RR, mbedtls_mpi *r)
{
    if (_RR == NULL || _RR->p == NULL)
        mbedtls_mpi_free(r);
}

/*
 * Sliding-window exponentiation: Z = X^Y mod M  (HAC 14.85)
 */
int mbedtls_mpi_exp_mod( mbedtls_mpi* Z, const mbedtls_mpi* X, const mbedtls_mpi* Y, const mbedtls_mpi* M, mbedtls_mpi* _RR )
{
    int ret = 0;
    size_t z_words = hardware_words_needed(Z);
    size_t x_words = hardware_words_needed(X);
    size_t y_words = hardware_words_needed(Y);
    size_t m_words = hardware_words_needed(M);
    size_t num_words;

    mbedtls_mpi r;
    mbedtls_mpi_uint Mi = 0;

    /* "all numbers must be the same length", so choose longest number
       as cardinal length of operation...
    */
    num_words = z_words;
    if (x_words > num_words) {
        num_words = x_words;
    }
    if (y_words > num_words) {
        num_words = y_words;
    }
    if (m_words > num_words) {
        num_words = m_words;
    }
    ESP_LOGI(TAG, "num_words = %d  # %d, %d, %d\n", num_words, x_words, y_words, m_words);

    if (num_words * 32 > 4096)
        return MBEDTLS_ERR_MPI_NOT_ACCEPTABLE;
    
    mbedtls_mpi_init(&r);
    ret = bignum_param_init(M, _RR, &r, &Mi, num_words);
    if (ret != 0) {
        return ret;
    }

    mbedtls_mpi_printf("X",X);
    mbedtls_mpi_printf("Y",Y);

    esp_mpi_acquire_hardware();

    /* "mode" register loaded with number of 512-bit blocks, minus 1 */
    REG_WRITE(RSA_MODEXP_MODE_REG, (num_words / 16) - 1);

    /* Load M, X, Rinv, M-prime (M-prime is mod 2^32) */
    mpi_to_mem_block(RSA_MEM_X_BLOCK_BASE, X, num_words);
    mpi_to_mem_block(RSA_MEM_Y_BLOCK_BASE, Y, num_words);
    mpi_to_mem_block(RSA_MEM_M_BLOCK_BASE, M, num_words);
    mpi_to_mem_block(RSA_MEM_RB_BLOCK_BASE, &r, num_words);
    REG_WRITE(RSA_M_DASH_REG, Mi);

    execute_op(RSA_START_MODEXP_REG);

    ret = mem_block_to_mpi(Z, RSA_MEM_Z_BLOCK_BASE, num_words);

    esp_mpi_release_hardware();

    mbedtls_mpi_printf("Z",Z);
    ESP_LOGI(TAG, "print (Z == (X ** Y) %% M)\n");

    bignum_param_deinit(_RR, &r);

    return ret;
}


#endif

#endif /* MBEDTLS_MPI_EXP_MOD_ALT */


/* The common parts of modulo multiplication and modular sliding
 *  window exponentiation:
 *
 * @param X first multiplication factor and/or base of exponent.
 * @param M modulo value for result
 * @param num_words size of modulo operation, in words (limbs).
 *        Should already be rounded up to a multiple of 16 words (512 bits) & range checked.
 *
 * Steps:
 * Calculate Rinv & Mprime based on M & num_words
 * Load all coefficients to memory
 * Set mode register
 *
 * @note This function calls esp_mpi_acquire_hardware. If successful,
 * returns 0 and it becomes the callers responsibility to call
 * esp_mpi_release_hardware(). If failure is returned, the caller does
 * not need to call esp_mpi_release_hardware().
 */
static int modular_op_prepare(const mbedtls_mpi *X, const mbedtls_mpi *M, size_t num_words)
{
    int ret = 0;
    mbedtls_mpi RR, Rinv, Mprime;
    size_t num_bits;

    /* Calculate number of bits */
    num_bits = num_words * 32;

    if(num_bits > 4096) {
        return MBEDTLS_ERR_MPI_NOT_ACCEPTABLE;
    }

    /* Rinv & Mprime are calculated via extended binary gcd
       algorithm, see references on extended_binary_gcd() above.
    */
    mbedtls_mpi_init(&Rinv);
    mbedtls_mpi_init(&RR);
    mbedtls_mpi_init(&Mprime);

    mbedtls_mpi_set_bit(&RR, num_bits, 1); /* R = b^n where b = 2^32, n=num_words,
                                              ie R = 2^N (where N=num_bits) */
    /* calculate Rinv & Mprime */
    extended_binary_gcd(&RR, M, &Rinv, &Mprime);

    /* Block of debugging data, output suitable to paste into Python
       TODO remove
    */
    mbedtls_mpi_printf("RR", &RR);
    mbedtls_mpi_printf("M", M);
    mbedtls_mpi_printf("Rinv", &Rinv);
    mbedtls_mpi_printf("Mprime", &Mprime);
    printf("print (R * Rinv - M * Mprime == 1)\n");
    printf("print (Rinv == (R * R) %% M)\n");

    esp_mpi_acquire_hardware();

    /* Load M, X, Rinv, M-prime (M-prime is mod 2^32) */
    mpi_to_mem_block(RSA_MEM_M_BLOCK_BASE, M, num_words);
    mpi_to_mem_block(RSA_MEM_X_BLOCK_BASE, X, num_words);
    mpi_to_mem_block(RSA_MEM_RB_BLOCK_BASE, &Rinv, num_words);
    REG_WRITE(RSA_M_DASH_REG, Mprime.p[0]);

    /* "mode" register loaded with number of 512-bit blocks, minus 1 */
    REG_WRITE(RSA_MULT_MODE_REG, (num_words / 16) - 1);

    mbedtls_mpi_free(&Rinv);
    mbedtls_mpi_free(&RR);
    mbedtls_mpi_free(&Mprime);

    return ret;
}

/* Second & final step of a modular multiply - load second multiplication
 * factor Y, run the multiply, read back the result into Z.
 *
 * @param Z result value
 * @param X first multiplication factor (used to set sign of result).
 * @param Y second multiplication factor.
 * @param num_words size of modulo operation, in words (limbs).
 *        Should already be rounded up to a multiple of 16 words (512 bits) & range checked.
 *
 *  Caller must have already called esp_mpi_acquire_hardware().
 */
inline static int modular_multiply_finish(mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, size_t num_words)
{
    int ret;
    /* Load Y to X input memory block, rerun */
    mpi_to_mem_block(RSA_MEM_X_BLOCK_BASE, Y, num_words);

    execute_op(RSA_MULT_START_REG);

    /* Read result into Z */
    ret = mem_block_to_mpi(Z, RSA_MEM_Z_BLOCK_BASE, num_words);

    Z->s = X->s * Y->s;

    return ret;
}

#if defined(MBEDTLS_MPI_MUL_MPI_ALT) /* MBEDTLS_MPI_MUL_MPI_ALT */

static int mpi_mult_mpi_failover_mod_mult(mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, size_t num_words);

/* Z = X * Y */
int mbedtls_mpi_mul_mpi( mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y )
{
    int ret;
    size_t words_x, words_y, words_mult, words_z;

    /* Count words needed for X & Y in hardware */
    words_x = hardware_words_needed(X);
    words_y = hardware_words_needed(Y);

    words_mult = (words_x > words_y ? words_x : words_y);

    /* Result Z has to have room for double the larger factor */
    words_z = words_mult * 2;

    /* If either factor is over 2048 bits, we can't use the standard hardware multiplier
       (it assumes result is double longest factor, and result is max 4096 bits.)

       However, we can fail over to mod_mult for up to 4096 bits of result (modulo
       multiplication doesn't have the same restriction, so result is simply the
       number of bits in X plus number of bits in in Y.)
    */
    //ESP_LOGE(TAG, "INFO: %d bit result (%d bits * %d bits)\n", words_z * 32, mbedtls_mpi_bitlen(X), mbedtls_mpi_bitlen(Y));
    if (words_mult * 32 > 2048) {
        /* Calculate new length of Z */
        words_z = words_x + words_y;
        if (words_z * 32 > 4096) {
            ESP_LOGE(TAG, "ERROR: %d bit result (%d bits * %d bits) too large for hardware unit\n", words_z * 32, mbedtls_mpi_bitlen(X), mbedtls_mpi_bitlen(Y));
            return MBEDTLS_ERR_MPI_NOT_ACCEPTABLE;
        }
        else {
            return mpi_mult_mpi_failover_mod_mult(Z, X, Y, words_z);
        }
    }

    /* Otherwise, we can use the (faster) multiply hardware unit */

    esp_mpi_acquire_hardware();

    /* Copy X (right-extended) & Y (left-extended) to memory block */
    mpi_to_mem_block(RSA_MEM_X_BLOCK_BASE, X, words_mult);
    mpi_to_mem_block(RSA_MEM_Z_BLOCK_BASE + words_mult * 4, Y, words_mult);
    /* NB: as Y is left-extended, we don't zero the bottom words_mult words of Y block.
       This is OK for now because zeroing is done by hardware when we do esp_mpi_acquire_hardware().
    */

    REG_WRITE(RSA_M_DASH_REG, 0);

    /* "mode" register loaded with number of 512-bit blocks in result,
       plus 7 (for range 9-12). (this is ((N~ / 32) - 1) + 8))
     */
    REG_WRITE(RSA_MULT_MODE_REG, (words_z / 16) + 7);

    execute_op(RSA_MULT_START_REG);

    /* Read back the result */
    ret = mem_block_to_mpi(Z, RSA_MEM_Z_BLOCK_BASE, words_z);

    Z->s = X->s * Y->s;

    esp_mpi_release_hardware();

    return ret;
}

/* Special-case of mbedtls_mpi_mult_mpi(), where we use hardware montgomery mod
   multiplication to solve the case where A or B are >2048 bits so
   can't use the standard multiplication method.

   This case is simpler than esp_mpi_mul_mpi_mod() as we control the arguments:

   * Modulus is chosen with M=(2^num_bits - 1) (ie M=R-1), so output
     isn't actually modulo anything.
   * Therefore of of M' and Rinv are predictable as follows:
      M' = 1
      Rinv = 1

   (See RSA Accelerator section in Technical Reference *
   extended_binary_gcd() function above for more about M', Rinv)
*/
static int mpi_mult_mpi_failover_mod_mult(mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, size_t num_words)
 {
     int ret = 0;

     /* Load coefficients to hardware */
     esp_mpi_acquire_hardware();

     /* M = 2^num_words - 1, so block is entirely FF */
     for(int i = 0; i < num_words; i++) {
         REG_WRITE(RSA_MEM_M_BLOCK_BASE + i * 4, UINT32_MAX);
     }
     /* Mprime = 1 */
     REG_WRITE(RSA_M_DASH_REG, 1);

     /* "mode" register loaded with number of 512-bit blocks, minus 1 */
     REG_WRITE(RSA_MULT_MODE_REG, (num_words / 16) - 1);

     /* Load X */
     mpi_to_mem_block(RSA_MEM_X_BLOCK_BASE, X, num_words);

     /* Rinv = 1 */
     REG_WRITE(RSA_MEM_RB_BLOCK_BASE, 1);
     for(int i = 1; i < num_words; i++) {
         REG_WRITE(RSA_MEM_RB_BLOCK_BASE + i * 4, 0);
     }

     execute_op(RSA_MULT_START_REG);

     MBEDTLS_MPI_CHK( modular_multiply_finish(Z, X, Y, num_words) );

     esp_mpi_release_hardware();

 cleanup:
     return ret;
}

#endif /* MBEDTLS_MPI_MUL_MPI_ALT */

#endif /* MBEDTLS_MPI_MUL_MPI_ALT || MBEDTLS_MPI_EXP_MOD_ALT */

