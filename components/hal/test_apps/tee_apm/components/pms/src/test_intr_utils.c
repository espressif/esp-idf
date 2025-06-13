/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>

#include "soc/soc_caps.h"
#include "esp_cpu.h"

#include "riscv/encoding.h"
#include "riscv/csr.h"
#include "riscv/rv_utils.h"

#include "esp_rom_sys.h"

#if SOC_INT_PLIC_SUPPORTED
#include "esp_private/interrupt_plic.h"
#elif SOC_INT_CLIC_SUPPORTED
#include "esp_private/interrupt_clic.h"
#else
#include "esp_private/interrupt_intc.h"
#endif

/********************************** Vector Table Redirection *******************************/

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

/********************************** Privilege Mode Switch *********************************/

void test_m2u_switch(void)
{
    int mode = esp_cpu_get_curr_privilege_level();
    assert(mode == PRV_M);

    asm volatile("ecall\n");

    mode = esp_cpu_get_curr_privilege_level();
    assert(mode == PRV_U);
}

void test_u2m_switch(void)
{
    int mode = esp_cpu_get_curr_privilege_level();
    assert(mode == PRV_U);

    asm volatile("ecall\n");

    mode = esp_cpu_get_curr_privilege_level();
    assert(mode == PRV_M);
}

/********************************** Interrupt Handler *************************************/

extern void _global_interrupt_handler(intptr_t sp, int mcause);

/* called from test_tee_vectors.S */
void _test_global_interrupt_handler(intptr_t sp, int mcause)
{
    _global_interrupt_handler(sp, mcause);
}
