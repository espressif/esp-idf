/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Core dump port implementation for RISC-V based boards.
 */

#include <string.h>
#include <stdbool.h>
#include "soc/soc_memory_layout.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "riscv/rvruntime-frames.h"
#include "esp_rom_sys.h"
#include "esp_core_dump_common.h"
#include "esp_core_dump_port.h"

/* TAG used for logs */
const static char TAG[] __attribute__((unused)) = "esp_core_dump_port";

/* Code associated to RISC-V in ELF format */
#define COREDUMP_EM_RISCV                   0xF3

#define COREDUMP_INVALID_CAUSE_VALUE        0xFFFF
#define COREDUMP_FAKE_STACK_START           0x20000000U
#define COREDUMP_FAKE_STACK_LIMIT           0x30000000U

#if CONFIG_ESP_COREDUMP_ENABLE

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) < (b) ? (b) : (a))

/**
 * Union representing the registers of the CPU as they will be written
 * in the core dump.
 * Registers can be addressed with their names thanks to the structure, or as
 * an array of 32 words.
 */
#define RISCV_GP_REGS_COUNT 32

typedef union {
    struct {
        uint32_t pc;
        uint32_t ra;
        uint32_t sp;
        uint32_t gp;
        uint32_t tp;
        uint32_t t0;
        uint32_t t1;
        uint32_t t2;
        uint32_t s0;
        uint32_t s1;
        uint32_t a0;
        uint32_t a1;
        uint32_t a2;
        uint32_t a3;
        uint32_t a4;
        uint32_t a5;
        uint32_t a6;
        uint32_t a7;
        uint32_t s2;
        uint32_t s3;
        uint32_t s4;
        uint32_t s5;
        uint32_t s6;
        uint32_t s7;
        uint32_t s8;
        uint32_t s9;
        uint32_t s10;
        uint32_t s11;
        uint32_t t3;
        uint32_t t4;
        uint32_t t5;
        uint32_t t6;
    };

    uint32_t as_array[RISCV_GP_REGS_COUNT];
} riscv_regs;

/**
 * The following structure represents the NOTE section in the coredump.
 * Its type must be PR_STATUS as it contains the registers value and the
 * program status.
 * As our coredump will be used with GDB, we only need to fill the info
 * it needs. We are going to use the macros taken from GDB's elf32-riscv.c
 * Here is the structure GDB needs for PRSTATUS note:
 *
 * +---------------------------+--------------+---------------------------------+
 * |          Offset           | Size (bytes) |              Data               |
 * +---------------------------+--------------+---------------------------------+
 * | PRSTATUS_OFFSET_PR_CURSIG |            2 | Signal that triggered the panic |
 * | PRSTATUS_OFFSET_PR_PID    |            4 | PID                             |
 * | PRSTATUS_OFFSET_PR_REG    |           32 | Registers value                 |
 * +---------------------------+--------------+---------------------------------+
 *
 * Other fields are not strictly required by GDB, we can then replace them
 * by a padding. Among these fields, we can find PPID, SID or system time.
 */
#define PRSTATUS_SIZE               204
#define PRSTATUS_OFFSET_PR_CURSIG   12
#define PRSTATUS_OFFSET_PR_PID      24
#define PRSTATUS_OFFSET_PR_REG      72
#define ELF_GREGSET_T_SIZE          128

/* We can determine the padding thanks to the previous macros */
#define PRSTATUS_SIG_PADDING        (PRSTATUS_OFFSET_PR_CURSIG)
#define PRSTATUS_PID_PADDING        (PRSTATUS_OFFSET_PR_PID - PRSTATUS_OFFSET_PR_CURSIG - sizeof(uint16_t))
#define PRSTATUS_REG_PADDING        (PRSTATUS_OFFSET_PR_REG - PRSTATUS_OFFSET_PR_PID - sizeof(uint32_t))
#define PRSTATUS_END_PADDING        (PRSTATUS_SIZE - PRSTATUS_OFFSET_PR_REG - ELF_GREGSET_T_SIZE)

