/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_gdbstub_common.h"
#include "soc/soc_memory_layout.h"
#include "xtensa/config/specreg.h"
#include "sdkconfig.h"
#include "esp_cpu.h"
#include "esp_ipc_isr.h"
#include "esp_private/crosscore_int.h"
#include <xtensa_context.h>

#if !XCHAL_HAVE_WINDOWED
#warning "gdbstub_xtensa: revisit the implementation for Call0 ABI"
#endif

extern int _invalid_pc_placeholder;

static inline void init_regfile(esp_gdbstub_gdb_regfile_t *dst)
{
    memset(dst, 0, sizeof(*dst));
}

static void update_regfile_common(esp_gdbstub_gdb_regfile_t *dst)
{
    if (dst->a[0] & 0x8000000U) {
        dst->a[0] = (uint32_t)esp_cpu_pc_to_addr(dst->a[0]);
    }
    if (!esp_stack_ptr_is_sane(dst->a[1])) {
        dst->a[1] = 0xDEADBEEF;
    }
    dst->windowbase = 0;
    dst->windowstart = 0x1;
    RSR(CONFIGID0, dst->configid0);
    RSR(CONFIGID1, dst->configid1);
}

#if XCHAL_HAVE_FP
/** @brief Read FPU registers to memory
*/
static void gdbstub_read_fpu_regs(void *data)
{
    float *ptr0;
    void  *ptr1;

    asm volatile ("mov %0, %1" : "=a" (ptr0) : "a" (data));

    asm volatile ("rur.FCR %0" : "=a" (ptr1));
    asm volatile ("s32i %0, %1, 64" : "=a" (ptr1) : "a" (ptr0));
    asm volatile ("rur.FSR %0" : "=a" (ptr1));
    asm volatile ("s32i %0, %1, 68" : "=a" (ptr1) : "a" (ptr0));

    asm volatile ("ssi f0, %0, 0" :: "a" (ptr0)); //*(ptr0 + 0) = f0;
    asm volatile ("ssi f1, %0, 4" :: "a" (ptr0)); //*(ptr0 + 4) = f1;
    asm volatile ("ssi f2, %0, 8" :: "a" (ptr0)); //...
    asm volatile ("ssi f3, %0, 12" :: "a" (ptr0));
    asm volatile ("ssi f4, %0, 16" :: "a" (ptr0));
    asm volatile ("ssi f5, %0, 20" :: "a" (ptr0));
    asm volatile ("ssi f6, %0, 24" :: "a" (ptr0));
    asm volatile ("ssi f7, %0, 28" :: "a" (ptr0));
    asm volatile ("ssi f8, %0, 32" :: "a" (ptr0));
    asm volatile ("ssi f9, %0, 36" :: "a" (ptr0));
    asm volatile ("ssi f10, %0, 40" :: "a" (ptr0));
    asm volatile ("ssi f11, %0, 44" :: "a" (ptr0));
    asm volatile ("ssi f12, %0, 48" :: "a" (ptr0));
    asm volatile ("ssi f13, %0, 52" :: "a" (ptr0));
    asm volatile ("ssi f14, %0, 56" :: "a" (ptr0));
    asm volatile ("ssi f15, %0, 60" :: "a" (ptr0));

}
#endif // XCHAL_HAVE_FP


extern const uint32_t offset_pxEndOfStack;
extern const uint32_t offset_cpsa;  /* Offset to start of the CPSA area on the stack. See uxInitialiseStackCPSA(). */
extern uint32_t _xt_coproc_owner_sa[2];

