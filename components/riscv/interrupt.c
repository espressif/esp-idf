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


#if SOC_INT_CLIC_SUPPORTED

/**
 * If the target is using the CLIC as the interrupt controller, we have 32 external interrupt lines and 16 internal
 * lines. Let's consider the internal ones reserved and not mappable to any handler.
 */
#define RV_EXTERNAL_INT_COUNT   32
#define RV_EXTERNAL_INT_OFFSET  (CLIC_EXT_INTR_NUM_OFFSET)

#else // !SOC_INT_CLIC_SUPPORTED

/**
 * In the case of INTC, all the interrupt lines are dedicated to external peripherals, so the offset is 0.
 * In the case of PLIC, the reserved interrupts are not contiguous, moreover, they are already marked as
 * unusable by the interrupt allocator, so the offset can also be 0 here.
 */
#define RV_EXTERNAL_INT_COUNT   32
#define RV_EXTERNAL_INT_OFFSET  0

/* Since DR_REG_INTERRUPT_CORE0_BASE is not defined on some single-core targets, use the former
 * DR_REG_INTERRUPT_BASE macro instead. */
#ifndef DR_REG_INTERRUPT_CORE0_BASE
#define DR_REG_INTERRUPT_CORE0_BASE DR_REG_INTERRUPT_BASE
#endif // DR_REG_INTERRUPT_CORE0_BASE


#endif // SOC_INT_CLIC_SUPPORTED


typedef struct {
    intr_handler_t handler;
    void *arg;
} intr_handler_item_t;

static intr_handler_item_t s_intr_handlers[SOC_CPU_CORES_NUM][RV_EXTERNAL_INT_COUNT];


static inline void assert_valid_rv_int_num(int rv_int_num)
{
#if !SOC_INT_CLIC_SUPPORTED
    assert(rv_int_num != 0 && "Invalid CPU interrupt number");
#endif
    assert(rv_int_num < RV_EXTERNAL_INT_COUNT && "Invalid CPU interrupt number");
}


static intr_handler_item_t* intr_get_item(int int_no)
{
    assert_valid_rv_int_num(int_no);

    const uint32_t id = rv_utils_get_core_id();

    return &s_intr_handlers[id][int_no];
}

/*************************** Software interrupt dispatcher ***************************/

void intr_handler_set(int int_no, intr_handler_t fn, void *arg)
{
    intr_handler_item_t* item = intr_get_item(int_no);

    *item = (intr_handler_item_t) {
        .handler = fn,
        .arg = arg
    };
}

intr_handler_t intr_handler_get(int rv_int_num)
{
    const intr_handler_item_t* item = intr_get_item(rv_int_num);
    return item->handler;
}

void *intr_handler_get_arg(int rv_int_num)
{
    const intr_handler_item_t* item = intr_get_item(rv_int_num);
    return item->arg;
}

/* called from vectors.S */
void _global_interrupt_handler(intptr_t sp, int mcause)
{
    /* mcause contains the interrupt number that triggered the current interrupt, this number
     * also take into account local/internal interrupt, however, this should not happen in practice,
     * since we never map any peripheral to those. */
    assert(mcause >= RV_EXTERNAL_INT_OFFSET && "Interrupt sources must not be mapped to local interrupts");
    const intr_handler_item_t* item = intr_get_item(mcause - RV_EXTERNAL_INT_OFFSET);
    if (item->handler) {
        (*item->handler)(item->arg);
    }
}

/*************************** RISC-V interrupt enable/disable ***************************/

void intr_matrix_route(int intr_src, int intr_num)
{
    assert_valid_rv_int_num(intr_num);

    if (rv_utils_get_core_id() == 0) {
        REG_WRITE(DR_REG_INTERRUPT_CORE0_BASE + 4 * intr_src, intr_num + RV_EXTERNAL_INT_OFFSET);
    }
#if SOC_CPU_CORES_NUM > 1
    else {
        REG_WRITE(DR_REG_INTERRUPT_CORE1_BASE + 4 * intr_src, intr_num + RV_EXTERNAL_INT_OFFSET);
    }
#endif // SOC_CPU_CORES_NUM > 1
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


#if SOC_INT_CLIC_SUPPORTED

enum intr_type esprv_intc_int_get_type(int rv_int_num)
{
    uint32_t intr_type_reg = REG_GET_FIELD(CLIC_INT_CTRL_REG(rv_int_num + RV_EXTERNAL_INT_OFFSET), CLIC_INT_ATTR_TRIG);
    return (intr_type_reg & 1) ? INTR_TYPE_EDGE : INTR_TYPE_LEVEL;
}

int esprv_intc_int_get_priority(int rv_int_num)
{
    uint32_t intr_priority_reg = REG_GET_FIELD(CLIC_INT_CTRL_REG(rv_int_num + RV_EXTERNAL_INT_OFFSET), CLIC_INT_CTL);
    return (intr_priority_reg >> (8 - NLBITS));
}


bool esprv_intc_int_is_vectored(int rv_int_num)
{
    const uint32_t shv = REG_GET_FIELD(CLIC_INT_CTRL_REG(rv_int_num + RV_EXTERNAL_INT_OFFSET), CLIC_INT_ATTR_SHV);
    return shv != 0;
}

void esprv_intc_int_set_vectored(int rv_int_num, bool vectored)
{
    REG_SET_FIELD(CLIC_INT_CTRL_REG(rv_int_num + RV_EXTERNAL_INT_OFFSET), CLIC_INT_ATTR_SHV, vectored ? 1 : 0);
}


#else // !SOC_INT_CLIC_SUPPORTED

enum intr_type esprv_intc_int_get_type(int rv_int_num)
{
    uint32_t intr_type_reg = REG_READ(INTERRUPT_CORE0_CPU_INT_TYPE_REG);
    return (intr_type_reg & (1 << rv_int_num)) ? INTR_TYPE_EDGE : INTR_TYPE_LEVEL;
}

int esprv_intc_int_get_priority(int rv_int_num)
{
    uint32_t intr_priority_reg = REG_READ(INTC_INT_PRIO_REG(rv_int_num));
    return intr_priority_reg;
}

#endif // SOC_INT_CLIC_SUPPORTED

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
