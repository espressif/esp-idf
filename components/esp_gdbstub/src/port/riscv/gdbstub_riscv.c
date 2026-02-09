/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_gdbstub.h"
#include "esp_gdbstub_common.h"
#include "soc/soc_caps.h"
#include "esp_cpu.h"
#include "esp_ipc_isr.h"
#include "rv_decode.h"
#include "sdkconfig.h"
#include "esp_private/crosscore_int.h"
#include "freertos/freertos_debug.h"
#include "freertos/portmacro.h"
#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"

#define GDBSTUB_CSA_NOT_INITIALIZED ((void *) 0xFFFFFFFF)

extern volatile esp_gdbstub_frame_t *temp_regs_frame;

static inline void init_regfile(esp_gdbstub_gdb_regfile_t *dst)
{
    memset(dst, 0, sizeof(*dst));
}

#if SOC_CPU_HAS_FPU
static void esp_gdbstub_fpu_regs_to_regfile (esp_gdbstub_gdb_regfile_t *dst)
{
    /*
     * NOTE: The GDB stub logic executes within an ISR, where coprocessors are disabled.
     *       Therefore, we must enable the coprocessor before reading or writing its registers.
     */
    rv_utils_enable_fpu();
    __asm__ volatile ("fsw ft0, %0" : "=m" (dst->f.ft0));
    __asm__ volatile ("fsw ft1, %0" : "=m" (dst->f.ft1));
    __asm__ volatile ("fsw ft2, %0" : "=m" (dst->f.ft2));
    __asm__ volatile ("fsw ft3, %0" : "=m" (dst->f.ft3));
    __asm__ volatile ("fsw ft4, %0" : "=m" (dst->f.ft4));
    __asm__ volatile ("fsw ft5, %0" : "=m" (dst->f.ft5));
    __asm__ volatile ("fsw ft6, %0" : "=m" (dst->f.ft6));
    __asm__ volatile ("fsw ft7, %0" : "=m" (dst->f.ft7));
    __asm__ volatile ("fsw fs0, %0" : "=m" (dst->f.fs0));
    __asm__ volatile ("fsw fs1, %0" : "=m" (dst->f.fs1));
    __asm__ volatile ("fsw fa0, %0" : "=m" (dst->f.fa0));
    __asm__ volatile ("fsw fa1, %0" : "=m" (dst->f.fa1));
    __asm__ volatile ("fsw fa2, %0" : "=m" (dst->f.fa2));
    __asm__ volatile ("fsw fa3, %0" : "=m" (dst->f.fa3));
    __asm__ volatile ("fsw fa4, %0" : "=m" (dst->f.fa4));
    __asm__ volatile ("fsw fa5, %0" : "=m" (dst->f.fa5));
    __asm__ volatile ("fsw fa6, %0" : "=m" (dst->f.fa6));
    __asm__ volatile ("fsw fa7, %0" : "=m" (dst->f.fa7));
    __asm__ volatile ("fsw fs2, %0" : "=m" (dst->f.fs2));
    __asm__ volatile ("fsw fs3, %0" : "=m" (dst->f.fs3));
    __asm__ volatile ("fsw fs4, %0" : "=m" (dst->f.fs4));
    __asm__ volatile ("fsw fs5, %0" : "=m" (dst->f.fs5));
    __asm__ volatile ("fsw fs6, %0" : "=m" (dst->f.fs6));
    __asm__ volatile ("fsw fs7, %0" : "=m" (dst->f.fs7));
    __asm__ volatile ("fsw fs8, %0" : "=m" (dst->f.fs8));
    __asm__ volatile ("fsw fs9, %0" : "=m" (dst->f.fs9));
    __asm__ volatile ("fsw fs10, %0" : "=m" (dst->f.fs10));
    __asm__ volatile ("fsw fs11, %0" : "=m" (dst->f.fs11));
    __asm__ volatile ("fsw ft8, %0" : "=m" (dst->f.ft8));
    __asm__ volatile ("fsw ft9, %0" : "=m" (dst->f.ft9));
    __asm__ volatile ("fsw ft10, %0" : "=m" (dst->f.ft10));
    __asm__ volatile ("fsw ft11, %0" : "=m" (dst->f.ft11));
    __asm__ volatile ("csrr %0, fcsr" : "=r" (dst->f.fcsr));
    rv_utils_disable_fpu();
}
#endif

