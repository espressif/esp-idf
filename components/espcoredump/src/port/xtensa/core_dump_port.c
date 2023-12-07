/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Core dump port implementation for Xtensa based boards.
 */

#include <string.h>
#include <stdbool.h>
#include "soc/soc_memory_layout.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "xtensa_context.h"
#include "esp_rom_sys.h"
#include "esp_core_dump_common.h"
#include "esp_core_dump_port.h"
#include "esp_debug_helpers.h"
#include "esp_cpu_utils.h"

const static DRAM_ATTR char TAG[] __attribute__((unused)) = "esp_core_dump_port";

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) < (b) ? (b) : (a))

#define COREDUMP_EM_XTENSA                  0x5E
#define COREDUMP_INVALID_CAUSE_VALUE        0xFFFF
#define COREDUMP_FAKE_STACK_START           0x20000000
#define COREDUMP_FAKE_STACK_LIMIT           0x30000000
#define COREDUMP_EXTRA_REG_NUM              16

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
    uint32_t isr_context;
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

#if CONFIG_ESP_COREDUMP_ENABLE

static XtExcFrame s_fake_stack_frame = {
    .pc   = (UBaseType_t) COREDUMP_FAKE_STACK_START,                        // task entrypoint fake_ptr
    .a0   = (UBaseType_t) 0,                                                // to terminate GDB backtrace
    .a1   = (UBaseType_t) (COREDUMP_FAKE_STACK_START + sizeof(XtExcFrame)), // physical top of stack frame
    .exit = (UBaseType_t) 0,                                                // user exception exit dispatcher
    .ps = (PS_UM | PS_EXCM),
    .exccause = (UBaseType_t) COREDUMP_INVALID_CAUSE_VALUE,
};

/* Keep a track of the number of fake stack distributed. Avoid giving the
 * same fake address to two different tasks. */
static uint32_t s_fake_stacks_num = 0;

static xtensa_extra_info_t s_extra_info;

/**
 * The function creates small fake stack for task as deep as exception frame size
 * It is required for gdb to take task into account but avoid back trace of stack.
 * The espcoredump.py script is able to recognize that task is broken
 */
static void *esp_core_dump_get_fake_stack(uint32_t *stk_len)
{
    *stk_len = sizeof(s_fake_stack_frame);
    return (uint8_t*)COREDUMP_FAKE_STACK_START + sizeof(s_fake_stack_frame)*s_fake_stacks_num++;
}