void esp_gdbstub_frame_to_regfile(const esp_gdbstub_frame_t *frame, esp_gdbstub_gdb_regfile_t *dst)
{
    init_regfile(dst);
    const uint32_t *a_regs = (const uint32_t *) &frame->a0;

    if (!(esp_ptr_executable(esp_cpu_pc_to_addr(frame->pc)) && (frame->pc & 0xC0000000U))) {
        /* Xtensa ABI sets the 2 MSBs of the PC according to the windowed call size
         * In case the PC is invalid, GDB will fail to translate addresses to function names
         * Hence replacing the PC to a placeholder address in case of invalid PC
         */
        dst->pc = (uint32_t)&_invalid_pc_placeholder;
    } else {
        dst->pc = (uint32_t)esp_cpu_pc_to_addr(frame->pc);
    }

    for (int i = 0; i < 16; i++) {
        dst->a[i] = a_regs[i];
    }
    for (int i = 16; i < 64; i++) {
        dst->a[i] = 0xDEADBEEF;
    }

#if XCHAL_HAVE_FP

    extern void *pxCurrentTCBs[2];
    void *current_tcb_ptr = pxCurrentTCBs[0];
    uint32_t *current_fpu_ptr = NULL;

#if !CONFIG_FREERTOS_UNICORE
    current_tcb_ptr = pxCurrentTCBs[esp_cpu_get_core_id()];
#endif
    uint32_t cp_enabled;
    RSR(CPENABLE, cp_enabled);

    // Check if the co-processor is enabled
    if (cp_enabled) {
        gdbstub_read_fpu_regs(dst->f);
    } else {
        current_tcb_ptr += offset_pxEndOfStack;
        current_tcb_ptr = *(void **)current_tcb_ptr;
        current_tcb_ptr -= offset_cpsa;
        // Operation (&~0xf) required in .macro get_cpsa_from_tcb reg_A reg_B
        current_tcb_ptr = (void*)((uint32_t)current_tcb_ptr&~0xf);
        current_fpu_ptr = *(uint32_t **)(current_tcb_ptr + XT_CP_ASA);

        dst->fcr = current_fpu_ptr[0];
        dst->fsr = current_fpu_ptr[1];
        for (int i = 0; i < 16; i++) {
            dst->f[i] = current_fpu_ptr[i + 2];
        }
    }
#endif //XCHAL_HAVE_FP
#if XCHAL_HAVE_LOOPS
    dst->lbeg = frame->lbeg;
    dst->lend = frame->lend;
    dst->lcount = frame->lcount;
#endif

    dst->ps = (frame->ps & PS_UM) ? (frame->ps & ~PS_EXCM) : frame->ps;
    dst->sar = frame->sar;
    update_regfile_common(dst);
}

#ifdef CONFIG_ESP_GDBSTUB_SUPPORT_TASKS

/* Represents FreeRTOS TCB structure */
typedef struct {
    uint8_t *top_of_stack;
    /* Other members aren't needed */
} dummy_tcb_t;

void esp_gdbstub_tcb_frame_to_regfile(dummy_tcb_t *tcb, esp_gdbstub_gdb_regfile_t *dst)
{
    const XtExcFrame *frame = (XtExcFrame *) tcb->top_of_stack;

    init_regfile(dst);
    const uint32_t *a_regs = (const uint32_t *) &frame->a0;

    if (!(esp_ptr_executable(esp_cpu_pc_to_addr(frame->pc)) && (frame->pc & 0xC0000000U))) {
        /* Xtensa ABI sets the 2 MSBs of the PC according to the windowed call size
         * In case the PC is invalid, GDB will fail to translate addresses to function names
         * Hence replacing the PC to a placeholder address in case of invalid PC
         */
        dst->pc = (uint32_t)&_invalid_pc_placeholder;
    } else {
        dst->pc = (uint32_t)esp_cpu_pc_to_addr(frame->pc);
    }

    for (int i = 0; i < 16; i++) {
        dst->a[i] = a_regs[i];
    }
    for (int i = 16; i < 64; i++) {
        dst->a[i] = 0xDEADBEEF;
    }

#if XCHAL_HAVE_FP
    uint32_t *current_xt_coproc_owner_sa = (uint32_t *)_xt_coproc_owner_sa[0];

#if !CONFIG_FREERTOS_UNICORE
    current_xt_coproc_owner_sa = (uint32_t *)_xt_coproc_owner_sa[esp_cpu_get_core_id()];
#endif

    uint32_t cp_enabled;
    RSR(CPENABLE, cp_enabled);

    void *current_tcb_ptr = tcb;
    uint32_t *current_fpu_ptr = NULL;
    {
        current_tcb_ptr += offset_pxEndOfStack;
        current_tcb_ptr = *(void **)current_tcb_ptr;
        current_tcb_ptr -= offset_cpsa;
        // Operation (&~0xf) required in .macro get_cpsa_from_tcb reg_A reg_B
        current_tcb_ptr = (void*)((uint32_t)current_tcb_ptr&~0xf);
        current_fpu_ptr = *(uint32_t **)(current_tcb_ptr + XT_CP_ASA);

        bool use_fpu_regs =  ((false == cp_enabled) &&  (current_xt_coproc_owner_sa[0] == 1) && (current_fpu_ptr == (uint32_t*)current_xt_coproc_owner_sa[2]));

        dst->fcr = current_fpu_ptr[0];
        dst->fsr = current_fpu_ptr[1];
        for (int i = 0; i < 16; i++) {
            dst->f[i] = current_fpu_ptr[i + 2];
        }

        /* We have situation when FPU is in use, but the context not stored
          to the memory, and we have to read from CPU registers.
        */
        if (use_fpu_regs) {
            gdbstub_read_fpu_regs(dst->f);
        }
    }
#endif // XCHAL_HAVE_FP

#if XCHAL_HAVE_LOOPS
    dst->lbeg = frame->lbeg;
    dst->lend = frame->lend;
    dst->lcount = frame->lcount;
#endif

    dst->ps = (frame->ps & PS_UM) ? (frame->ps & ~PS_EXCM) : frame->ps;
    dst->sar = frame->sar;
    update_regfile_common(dst);
}

