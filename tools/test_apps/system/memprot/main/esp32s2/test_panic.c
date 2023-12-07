/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "xtensa_context.h"
#include "esp_private/panic_internal.h"

extern void esp_panic_handler(panic_info_t *info);
extern volatile bool g_override_illegal_instruction;

void __real_esp_panic_handler(panic_info_t *info);
void __real_esp_cpu_stall(int core_id);

/* Memprot test specific IllegalInstruction exception handler:
 * when testing the protection against a code execution, sample code
 * is being injected into various memory regions which produces
 * EXCCAUSE_ILLEGAL on execution attempt. Such a result is expected
 * but it causes system reboot in the standard panic handler.
 * The following variant of panic handling simply returns back to the
 * next instruction and continues normal execution.
 *
 * NOTE: if EXCCAUSE_ILLEGAL comes from a different source than the testing code
 * the behavior is undefined
 * */
void __wrap_esp_panic_handler(panic_info_t *info)
{
    XtExcFrame *frm = (XtExcFrame *)info->frame;
    if (frm->exccause == EXCCAUSE_ILLEGAL && g_override_illegal_instruction == true) {
        frm->pc = frm->a0;
        return;
    } else {
        __real_esp_panic_handler(info);
    }
}

void __wrap_esp_cpu_stall(int core_id)
{
    if (g_override_illegal_instruction == true) {
        return;
    } else {
        __real_esp_cpu_stall(core_id);
    }
}