static core_dump_reg_pair_t *esp_core_dump_get_epc_regs(core_dump_reg_pair_t* src)
{
#pragma GCC diagnostic push
#if     __GNUC__ >= 9
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#endif
    uint32_t* reg_ptr = (uint32_t*)src;
#pragma GCC diagnostic pop
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
#pragma GCC diagnostic push
#if     __GNUC__ >= 9
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#endif
    uint32_t* reg_ptr = (uint32_t*)src;
#pragma GCC diagnostic pop
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
#if XCHAL_HAVE_LOOPS
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

inline void esp_core_dump_port_init(panic_info_t *info, bool isr_context)
{
    s_extra_info.crashed_task_tcb = COREDUMP_CURR_TASK_MARKER;
    // Initialize exccause register to default value (required if current task corrupted)
    s_extra_info.exccause.reg_val = COREDUMP_INVALID_CAUSE_VALUE;
    s_extra_info.exccause.reg_index = EXCCAUSE;

    XtExcFrame *s_exc_frame = (XtExcFrame *) info->frame;
    s_exc_frame->exit = COREDUMP_CURR_TASK_MARKER;
    if (info->pseudo_excause) {
        s_exc_frame->exccause += XCHAL_EXCCAUSE_NUM;
    }
    s_extra_info.isr_context = isr_context;
}

/**
 * Get the architecture ID.
 * Check core dump port interface for more information about this function.
 */
inline uint16_t esp_core_dump_get_arch_id()
{
    return COREDUMP_EM_XTENSA;
}

void esp_core_dump_reset_fake_stacks(void)
{
    s_fake_stacks_num = 0;
}

/* Get the top of the ISR stack.
 * Check core dump port interface for more information about this function.
 */
uint8_t* esp_core_dump_get_isr_stack_top(void)
{
    extern uint8_t port_IntStack[];
    return port_IntStack;
}

uint32_t esp_core_dump_get_isr_stack_end(void)
{
    uint8_t* isr_top_stack = esp_core_dump_get_isr_stack_top();
    return (uint32_t)(isr_top_stack + (xPortGetCoreID()+1)*configISR_STACK_SIZE);
}


static inline bool esp_core_dump_task_stack_end_is_sane(uint32_t sp)
{
    return esp_ptr_in_dram((void *)sp)
#if CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
        || esp_stack_ptr_in_extram(sp)
#endif
#if CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
        || esp_ptr_in_rtc_dram_fast((void*) sp)
#endif
    ;
}


bool esp_core_dump_check_stack(core_dump_task_header_t *task)
{
    // Check task's stack
    if (!esp_stack_ptr_is_sane(task->stack_start) ||
        !esp_core_dump_task_stack_end_is_sane(task->stack_end) ||
        (task->stack_start >= task->stack_end) ||
        ((task->stack_end-task->stack_start) > COREDUMP_MAX_TASK_STACK_SIZE)) {
        // Check if current task stack is corrupted
        ESP_COREDUMP_LOG_PROCESS("Invalid stack (%x...%x)!", task->stack_start, task->stack_end);
        return false;
    }
    return true;
}

/**
 * Check if the memory segment is sane
 *
 * Check the header file for more information.
 */
bool esp_core_dump_mem_seg_is_sane(uint32_t addr, uint32_t sz)
{
    return (esp_ptr_in_dram((void *)addr) && esp_ptr_in_dram((void *)(addr+sz-1)))
        || (esp_ptr_in_rtc_slow((void *)addr) && esp_ptr_in_rtc_slow((void *)(addr+sz-1)))
        || (esp_ptr_in_rtc_dram_fast((void *)addr) && esp_ptr_in_rtc_dram_fast((void *)(addr+sz-1)))
        || (esp_ptr_external_ram((void *)addr) && esp_ptr_external_ram((void *)(addr+sz-1)))
        || (esp_ptr_in_iram((void *)addr) && esp_ptr_in_iram((void *)(addr+sz-1)));
}

/**
 * Get the stack of a task.
 * Check core dump port interface for more information about this function.
 */
uint32_t esp_core_dump_get_stack(core_dump_task_header_t *task,
                                 uint32_t* stk_vaddr, uint32_t* stk_paddr)
{
    const uint32_t stack_addr = min(task->stack_start, task->stack_end);
    const uint32_t stack_addr2 = max(task->stack_start, task->stack_end);
    const uint32_t stack_len = stack_addr2 - stack_addr;

    ESP_COREDUMP_DEBUG_ASSERT(stk_paddr != NULL && stk_vaddr != NULL);

    /* Provide the virtual stack address for any task. */
    *stk_vaddr = stack_addr;

    if (stack_addr >= COREDUMP_FAKE_STACK_START &&
        stack_addr < COREDUMP_FAKE_STACK_LIMIT) {
        /* In this case, the stack address pointed by the task is a fake stack
         * generated previously. So it doesn't really point to actual data.
         * Thus, we must provide the address of the fake stack data. */
        *stk_paddr = (uint32_t) &s_fake_stack_frame;
    } else {
        *stk_paddr = stack_addr;
    }

    return stack_len;
}

/**
 * Check the task passed as a parameter.
 * Check core dump port interface for more information about this function.
 */
bool esp_core_dump_check_task(core_dump_task_header_t *task)
{
    uint32_t stk_size = 0;
    bool stack_is_valid = false;

    if (!esp_core_dump_tcb_addr_is_sane((uint32_t)task->tcb_addr)) {
        ESP_COREDUMP_LOG_PROCESS("Bad TCB addr=%x!", task->tcb_addr);
        return false;
    }

    stack_is_valid = esp_core_dump_check_stack(task);
    if (!stack_is_valid) {
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
    } else {
        /* This shall be done only if the stack was correct, else, stack_start
         * would point to a fake address. */
        XtSolFrame *sol_frame = (XtSolFrame *)task->stack_start;
        if (sol_frame->exit == 0) {
            ESP_COREDUMP_LOG_PROCESS("Task (TCB:%x), EXIT/PC/PS/A0/SP %x %x %x %x %x",
                                        task->tcb_addr,
                                        sol_frame->exit,
                                        sol_frame->pc,
                                        sol_frame->ps,
                                        sol_frame->a0,
                                        sol_frame->a1);
        } else {
    // to avoid warning that 'exc_frame' is unused when ESP_COREDUMP_LOG_PROCESS does nothing
    #if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_LOGS
            XtExcFrame *exc_frame = (XtExcFrame *)task->stack_start;
            ESP_COREDUMP_LOG_PROCESS("Task (TCB:%x) EXIT/PC/PS/A0/SP %x %x %x %x %x",
                                        task->tcb_addr,
                                        exc_frame->exit,
                                        exc_frame->pc,
                                        exc_frame->ps,
                                        exc_frame->a0,
                                        exc_frame->a1);
    #endif
        }
    }
    return true;
}


/**
 * Get a dump of the task's registers.
 * Check core dump port interface for more information about this function.
 */
uint32_t esp_core_dump_get_task_regs_dump(core_dump_task_header_t *task, void **reg_dump)
{
    uint32_t stack_vaddr = 0;
    uint32_t stack_paddr = 0;
    uint32_t stack_len = 0;
    static xtensa_elf_reg_dump_t s_reg_dump = { 0 };

    ESP_COREDUMP_DEBUG_ASSERT(task != NULL && reg_dump != NULL);

    stack_len = esp_core_dump_get_stack(task, &stack_vaddr, &stack_paddr);

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


void esp_core_dump_port_set_crashed_tcb(uint32_t handle) {
    s_extra_info.crashed_task_tcb = handle;
}

/**
 * Retrieve the extra information.
 * Check core dump port interface for more information about this function.
 */
uint32_t esp_core_dump_get_extra_info(void **info)
{
    if (info) {
        *info = &s_extra_info;
    }
    return sizeof(s_extra_info);
}

#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF

void esp_core_dump_summary_parse_extra_info(esp_core_dump_summary_t *summary, void *ei_data)
{
    int i;
    xtensa_extra_info_t *ei = (xtensa_extra_info_t *) ei_data;
    summary->exc_tcb = ei->crashed_task_tcb;
    summary->ex_info.exc_vaddr = ei->excvaddr.reg_val;
    summary->ex_info.exc_cause = ei->exccause.reg_val;
    ESP_COREDUMP_LOGD("Crash TCB 0x%x", summary->exc_tcb);
    ESP_COREDUMP_LOGD("excvaddr 0x%x", summary->ex_info.exc_vaddr);
    ESP_COREDUMP_LOGD("exccause 0x%x", summary->ex_info.exc_cause);

    memset(summary->ex_info.epcx, 0, sizeof(summary->ex_info.epcx));
    summary->ex_info.epcx_reg_bits = 0;
    for (i = 0; i < COREDUMP_EXTRA_REG_NUM; i++ ) {
        if (ei->extra_regs[i].reg_index >= EPC_1
            && ei->extra_regs[i].reg_index < (EPC_1 + XCHAL_NUM_INTLEVELS)) {
            summary->ex_info.epcx[ei->extra_regs[i].reg_index - EPC_1] = ei->extra_regs[i].reg_val;
            summary->ex_info.epcx_reg_bits |= (1 << (ei->extra_regs[i].reg_index - EPC_1));
        }
    }
}

void esp_core_dump_summary_parse_exc_regs(esp_core_dump_summary_t *summary, void *stack_data)
{
    int i;
    long *a_reg;
    XtExcFrame *stack = (XtExcFrame *) stack_data;
    summary->exc_pc = esp_cpu_process_stack_pc(stack->pc);
    ESP_COREDUMP_LOGD("Crashing PC 0x%x", summary->exc_pc);

    a_reg = &stack->a0;
    for (i = 0; i < 16; i++) {
        summary->ex_info.exc_a[i] = a_reg[i];
        ESP_COREDUMP_LOGD("A[%d] 0x%x", i, summary->ex_info.exc_a[i]);
    }
}

void esp_core_dump_summary_parse_backtrace_info(esp_core_dump_bt_info_t *bt_info, const void *vaddr,
                                                const void *paddr, uint32_t stack_size)
{
    if (!vaddr || !paddr || !bt_info) {
        return;
    }

    int offset;
    bool corrupted;
    esp_backtrace_frame_t frame;
    XtExcFrame *stack = (XtExcFrame *) paddr;
    int max_depth = (int) (sizeof(bt_info->bt) / sizeof(bt_info->bt[0]));
    int index = 0;

    frame.pc = stack->pc;
    frame.sp = stack->a1;
    frame.next_pc = stack->a0;

    corrupted = !(esp_stack_ptr_is_sane(frame.sp) &&
                (esp_ptr_executable((void *)esp_cpu_process_stack_pc(frame.pc)) ||
                stack->exccause == EXCCAUSE_INSTR_PROHIBITED)); /* Ignore the first corrupted PC in case of InstrFetchProhibited */

    /* vaddr is actual stack address when crash occurred. However that stack is now saved
     * in the flash at a different location. Hence for each SP, we need to adjust the offset
     * to point to next frame in the flash */
    offset = (uint32_t) stack - (uint32_t) vaddr;

    ESP_COREDUMP_LOGD("Crash Backtrace");
    bt_info->bt[index] = esp_cpu_process_stack_pc(frame.pc);
    ESP_COREDUMP_LOGD(" 0x%x", bt_info->bt[index]);
    index++;

    while (max_depth-- > 0 && frame.next_pc && !corrupted) {
        /* Check if the Stack Pointer is in valid address range */
        if (!((uint32_t)frame.sp >= (uint32_t)vaddr &&
            ((uint32_t)frame.sp <= (uint32_t)vaddr + stack_size))) {
            corrupted = true;
            break;
        }
        /* Adjusting the SP to address in flash than in actual RAM */
        frame.sp += offset;
        if (!esp_backtrace_get_next_frame(&frame)) {
            corrupted = true;
        }
        if (corrupted == false) {
            bt_info->bt[index] = esp_cpu_process_stack_pc(frame.pc);
            ESP_COREDUMP_LOGD(" 0x%x", bt_info->bt[index]);
            index++;
        }
    }
    bt_info->depth = index;
    bt_info->corrupted = corrupted;
}

#endif /* #if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF */

#endif