#if SOC_CPU_HAS_PIE
static void esp_gdbstub_pie_regs_to_regfile (esp_gdbstub_gdb_regfile_t *dst)
{
    /*
     * NOTE: The GDB stub logic executes within an ISR, where coprocessors are disabled.
     *       Therefore, we must enable the coprocessor before reading or writing its registers.
     */
    rv_utils_enable_pie();
    register void* ptr asm("a5") = &dst->pie;
    __asm__ volatile ("esp.vst.128.ip q0, %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.vst.128.ip q1, %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.vst.128.ip q2, %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.vst.128.ip q3, %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.vst.128.ip q4, %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.vst.128.ip q5, %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.vst.128.ip q6, %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.vst.128.ip q7, %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.st.qacc.l.l.128.ip %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.st.qacc.l.h.128.ip %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.st.qacc.h.l.128.ip %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.st.qacc.h.h.128.ip %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.st.ua.state.ip %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.st.s.xacc.ip %0, 0" :: "r" (ptr));
    rv_utils_disable_pie();
}
#endif

#if SOC_CPU_HAS_FPU || SOC_CPU_HAS_PIE
static void *esp_gdbstub_coproc_saved_area(const StaticTask_t *tcb, int coproc, bool is_read) {
    /*
     * Coprocessors have lazy register saving mechanism:
     * 1. Defer saving coprocessor registers until a task actually uses a coprocessor instruction.
     *    This triggers an exception when the instruction is executed.
     * 2. In the exception handler:
     *    - Enable the coprocessor and designate the task as the new coprocessor owner.
     *    - If another task previously owned the coprocessor, save its registers.
     *    - Restore the coprocessor registers for the new owner before execution resumes.
     *
     *
     * To determine whether to read/write coprocessor registers directly or use stack memory:
     * - Check if the current task is the coprocessor owner:
     *   - Yes: Perform direct read/write operations.
     *   - No: Read/write from the stack.
     */

    RvCoprocSaveArea* pxPortGetCoprocArea(StaticTask_t* task, bool allocate, int coproc);

    RvCoprocSaveArea* csa;
    extern StaticTask_t* port_uxCoprocOwner[portNUM_PROCESSORS][SOC_CPU_COPROC_NUM];
    uint32_t core = rv_utils_get_core_id();
    uint32_t coproc_bit;
    if (port_uxCoprocOwner[core][coproc] == tcb) {
        return NULL;
    }

    csa = pxPortGetCoprocArea((StaticTask_t*)tcb, false, coproc);
    if (csa->sa_coprocs[coproc]) {
        return csa->sa_coprocs[coproc];
    }

    if (is_read) {
        /* Don't allocate csa for coprocessor for read. (Just return zeroed registers) */
        return GDBSTUB_CSA_NOT_INITIALIZED;
    }

    /*
     * Ignore 'csa->sa_enable' flag to read/write from the frame if the coprocessor has not been used by this task yet.
     * This ensures that the registers are correctly restored when the task's coprocessor context is switched.
     * That's why true is passed to the allocate parameter of pxPortGetCoprocArea.
     */
    /* TODO: IDF-12550. Provide correct read/write access for coprocessor owned by another CPU.
     * Accessing registers in stack-frame is not correct in this case.
     */
    csa = pxPortGetCoprocArea((StaticTask_t*)tcb, true, coproc);
    coproc_bit = 1 << coproc;
    if (!(csa->sa_enable & coproc_bit)) {
        uint32_t sa_coproc_size = coproc == 0 ? RV_COPROC0_SIZE : (coproc == 1 ? RV_COPROC1_SIZE : RV_COPROC2_SIZE);
        /* coproc registers were not saved for this task yet. Initialize with zeroes. */
        memset(csa->sa_coprocs[coproc], 0, sa_coproc_size);
        csa->sa_enable |= coproc_bit;
    }
    return csa->sa_coprocs[coproc];
}

static void esp_gdbstub_coproc_regs_to_regfile(const esp_gdbstub_frame_t *frame, esp_gdbstub_gdb_regfile_t *dst)
{
    const StaticTask_t *tcb = esp_gdbstub_find_tcb_by_frame(frame);
    void *csa;
#if SOC_CPU_HAS_FPU
    csa = esp_gdbstub_coproc_saved_area(tcb, FPU_COPROC_IDX, true);
    if (csa == NULL) {
        esp_gdbstub_fpu_regs_to_regfile(dst);
    } else if (csa == GDBSTUB_CSA_NOT_INITIALIZED) {
        memset(&dst->f, 0, sizeof(dst->f));
    } else {
        memcpy(&dst->f, csa, sizeof(dst->f));
    }
#endif
#if SOC_CPU_HAS_PIE
    csa = esp_gdbstub_coproc_saved_area(tcb, PIE_COPROC_IDX, true);
    if (csa == NULL) {
        esp_gdbstub_pie_regs_to_regfile(dst);
    } else if (csa == GDBSTUB_CSA_NOT_INITIALIZED) {
        memset(&dst->pie, 0, sizeof(dst->pie));
    } else {
        memcpy(&dst->pie, csa, sizeof(dst->pie));
    }
#endif
}
#endif /* SOC_CPU_HAS_FPU || SOC_CPU_HAS_PIE */

