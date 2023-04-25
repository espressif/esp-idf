/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hal/mpi_hal.h"
#include "hal/mpi_ll.h"
#include "sdkconfig.h"


size_t mpi_hal_calc_hardware_words(size_t words)
{
    return mpi_ll_calculate_hardware_words(words);
}

void mpi_hal_enable_hardware_hw_op(void)
{
    mpi_ll_clear_power_control_bit();
    while (mpi_ll_check_memory_init_complete()) {
    }
    // Note: from enabling RSA clock to here takes about 1.3us

#if !CONFIG_IDF_TARGET_ESP32
    mpi_ll_disable_interrupt();
#endif
}

void mpi_hal_disable_hardware_hw_op(void)
{
    mpi_ll_set_power_control_bit();
}

void mpi_hal_interrupt_enable(bool enable)
{
    if (enable){
        mpi_ll_enable_interrupt();
    }
    else {
        mpi_ll_disable_interrupt();
    }
}

void mpi_hal_clear_interrupt(void)
{
    mpi_ll_clear_interrupt();
}

void mpi_hal_set_mode(size_t num_words)
{
    mpi_ll_set_mode(num_words);
}

void mpi_hal_write_to_mem_block(mpi_param_t param, size_t offset, const uint32_t* p, size_t n, size_t num_words)
{
    mpi_ll_write_to_mem_block(param, offset, p, n, num_words);
}

void mpi_hal_write_at_offset(mpi_param_t param, int offset, uint32_t value)
{
    mpi_ll_write_at_offset(param, offset, value);
}

void mpi_hal_write_m_prime(uint32_t Mprime)
{
    mpi_ll_write_m_prime(Mprime);
}

void mpi_hal_write_rinv(uint32_t rinv)
{
    mpi_ll_write_rinv(rinv);
}

// Acceleration options
#if !CONFIG_IDF_TARGET_ESP32
void mpi_hal_enable_constant_time(bool enable)
{
    if (enable){
        mpi_ll_enable_constant_time();
    }
    else {
        mpi_ll_disable_constant_time();
    }
}

void mpi_hal_enable_search(bool enable)
{
    if (enable){
        mpi_ll_enable_search();
    }
    else {
        mpi_ll_disable_search();
    }
}

void mpi_hal_set_search_position(size_t position)
{
    mpi_ll_set_search_position(position);
}
#endif /* !CONFIG_IDF_TARGET_ESP32 */

/* Begin an RSA operation.
*/
void mpi_hal_start_op(mpi_op_t op)
{
    /* Clear interrupt status */
    mpi_hal_clear_interrupt();
    mpi_ll_start_op(op);
}

/* Wait for an RSA operation to complete.
*/
void mpi_hal_wait_op_complete(void)
{
    while (mpi_ll_get_int_status())
    { }

    /* Clear interrupt status */
    mpi_hal_clear_interrupt();
}

void mpi_hal_read_result_hw_op(uint32_t* p, size_t n, size_t z_words)
{
    /* Wait for an RSA operation to complete. */
    mpi_hal_wait_op_complete();
    mpi_ll_read_from_mem_block(p, n, z_words);
}
