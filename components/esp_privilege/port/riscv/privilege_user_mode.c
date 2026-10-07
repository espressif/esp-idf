/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Trap handlers for U-mode code and their registration at startup */

#include <stdint.h>
#include <stdbool.h>
#include "riscv/rv_utils.h"
#include "riscv/rvruntime-frames.h"
#include "esp_privilege_abi.h"
#include "esp_private/privilege_entry.h"
#include "esp_err.h"
#include "esp_private/startup_internal.h"
#include "esp_private/privilege_trap.h"

bool esp_privilege_ecall_handler(void *frame)
{
    RvExcFrame *regs = (RvExcFrame *)frame;

    RvUserCallCtx *context = rv_user_call_ctx_from_frame(frame);
    context->user_gp = regs->gp;
    context->user_tp = regs->tp;
    if (regs->a7 == ESP_PRIVILEGE_ECALL_EXIT) {
        /* Resume the task in M-mode, on its M-mode stack, where it entered U-mode.
         * a0 holds the value returned by the U-mode code. */
        regs->mepc = (uint32_t)esp_privilege_return_to_caller;
        esp_privilege_frame_to_machine(frame);
        return true;
    }
    return false;
}

void esp_privilege_frame_to_machine(void *frame)
{
    RvExcFrame *regs = frame;
    const RvUserCallCtx *context = rv_user_call_ctx_from_frame(frame);
    regs->gp = context->gp;
    regs->tp = context->tp;
    regs->mstatus |= MSTATUS_MPP;
}

ESP_SYSTEM_INIT_FN(init_privilege_user_traps, SECONDARY, BIT(0), 170)
{
    rv_user_trap_set_handlers(esp_privilege_ecall_handler);
    return ESP_OK;
}

void esp_privilege_init_include_func(void)
{
    // Hook to force the linker to keep the init function
}