typedef struct {
    uint8_t   _PADDING1[PRSTATUS_SIG_PADDING];
    uint16_t   signal;
    uint8_t   _PADDING2[PRSTATUS_PID_PADDING];
    uint32_t   pid;
    uint8_t   _PADDING3[PRSTATUS_REG_PADDING];
    riscv_regs regs;
    uint8_t   _PADDING4[PRSTATUS_END_PADDING];
} riscv_prstatus;

/**
 * Assert that our structure is designed the way we are expecting it to be.
 */
_Static_assert(offsetof(riscv_prstatus, signal) == PRSTATUS_OFFSET_PR_CURSIG,
               "Wrong offset for signal field in riscv_prstatus structure");
_Static_assert(offsetof(riscv_prstatus, pid) == PRSTATUS_OFFSET_PR_PID,
               "Wrong offset for pid field in riscv_prstatus structure");
_Static_assert(offsetof(riscv_prstatus, regs) == PRSTATUS_OFFSET_PR_REG,
               "Wrong offset for regs field in riscv_prstatus structure");
_Static_assert(sizeof(riscv_regs) == ELF_GREGSET_T_SIZE,
               "Wrong size for riscv_regs union");
_Static_assert(sizeof(riscv_prstatus) == PRSTATUS_SIZE,
               "Wrong size for riscv_prstatus structure");

/**
 * Structure used to add some extra info inside core file.
 */
typedef struct {
    uint32_t crashed_task_tcb;
    uint32_t isr_context;
} riscv_extra_info_t;

/* Allocate the fake stack that will be used by broken tasks. */
static RvExcFrame s_fake_stack_frame = {
    .mepc = COREDUMP_FAKE_STACK_START,
    .sp = COREDUMP_FAKE_STACK_START + sizeof(RvExcFrame),
};

/* Keep a track of the number of fake stack distributed. Avoid giving the
 * same fake address to two different tasks. */
static uint32_t s_fake_stacks_num = 0;

/* Statically initialize the extra information structure. */
static riscv_extra_info_t s_extra_info = { 0 };

inline void esp_core_dump_port_init(panic_info_t *info, bool isr_context)
{
    s_extra_info.crashed_task_tcb = COREDUMP_CURR_TASK_MARKER;
    s_extra_info.isr_context = isr_context;
}

/**
 * Return the current architecture (RISC-V) ID. This will be written in the
 * ELF file header.
 */
inline uint16_t esp_core_dump_get_arch_id()
{
    return COREDUMP_EM_RISCV;
}

/**
 * Reset fake tasks' stack counter. This lets us reuse the previously allocated
 * fake stacks.
 */
void esp_core_dump_reset_fake_stacks(void)
{
    s_fake_stacks_num = 0;
}

/**
 * Function generating a fake stack address for task as deep as exception
 * frame size. It is required for GDB to take task into account but avoid
 * backtrace of stack. The espcoredump.py script is able to recognize
 * that task is broken.
 */
static uint32_t esp_core_dump_generate_fake_stack(uint32_t *stk_addr)
{
    /* Offset of this new fake task stask. */
    const uint32_t offset = sizeof(s_fake_stack_frame) * s_fake_stacks_num++;

    /* Return the size and the fake address */
    *stk_addr = COREDUMP_FAKE_STACK_START + offset;

    return sizeof(s_fake_stack_frame);
}

/**
 * Return the top of the ISR stack.
 */
uint8_t* esp_core_dump_get_isr_stack_top(void)
{
    extern uint8_t* xIsrStackTop;
    return xIsrStackTop;
}

/**
 * Return the end of the ISR stack .
 */
