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
#include "freertos/xtensa_context.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_debug_helpers.h"

#include "esp_private/panic_internal.h"
#include "esp_private/panic_reason.h"
#include "soc/soc.h"

#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/cache_err_int.h"
#else
#include "soc/extmem_reg.h"
#include "soc/cache_memory.h"
#include "soc/rtc_cntl_reg.h"
#if CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/cache_err_int.h"
#ifdef CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
#include "esp32s2/memprot.h"
#endif
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/cache_err_int.h"
#ifdef CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
#include "esp32s3/memprot.h"
#endif
#endif
#endif // CONFIG_IDF_TARGET_ESP32

void panic_print_registers(const void *f, int core)
{
    XtExcFrame *frame = (XtExcFrame *) f;
    int *regs = (int *)frame;

    const char *sdesc[] = {
        "PC      ", "PS      ", "A0      ", "A1      ", "A2      ", "A3      ", "A4      ", "A5      ",
        "A6      ", "A7      ", "A8      ", "A9      ", "A10     ", "A11     ", "A12     ", "A13     ",
        "A14     ", "A15     ", "SAR     ", "EXCCAUSE", "EXCVADDR", "LBEG    ", "LEND    ", "LCOUNT  "
    };

    /* only dump registers for 'real' crashes, if crashing via abort()
       the register window is no longer useful.
    */
    panic_print_str("Core ");
    panic_print_dec(core);
    panic_print_str(" register dump:");

    for (int x = 0; x < 24; x += 4) {
        panic_print_str("\r\n");
        for (int y = 0; y < 4; y++) {
            if (sdesc[x + y][0] != 0) {
                panic_print_str(sdesc[x + y]);
                panic_print_str(": 0x");
                panic_print_hex(regs[x + y + 1]);
                panic_print_str("  ");
            }
        }
    }

    // If the core which triggers the interrupt watchpoint was in ISR context, dump the epc registers.
    if (xPortInterruptedFromISRContext()
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
            && ((core == 0 && frame->exccause == PANIC_RSN_INTWDT_CPU0) ||
                (core == 1 && frame->exccause == PANIC_RSN_INTWDT_CPU1))
#endif //!CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
        ) {

        panic_print_str("\r\n");

        uint32_t __value;
        panic_print_str("Core ");
        panic_print_dec(core);
        panic_print_str(" was running in ISR context:\r\n");

        __asm__("rsr.epc1 %0" : "=a"(__value));
        panic_print_str("EPC1    : 0x");
        panic_print_hex(__value);

        __asm__("rsr.epc2 %0" : "=a"(__value));
        panic_print_str("  EPC2    : 0x");
        panic_print_hex(__value);

        __asm__("rsr.epc3 %0" : "=a"(__value));
        panic_print_str("  EPC3    : 0x");
        panic_print_hex(__value);

        __asm__("rsr.epc4 %0" : "=a"(__value));
        panic_print_str("  EPC4    : 0x");
        panic_print_hex(__value);
    }
}

static void print_illegal_instruction_details(const void *f)
{
    XtExcFrame *frame  = (XtExcFrame *) f;
    /* Print out memory around the instruction word */
    uint32_t epc = frame->pc;
    epc = (epc & ~0x3) - 4;

    /* check that the address was sane */
    if (epc < SOC_IROM_MASK_LOW || epc >= SOC_IROM_HIGH) {
        return;
    }
    volatile uint32_t *pepc = (uint32_t *)epc;

    panic_print_str("Memory dump at 0x");
    panic_print_hex(epc);
    panic_print_str(": ");

    panic_print_hex(*pepc);
    panic_print_str(" ");
    panic_print_hex(*(pepc + 1));
    panic_print_str(" ");
    panic_print_hex(*(pepc + 2));
}


