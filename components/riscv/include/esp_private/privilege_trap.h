/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#if CONFIG_RISCV_USER_MODE_TRAPS
#include "riscv/rvruntime-frames.h"
#if !(defined(_ASMLANGUAGE) || defined(__ASSEMBLER__))
#include <stdbool.h>
#include <stdint.h>
#endif /* !(defined(_ASMLANGUAGE) || defined(__ASSEMBLER__)) */

/*
 * Trap handling for tasks running code in U-mode (CONFIG_RISCV_USER_MODE_TRAPS).
 *
 * While a task runs U-mode code, mscratch holds the top of the task's M-mode stack. The call
 * context saved there when the task entered U-mode holds the M-mode gp and tp, which trap entry
 * restores before running C code. While M-mode code runs, the value of mscratch is not used,
 * because the CPU picks the stack from the privilege mode the trap was taken from.
 */

/* Call context, saved at the top of the task's M-mode stack when the task enters U-mode */
STRUCT_BEGIN
STRUCT_FIELD (uint32_t, 4, RV_UCTX_RA, ra)
STRUCT_FIELD (uint32_t, 4, RV_UCTX_GP, gp)
STRUCT_FIELD (uint32_t, 4, RV_UCTX_TP, tp)
STRUCT_FIELD (uint32_t, 4, RV_UCTX_S0, s0)
STRUCT_FIELD (uint32_t, 4, RV_UCTX_S1, s1)
STRUCT_FIELD (uint32_t, 4, RV_UCTX_S2, s2)
STRUCT_FIELD (uint32_t, 4, RV_UCTX_S3, s3)
STRUCT_FIELD (uint32_t, 4, RV_UCTX_S4, s4)
STRUCT_FIELD (uint32_t, 4, RV_UCTX_S5, s5)
STRUCT_FIELD (uint32_t, 4, RV_UCTX_S6, s6)
STRUCT_FIELD (uint32_t, 4, RV_UCTX_S7, s7)
STRUCT_FIELD (uint32_t, 4, RV_UCTX_S8, s8)
STRUCT_FIELD (uint32_t, 4, RV_UCTX_S9, s9)
STRUCT_FIELD (uint32_t, 4, RV_UCTX_S10, s10)
STRUCT_FIELD (uint32_t, 4, RV_UCTX_S11, s11)
STRUCT_FIELD (uint32_t, 4, RV_UCTX_SATP, satp)                    /* satp value used while the task runs U-mode code */
STRUCT_FIELD (uint32_t, 4, RV_UCTX_FAULT, fault)                  /* zero, the high word of the result of esp_privilege_call_user_code() */
STRUCT_FIELD (uint32_t, 4, RV_UCTX_USER_GP, user_gp)
STRUCT_FIELD (uint32_t, 4, RV_UCTX_USER_TP, user_tp)
STRUCT_END(RvUserCallCtx)

#if defined(_ASMLANGUAGE) || defined(__ASSEMBLER__)
#define RV_UCTX_SIZE      ALIGNUP(16, RvUserCallCtxSize)
#else /* defined(_ASMLANGUAGE) || defined(__ASSEMBLER__) */
#define RV_UCTX_SIZE      ALIGNUP(16, sizeof(RvUserCallCtx))
#endif /* defined(_ASMLANGUAGE) || defined(__ASSEMBLER__) */

/* Offsets of the fields of privilege_domain_run_context_t that the assembly code reads. The
 * structure itself is defined by esp_privilege, which checks the offsets. */
#define RV_URUN_GP 0
#define RV_URUN_TP 4
#define RV_URUN_RETURN 8

#if !(defined(_ASMLANGUAGE) || defined(__ASSEMBLER__))
/**
 * @brief Call context of a task that runs U-mode code, from a trap frame taken from U-mode
 *
 * Trap entry places the frame right below the call context, at the top of the task's M-mode stack.
 *
 * @param frame Trap frame of RV_STK_FRMSZ bytes
 * @return Call context above the frame
 */
static inline RvUserCallCtx *rv_user_call_ctx_from_frame(const void *frame)
{
    return (RvUserCallCtx *)((uintptr_t)frame + RV_STK_FRMSZ);
}

/**
 * @brief Handler of an ECALL from U-mode
 *
 * @param frame Trap frame saved by the exception entry code
 * @return true if the handler took the ECALL, false to let the panic handler take it
 */
typedef bool (*rv_user_ecall_handler_t)(void *frame);

/**
 * @brief Set the handler that the exception entry code calls for an ECALL from U-mode
 *
 * Until a handler is set, an ECALL from U-mode reaches the panic handler.
 *
 * @param ecall Handler of ECALLs, or NULL
 */
