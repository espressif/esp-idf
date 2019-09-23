// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <string.h>
#include <stdbool.h>
#include "soc/soc_memory_layout.h"
#include "esp_core_dump_priv.h"

const static DRAM_ATTR char TAG[] __attribute__((unused)) = "esp_core_dump_port";

#if CONFIG_ESP32_ENABLE_COREDUMP

inline bool esp_task_stack_start_is_sane(uint32_t sp)
{
    return !(sp < 0x3ffae010UL || sp > 0x3fffffffUL);
}

inline bool esp_tcb_addr_is_sane(uint32_t addr, uint32_t sz)
{
    //TODO: currently core dump supports TCBs in DRAM only, external SRAM not supported yet
    return !(addr < 0x3ffae000UL || (addr + sz) > 0x40000000UL);
}

uint32_t esp_core_dump_get_tasks_snapshot(core_dump_task_header_t* const tasks,
                        const uint32_t snapshot_size, uint32_t* const tcb_sz)
{
    uint32_t task_num = (uint32_t)uxTaskGetSnapshotAll((TaskSnapshot_t*)tasks, (UBaseType_t)snapshot_size, (UBaseType_t*)tcb_sz);
    return task_num;
}

bool esp_core_dump_process_tcb(void *frame, core_dump_task_header_t *task_snaphort, uint32_t tcb_sz)
{
    XtExcFrame *exc_frame = (XtExcFrame*)frame;

    if (!esp_tcb_addr_is_sane((uint32_t)task_snaphort->tcb_addr, tcb_sz)) {
        ESP_COREDUMP_LOG_PROCESS("Bad TCB addr %x!", task_snaphort->tcb_addr);
        return false;
    }
    if (task_snaphort->tcb_addr == xTaskGetCurrentTaskHandleForCPU(xPortGetCoreID())) {
        // Set correct stack top for current task
        task_snaphort->stack_start = (uint32_t)exc_frame;
        // This field is not initialized for crashed task, but stack frame has the structure of interrupt one,
        // so make workaround to allow espcoredump to parse it properly.
        if (exc_frame->exit == 0)
            exc_frame->exit = -1;
        ESP_COREDUMP_LOG_PROCESS("Current task %x EXIT/PC/PS/A0/SP %x %x %x %x %x",
                task_snaphort->tcb_addr, exc_frame->exit, exc_frame->pc, exc_frame->ps, exc_frame->a0, exc_frame->a1);
    }
    else {
        XtSolFrame *task_frame = (XtSolFrame *)task_snaphort->stack_start;
        if (task_frame->exit == 0) {
                ESP_COREDUMP_LOG_PROCESS("Task %x EXIT/PC/PS/A0/SP %x %x %x %x %x",
                        task_snaphort->tcb_addr, task_frame->exit, task_frame->pc, task_frame->ps, task_frame->a0, task_frame->a1);
        }
        else {
#if CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH
                XtExcFrame *task_frame2 = (XtExcFrame *)task_snaphort->stack_start;
                ESP_COREDUMP_LOG_PROCESS("Task %x EXIT/PC/PS/A0/SP %x %x %x %x %x",
                        task_snaphort->tcb_addr, task_frame2->exit, task_frame2->pc, task_frame2->ps, task_frame2->a0, task_frame2->a1);
#endif
        }
    }
    return true;
}

bool esp_core_dump_process_stack(core_dump_task_header_t* task_snaphort, uint32_t *length)
{
    uint32_t len = 0;
    bool task_is_valid = false;
    len = (uint32_t)task_snaphort->stack_end - (uint32_t)task_snaphort->stack_start;
    // Check task's stack
    if (!esp_stack_ptr_is_sane(task_snaphort->stack_start) ||
        !esp_task_stack_start_is_sane((uint32_t)task_snaphort->stack_end) ||
        (len > COREDUMP_MAX_TASK_STACK_SIZE)) {
        // Check if current task stack corrupted
        if (task_snaphort->tcb_addr == xTaskGetCurrentTaskHandleForCPU(xPortGetCoreID())) {
            ESP_COREDUMP_LOG_PROCESS("Crashed task will be skipped!");
        }
        ESP_COREDUMP_LOG_PROCESS("Corrupted TCB %x: stack len %lu, top %x, end %x!",
            task_snaphort->tcb_addr, len, task_snaphort->stack_start, task_snaphort->stack_end);
        task_snaphort->tcb_addr = 0; // make TCB addr invalid to skip it in dump
        task_is_valid = false;
    } else {
        ESP_COREDUMP_LOG_PROCESS("Stack len = %lu (%x %x)", len,
                task_snaphort->stack_start, task_snaphort->stack_end);
        // Take stack padding into account
        if (length) {
            *length = (len + sizeof(uint32_t) - 1) & ~(sizeof(uint32_t) - 1);
        }
        task_is_valid = true;
    }
    return task_is_valid;
}

#endif