uint32_t esp_core_dump_get_isr_stack_end(void)
{
    uint8_t* isr_top_stack = esp_core_dump_get_isr_stack_top();
    return (uint32_t)(isr_top_stack + (xPortGetCoreID() + 1) * configISR_STACK_SIZE);
}

/**
 * Check if the given stack is sane or not.
 */
static inline bool esp_core_dump_task_stack_end_is_sane(uint32_t sp)
{
    return esp_ptr_in_dram((void *)sp)
#if CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM
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
            ((task->stack_end - task->stack_start) > COREDUMP_MAX_TASK_STACK_SIZE)) {
        // Check if current task stack is corrupted
        ESP_COREDUMP_LOG_PROCESS("Invalid stack (%x...%x)!", task->stack_start, task->stack_end);
        return false;
    }
    return true;
}

/**
 * Get the stack addresses (virtual and physical) and stack length of the task
 * passed as argument.
 * Returns the stack length.
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
 */
bool esp_core_dump_check_task(core_dump_task_header_t *task)
{
    bool is_stack_sane = false;

    if (!esp_core_dump_tcb_addr_is_sane((uint32_t)task->tcb_addr)) {
        ESP_COREDUMP_LOG_PROCESS("Bad TCB addr=%x!", task->tcb_addr);
        return false;
    }

    /* Check the sanity of the task's stack. If the stack is corrupted, replace
     * it with a fake stack containing fake registers value. This is required
     * by GDB. */
    is_stack_sane = esp_core_dump_check_stack(task);

    if (!is_stack_sane) {
        const uint32_t stk_size = esp_core_dump_generate_fake_stack(&task->stack_start);
        task->stack_end = (uint32_t)(task->stack_start + stk_size);
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
    return (esp_ptr_in_dram((void *)addr) && esp_ptr_in_dram((void *)(addr + sz - 1)))
           || (esp_ptr_in_rtc_slow((void *)addr) && esp_ptr_in_rtc_slow((void *)(addr + sz - 1)))
           || (esp_ptr_in_rtc_dram_fast((void *)addr) && esp_ptr_in_rtc_dram_fast((void *)(addr + sz - 1)))
           || (esp_ptr_external_ram((void *)addr) && esp_ptr_external_ram((void *)(addr + sz - 1)))
           || (esp_ptr_in_iram((void *)addr) && esp_ptr_in_iram((void *)(addr + sz - 1)));
}

/**
 * Get the task's registers dump when the panic occurred.
 * Returns the size, in bytes, of the data pointed by reg_dumps.
 * The data pointed by reg_dump are allocated statically, thus, they must be
 * used (or copied) before calling this function again.
 */
uint32_t esp_core_dump_get_task_regs_dump(core_dump_task_header_t *task, void **reg_dump)
{
    static riscv_prstatus prstatus = { 0 };
    RvExcFrame* exc_frame = NULL;
    uint32_t stack_vaddr = 0;
    uint32_t stack_paddr = 0;
    uint32_t stack_len = 0;

    ESP_COREDUMP_LOG_PROCESS("Add regs for task 0x%x", task->tcb_addr);

    stack_len = esp_core_dump_get_stack(task, &stack_vaddr, &stack_paddr);

    if (stack_len < sizeof(RvExcFrame)) {
        ESP_COREDUMP_LOGE("Too small stack to keep frame: %d bytes!", stack_len);
    }

    /* The RISC-V frame has been saved at the top of the stack for the
     * pre-empted tasks. We can then retrieve the registers by performing
     * a cast on the stack address. For the current crashed task, the stack
     * address has been adjusted by esp_core_dump_check_task function. */
    exc_frame = (RvExcFrame*) stack_paddr;

    /* Fill the PR_STATUS structure and copy the registers from the stack frame to it. */
    prstatus.signal = 0;
    prstatus.pid = (uint32_t)task->tcb_addr;
    memcpy(prstatus.regs.as_array, exc_frame, sizeof(riscv_regs));

    *reg_dump = &prstatus;
    return sizeof(riscv_prstatus);
}

/**
 * Save the crashed task handle in the extra info structure.
 */
void esp_core_dump_port_set_crashed_tcb(uint32_t handle)
{
    s_extra_info.crashed_task_tcb = handle;
}

/**
 * Function returning the extra info to be written to the dedicated section in
 * the core file. *info must not be NULL, it will be assigned to the extra info
 * data.
 * The size, in bytes, of the data pointed by info is returned.
 */
uint32_t esp_core_dump_get_extra_info(void **info)
{
    uint32_t size = 0;

    if (info != NULL) {
        size = sizeof(s_extra_info);
        *info = &s_extra_info;
    }

    return size;
}

#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF

void esp_core_dump_summary_parse_extra_info(esp_core_dump_summary_t *summary, void *ei_data)
{
    riscv_extra_info_t *ei = (riscv_extra_info_t *)ei_data;
    summary->exc_tcb = ei->crashed_task_tcb;
    ESP_COREDUMP_LOGD("Crash TCB 0x%x", summary->exc_tcb);
}

void esp_core_dump_summary_parse_exc_regs(esp_core_dump_summary_t *summary, void *stack_data)
{
    int i;
    long *a_reg;
    RvExcFrame *stack = (RvExcFrame *)stack_data;
    summary->exc_pc = stack->mepc;
    ESP_COREDUMP_LOGD("Crashing PC 0x%x", summary->exc_pc);

    summary->ex_info.mstatus = stack->mstatus;
    summary->ex_info.mtvec = stack->mtvec;
    summary->ex_info.mcause = stack->mcause;
    summary->ex_info.mtval = stack->mtval;
    summary->ex_info.ra = stack->ra;
    summary->ex_info.sp = stack->sp;
    ESP_COREDUMP_LOGD("mstatus:0x%x mtvec:0x%x mcause:0x%x mval:0x%x RA: 0x%x SP: 0x%x",
                      stack->mstatus, stack->mtvec, stack->mcause, stack->mtval, stack->ra, stack->sp);
    a_reg = &stack->a0;
    for (i = 0; i < 8; i++) {
        summary->ex_info.exc_a[i] = a_reg[i];
        ESP_COREDUMP_LOGD("A[%d] 0x%x", i, summary->ex_info.exc_a[i]);
    }
}

void esp_core_dump_summary_parse_backtrace_info(esp_core_dump_bt_info_t *bt_info, const void *vaddr,
                                                const void *paddr, uint32_t stack_size)
{
    if (!vaddr || !paddr || !bt_info) {
        bt_info->dump_size = 0;
        return;
    }

    /* Check whether the stack is a fake stack created during coredump generation
     * If its a fake stack, we don't have any actual stack dump
     */
    if (vaddr >= (void *) COREDUMP_FAKE_STACK_START && vaddr < (void *) COREDUMP_FAKE_STACK_LIMIT) {
        bt_info->dump_size = 0;
        return;
    }

    /* Top of the stack consists of the context registers saved after crash,
     * extract the value of stack pointer (SP) at the time of crash
     */
    RvExcFrame *stack = (RvExcFrame *) paddr;
    uint32_t *sp = (uint32_t *)stack->sp;

    /* vaddr is actual stack address when crash occurred. However that stack is now saved
     * in the flash at a different location. Hence, we need to adjust the offset
     * to point to correct data in the flash */
    int offset = (uint32_t)stack - (uint32_t)vaddr;

    // Skip the context saved register frame
    uint32_t regframe_size = (uint32_t)sp - (uint32_t)vaddr;

    uint32_t dump_size = MIN(stack_size - regframe_size, CONFIG_ESP_COREDUMP_SUMMARY_STACKDUMP_SIZE);

    memcpy(&bt_info->stackdump[0], (uint8_t *)sp + offset, dump_size);
    bt_info->dump_size = dump_size;
}

#endif /* #if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF */

#endif
