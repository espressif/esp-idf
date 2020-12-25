// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include "riscv/interrupt.h"
#include "soc/interrupt_reg.h"
#include "riscv/csr.h"
#include "esp_attr.h"

#define RV_INT_COUNT 32

static inline void assert_valid_rv_int_num(int rv_int_num)
{
    assert(rv_int_num != 0 && rv_int_num < RV_INT_COUNT && "Invalid CPU interrupt number");
}

/*************************** Software interrupt dispatcher ***************************/


typedef struct {
    intr_handler_t handler;
    void *arg;
} intr_handler_item_t;

static intr_handler_item_t s_intr_handlers[32];

void intr_handler_set(int int_no, intr_handler_t fn, void *arg)
{
    assert_valid_rv_int_num(int_no);

    s_intr_handlers[int_no] = (intr_handler_item_t) {
        .handler = fn,
        .arg = arg
    };
}

intr_handler_t intr_handler_get(int rv_int_num)
{
    return s_intr_handlers[rv_int_num].handler;
}

void *intr_handler_get_arg(int rv_int_num)
{
    return s_intr_handlers[rv_int_num].arg;
}

/* called from vectors.S */
void _global_interrupt_handler(intptr_t sp, int mcause)
{
    intr_handler_item_t it = s_intr_handlers[mcause];
    if (it.handler) {
        (*it.handler)(it.arg);
    }
}

/*************************** RISC-V interrupt enable/disable ***************************/

void intr_matrix_route(int intr_src, int intr_num)
{
    assert(intr_num != 0);

    REG_WRITE(DR_REG_INTERRUPT_BASE + 4 * intr_src, intr_num);
}

void riscv_global_interrupts_enable(void)
{
    RV_SET_CSR(mstatus, MSTATUS_MIE);
}

void riscv_global_interrupts_disable(void)
{
    RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
}

uint32_t esprv_intc_get_interrupt_unmask(void)
{
    return REG_READ(INTERRUPT_CORE0_CPU_INT_ENABLE_REG);
}

/*************************** Exception names. Used in .gdbinit file. ***************************/

const char *riscv_excp_names[16] __attribute__((used)) = {
    "misaligned_fetch",
    "fault_fetch",
    "illegal_instruction",
    "breakpoint",
    "misaligned_load",
    "fault_load",
    "misaligned_store",
    "fault_store",
    "user_ecall",
    "supervisor_ecall",
    "hypervisor_ecall",
    "machine_ecall",
    "exec_page_fault",
    "load_page_fault",
    "reserved",
    "store_page_fault"
};
