/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The HAL is not public api, don't use in application code.
 * See readme.md in soc/README.md
 ******************************************************************************/

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <sys/param.h>
#include "hal/mpi_types.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Calculate the number of words needed to represent the input word in hardware.
 *
 * @param words The number of words to be represented.
 * @return size_t Number of words required.
 */
size_t mpi_hal_calc_hardware_words(size_t words);

/**
 * @brief Clear the MPI power control bit and initialise the MPI hardware.
 *
 */
void mpi_hal_enable_hardware_hw_op(void);

/**
 * @brief Set the MPI power control bit to disable the MPI hardware.
 *
 */
void mpi_hal_disable_hardware_hw_op(void);

/**
 * @brief Enable/disables MPI operation complete interrupt.
 *
 * @param enable true: enable, false: disable.
 */
void mpi_hal_interrupt_enable(bool enable);

/**
 * @brief Clears the MPI operation complete interrupt status.
 *
 */
void mpi_hal_clear_interrupt(void);

/**
 * @brief Configure RSA length.
 *
 * @param num_words Number of words representing the RSA length.
 */
void mpi_hal_set_mode(size_t num_words);

/**
 * @brief Copy the large number (array of words) representation of the parameter 'param' to hardware memory block.
 *
 * @param param Type of parameter (enum).
 * @param offset Offset to copy in the memory from the base address of the parameter.
 * @param p Pointer to large number (array of words) representation of the parameter.
 * @param n Number of words needed to represent the large number as an array of words.
 * @param num_words Maximum hardware words needed.
 */
void mpi_hal_write_to_mem_block(mpi_param_t param, size_t offset, const uint32_t* p, size_t n, size_t num_words);

/**
 * @brief Write a word-sized value to hardware memory block of a parameter.
 *
 * @param param Type of parameter (enum).
 * @param offset Offset to copy in the memory from the base address of the parameter.
 * @param value Value to be written in the memory.
 */
void mpi_hal_write_at_offset(mpi_param_t param, int offset, uint32_t value);

/**
 * @brief Write the modular multiplicative inverse of M.
 *
 * @param Mprime Modular multiplicative inverse of M.
 */
void mpi_hal_write_m_prime(uint32_t Mprime);

/**
 * @brief Write first word of the parameter Rinv.
 *
 * @param rinv Value of first word of rinv.
 */
void mpi_hal_write_rinv(uint32_t rinv);

#if !SOC_IS(ESP32)
/**
 * @brief Enable/Disable constant time acceleration option.
 *
 * @param enable true: enable, false: disable.
 */
void mpi_hal_enable_constant_time(bool enable);

/**
 * @brief Enable/Disable search time acceleration option.
 *
 * @param enable
 */
void mpi_hal_enable_search(bool enable);

/**
 * @brief Configures the starting address to start search.
 *
 * @param position Address to start search.
 */
void mpi_hal_set_search_position(size_t position);
#endif /* !SOC_IS(ESP32) */

/**
 * @brief Begin an MPI operation.
 *
 * @param op Operation type (enum).
 */
void mpi_hal_start_op(mpi_op_t op);

/**
 * @brief  Wait for an MPI operation to complete.
 *
 */
void mpi_hal_wait_op_complete(void);

/**
 * @brief Wait for an MPI operation to complete and Read result from last MPI operation into parameter Z.
 *
 * @param p Pointer to large number (array of words) representation of the parameter.
 * @param n Number of words needed to represent the large number as an array of words.
 * @param z_words Calculated number of words of parameter Z.
 */
void mpi_hal_read_result_hw_op(uint32_t* p, size_t n, size_t z_words);

#ifdef __cplusplus
}
#endif