static void print_debug_exception_details(const void *f)
{
    int debug_rsn;
    asm("rsr.debugcause %0":"=r"(debug_rsn));
    panic_print_str("Debug exception reason: ");
    if (debug_rsn & XCHAL_DEBUGCAUSE_ICOUNT_MASK) {
        panic_print_str("SingleStep ");
    }
    if (debug_rsn & XCHAL_DEBUGCAUSE_IBREAK_MASK) {
        panic_print_str("HwBreakpoint ");
    }
    if (debug_rsn & XCHAL_DEBUGCAUSE_DBREAK_MASK) {
        //Unlike what the ISA manual says, this core seemingly distinguishes from a DBREAK
        //reason caused by watchdog 0 and one caused by watchdog 1 by setting bit 8 of the
        //debugcause if the cause is watchpoint 1 and clearing it if it's watchpoint 0.
        if (debug_rsn & (1 << 8)) {
#if CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK
            int core = 0;

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
            if (f == g_exc_frames[1]) {
                core = 1;
            }
#endif

            const char *name = pcTaskGetTaskName(xTaskGetCurrentTaskHandleForCPU(core));
            panic_print_str("Stack canary watchpoint triggered (");
            panic_print_str(name);
            panic_print_str(") ");
#else
            panic_print_str("Watchpoint 1 triggered ");
#endif
        } else {
            panic_print_str("Watchpoint 0 triggered ");
        }
    }
    if (debug_rsn & XCHAL_DEBUGCAUSE_BREAK_MASK) {
        panic_print_str("BREAK instr ");
    }
    if (debug_rsn & XCHAL_DEBUGCAUSE_BREAKN_MASK) {
        panic_print_str("BREAKN instr ");
    }
    if (debug_rsn & XCHAL_DEBUGCAUSE_DEBUGINT_MASK) {
        panic_print_str("DebugIntr ");
    }
}

#if CONFIG_IDF_TARGET_ESP32S2
static inline void print_cache_err_details(const void *f)
{
    uint32_t vaddr = 0, size = 0;
    uint32_t status[2];
    status[0] = REG_READ(EXTMEM_CACHE_DBG_STATUS0_REG);
    status[1] = REG_READ(EXTMEM_CACHE_DBG_STATUS1_REG);
    for (int i = 0; i < 32; i++) {
        switch (status[0] & BIT(i)) {
        case EXTMEM_IC_SYNC_SIZE_FAULT_ST:
            vaddr = REG_READ(EXTMEM_PRO_ICACHE_MEM_SYNC0_REG);
            size = REG_READ(EXTMEM_PRO_ICACHE_MEM_SYNC1_REG);
            panic_print_str("Icache sync parameter configuration error, the error address and size is 0x");
            panic_print_hex(vaddr);
            panic_print_str("(0x");
            panic_print_hex(size);
            panic_print_str(")\r\n");
            break;
        case EXTMEM_IC_PRELOAD_SIZE_FAULT_ST:
            vaddr = REG_READ(EXTMEM_PRO_ICACHE_PRELOAD_ADDR_REG);
            size = REG_READ(EXTMEM_PRO_ICACHE_PRELOAD_SIZE_REG);
            panic_print_str("Icache preload parameter configuration error, the error address and size is 0x");
            panic_print_hex(vaddr);
            panic_print_str("(0x");
            panic_print_hex(size);
            panic_print_str(")\r\n");
            break;
        case EXTMEM_ICACHE_REJECT_ST:
            vaddr = REG_READ(EXTMEM_PRO_ICACHE_REJECT_VADDR_REG);
            panic_print_str("Icache reject error occurred while accessing the address 0x");
            panic_print_hex(vaddr);

            if (REG_READ(EXTMEM_PRO_CACHE_MMU_FAULT_CONTENT_REG) & MMU_INVALID) {
                panic_print_str(" (invalid mmu entry)");
            }
            panic_print_str("\r\n");
            break;
        default:
            break;
        }
        switch (status[1] & BIT(i)) {
        case EXTMEM_DC_SYNC_SIZE_FAULT_ST:
            vaddr = REG_READ(EXTMEM_PRO_DCACHE_MEM_SYNC0_REG);
            size = REG_READ(EXTMEM_PRO_DCACHE_MEM_SYNC1_REG);
            panic_print_str("Dcache sync parameter configuration error, the error address and size is 0x");
            panic_print_hex(vaddr);
            panic_print_str("(0x");
            panic_print_hex(size);
            panic_print_str(")\r\n");
            break;
        case EXTMEM_DC_PRELOAD_SIZE_FAULT_ST:
            vaddr = REG_READ(EXTMEM_PRO_DCACHE_PRELOAD_ADDR_REG);
            size = REG_READ(EXTMEM_PRO_DCACHE_PRELOAD_SIZE_REG);
            panic_print_str("Dcache preload parameter configuration error, the error address and size is 0x");
            panic_print_hex(vaddr);
            panic_print_str("(0x");
            panic_print_hex(size);
            panic_print_str(")\r\n");
            break;
        case EXTMEM_DCACHE_WRITE_FLASH_ST:
            panic_print_str("Write back error occurred while dcache tries to write back to flash\r\n");
            break;
        case EXTMEM_DCACHE_REJECT_ST:
            vaddr = REG_READ(EXTMEM_PRO_DCACHE_REJECT_VADDR_REG);
            panic_print_str("Dcache reject error occurred while accessing the address 0x");
            panic_print_hex(vaddr);

            if (REG_READ(EXTMEM_PRO_CACHE_MMU_FAULT_CONTENT_REG) & MMU_INVALID) {
                panic_print_str(" (invalid mmu entry)");
            }
            panic_print_str("\r\n");
            break;
        case EXTMEM_MMU_ENTRY_FAULT_ST:
            vaddr = REG_READ(EXTMEM_PRO_CACHE_MMU_FAULT_VADDR_REG);
            panic_print_str("MMU entry fault error occurred while accessing the address 0x");
            panic_print_hex(vaddr);

            if (REG_READ(EXTMEM_PRO_CACHE_MMU_FAULT_CONTENT_REG) & MMU_INVALID) {
                panic_print_str(" (invalid mmu entry)");
            }
            panic_print_str("\r\n");
            break;
        default:
            break;
        }
    }
}

