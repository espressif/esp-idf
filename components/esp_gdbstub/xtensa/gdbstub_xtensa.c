// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include "esp_gdbstub_common.h"
#include "soc/cpu.h"
#include "soc/soc_memory_layout.h"
#include "xtensa/config/specreg.h"
#include "sdkconfig.h"

#if !XCHAL_HAVE_WINDOWED
#warning "gdbstub_xtensa: revisit the implementation for Call0 ABI"
#endif

extern int _invalid_pc_placeholder;

static void init_regfile(esp_gdbstub_gdb_regfile_t *dst)
{
    memset(dst, 0, sizeof(*dst));
}

static void update_regfile_common(esp_gdbstub_gdb_regfile_t *dst)
{
    if (dst->a[0] & 0x8000000U) {
        dst->a[0] = (uint32_t)cpu_ll_pc_to_ptr(dst->a[0]);
    }
    if (!esp_stack_ptr_is_sane(dst->a[1])) {
        dst->a[1] = 0xDEADBEEF;
    }
    dst->windowbase = 0;
    dst->windowstart = 0x1;
    RSR(CONFIGID0, dst->configid0);
    RSR(CONFIGID1, dst->configid1);
}

void esp_gdbstub_frame_to_regfile(const esp_gdbstub_frame_t *frame, esp_gdbstub_gdb_regfile_t *dst)
{
    init_regfile(dst);
    const uint32_t *a_regs = (const uint32_t *) &frame->a0;
    if (!(esp_ptr_executable(cpu_ll_pc_to_ptr(frame->pc)) && (frame->pc & 0xC0000000U))) {
        /* Xtensa ABI sets the 2 MSBs of the PC according to the windowed call size
         * Incase the PC is invalid, GDB will fail to translate addresses to function names
         * Hence replacing the PC to a placeholder address in case of invalid PC
         */
        dst->pc = (uint32_t)&_invalid_pc_placeholder;
    } else {
        dst->pc = (uint32_t)cpu_ll_pc_to_ptr(frame->pc);
    }

    for (int i = 0; i < 16; i++) {
        dst->a[i] = a_regs[i];
    }
    for (int i = 16; i < 64; i++) {
        dst->a[i] = 0xDEADBEEF;
    }

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

static void solicited_frame_to_regfile(const XtSolFrame *frame, esp_gdbstub_gdb_regfile_t *dst)
{
    init_regfile(dst);
    const uint32_t *a_regs = (const uint32_t *) &frame->a0;
    if (!(esp_ptr_executable(cpu_ll_pc_to_ptr(frame->pc)) && (frame->pc & 0xC0000000U))) {
        dst->pc = (uint32_t)&_invalid_pc_placeholder;
    } else {
        dst->pc = (uint32_t)cpu_ll_pc_to_ptr(frame->pc);
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

/* Represents FreeRTOS TCB structure */
typedef struct {
    uint8_t *top_of_stack;
    /* Other members aren't needed */
} dummy_tcb_t;


void esp_gdbstub_tcb_to_regfile(TaskHandle_t tcb, esp_gdbstub_gdb_regfile_t *dst)
{
    const dummy_tcb_t *dummy_tcb = (const dummy_tcb_t *) tcb;

    const XtExcFrame *frame = (XtExcFrame *) dummy_tcb->top_of_stack;
    if (frame->exit != 0) {
        esp_gdbstub_frame_to_regfile(frame, dst);
    } else {
        const XtSolFrame *taskFrame = (const XtSolFrame *) dummy_tcb->top_of_stack;
        solicited_frame_to_regfile(taskFrame, dst);
    }
}

#endif // CONFIG_ESP_GDBSTUB_SUPPORT_TASKS

int esp_gdbstub_get_signal(const esp_gdbstub_frame_t *frame)
{
    const char exccause_to_signal[] = {4, 31, 11, 11, 2, 6, 8, 0, 6, 7, 0, 0, 7, 7, 7, 7};
    if (frame->exccause > sizeof(exccause_to_signal)) {
        return 11;
    }
    return (int) exccause_to_signal[frame->exccause];
}
