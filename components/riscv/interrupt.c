/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
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


typedef struct {
    intr_handler_t handler;
    void *arg;
} intr_handler_item_t;

static intr_handler_item_t s_intr_handlers[SOC_CPU_CORES_NUM][RV_EXTERNAL_INT_COUNT];


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
