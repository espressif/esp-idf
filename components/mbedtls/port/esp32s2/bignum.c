/*
 * Multi-precision integer library
 * ESP32 S2 hardware accelerated parts based on mbedTLS implementation
 *
 * SPDX-FileCopyrightText: The Mbed TLS Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2016-2022 Espressif Systems (Shanghai) CO LTD
 */
#include "soc/hwcrypto_periph.h"
#include "esp_private/periph_ctrl.h"
#include <mbedtls/bignum.h>
#include "bignum_impl.h"
#include "soc/dport_reg.h"
#include "soc/periph_defs.h"
#include <sys/param.h>
#include "esp_crypto_lock.h"

size_t esp_mpi_hardware_words(size_t words)
{
    return words;
}

void esp_mpi_enable_hardware_hw_op( void )
{
    esp_crypto_mpi_lock_acquire();

    /* Enable RSA hardware */
    periph_module_enable(PERIPH_RSA_MODULE);

    DPORT_REG_CLR_BIT(DPORT_RSA_PD_CTRL_REG, DPORT_RSA_MEM_PD);

    while (DPORT_REG_READ(RSA_QUERY_CLEAN_REG) != 1) {
    }
    // Note: from enabling RSA clock to here takes about 1.3us

    REG_WRITE(RSA_INTERRUPT_REG, 0);
}

void esp_mpi_disable_hardware_hw_op( void )
{
    DPORT_REG_SET_BIT(DPORT_RSA_PD_CTRL_REG, DPORT_RSA_PD);

    /* Disable RSA hardware */
    periph_module_disable(PERIPH_RSA_MODULE);

    esp_crypto_mpi_lock_release();
}

void esp_mpi_interrupt_enable( bool enable )
{
    REG_WRITE(RSA_INTERRUPT_REG, enable);
}

void esp_mpi_interrupt_clear( void )
{
    REG_WRITE(RSA_CLEAR_INTERRUPT_REG, 1);
}

/* Copy mbedTLS MPI bignum 'mpi' to hardware memory block at 'mem_base'.

   If num_words is higher than the number of words in the bignum then
   these additional words will be zeroed in the memory buffer.
*/
static inline void mpi_to_mem_block(uint32_t mem_base, const mbedtls_mpi *mpi, size_t num_words)
{
    uint32_t *pbase = (uint32_t *)mem_base;
    uint32_t copy_words = MIN(num_words, mpi->MBEDTLS_PRIVATE(n));

    /* Copy MPI data to memory block registers */
    for (uint32_t i = 0; i < copy_words; i++) {
        pbase[i] = mpi->MBEDTLS_PRIVATE(p)[i];
    }

    /* Zero any remaining memory block data */
    for (uint32_t i = copy_words; i < num_words; i++) {
        pbase[i] = 0;
    }
}

/* Read mbedTLS MPI bignum back from hardware memory block.

   Reads num_words words from block.
*/
static inline void mem_block_to_mpi(mbedtls_mpi *x, uint32_t mem_base, int num_words)
{

    /* Copy data from memory block registers */
    esp_dport_access_read_buffer(x->MBEDTLS_PRIVATE(p), mem_base, num_words);
    /* Zero any remaining limbs in the bignum, if the buffer is bigger
       than num_words */
    for (size_t i = num_words; i < x->MBEDTLS_PRIVATE(n); i++) {
        x->MBEDTLS_PRIVATE(p)[i] = 0;
    }
}



/* Begin an RSA operation. op_reg specifies which 'START' register
   to write to.
*/
static inline void start_op(uint32_t op_reg)
{
    /* Clear interrupt status */
    DPORT_REG_WRITE(RSA_CLEAR_INTERRUPT_REG, 1);

    /* Note: above REG_WRITE includes a memw, so we know any writes
       to the memory blocks are also complete. */

    DPORT_REG_WRITE(op_reg, 1);
}

/* Wait for an RSA operation to complete.
*/
static inline void wait_op_complete(void)
{
    while (DPORT_REG_READ(RSA_QUERY_INTERRUPT_REG) != 1)
    { }

    /* clear the interrupt */
    DPORT_REG_WRITE(RSA_CLEAR_INTERRUPT_REG, 1);
}


/* Read result from last MPI operation */
void esp_mpi_read_result_hw_op(mbedtls_mpi *Z, size_t z_words)
{
    wait_op_complete();
    mem_block_to_mpi(Z, RSA_MEM_Z_BLOCK_BASE, z_words);
}


