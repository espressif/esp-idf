/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_gdbstub_common.h"
#include "soc/soc_memory_layout.h"
#include "xtensa/config/xt_specreg.h"
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
    RSR(XT_REG_CONFIGID0, dst->configid0);
    RSR(XT_REG_CONFIGID1, dst->configid1);
}

#if XCHAL_HAVE_FP
/** @brief Read FPU registers to memory
*/
static void gdbstub_read_fpu_regs(xtensa_fpu_regs_t *fpu)
{
    uint32_t tmp;

    /* Read FPU registers from memory */
    asm volatile ("ssi f0,  %0, 0" :: "a" (&fpu->f[0]));
    asm volatile ("ssi f1,  %0, 0" :: "a" (&fpu->f[1]));
    asm volatile ("ssi f2,  %0, 0" :: "a" (&fpu->f[2]));
    asm volatile ("ssi f3,  %0, 0" :: "a" (&fpu->f[3]));
    asm volatile ("ssi f4,  %0, 0" :: "a" (&fpu->f[4]));
    asm volatile ("ssi f5,  %0, 0" :: "a" (&fpu->f[5]));
    asm volatile ("ssi f6,  %0, 0" :: "a" (&fpu->f[6]));
    asm volatile ("ssi f7,  %0, 0" :: "a" (&fpu->f[7]));
    asm volatile ("ssi f8,  %0, 0" :: "a" (&fpu->f[8]));
    asm volatile ("ssi f9,  %0, 0" :: "a" (&fpu->f[9]));
    asm volatile ("ssi f10, %0, 0" :: "a" (&fpu->f[10]));
    asm volatile ("ssi f11, %0, 0" :: "a" (&fpu->f[11]));
    asm volatile ("ssi f12, %0, 0" :: "a" (&fpu->f[12]));
    asm volatile ("ssi f13, %0, 0" :: "a" (&fpu->f[13]));
    asm volatile ("ssi f14, %0, 0" :: "a" (&fpu->f[14]));
    asm volatile ("ssi f15, %0, 0" :: "a" (&fpu->f[15]));

    /* Read FCR and FSR from CPU registers */
    asm volatile ("rur.FCR %0" : "=a" (tmp));
    asm volatile ("s32i %0, %1, 0" : "=a" (tmp) : "a" (&fpu->fcr));
    asm volatile ("rur.FSR %0" : "=a" (tmp));
    asm volatile ("s32i %0, %1, 0" : "=a" (tmp) : "a" (&fpu->fsr));
}

static void *esp_gdbstub_coproc_saved_area(void *tcb, int coproc)
{
    /**
     * Offset to start of the CPSA area on the stack. See uxInitialiseStackCPSA().
     */
    extern const uint32_t offset_cpsa;
    extern const uint32_t offset_pxEndOfStack;
    extern uintptr_t _xt_coproc_owner_sa[portNUM_PROCESSORS][XCHAL_CP_MAX];
    uint32_t core = esp_cpu_get_core_id();
    uint16_t coproc_bit = 1 << coproc;
    /*
     * Calculate CP save area header pointer (same as get_cpsa_from_tcb macro):
     * 1. Get pxEndOfStack from TCB
     * 2. Subtract offset_cpsa
     * 3. Align down to 16 bytes
     *
     * For more details refer to comments in uxInitialiseStackCPSA() in port.c.
     */
    void *cpsa_header_ptr = *(void **)((char *)tcb + offset_pxEndOfStack);
    cpsa_header_ptr = (char *)cpsa_header_ptr - offset_cpsa;
    cpsa_header_ptr = (void *)((uintptr_t)cpsa_header_ptr & ~0xF);

    /* For more details about fields, refer to comments in xtensa_context.h */
    typedef struct {
        uint16_t xt_cpenable;
        uint16_t xt_cpstored;
        uint16_t xt_cp_cs_st;
        uint16_t dummy;
        void *xt_cp_asa;
    } cpsa_header_t;
    cpsa_header_t *cpsa_header = (cpsa_header_t *)cpsa_header_ptr;

    /* TODO: IDF-12550. Provide correct read access for coprocessor owned by another CPU.
    * Accessing registers in stack-frame is not correct in this case.
    */
    for (uint32_t i = 0; i < portNUM_PROCESSORS; i++) {
        if (i == core) {
            continue;
        }
        if (_xt_coproc_owner_sa[i][coproc] == (uintptr_t)cpsa_header) {
            return cpsa_header->xt_cp_asa;
        }
    }

    /* TODO IDF-15054:
     * - Handle case when coprocessor instructions have not been called yet for this task
     */
    if ((cpsa_header->xt_cpstored & coproc_bit) ||
        (cpsa_header->xt_cp_cs_st & coproc_bit)) {
        return cpsa_header->xt_cp_asa;
    }

    return NULL;
}

static uint32_t enable_coproc(int coproc)
{
    bool fpu_enabled = false;
    uint32_t cp_enabled;

    RSR(XT_REG_CPENABLE, cp_enabled);
    if (cp_enabled & (1 << coproc)) {
        fpu_enabled = true;
    }

    if (!fpu_enabled) {
        uint32_t new_cp_enabled = cp_enabled | (1 << coproc);
        WSR(XT_REG_CPENABLE, new_cp_enabled);
    }

    return cp_enabled;
}

