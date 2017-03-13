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
#include <stdlib.h>
#include "mbedtls/bignum.h"
#include "rom/bigint.h"
#include "soc/hwcrypto_reg.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_intr.h"
#include "esp_intr_alloc.h"
#include "esp_attr.h"

#include "soc/dport_reg.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

static const __attribute__((unused)) char *TAG = "bignum";

#define ciL    (sizeof(mbedtls_mpi_uint))         /* chars in limb  */
#define biL    (ciL << 3)                         /* bits  in limb  */

#if defined(CONFIG_MBEDTLS_MPI_USE_INTERRUPT)
static SemaphoreHandle_t op_complete_sem;

static IRAM_ATTR void rsa_complete_isr(void *arg)
{
    BaseType_t higher_woken;
    REG_WRITE(RSA_INTERRUPT_REG, 1);
    xSemaphoreGiveFromISR(op_complete_sem, &higher_woken);
    if (higher_woken) {
        portYIELD_FROM_ISR();
    }
}

static void rsa_isr_initialise()
{
    if (op_complete_sem == NULL) {
        op_complete_sem = xSemaphoreCreateBinary();
        esp_intr_alloc(ETS_RSA_INTR_SOURCE, 0, rsa_complete_isr, NULL, NULL);
    }
}

#endif /* CONFIG_MBEDTLS_MPI_USE_INTERRUPT */

static _lock_t mpi_lock;

void esp_mpi_acquire_hardware( void )
{
    /* newlib locks lazy initialize on ESP-IDF */
    _lock_acquire(&mpi_lock);

    REG_SET_BIT(DPORT_PERI_CLK_EN_REG, DPORT_PERI_EN_RSA);
    /* also clear reset on digital signature, otherwise RSA is held in reset */
    REG_CLR_BIT(DPORT_PERI_RST_EN_REG,
                DPORT_PERI_EN_RSA
                | DPORT_PERI_EN_DIGITAL_SIGNATURE);

    REG_CLR_BIT(DPORT_RSA_PD_CTRL_REG, DPORT_RSA_PD);

    while(REG_READ(RSA_CLEAN_REG) != 1);

    // Note: from enabling RSA clock to here takes about 1.3us

#ifdef CONFIG_MBEDTLS_MPI_USE_INTERRUPT
    rsa_isr_initialise();
#endif
}