static void solicited_frame_to_regfile(const XtSolFrame *frame, esp_gdbstub_gdb_regfile_t *dst)
{
    init_regfile(dst);
    const uint32_t *a_regs = (const uint32_t *) &frame->a0;
    if (!(esp_ptr_executable(esp_cpu_pc_to_addr(frame->pc)) && (frame->pc & 0xC0000000U))) {
        dst->pc = (uint32_t)&_invalid_pc_placeholder;
    } else {
        dst->pc = (uint32_t)esp_cpu_pc_to_addr(frame->pc);
    }

    /* only 4 registers saved in the solicited frame */
    for (int i = 0; i < 4; i++) {
        dst->a[i] = a_regs[i];
    }
    for (int i = 4; i < 64; i++) {
        dst->a[i] = 0xDEADBEEF;
    }

    dst->ps = (frame->ps & PS_UM) ? (frame->ps & ~PS_EXCM) : frame->ps;
    update_regfile_common(dst);
}

void esp_gdbstub_tcb_to_regfile(TaskHandle_t tcb, esp_gdbstub_gdb_regfile_t *dst)
{
    dummy_tcb_t *dummy_tcb = (dummy_tcb_t *) tcb;
    const XtExcFrame *frame = (XtExcFrame *) dummy_tcb->top_of_stack;
    if (frame->exit != 0) {
        esp_gdbstub_tcb_frame_to_regfile(dummy_tcb, dst);
    } else {
        const XtSolFrame *taskFrame = (const XtSolFrame *) dummy_tcb->top_of_stack;
        solicited_frame_to_regfile(taskFrame, dst);
    }
}

#endif // CONFIG_ESP_GDBSTUB_SUPPORT_TASKS

int esp_gdbstub_get_signal(const esp_gdbstub_frame_t *frame)
{
    const char exccause_to_signal[] = {4, 31, 11, 11, 2, 6, 8, 0, 6, 7, 0, 0, 7, 7, 7, 7};
    if (frame->exccause >= sizeof(exccause_to_signal)) {
        return 11;
    }
    return (int) exccause_to_signal[frame->exccause];
}

/** @brief Init dport for GDB
 * Init dport for iterprocessor communications
 * */
void esp_gdbstub_init_dports(void)
{
}

#if CONFIG_IDF_TARGET_ARCH_XTENSA && (!CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE) && CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
static bool stall_started = false;
#endif

/** @brief GDB stall other CPU
 * GDB stall other CPU
 * */
void esp_gdbstub_stall_other_cpus_start(void)
{
#if CONFIG_IDF_TARGET_ARCH_XTENSA && (!CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE) && CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
    if (stall_started == false) {
        esp_ipc_isr_stall_other_cpu();
        stall_started = true;
    }
#endif
}

/** @brief GDB end stall other CPU
 * GDB end stall other CPU
 * */
void esp_gdbstub_stall_other_cpus_end(void)
{
#if CONFIG_IDF_TARGET_ARCH_XTENSA && (!CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE) && CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
    if (stall_started == true) {
        esp_ipc_isr_release_other_cpu();
        stall_started = false;
    }
#endif
}

/** @brief GDB clear step
 * GDB clear step registers
 * */