void rv_user_trap_set_handlers(rv_user_ecall_handler_t ecall);
#endif /* !(defined(_ASMLANGUAGE) || defined(__ASSEMBLER__)) */
#endif /* CONFIG_RISCV_USER_MODE_TRAPS */

#if (defined(_ASMLANGUAGE) || defined(__ASSEMBLER__)) && CONFIG_RISCV_USER_MODE_TRAPS

#include "riscv/sv32_asm.h"
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
/* The guard macros pick the core from SOC_CPU_CORES_NUM */
#include "soc/soc_caps.h"
#include "esp_private/hw_stack_guard.h"
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */
#include "riscv/encoding.h"
#include "riscv/csr_clic.h"

/**
 * @brief Switch to the M-mode stack if the trap came from U-mode.
 *
 * Must be the first instruction of every trap handler. If the trap came from U-mode, mscratch
 * holds the interrupted user sp afterwards.
 *
 * Uses mscratchcsw of the CLIC. On a CPU without a CLIC, the trap entry would swap sp and mscratch
 * in software and check mstatus.MPP to undo the swap for a trap taken from M-mode.
 */
.macro rv_user_trap_enter
    /* mscratchcsw, a CSR of the CLIC, swaps sp and mscratch only for a trap taken from U-mode */
    csrrw   sp, MSCRATCHCSW_CSR, sp
.endm

/**
 * @brief Branch to label if mret will return to U-mode (mstatus.MPP == U)
 */
.macro rv_user_branch_if_user reg, label
    csrr    \reg, mstatus
    srli    \reg, \reg, 11
    andi    \reg, \reg, 3
    beqz    \reg, \label
.endm

/**
 * @brief Get the interrupted sp and, if the trap came from U-mode, restore the M-mode state of the
 *        task, such as its gp and tp and the hardware stack guard.
 *
 * Must be used after the interrupted gp and tp have been saved in a frame of frame_size bytes
 * pointed to by sp.
 *
 * @param reg Register receiving the sp value at the time of the trap
 * @param frame_size Size of the frame allocated on the stack by the trap handler
 */
.macro rv_user_trap_restore_m_context reg, frame_size
    /* The user sp may be zero, so the origin of the trap is taken from mstatus.MPP and not from sp */
    rv_user_branch_if_user \reg, .Lrv_user_trap_restore_m_context_user\@
    addi    \reg, sp, \frame_size
    j       .Lrv_user_trap_restore_m_context_end\@
.Lrv_user_trap_restore_m_context_user\@:
    csrr    \reg, mscratch
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* The stack guard is paused while U-mode code runs on its own stack. The task's bounds are
     * still set, and sp is on the task's M-mode stack again. */
    ESP_HW_STACK_GUARD_MONITOR_START_CUR_CORE gp tp
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */
    lw      gp, (\frame_size + RV_UCTX_GP)(sp)
    lw      tp, (\frame_size + RV_UCTX_TP)(sp)
.Lrv_user_trap_restore_m_context_end\@:
.endm

/**
 * @brief Activate the address space of the U-mode code. The translations of a satp value stay valid,
 *        because every change of its page tables is followed by a flush on every core, so only a
 *        different satp value needs a flush.
 *
 * @param reg Register holding the satp value
 * @param tmp Register clobbered
 */
.macro rv_user_activate_satp reg, tmp
    csrr    \tmp, satp
    beq     \reg, \tmp, .Lrv_user_satp_unchanged\@
    riscv_sv32_write_satp \reg
    sfence.vma
.Lrv_user_satp_unchanged\@:
.endm

/**
 * @brief Prepare the return to U-mode. user_sp is stored in mscratch and the task's address space
 *        (satp) is activated. After the registers have been restored, rv_user_trap_exit_to_user must be used.
 *
 * @param reg Register holding the user sp, clobbered. t1 is clobbered as well.
 * @param frame_size Size of the frame pointed to by sp, which ends at the top of the task's M-mode stack
 */
.macro rv_user_trap_set_user_sp reg, frame_size
    csrw    mscratch, \reg
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    ESP_HW_STACK_GUARD_MONITOR_STOP_CUR_CORE \reg t1
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */
    lw      \reg, (\frame_size + RV_UCTX_SATP)(sp)
    rv_user_activate_satp \reg, t1
.endm

/**
 * @brief Switch from the M-mode stack to the user stack and return to U-mode.
 *
 * sp must point to the top of the task's M-mode stack, so the trap frame must have been freed.
 * Afterwards, mscratch holds the top of the task's M-mode stack for the next trap entry.
 */
.macro rv_user_trap_exit_to_user
    csrrw   sp, mscratch, sp
    mret
.endm

#endif /* (defined(_ASMLANGUAGE) || defined(__ASSEMBLER__)) && CONFIG_RISCV_USER_MODE_TRAPS */