void esp_gdbstub_frame_to_regfile(const esp_gdbstub_frame_t *frame, esp_gdbstub_gdb_regfile_t *dst)
{
    init_regfile(dst);
    dst->pc = frame->mepc;

    // We omit register x0 here since it's the zero register and always hard-wired to 0.
    // See The RISC-V Instruction Set Manual Volume I: Unprivileged ISA Document Version 20191213 for more details.
    memcpy(&(dst->x[1]), &frame->ra, sizeof(uint32_t) * 31);

#if SOC_CPU_HAS_FPU || SOC_CPU_HAS_PIE
    esp_gdbstub_coproc_regs_to_regfile(frame, dst);
#endif
}

#if CONFIG_ESP_GDBSTUB_SUPPORT_TASKS

void esp_gdbstub_tcb_to_regfile(TaskHandle_t tcb, esp_gdbstub_gdb_regfile_t *dst)
{
    const RvExcFrame *frame = (RvExcFrame *) ((StaticTask_t *) tcb)->pxDummy1 /* top_of_stack */;
    esp_gdbstub_frame_to_regfile(frame, dst);
}

#endif // CONFIG_ESP_GDBSTUB_SUPPORT_TASKS

int esp_gdbstub_get_signal(const esp_gdbstub_frame_t *frame)
{
    switch (frame->mcause) {
        case 0:   /* Instruction address misaligned */
        case 1:   /* Instruction access fault */
        case 2:   /* Illegal instruction */
            return 4;  /* SIGILL  */
        case 3:   /* Breakpoint */
            return 5;  /* SIGTRAP  */
        case 4:   /* Load address misaligned */
        case 5:   /* Load access fault */
        case 6:   /* Store/AMO address misaligned */
        case 7:   /* Store/AMO access fault */
            return 11;  /* SIGSEGV  */
        case 8:   /* Environment call from U-mode */
        case 9:   /* Environment call from S-mode */
        // case 10:  /* Reserved */
        case 11:  /* Environment call from M-mode */
            return 5;  /* SIGTRAP  */
        case 12:  /* Instruction page fault */
        case 13:  /* Load page fault */
        // case 14:  /* Reserved */
        case 15:  /* Store/AMO page fault */
            return 11;  /* SIGSEGV  */
    };

    return 5;  /* SIGTRAP  */
}

#ifdef CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
void esp_gdbstub_int(__attribute__((unused)) void *frame)
{
   /* Pointer to saved frame is in pxCurrentTCB
    * See rtos_int_enter function
    */
    StaticTask_t *tcb = (StaticTask_t *)pvTaskGetCurrentTCBForCore(esp_cpu_get_core_id());
    gdbstub_handle_uart_int((esp_gdbstub_frame_t *)tcb->pxDummy1 /* top_of_stack */);
}

void esp_gdbstub_init_dports(void)
{
}

#endif // CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME

#if (!CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE) && CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
static bool stall_started = false;
#endif

void esp_gdbstub_stall_other_cpus_start(void)
{
#if (!CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE) && CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
    if (stall_started == false) {
        esp_ipc_isr_stall_other_cpu();
        stall_started = true;
    }
#endif
}

void esp_gdbstub_stall_other_cpus_end(void)
{
#if (!CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE) && CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
    if (stall_started == true) {
        esp_ipc_isr_release_other_cpu();
        stall_started = false;
    }
#endif
}

void esp_gdbstub_clear_step(void)
{
#ifdef CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
    /* Setup triggers again because we removed them in esp_gdbstub_do_step()  */
    update_breakpoints();
#endif // CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
}

void esp_gdbstub_do_step(esp_gdbstub_frame_t *frame)
{
#ifdef CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
    uint32_t pc = (uint32_t) frame->mepc;
    uint32_t next_pc = rv_compute_next_pc(frame, pc);
    esp_cpu_set_breakpoint(0, (void *) next_pc);
    for (size_t i = 1; i < SOC_CPU_BREAKPOINTS_NUM; i++) {
        esp_cpu_clear_breakpoint(i);
    }
#endif // CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
}

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

