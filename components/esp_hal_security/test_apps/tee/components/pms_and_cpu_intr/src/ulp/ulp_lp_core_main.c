/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>

#include "soc/soc.h"
#include "riscv/csr.h"
#include "riscv/rv_utils.h"

#include "ulp_lp_core_utils.h"
#include "test_pms_priv.h"

extern int _vector_table;
extern int _test_vector_table;

void set_test_vector_table(void)
{
    rv_utils_intr_global_disable();
    rv_utils_set_mtvec((uintptr_t)&_test_vector_table);
    rv_utils_intr_global_enable();
}

void restore_default_vector_table(void)
{
    rv_utils_intr_global_disable();
    rv_utils_set_mtvec((uintptr_t)&_vector_table);
    rv_utils_intr_global_enable();
}

void test_panicHandler(void *frame, int exccause)
{
    switch (exccause) {
    case MCAUSE_INST_ACCESS_FAULT:
        SEND_EXCP(1);
        break;
    case MCAUSE_LOAD_ACCESS_FAULT:
        SEND_EXCP(5);
        break;
    case MCAUSE_STORE_ACCESS_FAULT:
        SEND_EXCP(7);
        break;
    default:
        SEND_EXCP(0xFFU);
        break;
    }

    SEND_ADDR(RV_READ_CSR(mtval));
    ulp_lp_core_delay_us(3000U);
    return;
}

/**
 * @brief Test LP CPU access to memory/peripherals
 *
 * The HP CPU communicates with LP CPU through LP_AON_STORE registers
 * and sends the following parameters:
 *   - Access command: read, write, execute, or clear (MSG_SLAVE_*)
 *   - Target address for the operation
 *   - Data or size value (for write operation)
 *
 * The LP CPU continuously polls for commands and processes them:
 *   - MSG_SLAVE_READ: Reads a 32-bit value from the given address
 *   - MSG_SLAVE_WRITE: Writes the given 32-bit value to the given address
 *   - MSG_SLAVE_EXEC: Jumps to and executes code at the given address
 *   - MSG_SLAVE_CLEAR: No operation; polling continues
 *
 * If an access fault occurs (instruction, load, or store fault), the panic handler
 * captures the cause and faulting address using the exception frame and mtval CSR.
 * The handler then sends this information back to the HP CPU for verifying the fault.
 */
int main(void)
{
    set_test_vector_table();

    while (1) {
        uint32_t test_msg = RECV_MSG();
        if (test_msg == MSG_SLAVE_CLEAR) {
            ulp_lp_core_delay_us(5000U);
            continue;
        }

        uint32_t test_addr = RECV_ADDR();
        uint32_t test_size = RECV_SIZE();

        switch (test_msg) {
        case MSG_SLAVE_READ:
            (void)REG_READ(test_addr);
            break;
        case MSG_SLAVE_WRITE:
            REG_WRITE(test_addr, test_size);
            break;
        case MSG_SLAVE_EXEC: {
            void (*func_ptr)(void) = (void (*)(void))(test_addr);
            func_ptr();
            break;
        }
        default:
            break;
        }
        SEND_MSG(MSG_SLAVE_CLEAR);
    }

    restore_default_vector_table();
    return 0;
}
