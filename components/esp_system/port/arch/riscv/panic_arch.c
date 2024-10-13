/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#include "soc/soc_caps.h"
#include "esp_private/panic_internal.h"
#include "esp_private/panic_reason.h"
#include "riscv/rvruntime-frames.h"
#include "riscv/rv_utils.h"
#include "esp_private/cache_err_int.h"
#include "soc/timer_periph.h"

#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
#include "esp_private/esp_memprot_internal.h"
#include "esp_memprot.h"
#endif

#if CONFIG_ESP_SYSTEM_USE_EH_FRAME
#include "esp_private/eh_frame_parser.h"
#include "esp_private/cache_utils.h"
#endif

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_cpu.h"
#include "esp_private/hw_stack_guard.h"
#endif

#define DIM(array) (sizeof(array)/sizeof(*array))

/**
 * Function called when a cache error occurs. It prints details such as the
 * explanation of why the panic occurred.
 */
static inline void print_cache_err_details(const void *frame)
{
    const char* cache_err_msg = esp_cache_err_panic_string();
    if (cache_err_msg) {
        panic_print_str(cache_err_msg);
    } else {
        panic_print_str("Cache error active, but failed to find a corresponding error message");
    }
    panic_print_str("\r\n");
}

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
static inline void print_assist_debug_details(const void *frame)
{
    uint32_t core_id = esp_hw_stack_guard_get_fired_cpu();
    if (core_id == ESP_HW_STACK_GUARD_NOT_FIRED) {
        panic_print_str("ASSIST_DEBUG is not triggered BUT interrupt occurred!\r\n\r\n");
        core_id = 0;
    }
    uint32_t sp_min, sp_max;
    const char *task_name = pcTaskGetName(xTaskGetCurrentTaskHandleForCore(core_id));
    esp_hw_stack_guard_get_bounds(core_id, &sp_min, &sp_max);

    panic_print_str("\r\n");

    panic_print_str("Detected in task \"");
    panic_print_str(task_name);
    panic_print_str("\" at 0x");
    panic_print_hex((int) esp_hw_stack_guard_get_pc(core_id));
    panic_print_str("\r\n");
    panic_print_str("Stack pointer: 0x");
    panic_print_hex((int)((RvExcFrame *)frame)->sp);
    panic_print_str("\r\n");
    panic_print_str("Stack bounds: 0x");
    panic_print_hex((int) sp_min);
    panic_print_str(" - 0x");
    panic_print_hex((int) sp_max);
    panic_print_str("\r\n\r\n");
}
#endif // CONFIG_ESP_SYSTEM_HW_STACK_GUARD

/**
 * Function called when a memory protection error occurs (PMS). It prints details such as the
 * explanation of why the panic occurred.
 */
#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE

static esp_memp_intr_source_t s_memp_intr = {MEMPROT_TYPE_INVALID, -1};

#define PRINT_MEMPROT_ERROR(err) \
        do { \
            panic_print_str("N/A (error "); \
            panic_print_str(esp_err_to_name(err)); \
            panic_print_str(")"); \
        } while(0)

static inline void print_memprot_err_details(const void *frame __attribute__((unused)))
{
    if (s_memp_intr.mem_type == MEMPROT_TYPE_INVALID && s_memp_intr.core == -1) {
        panic_print_str("  - no details available -\r\n");
        return;
    }

    //common memprot fault info
    panic_print_str("  memory type: ");
    panic_print_str(esp_mprot_mem_type_to_str(s_memp_intr.mem_type));

    panic_print_str("\r\n  faulting address: ");
    void *faulting_addr;
    esp_err_t res = esp_mprot_get_violate_addr(s_memp_intr.mem_type, &faulting_addr, s_memp_intr.core);
    if (res == ESP_OK) {
        panic_print_str("0x");
        panic_print_hex((int)faulting_addr);
    } else {
        PRINT_MEMPROT_ERROR(res);
    }

    panic_print_str("\r\n  world: ");
    esp_mprot_pms_world_t world;
    res = esp_mprot_get_violate_world(s_memp_intr.mem_type, &world, s_memp_intr.core);
    if (res == ESP_OK) {
        panic_print_str(esp_mprot_pms_world_to_str(world));
    } else {
        PRINT_MEMPROT_ERROR(res);
    }

    panic_print_str("\r\n  operation type: ");
    uint32_t operation;
    res = esp_mprot_get_violate_operation(s_memp_intr.mem_type, &operation, s_memp_intr.core);
    if (res == ESP_OK) {
        panic_print_str(esp_mprot_oper_type_to_str(operation));
    } else {
        PRINT_MEMPROT_ERROR(res);
    }

    if (esp_mprot_has_byte_enables(s_memp_intr.mem_type)) {
        panic_print_str("\r\n  byte-enables: ");
        uint32_t byte_enables;
        res = esp_mprot_get_violate_byte_enables(s_memp_intr.mem_type, &byte_enables, s_memp_intr.core);
        if (res == ESP_OK) {
            panic_print_hex(byte_enables);
        } else {
            PRINT_MEMPROT_ERROR(res);
        }
    }

    panic_print_str("\r\n");
}
#endif