#if SOC_CPU_HAS_FPU
void esp_gdbstub_set_fpu_register(uint32_t reg_index, uint32_t value)
{
    /*
     * NOTE: The GDB stub logic executes within an ISR, where coprocessors are disabled.
     *       Therefore, we must enable the coprocessor before reading or writing its registers.
     */
    rv_utils_enable_fpu();
    if (reg_index == 0) {
        __asm__ volatile ("flw ft0, %0" : "=m" (value));
    } else if (reg_index == 1) {
        __asm__ volatile ("flw ft1, %0" : "=m" (value));
    } else if (reg_index == 2) {
        __asm__ volatile ("flw ft2, %0" : "=m" (value));
    } else if (reg_index == 3) {
        __asm__ volatile ("flw ft3, %0" : "=m" (value));
    } else if (reg_index == 4) {
        __asm__ volatile ("flw ft4, %0" : "=m" (value));
    } else if (reg_index == 5) {
        __asm__ volatile ("flw ft5, %0" : "=m" (value));
    } else if (reg_index == 6) {
        __asm__ volatile ("flw ft6, %0" : "=m" (value));
    } else if (reg_index == 7) {
        __asm__ volatile ("flw ft7, %0" : "=m" (value));
    } else if (reg_index == 8) {
        __asm__ volatile ("flw fs0, %0" : "=m" (value));
    } else if (reg_index == 9) {
        __asm__ volatile ("flw fs1, %0" : "=m" (value));
    } else if (reg_index == 10) {
        __asm__ volatile ("flw fa0, %0" : "=m" (value));
    } else if (reg_index == 11) {
        __asm__ volatile ("flw fa1, %0" : "=m" (value));
    } else if (reg_index == 12) {
        __asm__ volatile ("flw fa2, %0" : "=m" (value));
    } else if (reg_index == 13) {
        __asm__ volatile ("flw fa3, %0" : "=m" (value));
    } else if (reg_index == 14) {
        __asm__ volatile ("flw fa4, %0" : "=m" (value));
    } else if (reg_index == 15) {
        __asm__ volatile ("flw fa5, %0" : "=m" (value));
    } else if (reg_index == 16) {
        __asm__ volatile ("flw fa6, %0" : "=m" (value));
    } else if (reg_index == 17) {
        __asm__ volatile ("flw fa7, %0" : "=m" (value));
    } else if (reg_index == 18) {
        __asm__ volatile ("flw fs2, %0" : "=m" (value));
    } else if (reg_index == 19) {
        __asm__ volatile ("flw fs3, %0" : "=m" (value));
    } else if (reg_index == 20) {
        __asm__ volatile ("flw fs4, %0" : "=m" (value));
    } else if (reg_index == 21) {
        __asm__ volatile ("flw fs5, %0" : "=m" (value));
    } else if (reg_index == 22) {
        __asm__ volatile ("flw fs6, %0" : "=m" (value));
    } else if (reg_index == 23) {
        __asm__ volatile ("flw fs7, %0" : "=m" (value));
    } else if (reg_index == 24) {
        __asm__ volatile ("flw fs8, %0" : "=m" (value));
    } else if (reg_index == 25) {
        __asm__ volatile ("flw fs9, %0" : "=m" (value));
    } else if (reg_index == 26) {
        __asm__ volatile ("flw fs10, %0" : "=m" (value));
    } else if (reg_index == 27) {
        __asm__ volatile ("flw fs11, %0" : "=m" (value));
    } else if (reg_index == 28) {
        __asm__ volatile ("flw ft8, %0" : "=m" (value));
    } else if (reg_index == 29) {
        __asm__ volatile ("flw ft9, %0" : "=m" (value));
    } else if (reg_index == 30) {
        __asm__ volatile ("flw ft10, %0" : "=m" (value));
    } else if (reg_index == 31) {
        __asm__ volatile ("flw ft11, %0" : "=m" (value));
    } else if (reg_index == 32) {
        __asm__ volatile ("csrw fcsr, %0" : : "r" (value));
    }
    rv_utils_disable_fpu();
}
#endif /* SOC_CPU_HAS_FPU */

