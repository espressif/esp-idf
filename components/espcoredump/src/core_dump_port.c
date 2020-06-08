// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/xtensa_context.h" // for exception register stack structure
#include "esp_core_dump_priv.h"

const static DRAM_ATTR char TAG[] __attribute__((unused)) = "esp_core_dump_port";

#define COREDUMP_EM_XTENSA                  0x5E
#define COREDUMP_INVALID_CAUSE_VALUE        0xFFFF
#define COREDUMP_EXTRA_REG_NUM              16
#define COREDUMP_FAKE_STACK_START           0x20000000
#define COREDUMP_FAKE_STACK_LIMIT           0x30000000

#define COREDUMP_GET_REG_PAIR(reg_idx, reg_ptr) {   *(uint32_t*)(reg_ptr++) = (uint32_t)reg_idx; \
                                                    RSR(reg_idx, *(uint32_t*)(reg_ptr++)); \
                                                }

#define COREDUMP_GET_EPC(reg, ptr) \
    if (reg - EPC_1 + 1 <= XCHAL_NUM_INTLEVELS) COREDUMP_GET_REG_PAIR(reg, ptr)

#define COREDUMP_GET_EPS(reg, ptr) \
    if (reg - EPS_2 + 2 <= XCHAL_NUM_INTLEVELS) COREDUMP_GET_REG_PAIR(reg, ptr)

// Enumeration of registers of exception stack frame
// and solicited stack frame
typedef enum
{
    // XT_SOL_EXIT = 0,
    XT_SOL_PC = 1,
    XT_SOL_PS = 2,
    // XT_SOL_NEXT = 3,
    XT_SOL_AR_START = 4,
    XT_SOL_AR_NUM = 4,
    // XT_SOL_FRMSZ = 8,
    XT_STK_EXIT = 0,
    XT_STK_PC = 1,
    XT_STK_PS = 2,
    XT_STK_AR_START = 3,
    XT_STK_AR_NUM = 16,
    XT_STK_SAR = 19,
    XT_STK_EXCCAUSE = 20,
    XT_STK_EXCVADDR = 21,
    XT_STK_LBEG = 22,
    XT_STK_LEND = 23,
    XT_STK_LCOUNT = 24,
    //XT_STK_FRMSZ = 25,
} stk_frame_t;

// Xtensa ELF core file register set representation ('.reg' section).
// Copied from target-side ELF header <xtensa/elf.h>.
typedef struct
{
    uint32_t pc;
    uint32_t ps;
    uint32_t lbeg;
    uint32_t lend;
    uint32_t lcount;
    uint32_t sar;
    uint32_t windowstart;
    uint32_t windowbase;
    uint32_t reserved[8+48];
    uint32_t ar[XCHAL_NUM_AREGS];
} __attribute__((packed)) xtensa_gregset_t;

typedef struct
{
    uint32_t reg_index;
    uint32_t reg_val;
} __attribute__((packed)) core_dump_reg_pair_t;

typedef struct
{
    uint32_t crashed_task_tcb;
    core_dump_reg_pair_t exccause;
    core_dump_reg_pair_t excvaddr;
    core_dump_reg_pair_t extra_regs[COREDUMP_EXTRA_REG_NUM];
} __attribute__((packed)) xtensa_extra_info_t;

// Xtensa Program Status for GDB
typedef struct
{
    uint32_t si_signo;
    uint32_t si_code;
    uint32_t si_errno;
    uint16_t pr_cursig;
    uint16_t pr_pad0;
    uint32_t pr_sigpend;
    uint32_t pr_sighold;
    uint32_t pr_pid;
    uint32_t pr_ppid;
    uint32_t pr_pgrp;
    uint32_t pr_sid;
    uint64_t pr_utime;
    uint64_t pr_stime;
    uint64_t pr_cutime;
    uint64_t pr_cstime;
} __attribute__((packed)) xtensa_pr_status_t;

typedef struct
{
    xtensa_pr_status_t pr_status;
    xtensa_gregset_t regs;
    // Todo: acc to xtensa_gregset_t number of regs must be 128,
    // but gdb complains when it less than 129
    uint32_t reserved;
} __attribute__((packed)) xtensa_elf_reg_dump_t;