/* Z = (X * Y) mod M

   Not an mbedTLS function
*/
void esp_mpi_mul_mpi_mod_hw_op(const mbedtls_mpi *X, const mbedtls_mpi *Y, const mbedtls_mpi *M, const mbedtls_mpi *Rinv, mbedtls_mpi_uint Mprime, size_t num_words)
{
    DPORT_REG_WRITE(RSA_LENGTH_REG, (num_words - 1));

    /* Load M, X, Rinv, Mprime (Mprime is mod 2^32) */
    mpi_to_mem_block(RSA_MEM_X_BLOCK_BASE, X, num_words);
    mpi_to_mem_block(RSA_MEM_Y_BLOCK_BASE, Y, num_words);
    mpi_to_mem_block(RSA_MEM_M_BLOCK_BASE, M, num_words);
    mpi_to_mem_block(RSA_MEM_RB_BLOCK_BASE, Rinv, num_words);
    DPORT_REG_WRITE(RSA_M_DASH_REG, Mprime);

    start_op(RSA_MOD_MULT_START_REG);
}

/* Z = (X ^ Y) mod M
*/
void esp_mpi_exp_mpi_mod_hw_op(const mbedtls_mpi *X, const mbedtls_mpi *Y, const mbedtls_mpi *M, const mbedtls_mpi *Rinv, mbedtls_mpi_uint Mprime, size_t num_words)
{
    size_t y_bits = mbedtls_mpi_bitlen(Y);

    DPORT_REG_WRITE(RSA_LENGTH_REG, (num_words - 1));

    /* Load M, X, Rinv, Mprime (Mprime is mod 2^32) */
    mpi_to_mem_block(RSA_MEM_X_BLOCK_BASE, X, num_words);
    mpi_to_mem_block(RSA_MEM_Y_BLOCK_BASE, Y, num_words);
    mpi_to_mem_block(RSA_MEM_M_BLOCK_BASE, M, num_words);
    mpi_to_mem_block(RSA_MEM_RB_BLOCK_BASE, Rinv, num_words);
    DPORT_REG_WRITE(RSA_M_DASH_REG, Mprime);

    /* Enable acceleration options */
    DPORT_REG_WRITE(RSA_CONSTANT_TIME_REG, 0);
    DPORT_REG_WRITE(RSA_SEARCH_OPEN_REG, 1);
    DPORT_REG_WRITE(RSA_SEARCH_POS_REG, y_bits - 1);

    /* Execute first stage montgomery multiplication */
    start_op(RSA_MODEXP_START_REG);

    DPORT_REG_WRITE(RSA_SEARCH_OPEN_REG, 0);
}


/* Z = X * Y */
void esp_mpi_mul_mpi_hw_op(const mbedtls_mpi *X, const mbedtls_mpi *Y, size_t num_words)
{
    /* Copy X (right-extended) & Y (left-extended) to memory block */
    mpi_to_mem_block(RSA_MEM_X_BLOCK_BASE, X, num_words);
    mpi_to_mem_block(RSA_MEM_Z_BLOCK_BASE + num_words * 4, Y, num_words);
    /* NB: as Y is left-extended, we don't zero the bottom words_mult words of Y block.
       This is OK for now because zeroing is done by hardware when we do esp_mpi_acquire_hardware().
    */
    DPORT_REG_WRITE(RSA_LENGTH_REG, (num_words * 2 - 1));
    start_op(RSA_MULT_START_REG);
}



/**
 * @brief Special-case of (X * Y), where we use hardware montgomery mod
   multiplication to calculate result where either A or B are >2048 bits so
   can't use the standard multiplication method.
 *
 */
void esp_mpi_mult_mpi_failover_mod_mult_hw_op(const mbedtls_mpi *X, const mbedtls_mpi *Y, size_t num_words)
{
    /* M = 2^num_words - 1, so block is entirely FF */
    for (size_t i = 0; i < num_words; i++) {
        DPORT_REG_WRITE(RSA_MEM_M_BLOCK_BASE + i * 4, UINT32_MAX);
    }

    /* Mprime = 1 */
    DPORT_REG_WRITE(RSA_M_DASH_REG, 1);
    DPORT_REG_WRITE(RSA_LENGTH_REG, num_words - 1);

    /* Load X & Y */
    mpi_to_mem_block(RSA_MEM_X_BLOCK_BASE, X, num_words);
    mpi_to_mem_block(RSA_MEM_Y_BLOCK_BASE, Y, num_words);

    /* Rinv = 1, write first word */
    DPORT_REG_WRITE(RSA_MEM_RB_BLOCK_BASE, 1);

     /* Zero out rest of the Rinv words */
    for (size_t i = 1; i < num_words; i++) {
        DPORT_REG_WRITE(RSA_MEM_RB_BLOCK_BASE + i * 4, 0);
    }

    start_op(RSA_MOD_MULT_START_REG);
}
