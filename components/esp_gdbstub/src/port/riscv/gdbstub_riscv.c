/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_gdbstub.h"
#include "esp_gdbstub_common.h"
#include "sdkconfig.h"

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

#ifdef CONFIG_ESP_GDBSTUB_SUPPORT_TASKS

/* Represents FreeRTOS TCB structure */
typedef struct {
    uint8_t *top_of_stack;
    /* Other members aren't needed */
} dummy_tcb_t;


void esp_gdbstub_tcb_to_regfile(TaskHandle_t tcb, esp_gdbstub_gdb_regfile_t *dst)
{
    const dummy_tcb_t *dummy_tcb = (const dummy_tcb_t *) tcb;

    const RvExcFrame *frame = (RvExcFrame *) dummy_tcb->top_of_stack;
    esp_gdbstub_frame_to_regfile(frame, dst);
}

#endif // CONFIG_ESP_GDBSTUB_SUPPORT_TASKS

int esp_gdbstub_get_signal(const esp_gdbstub_frame_t *frame)
{
    return 5; // SIGTRAP, see IDF-2490
}

void _xt_gdbstub_int(void *frame)
{
}

void esp_gdbstub_init_dports()
{
}

void esp_gdbstub_stall_other_cpus_start()
{
}

void esp_gdbstub_stall_other_cpus_end()
{
}

void esp_gdbstub_clear_step()
{
}

void esp_gdbstub_do_step()
{
}

void esp_gdbstub_trigger_cpu(void)
{
}

void esp_gdbstub_set_register(esp_gdbstub_frame_t *frame, uint32_t reg_index, uint32_t value)
{
}