extern uint8_t port_IntStack;

#if CONFIG_ESP32_ENABLE_COREDUMP

static uint32_t s_total_length = 0;

static XtExcFrame s_fake_stack_frame = {
    .pc   = (UBaseType_t) COREDUMP_FAKE_STACK_START,                        // task entrypoint fake_ptr
    .a0   = (UBaseType_t) 0,                                                // to terminate GDB backtrace
    .a1   = (UBaseType_t) (COREDUMP_FAKE_STACK_START + sizeof(XtExcFrame)), // physical top of stack frame
    .exit = (UBaseType_t) 0,                                                // user exception exit dispatcher
    .ps = (PS_UM | PS_EXCM),
    .exccause = (UBaseType_t) COREDUMP_INVALID_CAUSE_VALUE,
};
static uint32_t s_fake_stacks_num;

static xtensa_extra_info_t s_extra_info;

#if ESP32_CORE_DUMP_STACK_SIZE > 0
uint8_t s_coredump_stack[ESP32_CORE_DUMP_STACK_SIZE];
uint8_t *s_core_dump_sp;

static uint32_t esp_core_dump_free_stack_space(const uint8_t *pucStackByte)
{
    uint32_t ulCount = 0U;
    while( *pucStackByte == (uint8_t)COREDUMP_STACK_FILL_BYTE ) {
        pucStackByte -= portSTACK_GROWTH;
        ulCount++;
    }
    ulCount /= (uint32_t)sizeof(uint8_t);
    return (uint32_t)ulCount;
}
#endif

void esp_core_dump_report_stack_usage(void)
{
#if ESP32_CORE_DUMP_STACK_SIZE > 0
    uint32_t bytes_free = esp_core_dump_free_stack_space(s_coredump_stack);
    ESP_COREDUMP_LOGD("Core dump used %u bytes on stack. %u bytes left free.",
        s_core_dump_sp - s_coredump_stack - bytes_free, bytes_free);
#endif
}

#if CONFIG_ESP32_COREDUMP_CHECKSUM_SHA256

// function to calculate SHA256 for solid data array
int esp_core_dump_sha(mbedtls_sha256_context *ctx,
                        const unsigned char *input,
                        size_t ilen,
                        unsigned char output[32] )
{
    assert(input);
    mbedtls_sha256_init(ctx);
    if((mbedtls_sha256_starts_ret(ctx, 0) != 0)) goto exit;
#if CONFIG_MBEDTLS_HARDWARE_SHA
    // set software mode for SHA calculation
    ctx->mode = ESP_MBEDTLS_SHA256_SOFTWARE;
#endif
    if((mbedtls_sha256_update_ret(ctx, input, ilen) != 0)) goto exit;
    if((mbedtls_sha256_finish_ret(ctx, output) != 0)) goto exit;
    esp_core_dump_print_sha256(DRAM_STR("Coredump SHA256"), (void*)output);
    s_total_length = ilen;
exit:
    mbedtls_sha256_free(ctx);
    return ilen;
}

void esp_core_dump_print_sha256(const char* msg, const uint8_t* sha_output)
{
    ets_printf(DRAM_STR("%s='"), msg);
    for (int i = 0; i < COREDUMP_SHA256_LEN; i++) {
        ets_printf(DRAM_STR("%02x"), sha_output[i]);
    }
    ets_printf(DRAM_STR("'\r\n"));
}
#endif

void esp_core_dump_checksum_init(core_dump_write_data_t* wr_data)
{
    if (wr_data) {
#if CONFIG_ESP32_COREDUMP_CHECKSUM_CRC32
        wr_data->crc = 0;
#elif CONFIG_ESP32_COREDUMP_CHECKSUM_SHA256
        mbedtls_sha256_init(&wr_data->ctx);
        (void)mbedtls_sha256_starts_ret(&wr_data->ctx, 0);
#endif
        s_total_length = 0;
    }
}