static void write_fpu_regs_to_regfile(void *tcb, esp_gdbstub_gdb_regfile_t *dst)
{
    xtensa_fpu_regs_t *fpu_save_area = esp_gdbstub_coproc_saved_area(tcb, XCHAL_CP_ID_FPU);

    /*
     * In case of current thread is the owner of FPU, that means FPU registers was not stored to thread TCB.
     * According to the lazy saving of FPU registers, we have to read from CPU registers.
     *
     * NOTE: FPU must be enabled before reading from CPU registers to avoid triggering exception.
     */
    if (fpu_save_area == NULL) {
        /* enable FPU first to avoid triggering exception */
        uint32_t cp_enabled = enable_coproc(XCHAL_CP_ID_FPU);

        /* Read FPU registers from CPU registers */
        gdbstub_read_fpu_regs(&dst->fpu);

        /* Restore FPU enabled state */
        WSR(XT_REG_CPENABLE, cp_enabled);
    } else {
        /* FPU registers was stored to thread TCB, copy them to the register file */
        memcpy (&dst->fpu, fpu_save_area, sizeof(dst->fpu));
    }
}
#endif // XCHAL_HAVE_FP


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
    extern void *pxCurrentTCBs[portNUM_PROCESSORS];
    write_fpu_regs_to_regfile(pxCurrentTCBs[esp_cpu_get_core_id()], dst);
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
    write_fpu_regs_to_regfile(tcb, dst);
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
    WSR(XT_REG_ICOUNT, 0);
    WSR(XT_REG_ICOUNTLEVEL, 0);
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

    WSR(XT_REG_ICOUNTLEVEL, level);
    WSR(XT_REG_ICOUNT, -2);
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

#if XCHAL_HAVE_FP
static void gdbstub_set_fpu_register(uint32_t fpu_reg_index, float *value_ptr)
{
    if (fpu_reg_index == 0) {
        asm volatile ("lsi f0, %0, 0" :: "a" (value_ptr));
    } else if (fpu_reg_index == 1) {
        asm volatile ("lsi f1, %0, 0" :: "a" (value_ptr));
    } else if (fpu_reg_index == 2) {
        asm volatile ("lsi f2, %0, 0" :: "a" (value_ptr));
    } else if (fpu_reg_index == 3) {
        asm volatile ("lsi f3, %0, 0" :: "a" (value_ptr));
    } else if (fpu_reg_index == 4) {
        asm volatile ("lsi f4, %0, 0" :: "a" (value_ptr));
    } else if (fpu_reg_index == 5) {
        asm volatile ("lsi f5, %0, 0" :: "a" (value_ptr));
    } else if (fpu_reg_index == 6) {
        asm volatile ("lsi f6, %0, 0" :: "a" (value_ptr));
    } else if (fpu_reg_index == 7) {
        asm volatile ("lsi f7, %0, 0" :: "a" (value_ptr));
    } else if (fpu_reg_index == 8) {
        asm volatile ("lsi f8, %0, 0" :: "a" (value_ptr));
    } else if (fpu_reg_index == 9) {
        asm volatile ("lsi f9, %0, 0" :: "a" (value_ptr));
    } else if (fpu_reg_index == 10) {
        asm volatile ("lsi f10, %0, 0" :: "a" (value_ptr));
    } else if (fpu_reg_index == 11) {
        asm volatile ("lsi f11, %0, 0" :: "a" (value_ptr));
    } else if (fpu_reg_index == 12) {
        asm volatile ("lsi f12, %0, 0" :: "a" (value_ptr));
    } else if (fpu_reg_index == 13) {
        asm volatile ("lsi f13, %0, 0" :: "a" (value_ptr));
    } else if (fpu_reg_index == 14) {
        asm volatile ("lsi f14, %0, 0" :: "a" (value_ptr));
    } else if (fpu_reg_index == 15) {
        asm volatile ("lsi f15, %0, 0" :: "a" (value_ptr));
    } else if (fpu_reg_index == 16) {
        asm volatile ("wur.FCR %0" :: "a" (*value_ptr));
    } else if (fpu_reg_index == 17) {
        asm volatile ("wur.FSR %0" :: "a" (*value_ptr));
    }
}

static void gdbstub_write_fpu_regs(esp_gdbstub_frame_t *frame, uint32_t reg_index, uint32_t *value_ptr)
{
#if CONFIG_IDF_TARGET_ESP32
    const uint32_t fpu_start_register = 87;
#elif CONFIG_IDF_TARGET_ESP32S3
    const uint32_t fpu_start_register = 84;
#else
#error "Unknown Xtensa chip"
#endif
    const StaticTask_t *tcb;
    uint32_t *fpu_save_area;

    uint32_t fpu_reg_index = reg_index - fpu_start_register;
    if (fpu_reg_index >= (16 + 2)) {
        return;
    }

    tcb = esp_gdbstub_find_tcb_by_frame(frame);
    fpu_save_area = esp_gdbstub_coproc_saved_area((void *)tcb, XCHAL_CP_ID_FPU);

    if (fpu_save_area == NULL) {
        uint32_t cp_enabled = enable_coproc(XCHAL_CP_ID_FPU);

        gdbstub_set_fpu_register(fpu_reg_index, (float *)value_ptr);

        WSR(XT_REG_CPENABLE, cp_enabled);
    } else {
        fpu_save_area[fpu_reg_index] = *value_ptr;
    }
}
#endif // XCHAL_HAVE_FP

/** @brief GDB set register in frame
 * Set register in frame with address to value
 *
 * */
void esp_gdbstub_set_register(esp_gdbstub_frame_t *frame, uint32_t reg_index, uint32_t *value_ptr)
{
    uint32_t value = *value_ptr;

    if (reg_index == 0) {
        frame->pc = value;
    } else if (reg_index > 0 && (reg_index <= 27)) {
        (&frame->a0)[reg_index - 1] = value;
    }
#if XCHAL_HAVE_FP
    gdbstub_write_fpu_regs(frame, reg_index, value_ptr);
#endif // XCHAL_HAVE_FP
}
