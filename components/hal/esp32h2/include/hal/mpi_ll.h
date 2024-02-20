/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <string.h>
#include <sys/param.h>
#include "hal/assert.h"
#include "hal/mpi_types.h"
#include "soc/pcr_reg.h"
#include "soc/pcr_struct.h"
#include "soc/rsa_reg.h"
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
    PCR.rsa_conf.rsa_clk_en = enable;
}

/**
 * @brief Reset the MPI peripheral module
 */
static inline void mpi_ll_reset_register(void)
{
    PCR.rsa_conf.rsa_rst_en = 1;
    PCR.rsa_conf.rsa_rst_en = 0;

    // Clear reset on digital signature also, otherwise RSA is held in reset
    PCR.ds_conf.ds_rst_en = 0;
    PCR.ecdsa_conf.ecdsa_rst_en = 0;
}

static inline size_t mpi_ll_calculate_hardware_words(size_t words)
{
    return words;
}

static inline void mpi_ll_clear_power_control_bit(void)
{
    REG_CLR_BIT(PCR_RSA_PD_CTRL_REG, PCR_RSA_MEM_PD);
}

static inline void mpi_ll_set_power_control_bit(void)
{
    REG_SET_BIT(PCR_RSA_PD_CTRL_REG, PCR_RSA_MEM_PD);
}

static inline void mpi_ll_enable_interrupt(void)
{
    REG_WRITE(RSA_INT_ENA_REG, 1);
}

static inline void mpi_ll_disable_interrupt(void)
{
    REG_WRITE(RSA_INT_ENA_REG, 0);
}

static inline void mpi_ll_clear_interrupt(void)
{
    REG_WRITE(RSA_INT_CLR_REG, 1);
}

static inline bool mpi_ll_check_memory_init_complete(void)
{
    return REG_READ(RSA_QUERY_CLEAN_REG) == 0;
}

static inline void mpi_ll_start_op(mpi_op_t op)
{
    REG_WRITE(MPI_LL_OPERATIONS[op], 1);
}

static inline bool mpi_ll_get_int_status(void)
{
    return REG_READ(RSA_QUERY_IDLE_REG) == 0;
}

/* Copy MPI bignum (p) to hardware memory block at 'mem_base'.

   If num_words is higher than the number of words (n) in the bignum then
   these additional words will be zeroed in the memory buffer.
*/
static inline void mpi_ll_write_to_mem_block(mpi_param_t param, size_t offset, const uint32_t* p, size_t n, size_t num_words)
{
    uint32_t mem_base = MPI_LL_BLOCK_BASES[param] + offset;
    uint32_t* pbase = (uint32_t*) mem_base;
    uint32_t copy_words = MIN(num_words, n);

    /* Copy MPI data to memory block registers */
    for (int i = 0; i < copy_words; i++) {
        pbase[i] = p[i];
    }

    /* Zero any remaining memory block data */
    for (int i = copy_words; i < num_words; i++) {
        pbase[i] = 0;
    }
}

static inline void mpi_ll_write_m_prime(uint32_t Mprime)
{
    REG_WRITE(RSA_M_PRIME_REG, Mprime);
}

static inline void mpi_ll_write_rinv(uint32_t rinv)
{
    REG_WRITE(MPI_LL_BLOCK_BASES[MPI_PARAM_Z], rinv);
}

static inline void mpi_ll_write_at_offset(mpi_param_t param, int offset, uint32_t value)
{
    uint32_t mem_base = MPI_LL_BLOCK_BASES[param] + offset;
    REG_WRITE(mem_base, value);
}

/* Read MPI bignum (p) back from hardware memory block.

   Reads z_words words from block.
*/
static inline void mpi_ll_read_from_mem_block(uint32_t* p, size_t n, size_t num_words)
{
    uint32_t mem_base = MPI_LL_BLOCK_BASES[MPI_PARAM_Z];
    /* Copy data from memory block registers */
    const size_t REG_WIDTH = sizeof(uint32_t);
    for (size_t i = 0; i < num_words; i++) {
        p[i] = REG_READ(mem_base + (i * REG_WIDTH));
    }
    /* Zero any remaining limbs in the bignum, if the buffer is bigger
       than num_words */
    for (size_t i = num_words; i < n; i++) {
        p[i] = 0;
    }
}

static inline void mpi_ll_set_mode(size_t length)
{
    REG_WRITE(RSA_MODE_REG, length);
}

static inline void mpi_ll_disable_constant_time(void)
{
    REG_WRITE(RSA_CONSTANT_TIME_REG, 0);
}

static inline void mpi_ll_enable_constant_time(void)
{
    REG_WRITE(RSA_CONSTANT_TIME_REG, 1);
}

static inline void mpi_ll_disable_search(void)
{
    REG_WRITE(RSA_SEARCH_ENABLE_REG, 0);
}

static inline void mpi_ll_enable_search(void)
{
    REG_WRITE(RSA_SEARCH_ENABLE_REG, 1);
}

static inline void mpi_ll_set_search_position(size_t pos)
{
    REG_WRITE(RSA_SEARCH_POS_REG, pos);
}

#ifdef __cplusplus
}
#endif