void esp_core_dump_checksum_update(core_dump_write_data_t* wr_data, void* data, size_t data_len)
{
    if (wr_data && data) {
#if CONFIG_ESP32_COREDUMP_CHECKSUM_CRC32
        wr_data->crc = crc32_le(wr_data->crc, data, data_len);
#elif CONFIG_ESP32_COREDUMP_CHECKSUM_SHA256
#if CONFIG_MBEDTLS_HARDWARE_SHA
        // set software mode of SHA calculation
        wr_data->ctx.mode = ESP_MBEDTLS_SHA256_SOFTWARE;
#endif
        (void)mbedtls_sha256_update_ret(&wr_data->ctx, data, data_len);
#endif
        s_total_length += data_len; // keep counter of cashed bytes
    } else {
        ESP_COREDUMP_LOGE("Wrong write data info!");
    }
}

uint32_t esp_core_dump_checksum_finish(core_dump_write_data_t* wr_data, void** chs_ptr)
{
    // get core dump checksum
    uint32_t chs_len = 0;
#if CONFIG_ESP32_COREDUMP_CHECKSUM_CRC32
    if (chs_ptr) {
        wr_data->crc = wr_data->crc;
        *chs_ptr = (void*)&wr_data->crc;
        ESP_COREDUMP_LOG_PROCESS("Dump data CRC = 0x%x, offset = 0x%x", wr_data->crc, wr_data->off);
    }
    chs_len = sizeof(wr_data->crc);
#elif CONFIG_ESP32_COREDUMP_CHECKSUM_SHA256
    if (chs_ptr) {
        ESP_COREDUMP_LOG_PROCESS("Dump data offset = %d", wr_data->off);
        (void)mbedtls_sha256_finish_ret(&wr_data->ctx, (uint8_t*)&wr_data->sha_output);
        *chs_ptr = (void*)&wr_data->sha_output[0];
        mbedtls_sha256_free(&wr_data->ctx);
    }
    chs_len = sizeof(wr_data->sha_output);
#endif
    ESP_COREDUMP_LOG_PROCESS("Total length of hashed data: %d!", s_total_length);
    return chs_len;
}

inline uint16_t esp_core_dump_get_arch_id()
{
    return COREDUMP_EM_XTENSA;
}

inline bool esp_core_dump_mem_seg_is_sane(uint32_t addr, uint32_t sz)
{
    //TODO: currently core dump supports memory segments in DRAM only, external SRAM not supported yet
    return esp_ptr_in_dram((void *)addr) && esp_ptr_in_dram((void *)(addr+sz-1));
}

inline bool esp_core_dump_task_stack_end_is_sane(uint32_t sp)
{
    //TODO: currently core dump supports stacks in DRAM only, external SRAM not supported yet
    return esp_ptr_in_dram((void *)sp);
}

inline bool esp_core_dump_tcb_addr_is_sane(uint32_t addr)
{
    return esp_core_dump_mem_seg_is_sane(addr, COREDUMP_TCB_SIZE);
}

uint32_t esp_core_dump_get_tasks_snapshot(core_dump_task_header_t** const tasks,
                        const uint32_t snapshot_size)
{
    static TaskSnapshot_t s_tasks_snapshots[CONFIG_ESP32_CORE_DUMP_MAX_TASKS_NUM];
    uint32_t tcb_sz; // unused

    /* implying that TaskSnapshot_t extends core_dump_task_header_t by adding extra fields */
    _Static_assert(sizeof(TaskSnapshot_t) >= sizeof(core_dump_task_header_t), "FreeRTOS task snapshot binary compatibility issue!");

    uint32_t task_num = (uint32_t)uxTaskGetSnapshotAll(s_tasks_snapshots,
                                                         (UBaseType_t)snapshot_size,
                                                         (UBaseType_t*)&tcb_sz);
    for (uint32_t i = 0; i < task_num; i++) {
        tasks[i] = (core_dump_task_header_t *)&s_tasks_snapshots[i];
    }
    return task_num;
}

