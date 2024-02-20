/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_crypto_lock.h"
#include "bignum_impl.h"
#include "mbedtls/bignum.h"
#include "esp_private/esp_crypto_lock_internal.h"

#include "hal/mpi_hal.h"
#include "hal/mpi_ll.h"

void esp_mpi_enable_hardware_hw_op( void )
{
    esp_crypto_mpi_lock_acquire();

    /* Enable RSA hardware */
    MPI_RCC_ATOMIC() {
        mpi_ll_enable_bus_clock(true);
        mpi_ll_reset_register();
    }

    mpi_hal_enable_hardware_hw_op();
}


void esp_mpi_disable_hardware_hw_op( void )
{
    mpi_hal_disable_hardware_hw_op();

    /* Disable RSA hardware */
    MPI_RCC_ATOMIC() {
        mpi_ll_enable_bus_clock(false);
    }

    esp_crypto_mpi_lock_release();
}

size_t esp_mpi_hardware_words(size_t words)
{
    return mpi_hal_calc_hardware_words(words);
}


void esp_mpi_interrupt_enable(bool enable)
{
    mpi_hal_interrupt_enable(enable);
}


void esp_mpi_interrupt_clear(void)
{
    mpi_hal_clear_interrupt();
}


/* Z = (X * Y) mod M */
void esp_mpi_mul_mpi_mod_hw_op(const mbedtls_mpi *X, const mbedtls_mpi *Y, const mbedtls_mpi *M, const mbedtls_mpi *Rinv, mbedtls_mpi_uint Mprime, size_t num_words)
{
#if CONFIG_IDF_TARGET_ESP32
    /* "mode" register loaded with number of 512-bit blocks, minus 1 */
    mpi_hal_set_mode((num_words / 16) - 1);
#else
    mpi_hal_set_mode(num_words - 1);
#endif

    /* Load M, X, Rinv, Mprime (Mprime is mod 2^32) */
    mpi_hal_write_to_mem_block(MPI_PARAM_M, 0, M->MBEDTLS_PRIVATE(p), M->MBEDTLS_PRIVATE(n), num_words);
    mpi_hal_write_to_mem_block(MPI_PARAM_X, 0, X->MBEDTLS_PRIVATE(p), X->MBEDTLS_PRIVATE(n), num_words);

#if !CONFIG_IDF_TARGET_ESP32
    mpi_hal_write_to_mem_block(MPI_PARAM_Y, 0, Y->MBEDTLS_PRIVATE(p), Y->MBEDTLS_PRIVATE(n), num_words);
#endif

    mpi_hal_write_to_mem_block(MPI_PARAM_Z, 0, Rinv->MBEDTLS_PRIVATE(p), Rinv->MBEDTLS_PRIVATE(n), num_words);
    mpi_hal_write_m_prime(Mprime);

#if CONFIG_IDF_TARGET_ESP32
    mpi_hal_start_op(MPI_MULT);
    mpi_hal_wait_op_complete();
    /* execute second stage */
    /* Load Y to X input memory block, rerun */
    mpi_hal_write_to_mem_block(MPI_PARAM_X, 0, Y->MBEDTLS_PRIVATE(p), Y->MBEDTLS_PRIVATE(n), num_words);
    mpi_hal_start_op(MPI_MULT);
#else
    mpi_hal_start_op(MPI_MODMULT);
#endif

}


/* Z = X * Y */
void esp_mpi_mul_mpi_hw_op(const mbedtls_mpi *X, const mbedtls_mpi *Y, size_t num_words)
{
    /* Copy X (right-extended) & Y (left-extended) to memory block */
    mpi_hal_write_to_mem_block(MPI_PARAM_X, 0, X->MBEDTLS_PRIVATE(p), X->MBEDTLS_PRIVATE(n), num_words);
    mpi_hal_write_to_mem_block(MPI_PARAM_Z, num_words * 4, Y->MBEDTLS_PRIVATE(p), Y->MBEDTLS_PRIVATE(n), num_words);
    /* NB: as Y is left-exte, we don't zero the bottom words_mult words of Y block.
       This is OK for now bec zeroing is done by hardware when we do esp_mpi_acquire_hardware().
    */
#if CONFIG_IDF_TARGET_ESP32
    mpi_hal_write_m_prime(0);
    /* "mode" register loaded with number of 512-bit blocks in result,
       plus 7 (for range 9-12). (this is ((N~ / 32) - 1) + 8))
    */
    mpi_hal_set_mode(((num_words * 2) / 16) + 7);
#else
    mpi_hal_set_mode(num_words * 2 - 1);
#endif

    mpi_hal_start_op(MPI_MULT);
}


/* Special-case of mbedtls_mpi_mult_mpi(), where we use hardware montgomery mod
   multiplication to calculate an mbedtls_mpi_mult_mpi result where either
   A or B are >2048 bits so can't use the standard multiplication method.

   Result (number of words, based on A bits + B bits) must still be less than 4096 bits.

   This case is simpler than the general case modulo multiply of
   esp_mpi_mul_mpi_mod() because we can control the other arguments:

   * Modulus is chosen with M=(2^num_bits - 1) (ie M=R-1), so output
   * Mprime and Rinv are therefore predictable as follows:
   isn't actually modulo anything.
   Mprime 1
   Rinv 1

   (See RSA Accelerator section in Technical Reference for more about Mprime, Rinv)
*/