void esp_gdbstub_clear_step(void)
{
    WSR(ICOUNT, 0);
    WSR(ICOUNTLEVEL, 0);
}

/** @brief GDB do step
 * GDB do one step
 * */
void esp_gdbstub_do_step( esp_gdbstub_frame_t *frame)
{
    // We have gdbstub uart interrupt, and if we will call step, with ICOUNTLEVEL=2 or higher, from uart interrupt, the
    // application will hang because it will try to step uart interrupt. That's why we have to set ICOUNTLEVEL=1
    // If we will stop by the breakpoint inside interrupt, we will handle this interrupt with ICOUNTLEVEL=ps.intlevel+1

    uint32_t level = s_scratch.regfile.ps;
    level &= 0x7;
    level += 1;

    WSR(ICOUNTLEVEL, level);
    WSR(ICOUNT, -2);
}

/** @brief GDB trigger other CPU
 * GDB trigger other CPU
 * */
void esp_gdbstub_trigger_cpu(void)
{
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    if (0 == esp_cpu_get_core_id()) {
        esp_crosscore_int_send_gdb_call(1);
    } else {
        esp_crosscore_int_send_gdb_call(0);
    }
#endif
}

/** @brief GDB set register in frame
 * Set register in frame with address to value
 *
 * */

void esp_gdbstub_set_register(esp_gdbstub_frame_t *frame, uint32_t reg_index, uint32_t value)
{
    uint32_t temp_fpu_value = value;
    float *ptr0;

    asm volatile ("mov %0, %1" : "=a" (ptr0) : "a" (&temp_fpu_value));

    if (reg_index == 0) {
        frame->pc = value;
    } else if (reg_index > 0 && (reg_index <= 27)) {
        (&frame->a0)[reg_index - 1] = value;
    }
#if XCHAL_HAVE_FP
    void  *ptr1;
    uint32_t cp_enabled;
    RSR(CPENABLE, cp_enabled);
    if (cp_enabled != 0) {
        if (reg_index == 87) {
            asm volatile ("lsi f0, %0, 0" :: "a" (ptr0));
        }
        if (reg_index == 88) {
            asm volatile ("lsi f1, %0, 0" :: "a" (ptr0));
        }
        if (reg_index == 89) {
            asm volatile ("lsi f2, %0, 0" :: "a" (ptr0));
        }
        if (reg_index == 90) {
            asm volatile ("lsi f3, %0, 0" :: "a" (ptr0));
        }
        if (reg_index == 91) {
            asm volatile ("lsi f4, %0, 0" :: "a" (ptr0));
        }
        if (reg_index == 92) {
            asm volatile ("lsi f5, %0, 0" :: "a" (ptr0));
        }
        if (reg_index == 93) {
            asm volatile ("lsi f6, %0, 0" :: "a" (ptr0));
        }
        if (reg_index == 94) {
            asm volatile ("lsi f7, %0, 0" :: "a" (ptr0));
        }
        if (reg_index == 95) {
            asm volatile ("lsi f8, %0, 0" :: "a" (ptr0));
        }
        if (reg_index == 96) {
            asm volatile ("lsi f9, %0, 0" :: "a" (ptr0));
        }
        if (reg_index == 97) {
            asm volatile ("lsi f10, %0, 0" :: "a" (ptr0));
        }
        if (reg_index == 98) {
            asm volatile ("lsi f11, %0, 0" :: "a" (ptr0));
        }
        if (reg_index == 99) {
            asm volatile ("lsi f12, %0, 0" :: "a" (ptr0));
        }
        if (reg_index == 100) {
            asm volatile ("lsi f13, %0, 0" :: "a" (ptr0));
        }
        if (reg_index == 101) {
            asm volatile ("lsi f14, %0, 0" :: "a" (ptr0));
        }
        if (reg_index == 102) {
            asm volatile ("lsi f15, %0, 0" :: "a" (ptr0));
        }
        if (reg_index == 103) {
            asm volatile ("l32i %0, %1, 0" : "=a" (ptr1) : "a" (ptr0));
            asm volatile ("wur.FCR %0" : "=a" (ptr1));
        }
        if (reg_index == 104) {
            asm volatile ("l32i %0, %1, 0" : "=a" (ptr1) : "a" (ptr0));
            asm volatile ("wur.FSR %0" : "=a" (ptr1));
        }
    }
#endif // XCHAL_HAVE_FP
}
