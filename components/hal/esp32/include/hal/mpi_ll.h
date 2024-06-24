/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <string.h>
#include <sys/param.h>
#include "hal/assert.h"
#include "hal/mpi_types.h"
#include "soc/dport_reg.h"
#include "soc/hwcrypto_periph.h"
#include "soc/mpi_periph.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Enable the bus clock for MPI peripheral module
 *
 * @param enable true to enable the module, false to disable the module
 */
static inline void mpi_ll_enable_bus_clock(bool enable)
{
    if (enable) {
        DPORT_SET_PERI_REG_MASK(DPORT_PERI_CLK_EN_REG, DPORT_PERI_EN_RSA);
    } else {
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PERI_CLK_EN_REG, DPORT_PERI_EN_RSA);
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mpi_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; mpi_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the MPI peripheral module
 */
static inline void mpi_ll_reset_register(void)
{
    DPORT_SET_PERI_REG_MASK(DPORT_PERI_RST_EN_REG, DPORT_PERI_EN_RSA);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERI_RST_EN_REG, DPORT_PERI_EN_RSA);

    // Clear reset on digital signature also, otherwise RSA is held in reset
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERI_RST_EN_REG, DPORT_PERI_EN_DIGITAL_SIGNATURE);
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mpi_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; mpi_ll_reset_register(__VA_ARGS__)

/* Round up number of words to nearest
   512 bit (16 word) block count.
*/
static inline size_t mpi_ll_calculate_hardware_words(size_t words)
{
    return (words + 0xF) & ~0xF;
}

static inline void mpi_ll_power_up(void)
{
    DPORT_REG_CLR_BIT(DPORT_RSA_PD_CTRL_REG, DPORT_RSA_PD);
}

static inline void mpi_ll_power_down(void)
{
    DPORT_REG_SET_BIT(DPORT_RSA_PD_CTRL_REG, DPORT_RSA_PD);
}

static inline void mpi_ll_enable_interrupt(void)
{
    DPORT_REG_WRITE(RSA_INTERRUPT_REG, 1);
}

static inline void mpi_ll_disable_interrupt(void)
{
    DPORT_REG_WRITE(RSA_INTERRUPT_REG, 0);
}

static inline void mpi_ll_clear_interrupt(void)
{
    DPORT_REG_WRITE(RSA_CLEAR_INTERRUPT_REG, 1);
}

static inline bool mpi_ll_check_memory_init_complete(void)
{
    return DPORT_REG_READ(RSA_CLEAN_REG) == 0;
}

static inline void mpi_ll_start_op(mpi_op_t op)
{
    DPORT_REG_WRITE(MPI_OPERATIONS_REG[op], 1);
}

static inline bool mpi_ll_get_int_status(void)
{
    return DPORT_REG_READ(RSA_INTERRUPT_REG) == 0;
}

/* Copy MPI bignum (p) to hardware memory block at 'mem_base'.

   If num_words is higher than the number of words (n) in the bignum then
   these additional words will be zeroed in the memory buffer.

*/

/* Please see detailed note inside the function body below.
 * Relevant: IDF-6029
             https://github.com/espressif/esp-idf/issues/8710
             https://github.com/espressif/esp-idf/issues/10403
 */
static inline void mpi_ll_write_to_mem_block(mpi_param_t param, size_t offset, const uint32_t* p, size_t n, size_t num_words)
{
    uint32_t mem_base = MPI_BLOCK_BASES[param] + offset;
    uint32_t copy_words = MIN(num_words, n);

    /* Copy MPI data to memory block registers */
    for (uint32_t i = 0; i < copy_words; i++) {
        DPORT_REG_WRITE(mem_base + i * 4, p[i]);
    }

    /* Zero any remaining memory block data */
    for (uint32_t i = copy_words; i < num_words; i++) {
        DPORT_REG_WRITE(mem_base + i * 4, 0);
    }
#if _INTERNAL_DEBUG_PURPOSE
    /*
     * With Xtensa GCC 11.2.0 (from ESP-IDF v5.x), it was observed that above zero initialization
     * loop gets optimized to `memset` call from the ROM library. This was causing an issue that
     * specific write (store) operation to the MPI peripheral block was getting lost erroneously.
     * Following data re-verify loop could catch it during runtime.
     *
     * As a workaround, we are using DPORT_WRITE_REG (volatile writes) wrappers to write to
     * the MPI peripheral.
     *
     */

    //for (uint32_t i = copy_words; i < hw_words; i++) { HAL_ASSERT(pbase[i] == 0); }
#endif
}

static inline void mpi_ll_write_m_prime(uint32_t Mprime)
{
    DPORT_REG_WRITE(RSA_M_DASH_REG, Mprime);
}

static inline void mpi_ll_write_rinv(uint32_t rinv)
{
    DPORT_REG_WRITE(MPI_BLOCK_BASES[MPI_PARAM_Z], rinv);
}

static inline void mpi_ll_write_at_offset(mpi_param_t param, int offset, uint32_t value)
{
    uint32_t mem_base = MPI_BLOCK_BASES[param] + offset;
    DPORT_REG_WRITE(mem_base, value);
}

/* Read MPI bignum (p) back from hardware memory block.

   Reads z_words words from block.
*/
static inline void mpi_ll_read_from_mem_block(uint32_t* p, size_t n, size_t num_words)
{
    HAL_ASSERT(n >= num_words);
    uint32_t mem_base = MPI_BLOCK_BASES[MPI_PARAM_Z];
    /* Copy data from memory block registers */
    esp_dport_access_read_buffer(p, mem_base, num_words);

    /* Zero any remaining limbs in the bignum, if the buffer is bigger
       than num_words */
    for (size_t i = num_words; i < n; i++) {
        p[i] = 0;
    }
}

static inline void mpi_ll_set_mode(size_t length)
{
    DPORT_REG_WRITE(RSA_MULT_MODE_REG, length);
}

#ifdef __cplusplus
}
#endif