void esp_mpi_mult_mpi_failover_mod_mult_hw_op(const mbedtls_mpi *X, const mbedtls_mpi *Y, size_t num_words)
{
     /* M = 2^num_words - 1, so block is entirely FF */
    for (int i = 0; i < num_words; i++) {
        mpi_hal_write_at_offset(MPI_PARAM_M, i * 4, UINT32_MAX);
    }

    /* Mprime = 1 */
    mpi_hal_write_m_prime(1);

#if CONFIG_IDF_TARGET_ESP32
    /* "mode" register loaded with number of 512-bit blocks, minus 1 */
    mpi_hal_set_mode((num_words / 16) - 1);
#else
    mpi_hal_set_mode(num_words - 1);
#endif

    /* Load X & Y */
    mpi_hal_write_to_mem_block(MPI_PARAM_X, 0, X->MBEDTLS_PRIVATE(p), X->MBEDTLS_PRIVATE(n), num_words);
#if !CONFIG_IDF_TARGET_ESP32
    mpi_hal_write_to_mem_block(MPI_PARAM_Y, 0, Y->MBEDTLS_PRIVATE(p), Y->MBEDTLS_PRIVATE(n), num_words);
#endif
    /* Rinv = 1, write first word */
    mpi_hal_write_rinv(1);

    /* Zero out rest of the Rinv words */
    for (int i = 1; i < num_words; i++) {
        mpi_hal_write_at_offset(MPI_PARAM_Z, i * 4, 0);
    }

#if CONFIG_IDF_TARGET_ESP32
    mpi_hal_start_op(MPI_MULT);
    mpi_hal_wait_op_complete();
    mpi_hal_write_to_mem_block(MPI_PARAM_X, 0, Y->MBEDTLS_PRIVATE(p), Y->MBEDTLS_PRIVATE(n), num_words);
    mpi_hal_start_op(MPI_MULT);
#else
    mpi_hal_start_op(MPI_MODMULT);
#endif
}

#ifdef ESP_MPI_USE_MONT_EXP
int esp_mont_hw_op(mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, const mbedtls_mpi *M, mbedtls_mpi_uint Mprime, size_t hw_words, bool again)
{
    // Note Z may be the same pointer as X or Y
    int ret = 0;

    // montgomery mult prepare
    if (again == false) {
        mpi_hal_write_to_mem_block(MPI_PARAM_M, 0, M->MBEDTLS_PRIVATE(p), M->MBEDTLS_PRIVATE(n), hw_words);
        mpi_hal_write_m_prime(Mprime);
        mpi_hal_set_mode((hw_words / 16) - 1);
    }

    mpi_hal_write_to_mem_block(MPI_PARAM_X, 0, X->MBEDTLS_PRIVATE(p), X->MBEDTLS_PRIVATE(n), hw_words);
    mpi_hal_write_to_mem_block(MPI_PARAM_Z, 0, Y->MBEDTLS_PRIVATE(p), Y->MBEDTLS_PRIVATE(n), hw_words);

    mpi_hal_start_op(MPI_MULT);

    Z->MBEDTLS_PRIVATE(s) = 1; // The sign of Z will be = M->s (but M->s is always 1)
    MBEDTLS_MPI_CHK( mbedtls_mpi_grow(Z, hw_words) );

    /* Read back the result */
    mpi_hal_read_result_hw_op(Z->MBEDTLS_PRIVATE(p), Z->MBEDTLS_PRIVATE(n), hw_words);

    /* from HAC 14.36 - 3. If Z >= M then Z = Z - M */
    if (mbedtls_mpi_cmp_mpi(Z, M) >= 0) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(Z, Z, M));
    }
cleanup:
    return ret;
}

#else
/* Z = (X ^ Y) mod M
*/
void esp_mpi_exp_mpi_mod_hw_op(const mbedtls_mpi *X, const mbedtls_mpi *Y, const mbedtls_mpi *M, const mbedtls_mpi *Rinv, mbedtls_mpi_uint Mprime, size_t num_words)
{
    size_t y_bits = mbedtls_mpi_bitlen(Y);
    mpi_hal_set_mode(num_words - 1);

    /* Load M, X, Rinv, Mprime (Mprime is mod 2^32) */
    mpi_hal_write_to_mem_block(MPI_PARAM_X, 0, X->MBEDTLS_PRIVATE(p), X->MBEDTLS_PRIVATE(n), num_words);
    mpi_hal_write_to_mem_block(MPI_PARAM_Y, 0, Y->MBEDTLS_PRIVATE(p), Y->MBEDTLS_PRIVATE(n), num_words);
    mpi_hal_write_to_mem_block(MPI_PARAM_M, 0, M->MBEDTLS_PRIVATE(p), M->MBEDTLS_PRIVATE(n), num_words);
    mpi_hal_write_to_mem_block(MPI_PARAM_Z, 0, Rinv->MBEDTLS_PRIVATE(p), Rinv->MBEDTLS_PRIVATE(n), num_words);

    mpi_hal_write_m_prime(Mprime);

    /* Enable acceleration options */
    mpi_hal_enable_constant_time(false);
    mpi_hal_enable_search(true);
    mpi_hal_set_search_position(y_bits - 1);

    /* Execute first stage montgomery multiplication */
    mpi_hal_start_op(MPI_MODEXP);

    mpi_hal_enable_search(false);
}
#endif //ESP_MPI_USE_MONT_EXP