inline uint32_t esp_core_dump_get_isr_stack_end(void)
{
    return (uint32_t)((uint8_t *)&port_IntStack + (xPortGetCoreID()+1)*configISR_STACK_SIZE);
}

uint32_t esp_core_dump_get_stack(core_dump_task_header_t *task_snapshot,
                                uint32_t *stk_vaddr, uint32_t *stk_len)
{
    if (task_snapshot->stack_end > task_snapshot->stack_start) {
        *stk_len = task_snapshot->stack_end - task_snapshot->stack_start;
        *stk_vaddr = task_snapshot->stack_start;
    } else {
        *stk_len = task_snapshot->stack_start - task_snapshot->stack_end;
        *stk_vaddr = task_snapshot->stack_end;
    }
    if (*stk_vaddr >= COREDUMP_FAKE_STACK_START && *stk_vaddr < COREDUMP_FAKE_STACK_LIMIT) {
        return (uint32_t)&s_fake_stack_frame;
    }
    return *stk_vaddr;
}

// The function creates small fake stack for task as deep as exception frame size
// It is required for gdb to take task into account but avoid back trace of stack.
// The espcoredump.py script is able to recognize that task is broken
static void *esp_core_dump_get_fake_stack(uint32_t *stk_len)
{
    *stk_len = sizeof(s_fake_stack_frame);
    return (uint8_t*)COREDUMP_FAKE_STACK_START + sizeof(s_fake_stack_frame)*s_fake_stacks_num++;
}

static core_dump_reg_pair_t *esp_core_dump_get_epc_regs(core_dump_reg_pair_t* src)
{
    uint32_t* reg_ptr = (uint32_t*)src;
    // get InterruptException program counter registers
    COREDUMP_GET_EPC(EPC_1, reg_ptr);
    COREDUMP_GET_EPC(EPC_2, reg_ptr);
    COREDUMP_GET_EPC(EPC_3, reg_ptr);
    COREDUMP_GET_EPC(EPC_4, reg_ptr);
    COREDUMP_GET_EPC(EPC_5, reg_ptr);
    COREDUMP_GET_EPC(EPC_6, reg_ptr);
    COREDUMP_GET_EPC(EPC_7, reg_ptr);
    return (core_dump_reg_pair_t*)reg_ptr;
}

static core_dump_reg_pair_t *esp_core_dump_get_eps_regs(core_dump_reg_pair_t* src)
{
    uint32_t* reg_ptr = (uint32_t*)src;
    // get InterruptException processor state registers
    COREDUMP_GET_EPS(EPS_2, reg_ptr);
    COREDUMP_GET_EPS(EPS_3, reg_ptr);
    COREDUMP_GET_EPS(EPS_4, reg_ptr);
    COREDUMP_GET_EPS(EPS_5, reg_ptr);
    COREDUMP_GET_EPS(EPS_6, reg_ptr);
    COREDUMP_GET_EPS(EPS_7, reg_ptr);
    return (core_dump_reg_pair_t*)reg_ptr;
}