static void panic_print_register_array(const char* names[], const uint32_t* regs, int size)
{
    const int regs_per_line = 4;
    for (int i = 0; i < size; i++) {
        if (i % regs_per_line == 0) {
            panic_print_str("\r\n");
        }
        panic_print_str(names[i]);
        panic_print_str(": 0x");
        panic_print_hex(regs[i]);
        panic_print_str("  ");
    }
}

void panic_print_registers(const void *f, int core)
{
    /**
     * General Purpose context, only print ABI name
     */
    const char *desc[] = {
        "MEPC    ", "RA      ", "SP      ", "GP      ", "TP      ", "T0      ", "T1      ", "T2      ",
        "S0/FP   ", "S1      ", "A0      ", "A1      ", "A2      ", "A3      ", "A4      ", "A5      ",
        "A6      ", "A7      ", "S2      ", "S3      ", "S4      ", "S5      ", "S6      ", "S7      ",
        "S8      ", "S9      ", "S10     ", "S11     ", "T3      ", "T4      ", "T5      ", "T6      ",
        "MSTATUS ", "MTVEC   ", "MCAUSE  ", "MTVAL   ", "MHARTID "
    };

    panic_print_str("Core ");
    panic_print_dec(core);
    panic_print_str(" register dump:");
    panic_print_register_array(desc, f, DIM(desc));
}

/**
 * This function will be called before the SoC-level panic is handled,
 * allowing us to check and override the exception cause for certain
 * pseudo-causes that do not have their own trigger
 */
bool panic_soc_check_pseudo_cause(void *f, panic_info_t *info)
{
    RvExcFrame *frame = (RvExcFrame *) f;
    bool pseudo_cause = false;

    /* Cache errors when reading instructions will result in an illegal instructions,
       before any cache error interrupts trigger. We override the exception cause if
       any cache errors are active to more accurately report the actual reason */
    if (esp_cache_err_has_active_err() && ((frame->mcause == MCAUSE_ILLEGAL_INSTRUCTION) || (frame->mcause == MCAUSE_ILLIGAL_INSTRUCTION_ACCESS) || (frame->mcause == MCAUSE_LOAD_ACCESS_FAULT))) {
        pseudo_cause = true;
        frame->mcause = ETS_CACHEERR_INUM;
    }

    return pseudo_cause;
}

/**
 * This function will be called when a SoC-level panic occurs.
 * SoC-level panics include cache errors and watchdog interrupts.
 */
void panic_soc_fill_info(void *f, panic_info_t *info)
{
    RvExcFrame *frame = (RvExcFrame *) f;

    info->reason = "Unknown reason";
    info->addr = (void *) frame->mepc;

    /* The mcause has been set by the CPU when the panic occurred.
     * All SoC-level panic will call this function, thus, this register
     * lets us know which error was triggered. */
    if (frame->mcause == ETS_CACHEERR_INUM) {
        /* Panic due to a cache error, multiple cache error are possible,
         * assign function print_cache_err_details to our structure's
         * details field. As its name states, it will give more details
         * about why the error happened. */

        info->core = esp_cache_err_get_cpuid();
        info->reason = "Cache error";
        info->details = print_cache_err_details;

    } else if (frame->mcause == PANIC_RSN_INTWDT_CPU0) {
        const int core = 0;
        info->core = core;
        info->exception = PANIC_EXCEPTION_IWDT;
        info->reason = "Interrupt wdt timeout on CPU0";
    }
#if SOC_CPU_CORES_NUM > 1
    else if (frame->mcause == PANIC_RSN_INTWDT_CPU1) {
        const int core = 1;
        info->core = core;
        info->exception = PANIC_EXCEPTION_IWDT;
        info->reason = "Interrupt wdt timeout on CPU1";
    }
#endif

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    else if (frame->mcause == ETS_ASSIST_DEBUG_INUM) {
        info->core = esp_hw_stack_guard_get_fired_cpu();
        if (info->core == ESP_HW_STACK_GUARD_NOT_FIRED) {
            info->core = 0;
        }
        info->reason = "Stack protection fault";
        info->details = print_assist_debug_details;
    }
#endif
#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
    else if (frame->mcause == ETS_MEMPROT_ERR_INUM) {
        info->reason = "Memory protection fault";
        info->details = print_memprot_err_details;
        info->core = esp_mprot_get_active_intr(&s_memp_intr) == ESP_OK ? s_memp_intr.core : -1;
    }
#endif
}

