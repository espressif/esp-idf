/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_gdbstub.h"
#include "esp_gdbstub_common.h"
#include "esp_cpu.h"
#include "esp_ipc_isr.h"
#include "rv_decode.h"
#include "sdkconfig.h"
#include "esp_private/crosscore_int.h"
#include "esp_private/freertos_debug.h"

extern volatile esp_gdbstub_frame_t *temp_regs_frame;

static inline void init_regfile(esp_gdbstub_gdb_regfile_t *dst)
{
    memset(dst, 0, sizeof(*dst));
}

void esp_gdbstub_frame_to_regfile(const esp_gdbstub_frame_t *frame, esp_gdbstub_gdb_regfile_t *dst)
{
    init_regfile(dst);
    dst->pc = frame->mepc;

    // We omit register x0 here since it's the zero register and always hard-wired to 0.
    // See The RISC-V Instruction Set Manual Volume I: Unprivileged ISA Document Version 20191213 for more details.
    memcpy(&(dst->x[1]), &frame->ra, sizeof(uint32_t) * 31);
}

#if CONFIG_ESP_GDBSTUB_SUPPORT_TASKS || CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME

/* Represents FreeRTOS TCB structure */
typedef struct {
    uint8_t *top_of_stack;
    /* Other members aren't needed */
} dummy_tcb_t;

#if CONFIG_ESP_GDBSTUB_SUPPORT_TASKS

void esp_gdbstub_tcb_to_regfile(TaskHandle_t tcb, esp_gdbstub_gdb_regfile_t *dst)
{
    const dummy_tcb_t *dummy_tcb = (const dummy_tcb_t *) tcb;

    const RvExcFrame *frame = (RvExcFrame *) dummy_tcb->top_of_stack;
    esp_gdbstub_frame_to_regfile(frame, dst);
}

#endif // CONFIG_ESP_GDBSTUB_SUPPORT_TASKS
#endif // CONFIG_ESP_GDBSTUB_SUPPORT_TASKS || CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME

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
    dummy_tcb_t *tcb = (dummy_tcb_t *)pvTaskGetCurrentTCBForCore(esp_cpu_get_core_id());
    gdbstub_handle_uart_int((esp_gdbstub_frame_t *)tcb->top_of_stack);
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

void esp_gdbstub_set_register(esp_gdbstub_frame_t *frame, uint32_t reg_index, uint32_t value)
{
    /* RISC-V base ISA has registers x0-x31  */
    if (reg_index == 0) { /* skip zero-wired register  */
        return;
    } else if (reg_index < 32) {
        (&frame->mepc)[reg_index] = value;
    } else if (reg_index == 32) { /* register 32 is PC  */
        frame->mepc = value;
    }
}