// Returns list of registers (in GDB format) from xtensa stack frame
static esp_err_t esp_core_dump_get_regs_from_stack(void* stack_addr,
                                               size_t size,
                                               xtensa_gregset_t* regs)
{
    XtExcFrame* exc_frame = (XtExcFrame*)stack_addr;
    uint32_t* stack_arr = (uint32_t*)stack_addr;

    if (size < sizeof(XtExcFrame)) {
        ESP_COREDUMP_LOGE("Too small stack to keep frame: %d bytes!", size);
        return ESP_FAIL;
    }

    // Stack frame type indicator is always the first item
    uint32_t rc = exc_frame->exit;

    // is this current crashed task?
    if (rc == COREDUMP_CURR_TASK_MARKER)
    {
        s_extra_info.exccause.reg_val = exc_frame->exccause;
        s_extra_info.exccause.reg_index = EXCCAUSE;
        s_extra_info.excvaddr.reg_val = exc_frame->excvaddr;
        s_extra_info.excvaddr.reg_index = EXCVADDR;
        // get InterruptException registers into extra_info
        core_dump_reg_pair_t *regs_ptr = esp_core_dump_get_eps_regs(s_extra_info.extra_regs);
        esp_core_dump_get_epc_regs(regs_ptr);
    } else {
        // initialize EXCCAUSE and EXCVADDR members of frames for all the tasks,
        // except for the crashed one
        exc_frame->exccause = COREDUMP_INVALID_CAUSE_VALUE;
        exc_frame->excvaddr = 0;
    }

    if (rc != 0) {
        regs->pc = exc_frame->pc;
        regs->ps = exc_frame->ps;
        for (int i = 0; i < XT_STK_AR_NUM; i++) {
            regs->ar[i] = stack_arr[XT_STK_AR_START + i];
        }
        regs->sar = exc_frame->sar;
#if CONFIG_IDF_TARGET_ESP32
        regs->lbeg = exc_frame->lbeg;
        regs->lend = exc_frame->lend;
        regs->lcount = exc_frame->lcount;
#endif
        // FIXME: crashed and some running tasks (e.g. prvIdleTask) have EXCM bit set
        // and GDB can not unwind callstack properly (it implies not windowed call0)
        if (regs->ps & PS_UM) {
            regs->ps &= ~PS_EXCM;
        }
    } else {
        regs->pc = stack_arr[XT_SOL_PC];
        regs->ps = stack_arr[XT_SOL_PS];
        for (int i = 0; i < XT_SOL_AR_NUM; i++) {
            regs->ar[i] = stack_arr[XT_SOL_AR_START + i];
        }
        regs->pc = (regs->pc & 0x3fffffff);
        if (regs->pc & 0x80000000) {
            regs->pc = (regs->pc & 0x3fffffff);
        }
        if (regs->ar[0] & 0x80000000) {
            regs->ar[0] = (regs->ar[0] & 0x3fffffff);
        }
    }
    return ESP_OK;
}

uint32_t esp_core_dump_get_task_regs_dump(core_dump_task_header_t *task, void **reg_dump)
{
    uint32_t stack_vaddr, stack_paddr, stack_len;
    static xtensa_elf_reg_dump_t s_reg_dump = { 0 };

    stack_paddr = esp_core_dump_get_stack(task, &stack_vaddr, &stack_len);

    ESP_COREDUMP_LOG_PROCESS("Add regs for task 0x%x", task->tcb_addr);

    // initialize program status for the task
    s_reg_dump.pr_status.pr_cursig = 0;
    s_reg_dump.pr_status.pr_pid = (uint32_t)task->tcb_addr;

    // fill the gdb registers structure from stack
    esp_err_t err = esp_core_dump_get_regs_from_stack((void*)stack_paddr,
                                                        stack_len,
                                                        &s_reg_dump.regs);
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Error while registers processing.");
    }
    *reg_dump = &s_reg_dump;
    return sizeof(s_reg_dump);
}

inline void* esp_core_dump_get_current_task_handle()
{
    return (void*)xTaskGetCurrentTaskHandleForCPU(xPortGetCoreID());
}