#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
static inline void print_memprot_err_details(const void *f)
{
    uint32_t *fault_addr;
    uint32_t op_type, op_subtype;
    mem_type_prot_t mem_type = esp_memprot_get_active_intr_memtype();
    esp_memprot_get_fault_status( mem_type, &fault_addr, &op_type, &op_subtype );

    char *operation_type = "Write";
    if ( op_type == 0 ) {
        operation_type = (mem_type == MEMPROT_IRAM0_SRAM && op_subtype == 0) ? "Instruction fetch" : "Read";
    }

    panic_print_str( operation_type );
    panic_print_str( " operation at address 0x" );
    panic_print_hex( (uint32_t)fault_addr );
    panic_print_str(" not permitted.\r\n");
}
#endif

#elif CONFIG_IDF_TARGET_ESP32S3
static inline void print_cache_err_details(const void* f)
{
    uint32_t vaddr = 0, size = 0;
    uint32_t status;
    status = REG_READ(EXTMEM_CACHE_ILG_INT_ST_REG);
    for (int i = 0; i < 32; i++) {
        switch (status & BIT(i)) {
        case EXTMEM_ICACHE_SYNC_OP_FAULT_ST:
            //TODO, which size should fetch
            //vaddr = REG_READ(EXTMEM_ICACHE_MEM_SYNC0_REG);
            //size = REG_READ(EXTMEM_ICACHE_MEM_SYNC1_REG);
            panic_print_str("Icache sync parameter configuration error, the error address and size is 0x");
            panic_print_hex(vaddr);
            panic_print_str("(0x");
            panic_print_hex(size);
            panic_print_str(")\r\n");
            break;
        case EXTMEM_ICACHE_PRELOAD_OP_FAULT_ST:
            //TODO, which size should fetch
            vaddr = REG_READ(EXTMEM_ICACHE_PRELOAD_ADDR_REG);
            size = REG_READ(EXTMEM_ICACHE_PRELOAD_SIZE_REG);
            panic_print_str("Icache preload parameter configuration error, the error address and size is 0x");
            panic_print_hex(vaddr);
            panic_print_str("(0x");
            panic_print_hex(size);
            panic_print_str(")\r\n");
            break;
        case EXTMEM_DCACHE_SYNC_OP_FAULT_ST:
            //TODO, which size should fetch
            //vaddr = REG_READ(EXTMEM_DCACHE_MEM_SYNC0_REG);
            //size = REG_READ(EXTMEM_DCACHE_MEM_SYNC1_REG);
            panic_print_str("Dcache sync parameter configuration error, the error address and size is 0x");
            panic_print_hex(vaddr);
            panic_print_str("(0x");
            panic_print_hex(size);
            panic_print_str(")\r\n");
            break;
        case EXTMEM_DCACHE_PRELOAD_OP_FAULT_ST:
            //TODO, which size should fetch
            vaddr = REG_READ(EXTMEM_DCACHE_PRELOAD_ADDR_REG);
            size = REG_READ(EXTMEM_DCACHE_PRELOAD_SIZE_REG);
            panic_print_str("Dcache preload parameter configuration error, the error address and size is 0x");
            panic_print_hex(vaddr);
            panic_print_str("(0x");
            panic_print_hex(size);
            panic_print_str(")\r\n");
            break;
        case EXTMEM_DCACHE_WRITE_FLASH_ST:
            panic_print_str("Write back error occurred while dcache tries to write back to flash\r\n");
            break;
        case EXTMEM_MMU_ENTRY_FAULT_ST:
            vaddr = REG_READ(EXTMEM_CACHE_MMU_FAULT_VADDR_REG);
            panic_print_str("MMU entry fault error occurred while accessing the address 0x");
            panic_print_hex(vaddr);

            if (REG_READ(EXTMEM_CACHE_MMU_FAULT_CONTENT_REG) & MMU_INVALID) {
                panic_print_str(" (invalid mmu entry)");
            }
            panic_print_str("\r\n");
            break;
        default:
            break;
        }
    }
    panic_print_str("\r\n");
}
#endif


