/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "riscv/rvruntime-frames.h"
#include "esp_private/panic_internal.h"
#include "esp_private/panic_reason.h"


extern void esp_panic_handler(panic_info_t *info);
volatile bool g_override_illegal_instruction = false;

void __real_esp_panic_handler(panic_info_t *info);
void __real_esp_cpu_stall(int core_id);

void return_from_panic_handler(RvExcFrame *frm) __attribute__((noreturn));

/* Memprot test specific IllegalInstruction exception handler:
 * when testing the protection against a code execution, sample code
 * is being injected into various memory regions which produces
 * Illegal instruction on execution attempt. Such a result is expected
 * but it causes system reboot in the standard panic handler.
 * The following variant of panic handling simply returns back to the
 * next instruction and continues normal execution.
 *
 * NOTE: if Illegal instruction comes from a different source than the testing code
 * the behavior is undefined
 * */
void __wrap_esp_panic_handler(panic_info_t *info)
{
    RvExcFrame *frm = (RvExcFrame *)info->frame;
    if ( frm->mcause == MCAUSE_ILLEGAL_INSTRUCTION && g_override_illegal_instruction == true ) {
        /* Return from exception to the return address that called the faulting function.
        */
        frm->mepc = frm->ra;

        /* Restore the CPU state and return from the exception.
        */
        return_from_panic_handler(frm);
    } else {
        __real_esp_panic_handler(info);
    }
}

void __wrap_esp_cpu_stall(int core_id)
{
    if ( g_override_illegal_instruction == true ) {
        return;
    } else {
        __real_esp_cpu_stall(core_id);
    }
}