void panic_arch_fill_info(void *frame, panic_info_t *info)
{
    RvExcFrame *regs = (RvExcFrame *) frame;
    info->core = rv_utils_get_core_id();
    info->exception = PANIC_EXCEPTION_FAULT;

    static const char *reason[] = {
        "Instruction address misaligned",
        "Instruction access fault",
        "Illegal instruction",
        "Breakpoint",
        "Load address misaligned",
        "Load access fault",
        "Store address misaligned",
        "Store access fault",
        "Environment call from U-mode",
        "Environment call from S-mode",
        NULL,
        "Environment call from M-mode",
        "Instruction page fault",
        "Load page fault",
        NULL,
        "Store page fault",
    };

    if (regs->mcause < (sizeof(reason) / sizeof(reason[0]))) {
        if (reason[regs->mcause] != NULL) {
            info->reason = (reason[regs->mcause]);
        }
    }

    info->description = "Exception was unhandled.";

#if SOC_ASYNCHRONOUS_BUS_ERROR_MODE
    uintptr_t bus_error_pc = rv_utils_asynchronous_bus_get_error_pc();
    if (bus_error_pc) {
        /* Change mepc with the fault pc address */
        regs->mepc = bus_error_pc;
    }
#endif // SOC_ASYNCHRONOUS_BUS_ERROR_MODE

    info->addr = (void *) regs->mepc;
}

static void panic_print_basic_backtrace(const void *frame, int core)
{
    // Basic backtrace
    panic_print_str("\r\nStack memory:\r\n");
    uint32_t sp = (uint32_t)((RvExcFrame *)frame)->sp;
    const int per_line = 8;
    for (int x = 0; x < 1024; x += per_line * sizeof(uint32_t)) {
        uint32_t *spp = (uint32_t *)(sp + x);
        panic_print_hex(sp + x);
        panic_print_str(": ");
        for (int y = 0; y < per_line; y++) {
            panic_print_str("0x");
            panic_print_hex(spp[y]);
            panic_print_str(y == per_line - 1 ? "\r\n" : " ");
        }
    }
}

void panic_print_backtrace(const void *frame, int core)
{
#if CONFIG_ESP_SYSTEM_USE_EH_FRAME
    if (!spi_flash_cache_enabled()) {
        panic_print_str("\r\nWarning: SPI Flash cache is disabled, cannot process eh_frame parsing. "
                        "Falling back to basic backtrace.\r\n");
        panic_print_basic_backtrace(frame, core);
    } else {
        esp_eh_frame_print_backtrace(frame);
    }
#else
    panic_print_basic_backtrace(frame, core);
#endif
}

uint32_t panic_get_address(const void *f)
{
    return ((RvExcFrame *)f)->mepc;
}

uint32_t panic_get_cause(const void *f)
{
    return ((RvExcFrame *)f)->mcause;
}

void panic_set_address(void *f, uint32_t addr)
{
    ((RvExcFrame *)f)->mepc = addr;
}

void panic_prepare_frame_from_ctx(void* frame)
{
    /* Cleanup the frame, status registers are not saved during context switches, so these will contain garbage
       values from the stack.
    */
    ((RvExcFrame *)frame)->mstatus = RV_READ_CSR(mstatus);
    ((RvExcFrame *)frame)->mtvec = RV_READ_CSR(mtvec);

    ((RvExcFrame *)frame)->mcause = MCAUSE_INVALID_VALUE;
    ((RvExcFrame *)frame)->mtval = MCAUSE_INVALID_VALUE;

    ((RvExcFrame *)frame)->mhartid = RV_READ_CSR(mhartid);
}