void panic_arch_fill_info(void *f, panic_info_t *info)
{
    XtExcFrame *frame = (XtExcFrame*) f;
    static const char *reason[] = {
        "IllegalInstruction", "Syscall", "InstructionFetchError", "LoadStoreError",
        "Level1Interrupt", "Alloca", "IntegerDivideByZero", "PCValue",
        "Privileged", "LoadStoreAlignment", "res", "res",
        "InstrPDAddrError", "LoadStorePIFDataError", "InstrPIFAddrError", "LoadStorePIFAddrError",
        "InstTLBMiss", "InstTLBMultiHit", "InstFetchPrivilege", "res",
        "InstrFetchProhibited", "res", "res", "res",
        "LoadStoreTLBMiss", "LoadStoreTLBMultihit", "LoadStorePrivilege", "res",
        "LoadProhibited", "StoreProhibited", "res", "res",
        "Cp0Dis", "Cp1Dis", "Cp2Dis", "Cp3Dis",
        "Cp4Dis", "Cp5Dis", "Cp6Dis", "Cp7Dis"
    };

    if (frame->exccause < (sizeof(reason) / sizeof(char *))) {
        info->reason = (reason[frame->exccause]);
    } else {
        info->reason = "Unknown";
    }

    info->description = "Exception was unhandled.";

    if (frame->exccause == EXCCAUSE_ILLEGAL) {
        info->details = print_illegal_instruction_details;
    }

    info->addr = ((void *) ((XtExcFrame *) frame)->pc);
}