bool esp_core_dump_check_task(panic_info_t *info,
                                core_dump_task_header_t *task,
                                bool* is_current,
                                bool* stack_is_valid)
{
    XtExcFrame *exc_frame = (XtExcFrame *)info->frame;
    bool is_curr_task = false;
    bool stack_is_sane = false;
    uint32_t stk_size = 0;

    if (!esp_core_dump_tcb_addr_is_sane((uint32_t)task->tcb_addr)) {
        ESP_COREDUMP_LOG_PROCESS("Bad TCB addr=%x!", task->tcb_addr);
        return false;
    }

    is_curr_task = task->tcb_addr == esp_core_dump_get_current_task_handle();
    if (is_curr_task) {
        // Set correct stack top for current task; only modify if we came from the task,
        // and not an ISR that crashed.
        if (!xPortInterruptedFromISRContext()) {
            task->stack_start = (uint32_t)exc_frame;
        }
        exc_frame->exit = COREDUMP_CURR_TASK_MARKER;
        if (info->pseudo_excause) {
            exc_frame->exccause += XCHAL_EXCCAUSE_NUM;
        }
        s_extra_info.crashed_task_tcb = (uint32_t)task->tcb_addr;
    }

    stack_is_sane = esp_core_dump_check_stack(task->stack_start, task->stack_end);
    if (!stack_is_sane) {
        // Skip saving of invalid task if stack corrupted
        ESP_COREDUMP_LOG_PROCESS("Task (TCB:%x), stack is corrupted (%x, %x)",
                                    task->tcb_addr,
                                    task->stack_start,
                                    task->stack_end);
        task->stack_start = (uint32_t)esp_core_dump_get_fake_stack(&stk_size);
        task->stack_end = (uint32_t)(task->stack_start + stk_size);
        ESP_COREDUMP_LOG_PROCESS("Task (TCB:%x), use start, end (%x, %x)",
                                            task->tcb_addr,
                                            task->stack_start,
                                            task->stack_end);
    }

    if (is_curr_task) {
        if (!stack_is_sane)
            ESP_COREDUMP_LOG_PROCESS("Current task 0x%x is broken!", task->tcb_addr);
        ESP_COREDUMP_LOG_PROCESS("Current task (TCB:%x), EXIT/PC/PS/A0/SP %x %x %x %x %x",
                                            task->tcb_addr,
                                            exc_frame->exit,
                                            exc_frame->pc,
                                            exc_frame->ps,
                                            exc_frame->a0,
                                            exc_frame->a1);
    } else {
        XtSolFrame *task_frame = (XtSolFrame *)task->stack_start;
        if (stack_is_sane) {
            if (task_frame->exit == 0) {
                    ESP_COREDUMP_LOG_PROCESS("Task (TCB:%x), EXIT/PC/PS/A0/SP %x %x %x %x %x",
                                                task->tcb_addr,
                                                task_frame->exit,
                                                task_frame->pc,
                                                task_frame->ps,
                                                task_frame->a0,
                                                task_frame->a1);
            } else {
#if CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH
                    XtExcFrame *task_frame2 = (XtExcFrame *)task->stack_start;
                    task_frame2->exccause = COREDUMP_INVALID_CAUSE_VALUE;
                    ESP_COREDUMP_LOG_PROCESS("Task (TCB:%x) EXIT/PC/PS/A0/SP %x %x %x %x %x",
                                                task->tcb_addr,
                                                task_frame2->exit,
                                                task_frame2->pc,
                                                task_frame2->ps,
                                                task_frame2->a0,
                                                task_frame2->a1);
#endif
            }
        } else {
            ESP_COREDUMP_LOG_PROCESS("Task (TCB:%x), stack_start=%x is incorrect, skip registers printing.",
                                        task->tcb_addr, task->stack_start);

        }
    }

    if (is_current) {
        *is_current = is_curr_task;
    }
    if (stack_is_valid) {
        *stack_is_valid = stack_is_sane;
    }

    return true;
}

bool esp_core_dump_check_stack(uint32_t stack_start, uint32_t stack_end)
{
    uint32_t len = stack_end - stack_start;
    bool task_is_valid = false;
    // Check task's stack
    if (!esp_stack_ptr_is_sane(stack_start) || !esp_core_dump_task_stack_end_is_sane(stack_end) ||
        (len > COREDUMP_MAX_TASK_STACK_SIZE)) {
        // Check if current task stack is corrupted
        task_is_valid = false;
    } else {
        ESP_COREDUMP_LOG_PROCESS("Stack len = %lu (%x %x)", len, stack_start, stack_end);
        task_is_valid = true;
    }
    return task_is_valid;
}

void esp_core_dump_init_extra_info()
{
    s_extra_info.crashed_task_tcb = COREDUMP_CURR_TASK_MARKER;
    // Initialize exccause register to default value (required if current task corrupted)
    s_extra_info.exccause.reg_val = COREDUMP_INVALID_CAUSE_VALUE;
    s_extra_info.exccause.reg_index = EXCCAUSE;
}

uint32_t esp_core_dump_get_extra_info(void **info)
{
    *info = &s_extra_info;
    return sizeof(s_extra_info);
}

#endif
