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
#include "esp_panic.h"
#include "esp_core_dump_priv.h"

const static DRAM_ATTR char TAG[] __attribute__((unused)) = "esp_core_dump_port";

#if CONFIG_ESP32_ENABLE_COREDUMP

#define COREDUMP_INVALID_CAUSE_VALUE        0xFFFF
#define COREDUMP_FAKE_STACK_START           0x20000000
#define COREDUMP_FAKE_STACK_LIMIT           0x30000000

extern uint8_t port_IntStack;

static XtExcFrame s_fake_stack_frame = {
    .pc   = (UBaseType_t) COREDUMP_FAKE_STACK_START,                        // task entrypoint fake_ptr
    .a0   = (UBaseType_t) 0,                                                // to terminate GDB backtrace
    .a1   = (UBaseType_t) (COREDUMP_FAKE_STACK_START + sizeof(XtExcFrame)), // physical top of stack frame
    .exit = (UBaseType_t) 0,                                                // user exception exit dispatcher
    .ps = (PS_UM | PS_EXCM),
    .exccause = (UBaseType_t) COREDUMP_INVALID_CAUSE_VALUE,
};

uint32_t s_fake_stacks_num;

_Static_assert(sizeof(TaskSnapshot_t) == sizeof(core_dump_task_header_t), "FreeRTOS task snapshot binary compatibility issue!");

// The function creates small fake stack for task as deep as exception frame size
// It is required for gdb to take task into account but avoid back trace of stack.
// The espcoredump.py script is able to recognize that task is broken
static void *esp_core_dump_get_fake_stack(uint32_t *stk_len)
{
    *stk_len = sizeof(s_fake_stack_frame);
    return (uint8_t*)COREDUMP_FAKE_STACK_START + sizeof(s_fake_stack_frame)*s_fake_stacks_num++;
}

static inline bool esp_task_stack_start_is_sane(uint32_t sp)
{
    return esp_ptr_in_dram((const void*)sp) || esp_ptr_external_ram((const void*)sp);
}

static inline bool esp_tcb_addr_is_sane(uint32_t addr)
{
    return esp_core_dump_mem_seg_is_sane(addr, COREDUMP_TCB_SIZE);
}

inline bool esp_core_dump_mem_seg_is_sane(uint32_t addr, uint32_t sz)
{
    return (esp_ptr_in_dram((const void*)addr) && esp_ptr_in_dram((const void*)(addr + sz))) ||
        (esp_ptr_external_ram((const void*)addr) && esp_ptr_external_ram((const void*)(addr + sz)));
}

inline uint32_t esp_core_dump_get_isr_stack_end(void)
{
    return (uint32_t)((uint8_t *)&port_IntStack + (xPortGetCoreID()+1)*configISR_STACK_SIZE);
}

uint32_t esp_core_dump_get_tasks_snapshot(core_dump_task_header_t* const tasks,
                        const uint32_t snapshot_size)
{
    UBaseType_t tcb_sz; //unused
    uint32_t task_num = (uint32_t)uxTaskGetSnapshotAll((TaskSnapshot_t*)tasks, (UBaseType_t)snapshot_size, &tcb_sz);
    return task_num;
}

bool esp_core_dump_check_in_bt_isr(int sp)
{
    extern uint32_t _l4_intr_stack, _l4_intr_stack_end;
    return (sp > &_l4_intr_stack && sp < &_l4_intr_stack_end) ? true : false;
}

bool esp_core_dump_process_tcb(void *frame, core_dump_task_header_t *task_snaphort, bool *is_curr_task)
{
    XtExcFrame *exc_frame = (XtExcFrame*)frame;

    *is_curr_task = false;
    if (!esp_tcb_addr_is_sane((uint32_t)task_snaphort->tcb_addr)) {
        ESP_COREDUMP_LOG_PROCESS("Bad TCB addr %x!", task_snaphort->tcb_addr);
        task_snaphort->tcb_addr = 0;
        return false;
    }
    if (task_snaphort->tcb_addr == xTaskGetCurrentTaskHandleForCPU(xPortGetCoreID())) {
        *is_curr_task = true;
        // Set correct stack top for current task; only modify if we came from the task,
        // and not an ISR that crashed.
        if (!xPortInterruptedFromISRContext() && !esp_core_dump_check_in_bt_isr(exc_frame->a1)) {
            task_snaphort->stack_start = (uint32_t)exc_frame;
        }
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

    len = (uint32_t)task_snaphort->stack_end - (uint32_t)task_snaphort->stack_start;
    // Check task's stack
    if (!esp_stack_ptr_is_sane(task_snaphort->stack_start) ||
        !esp_task_stack_start_is_sane((uint32_t)task_snaphort->stack_end) ||
        (task_snaphort->stack_end < task_snaphort->stack_start) ||
        (len > COREDUMP_MAX_TASK_STACK_SIZE)) {
        // Check if current task stack corrupted
        if (task_snaphort->tcb_addr == xTaskGetCurrentTaskHandleForCPU(xPortGetCoreID())) {
            ESP_COREDUMP_LOG_PROCESS("Crashed task stack is corrupted!");
        }
        ESP_COREDUMP_LOG_PROCESS("Corrupted TCB %x: stack len %lu, top %x, end %x!",
            task_snaphort->tcb_addr, len, task_snaphort->stack_start, task_snaphort->stack_end);
        task_snaphort->stack_start = (uint32_t)esp_core_dump_get_fake_stack(&len);
        task_snaphort->stack_end = (uint32_t)(task_snaphort->stack_start + len);
    }
    ESP_COREDUMP_LOG_PROCESS("Stack len = %lu (%x %x)", len,
            task_snaphort->stack_start, task_snaphort->stack_end);
    *length = len;
    return true;
}

uint32_t esp_core_dump_get_stack(core_dump_task_header_t *task_snapshot, uint32_t *stk_len)
{
    *stk_len = task_snapshot->stack_end - task_snapshot->stack_start;
    if (task_snapshot->stack_start >= COREDUMP_FAKE_STACK_START && task_snapshot->stack_start < COREDUMP_FAKE_STACK_LIMIT) {
        return (uint32_t)&s_fake_stack_frame;
    }
    return task_snapshot->stack_start;
}

static uint32_t (*_get_len)(void) = NULL;
static int * (*_get_ptr)(void) = NULL;

bool esp_log_dump_init(uint32_t (*get_len)(void), uint32_t * (*get_ptr)(void))
{
    _get_len = get_len;
    _get_ptr = get_ptr;
    return true;
}

bool esp_core_dump_process_log(core_dump_log_header_t *log)
{
    if (_get_len && _get_ptr) {
        log->len = _get_len();
        log->start = _get_ptr();
        return true;
    } else {
        log->len = 0;
        log->start = NULL;
        return false;
    }
}

#endif