void panic_soc_fill_info(void *f, panic_info_t *info)
{
    // [refactor-todo] this should be in the common port panic_handler.c, once
    // these special exceptions are supported in there.
    XtExcFrame *frame = (XtExcFrame*) f;
    if (frame->exccause == PANIC_RSN_INTWDT_CPU0) {
        info->core = 0;
        info->exception = PANIC_EXCEPTION_IWDT;
    } else if (frame->exccause == PANIC_RSN_INTWDT_CPU1) {
        info->core = 1;
        info->exception = PANIC_EXCEPTION_IWDT;
    } else if (frame->exccause == PANIC_RSN_CACHEERR) {
        info->core =  esp_cache_err_get_cpuid();
    } else {}

    //Please keep in sync with PANIC_RSN_* defines
    static const char *pseudo_reason[] = {
        "Unknown reason",
        "Unhandled debug exception",
        "Double exception",
        "Unhandled kernel exception",
        "Coprocessor exception",
        "Interrupt wdt timeout on CPU0",
        "Interrupt wdt timeout on CPU1",
#if CONFIG_IDF_TARGET_ESP32
        "Cache disabled but cached memory region accessed",
#elif CONFIG_IDF_TARGET_ESP32S2
        "Cache error",
#endif
    };

    info->reason = pseudo_reason[0];
    info->description = NULL;

    if (frame->exccause <= PANIC_RSN_MAX) {
        info->reason = pseudo_reason[frame->exccause];
    }

    if (frame->exccause == PANIC_RSN_DEBUGEXCEPTION) {
        info->details = print_debug_exception_details;
        info->exception = PANIC_EXCEPTION_DEBUG;
    }

#if CONFIG_IDF_TARGET_ESP32S2
    if (frame->exccause == PANIC_RSN_CACHEERR) {
#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
        if ( esp_memprot_is_intr_ena_any() ) {
            info->details = print_memprot_err_details;
            info->reason = "Memory protection fault";
        } else
#endif
        {
            info->details = print_cache_err_details;
        }
    }
#endif
}

static void print_backtrace_entry(uint32_t pc, uint32_t sp)
{
    panic_print_str("0x");
    panic_print_hex(pc);
    panic_print_str(":0x");
    panic_print_hex(sp);
}

uint32_t panic_get_address(const void* f)
{
    return ((XtExcFrame*)f)->pc;
}

uint32_t panic_get_cause(const void* f)
{
    return ((XtExcFrame*)f)->exccause;
}

void panic_set_address(void *f, uint32_t addr)
{
    ((XtExcFrame*)f)->pc = addr;
}

void panic_print_backtrace(const void *f, int core)
{
    // [refactor-todo] once debug helpers have support for both xtensa and riscv, move to
    // common panic_handler.c
    XtExcFrame *frame = (XtExcFrame *) f;
    int depth = 100;
    //Initialize stk_frame with first frame of stack
    esp_backtrace_frame_t stk_frame = {.pc = frame->pc, .sp = frame->a1, .next_pc = frame->a0};
    panic_print_str("\r\nBacktrace:");
    print_backtrace_entry(esp_cpu_process_stack_pc(stk_frame.pc), stk_frame.sp);

    //Check if first frame is valid
    bool corrupted = !(esp_stack_ptr_is_sane(stk_frame.sp) &&
                       (esp_ptr_executable((void *)esp_cpu_process_stack_pc(stk_frame.pc)) ||
                        /* Ignore the first corrupted PC in case of InstrFetchProhibited */
                        frame->exccause == EXCCAUSE_INSTR_PROHIBITED));

    uint32_t i = ((depth <= 0) ? INT32_MAX : depth) - 1;    //Account for stack frame that's already printed
    while (i-- > 0 && stk_frame.next_pc != 0 && !corrupted) {
        if (!esp_backtrace_get_next_frame(&stk_frame)) {    //Get next stack frame
            corrupted = true;
        }
        panic_print_str(" ");
        print_backtrace_entry(esp_cpu_process_stack_pc(stk_frame.pc), stk_frame.sp);
    }

    //Print backtrace termination marker
    if (corrupted) {
        panic_print_str(" |<-CORRUPTED");
    } else if (stk_frame.next_pc != 0) {    //Backtrace continues
        panic_print_str(" |<-CONTINUES");
    }
}