void esp_mpi_release_hardware( void )
{
    REG_SET_BIT(DPORT_RSA_PD_CTRL_REG, DPORT_RSA_PD);

    /* don't reset digital signature unit, as this resets AES also */
    REG_SET_BIT(DPORT_PERI_RST_EN_REG, DPORT_PERI_EN_RSA);
    REG_CLR_BIT(DPORT_PERI_CLK_EN_REG, DPORT_PERI_EN_RSA);

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

/* Convert number of bits to number of words, rounded up to nearest
   512 bit (16 word) block count.
*/
static inline size_t bits_to_hardware_words(size_t num_bits)
{
    return ((num_bits + 511) / 512) * 16;
}

/* Copy mbedTLS MPI bignum 'mpi' to hardware memory block at 'mem_base'.

   If num_words is higher than the number of words in the bignum then
   these additional words will be zeroed in the memory buffer.
*/
static inline void mpi_to_mem_block(uint32_t mem_base, const mbedtls_mpi *mpi, size_t num_words)
{
    uint32_t *pbase = (uint32_t *)mem_base;
    uint32_t copy_words = num_words < mpi->n ? num_words : mpi->n;

    /* Copy MPI data to memory block registers */
    memcpy(pbase, mpi->p, copy_words * 4);

    /* Zero any remaining memory block data */
    bzero(pbase + copy_words, (num_words - copy_words) * 4);

    /* Note: not executing memw here, can do it before we start a bignum operation */
}

/* Read mbedTLS MPI bignum back from hardware memory block.

   Reads num_words words from block.

   Can return a failure result if fails to grow the MPI result.
*/
static inline int mem_block_to_mpi(mbedtls_mpi *x, uint32_t mem_base, int num_words)
{
    int ret = 0;

    MBEDTLS_MPI_CHK( mbedtls_mpi_grow(x, num_words) );

    /* Copy data from memory block registers */
    memcpy(x->p, (uint32_t *)mem_base, num_words * 4);

    /* Zero any remaining limbs in the bignum, if the buffer is bigger
       than num_words */
    for(size_t i = num_words; i < x->n; i++) {
        x->p[i] = 0;
    }

    asm volatile ("memw");
 cleanup:
    return ret;
}


/**
 *
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

/* Calculate Rinv = RR^2 mod M, where:
 *
 *  R = b^n where b = 2^32, n=num_words,
 *  R = 2^N (where N=num_bits)
 *  RR = R^2 = 2^(2*N) (where N=num_bits=num_words*32)
 *
 * This calculation is computationally expensive (mbedtls_mpi_mod_mpi)
 * so caller should cache the result where possible.
 *
 * DO NOT call this function while holding esp_mpi_acquire_hardware().
 *
 */
static int calculate_rinv(mbedtls_mpi *Rinv, const mbedtls_mpi *M, int num_words)
{
    int ret;
    size_t num_bits = num_words * 32;
    mbedtls_mpi RR;
    mbedtls_mpi_init(&RR);
    MBEDTLS_MPI_CHK(mbedtls_mpi_set_bit(&RR, num_bits * 2, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(Rinv, &RR, M));

 cleanup:
    mbedtls_mpi_free(&RR);
    return ret;
}


/* Execute RSA operation. op_reg specifies which 'START' register
   to write to.
*/
static inline void execute_op(uint32_t op_reg)
{
    /* Clear interrupt status */
    REG_WRITE(RSA_INTERRUPT_REG, 1);

    /* Note: above REG_WRITE includes a memw, so we know any writes
       to the memory blocks are also complete. */

    REG_WRITE(op_reg, 1);

#ifdef CONFIG_MBEDTLS_MPI_USE_INTERRUPT
    if (!xSemaphoreTake(op_complete_sem, 2000 / portTICK_PERIOD_MS)) {
        ESP_LOGE(TAG, "Timed out waiting for RSA operation (op_reg 0x%x int_reg 0x%x)",
                 op_reg, REG_READ(RSA_INTERRUPT_REG));
        abort(); /* indicates a fundamental problem with driver */
    }
#else
    while(REG_READ(RSA_INTERRUPT_REG) != 1)
       { }
#endif

    /* clear the interrupt */
    REG_WRITE(RSA_INTERRUPT_REG, 1);
}

/* Sub-stages of modulo multiplication/exponentiation operations */
inline static int modular_multiply_finish(mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, size_t num_words);

/* Z = (X * Y) mod M

   Not an mbedTLS function
*/
int esp_mpi_mul_mpi_mod(mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, const mbedtls_mpi *M)
{
    int ret;
    size_t num_words = hardware_words_needed(M);
    mbedtls_mpi Rinv;
    mbedtls_mpi_uint Mprime;

    /* Calculate and load the first stage montgomery multiplication */
    mbedtls_mpi_init(&Rinv);
    MBEDTLS_MPI_CHK(calculate_rinv(&Rinv, M, num_words));
    Mprime = modular_inverse(M);

    esp_mpi_acquire_hardware();

    /* Load M, X, Rinv, Mprime (Mprime is mod 2^32) */
    mpi_to_mem_block(RSA_MEM_M_BLOCK_BASE, M, num_words);
    mpi_to_mem_block(RSA_MEM_X_BLOCK_BASE, X, num_words);
    mpi_to_mem_block(RSA_MEM_RB_BLOCK_BASE, &Rinv, num_words);
    REG_WRITE(RSA_M_DASH_REG, (uint32_t)Mprime);

    /* "mode" register loaded with number of 512-bit blocks, minus 1 */
    REG_WRITE(RSA_MULT_MODE_REG, (num_words / 16) - 1);

    /* Execute first stage montgomery multiplication */
    execute_op(RSA_MULT_START_REG);

    /* execute second stage */
    MBEDTLS_MPI_CHK( modular_multiply_finish(Z, X, Y, num_words) );

    esp_mpi_release_hardware();

 cleanup:
    mbedtls_mpi_free(&Rinv);
    return ret;
}

#if defined(MBEDTLS_MPI_EXP_MOD_ALT)

/*
 * Sliding-window exponentiation: Z = X^Y mod M  (HAC 14.85)
 *
 * _Rinv is optional pre-calculated version of Rinv (via calculate_rinv()).
 *
 * (See RSA Accelerator section in Technical Reference for more about Mprime, Rinv)
 *
 */
int mbedtls_mpi_exp_mod( mbedtls_mpi* Z, const mbedtls_mpi* X, const mbedtls_mpi* Y, const mbedtls_mpi* M, mbedtls_mpi* _Rinv )
{
    int ret = 0;
    size_t z_words = hardware_words_needed(Z);
    size_t x_words = hardware_words_needed(X);
    size_t y_words = hardware_words_needed(Y);
    size_t m_words = hardware_words_needed(M);
    size_t num_words;

    mbedtls_mpi Rinv_new; /* used if _Rinv == NULL */
    mbedtls_mpi *Rinv;    /* points to _Rinv (if not NULL) othwerwise &RR_new */
    mbedtls_mpi_uint Mprime;

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

    if (num_words * 32 > 4096) {
        return MBEDTLS_ERR_MPI_NOT_ACCEPTABLE;
    }

    /* Determine RR pointer, either _RR for cached value
       or local RR_new */
    if (_Rinv == NULL) {
        mbedtls_mpi_init(&Rinv_new);
        Rinv = &Rinv_new;
    } else {
        Rinv = _Rinv;
    }
    if (Rinv->p == NULL) {
        MBEDTLS_MPI_CHK(calculate_rinv(Rinv, M, num_words));
    }

    Mprime = modular_inverse(M);

    esp_mpi_acquire_hardware();

    /* "mode" register loaded with number of 512-bit blocks, minus 1 */
    REG_WRITE(RSA_MODEXP_MODE_REG, (num_words / 16) - 1);

    /* Load M, X, Rinv, M-prime (M-prime is mod 2^32) */
    mpi_to_mem_block(RSA_MEM_X_BLOCK_BASE, X, num_words);
    mpi_to_mem_block(RSA_MEM_Y_BLOCK_BASE, Y, num_words);
    mpi_to_mem_block(RSA_MEM_M_BLOCK_BASE, M, num_words);
    mpi_to_mem_block(RSA_MEM_RB_BLOCK_BASE, Rinv, num_words);
    REG_WRITE(RSA_M_DASH_REG, Mprime);

    execute_op(RSA_START_MODEXP_REG);

    ret = mem_block_to_mpi(Z, RSA_MEM_Z_BLOCK_BASE, num_words);

    esp_mpi_release_hardware();

 cleanup:
    if (_Rinv == NULL) {
        mbedtls_mpi_free(&Rinv_new);
    }

    return ret;
}

#endif /* MBEDTLS_MPI_EXP_MOD_ALT */

/* Second & final step of a modular multiply - load second multiplication
 * factor Y, run the multiply, read back the result into Z.
 *
 * Called from both mbedtls_mpi_exp_mod and mbedtls_mpi_mod_mpi.
 *
 * @param Z result value
 * @param X first multiplication factor (used to set sign of result).
 * @param Y second multiplication factor.
 * @param num_words size of modulo operation, in words (limbs).
 *        Should already be rounded up to a multiple of 16 words (512 bits) & range checked.
 *
 *  Caller must have already called esp_mpi_acquire_hardware().
 */
static int modular_multiply_finish(mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, size_t num_words)
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
static int mpi_mult_mpi_overlong(mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, size_t Y_bits, size_t words_result);

/* Z = X * Y */
int mbedtls_mpi_mul_mpi( mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y )
{
    int ret;
    size_t bits_x, bits_y, words_x, words_y, words_mult, words_z;

    /* Count words needed for X & Y in hardware */
    bits_x = mbedtls_mpi_bitlen(X);
    bits_y = mbedtls_mpi_bitlen(Y);
    /* Convert bit counts to words, rounded up to 512-bit
       (16 word) blocks */
    words_x = bits_to_hardware_words(bits_x);
    words_y = bits_to_hardware_words(bits_y);

    /* Short-circuit eval if either argument is 0 or 1.

       This is needed as the mpi modular division
       argument will sometimes call in here when one
       argument is too large for the hardware unit, but the other
       argument is zero or one.

       This leaks some timing information, although overall there is a
       lot less timing variation than a software MPI approach.
    */
    if (bits_x == 0 || bits_y == 0) {
        mbedtls_mpi_lset(Z, 0);
        return 0;
    }
    if (bits_x == 1) {
        return mbedtls_mpi_copy(Z, Y);
    }
    if (bits_y == 1) {
        return mbedtls_mpi_copy(Z, X);
    }

    words_mult = (words_x > words_y ? words_x : words_y);

    /* Result Z has to have room for double the larger factor */
    words_z = words_mult * 2;


    /* If either factor is over 2048 bits, we can't use the standard hardware multiplier
       (it assumes result is double longest factor, and result is max 4096 bits.)

       However, we can fail over to mod_mult for up to 4096 bits of result (modulo
       multiplication doesn't have the same restriction, so result is simply the
       number of bits in X plus number of bits in in Y.)
    */
    if (words_mult * 32 > 2048) {
        /* Calculate new length of Z */
        words_z = bits_to_hardware_words(bits_x + bits_y);
        if (words_z * 32 <= 4096) {
            /* Note: it's possible to use mpi_mult_mpi_overlong
               for this case as well, but it's very slightly
               slower and requires a memory allocation.
            */
            return mpi_mult_mpi_failover_mod_mult(Z, X, Y, words_z);
        } else {
            /* Still too long for the hardware unit... */
            if(bits_y > bits_x) {
                return mpi_mult_mpi_overlong(Z, X, Y, bits_y, words_z);
            } else {
                return mpi_mult_mpi_overlong(Z, Y, X, bits_x, words_z);
            }
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
   multiplication to calculate an mbedtls_mpi_mult_mpi result where either
   A or B are >2048 bits so can't use the standard multiplication method.

   Result (A bits + B bits) must still be less than 4096 bits.

   This case is simpler than the general case modulo multiply of
   esp_mpi_mul_mpi_mod() because we can control the other arguments:

   * Modulus is chosen with M=(2^num_bits - 1) (ie M=R-1), so output
   isn't actually modulo anything.
   * Mprime and Rinv are therefore predictable as follows:
   Mprime = 1
   Rinv = 1

   (See RSA Accelerator section in Technical Reference for more about Mprime, Rinv)
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

    /* finish the modular multiplication */
    MBEDTLS_MPI_CHK( modular_multiply_finish(Z, X, Y, num_words) );

    esp_mpi_release_hardware();

 cleanup:
    return ret;
}

/* Deal with the case when X & Y are too long for the hardware unit, by splitting one operand
   into two halves.

   Y must be the longer operand

   Slice Y into Yp, Ypp such that:
   Yp = lower 'b' bits of Y
   Ypp = upper 'b' bits of Y (right shifted)

   Such that
   Z = X * Y
   Z = X * (Yp + Ypp<<b)
   Z = (X * Yp) + (X * Ypp<<b)

   Note that this function may recurse multiple times, if both X & Y
   are too long for the hardware multiplication unit.
*/
static int mpi_mult_mpi_overlong(mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, size_t bits_y, size_t words_result)
{
    int ret;
    mbedtls_mpi Ztemp;
    const size_t limbs_y = (bits_y + biL - 1) / biL;
    /* Rather than slicing in two on bits we slice on limbs (32 bit words) */
    const size_t limbs_slice = limbs_y / 2;
    /* Yp holds lower bits of Y (declared to reuse Y's array contents to save on copying) */
    const mbedtls_mpi Yp = {
        .p = Y->p,
        .n = limbs_slice,
        .s = Y->s
    };
    /* Ypp holds upper bits of Y, right shifted (also reuses Y's array contents) */
    const mbedtls_mpi Ypp = {
        .p = Y->p + limbs_slice,
        .n = limbs_y - limbs_slice,
        .s = Y->s
    };
    mbedtls_mpi_init(&Ztemp);

    /* Grow Z to result size early, avoid interim allocations */
    mbedtls_mpi_grow(Z, words_result);

    /* Get result Ztemp = Yp * X (need temporary variable Ztemp) */
    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi(&Ztemp, X, &Yp) );

    /* Z = Ypp * Y */
    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi(Z, X, &Ypp) );

    /* Z = Z << b */
    MBEDTLS_MPI_CHK( mbedtls_mpi_shift_l(Z, limbs_slice * biL) );

    /* Z += Ztemp */
    MBEDTLS_MPI_CHK( mbedtls_mpi_add_mpi(Z, Z, &Ztemp) );

 cleanup:
    mbedtls_mpi_free(&Ztemp);

    return ret;
}

#endif /* MBEDTLS_MPI_MUL_MPI_ALT */

