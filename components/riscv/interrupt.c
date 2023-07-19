/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include "soc/soc.h"
#include "riscv/interrupt.h"
#include "soc/interrupt_reg.h"
#include "riscv/csr.h"
#include "esp_attr.h"
#include "riscv/rv_utils.h"


//TODO: IDF-7795, P4, see jira to know what changed and what need to be checked


#define RV_INT_COUNT 32

static inline void assert_valid_rv_int_num(int rv_int_num)
{
#if SOC_INT_CLIC_SUPPORTED
    assert(rv_int_num < RV_INT_COUNT && "Invalid CPU interrupt number");
#else
    assert(rv_int_num != 0 && rv_int_num < RV_INT_COUNT && "Invalid CPU interrupt number");
#endif
}

/*************************** Software interrupt dispatcher ***************************/


typedef struct {
    intr_handler_t handler;
    void *arg;
} intr_handler_item_t;

#if SOC_INT_CLIC_SUPPORTED
static intr_handler_item_t s_intr_handlers_core0[48];
static intr_handler_item_t s_intr_handlers_core1[48];
#else
static intr_handler_item_t s_intr_handlers[32];
#endif

void intr_handler_set(int int_no, intr_handler_t fn, void *arg)
{
    assert_valid_rv_int_num(int_no);

#if SOC_INT_CLIC_SUPPORTED
    if (rv_utils_get_core_id() == 0) {
        s_intr_handlers_core0[int_no + CLIC_EXT_INTR_NUM_OFFSET] = (intr_handler_item_t) {
            .handler = fn,
            .arg = arg,
        };
    } else {
        s_intr_handlers_core1[int_no + CLIC_EXT_INTR_NUM_OFFSET] = (intr_handler_item_t) {
            .handler = fn,
            .arg = arg,
        };
    }
#else
    s_intr_handlers[int_no] = (intr_handler_item_t) {
        .handler = fn,
        .arg = arg
    };
#endif
}

intr_handler_t intr_handler_get(int rv_int_num)
{
#if SOC_INT_CLIC_SUPPORTED
    if (rv_utils_get_core_id() == 0)
        return s_intr_handlers_core0[rv_int_num + CLIC_EXT_INTR_NUM_OFFSET].handler;
    else
        return s_intr_handlers_core1[rv_int_num + CLIC_EXT_INTR_NUM_OFFSET].handler;
#else
    return s_intr_handlers[rv_int_num].handler;
#endif
}

void *intr_handler_get_arg(int rv_int_num)
{
#if SOC_INT_CLIC_SUPPORTED
    if (rv_utils_get_core_id() == 0)
        return s_intr_handlers_core0[rv_int_num + CLIC_EXT_INTR_NUM_OFFSET].arg;
    else
        return s_intr_handlers_core1[rv_int_num + CLIC_EXT_INTR_NUM_OFFSET].arg;
#else
    return s_intr_handlers[rv_int_num].arg;
#endif
}

/* called from vectors.S */
void _global_interrupt_handler(intptr_t sp, int mcause)
{
#if SOC_INT_CLIC_SUPPORTED
    if (rv_utils_get_core_id() == 0) {
        intr_handler_item_t it = s_intr_handlers_core0[mcause];
        if (it.handler) {
            (*it.handler)(it.arg);
        }
    } else {
        intr_handler_item_t it = s_intr_handlers_core1[mcause];
        if (it.handler) {
            (*it.handler)(it.arg);
        }
    }
#else
    intr_handler_item_t it = s_intr_handlers[mcause];
    if (it.handler) {
        (*it.handler)(it.arg);
    }
#endif
}

/*************************** RISC-V interrupt enable/disable ***************************/

void intr_matrix_route(int intr_src, int intr_num)
{
#if !SOC_INT_CLIC_SUPPORTED
    assert(intr_num != 0);

    REG_WRITE(DR_REG_INTERRUPT_BASE + 4 * intr_src, intr_num);
#else
    if (rv_utils_get_core_id() == 0)
        REG_WRITE(DR_REG_INTERRUPT_CORE0_BASE + 4 * intr_src, intr_num + CLIC_EXT_INTR_NUM_OFFSET);
    else
        REG_WRITE(DR_REG_INTERRUPT_CORE1_BASE + 4 * intr_src, intr_num + CLIC_EXT_INTR_NUM_OFFSET);
#endif
}

// CLIC for each interrupt line provides a IE register
// this api is not used
#if !SOC_INT_CLIC_SUPPORTED
uint32_t esprv_intc_get_interrupt_unmask(void)
{
    return REG_READ(INTERRUPT_CORE0_CPU_INT_ENABLE_REG);
}
#endif

/*************************** ESP-RV Interrupt Controller ***************************/

enum intr_type esprv_intc_int_get_type(int intr_num)
{
#if SOC_INT_CLIC_SUPPORTED
    uint32_t intr_type_reg = REG_GET_FIELD(CLIC_INT_CTRL_REG(intr_num + CLIC_EXT_INTR_NUM_OFFSET), CLIC_INT_ATTR_TRIG);
    return (intr_type_reg & 1) ? INTR_TYPE_EDGE : INTR_TYPE_LEVEL;
    // May also support rising edge and falling edge.
#else
    uint32_t intr_type_reg = REG_READ(INTERRUPT_CORE0_CPU_INT_TYPE_REG);
    return (intr_type_reg & (1 << intr_num)) ? INTR_TYPE_EDGE : INTR_TYPE_LEVEL;
#endif
}

int esprv_intc_int_get_priority(int rv_int_num)
{
#if SOC_INT_CLIC_SUPPORTED
    uint32_t intr_priority_reg = REG_GET_FIELD(CLIC_INT_CTRL_REG(rv_int_num + CLIC_EXT_INTR_NUM_OFFSET), CLIC_INT_CTL);
    return (intr_priority_reg >> (8 - NLBITS));
#else
    uint32_t intr_priority_reg = REG_READ(INTC_INT_PRIO_REG(rv_int_num));
    return intr_priority_reg;
#endif
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