#if SOC_CPU_HAS_PIE
void esp_gdbstub_set_pie_register(uint32_t reg_index, uint32_t *value_ptr)
{
    /*
     * NOTE: The GDB stub logic executes within an ISR, where coprocessors are disabled.
     *       Therefore, we must enable the coprocessor before reading or writing its registers.
     */
    register void *ptr asm("a5") = value_ptr;
    rv_utils_enable_pie();
    if (reg_index == 0) {
        __asm__ volatile ("esp.vld.128.ip q0, %0, 0" :: "r" (ptr));
    } else if (reg_index == 1) {
        __asm__ volatile ("esp.vld.128.ip q1, %0, 0" :: "r" (ptr));
    } else if (reg_index == 2) {
        __asm__ volatile ("esp.vld.128.ip q2, %0, 0" :: "r" (ptr));
    } else if (reg_index == 3) {
        __asm__ volatile ("esp.vld.128.ip q3, %0, 0" :: "r" (ptr));
    } else if (reg_index == 4) {
        __asm__ volatile ("esp.vld.128.ip q4, %0, 0" :: "r" (ptr));
    } else if (reg_index == 5) {
        __asm__ volatile ("esp.vld.128.ip q5, %0, 0" :: "r" (ptr));
    } else if (reg_index == 6) {
        __asm__ volatile ("esp.vld.128.ip q6, %0, 0" :: "r" (ptr));
    } else if (reg_index == 7) {
        __asm__ volatile ("esp.vld.128.ip q7, %0, 0" :: "r" (ptr));
    } else if (reg_index == 8) {
        __asm__ volatile ("esp.ld.qacc.l.l.128.ip %0, 0" :: "r" (ptr));
    } else if (reg_index == 9) {
        __asm__ volatile ("esp.ld.qacc.l.h.128.ip %0, 0" :: "r" (ptr));
    } else if (reg_index == 10) {
        __asm__ volatile ("esp.ld.qacc.h.l.128.ip %0, 0" :: "r" (ptr));
    } else if (reg_index == 11) {
        __asm__ volatile ("esp.ld.qacc.h.h.128.ip %0, 0" :: "r" (ptr));
    } else if (reg_index == 12) {
        __asm__ volatile ("esp.ld.ua.state.ip %0, 0" :: "r" (ptr));
    } else if (reg_index == 13) {
        __asm__ volatile ("esp.ld.xacc.ip %0, 0" :: "r" (ptr));
    }
    rv_utils_disable_pie();
}
#endif /* SOC_CPU_HAS_PIE */

#if SOC_CPU_HAS_FPU || SOC_CPU_HAS_PIE
void esp_gdbstub_set_coproc_register(esp_gdbstub_frame_t *frame, uint32_t reg_index, uint32_t *value_ptr) {
    const StaticTask_t *tcb = esp_gdbstub_find_tcb_by_frame(frame);
    uint32_t *csa; /* points to coprocessor registers data. */
#if SOC_CPU_HAS_FPU
    if (reg_index >= 33 && reg_index <= 68) {
        reg_index -= 33;
        if (reg_index > 32 /* fcsr */) {
            reg_index = 32;
        }
        csa = esp_gdbstub_coproc_saved_area(tcb, FPU_COPROC_IDX, false);
        if (csa == NULL) {
            esp_gdbstub_set_fpu_register(reg_index, *value_ptr);
        } else {
            csa[reg_index] = *value_ptr;
        }
        return;
    }
#endif
#if SOC_CPU_HAS_PIE
    if (reg_index >= 4211 && reg_index <= 4224) {
        reg_index -= 4211;
        csa = esp_gdbstub_coproc_saved_area(tcb, PIE_COPROC_IDX, false);
        if (csa == NULL) {
            esp_gdbstub_set_pie_register(reg_index, value_ptr);
        } else {
            if (reg_index < 13) { /* 128-bit registers: q, qacc, ua_state */
                memcpy(&csa[reg_index * 4], value_ptr, sizeof(uint32_t) * 4);
            } else { /* 40-bit regiseter: xacc */
                memcpy(&csa[4 * 13], value_ptr, sizeof(uint8_t) * 5);
            }
        }
    }
#endif
}
#endif /* SOC_CPU_HAS_FPU || SOC_CPU_HAS_PIE */

void esp_gdbstub_set_register(esp_gdbstub_frame_t *frame, uint32_t reg_index, uint32_t *value_ptr)
{
    uint32_t value = *value_ptr;

    /* RISC-V base ISA has registers x0-x31  */
    if (reg_index == 0) { /* skip zero-wired register  */
        return;
    } else if (reg_index < 32) {
        (&frame->mepc)[reg_index] = value;
    } else if (reg_index == 32) { /* register 32 is PC  */
        frame->mepc = value;
    }
#if SOC_CPU_HAS_FPU || SOC_CPU_HAS_PIE
    esp_gdbstub_set_coproc_register(frame, reg_index, value_ptr);
#endif
}
