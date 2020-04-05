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
#include <stdlib.h>

#include "freertos/xtensa_context.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_spi_flash.h"

#include "esp_private/panic_reason.h"
#include "esp_private/system_internal.h"
#include "esp_debug_helpers.h"

#include "soc/soc_memory_layout.h"
#include "soc/cpu.h"
#include "soc/soc_caps.h"
#include "soc/rtc.h"

#include "hal/soc_hal.h"
#include "hal/cpu_hal.h"
#include "hal/wdt_types.h"
#include "hal/wdt_hal.h"


#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/cache_err_int.h"
#include "esp32/dport_access.h"
#include "esp32/rom/uart.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/cache_err_int.h"
#include "esp32s2/rom/uart.h"
#include "soc/extmem_reg.h"
#include "soc/cache_memory.h"
#include "soc/rtc_cntl_reg.h"
#endif

#include "panic_internal.h"

extern void esp_panic_handler(panic_info_t*);

static wdt_hal_context_t wdt0_context = {.inst = WDT_MWDT0, .mwdt_dev = &TIMERG0};

static XtExcFrame* xt_exc_frames[SOC_CPU_CORES_NUM] = {NULL};

/*
  Panic handlers; these get called when an unhandled exception occurs or the assembly-level
  task switching / interrupt code runs into an unrecoverable error. The default task stack
  overflow handler and abort handler are also in here.
*/

/*
  Note: The linker script will put everything in this file in IRAM/DRAM, so it also works with flash cache disabled.
*/
static void print_illegal_instruction_details(const void* f)
{
    XtExcFrame* frame  = (XtExcFrame*) f;
    /* Print out memory around the instruction word */
    uint32_t epc = frame->pc;
    epc = (epc & ~0x3) - 4;

    /* check that the address was sane */
    if (epc < SOC_IROM_MASK_LOW || epc >= SOC_IROM_HIGH) {
        return;
    }
    volatile uint32_t* pepc = (uint32_t*)epc;

    panic_print_str("Memory dump at 0x");
    panic_print_hex(epc);
    panic_print_str(": ");

    panic_print_hex(*pepc);
    panic_print_str(" ");
    panic_print_hex(*(pepc + 1));
    panic_print_str(" ");
    panic_print_hex(*(pepc + 2));
}

static void print_debug_exception_details(const void* f)
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

#if !CONFIG_FREERTOS_UNICORE
            if (f == xt_exc_frames[1]) {
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

static void print_backtrace_entry(uint32_t pc, uint32_t sp)
{
    panic_print_str("0x");
    panic_print_hex(pc);
    panic_print_str(":0x");
    panic_print_hex(sp);
}

static void print_backtrace(const void* f, int core)
{
    XtExcFrame *frame = (XtExcFrame*) f;
    int depth = 100;
    //Initialize stk_frame with first frame of stack
    esp_backtrace_frame_t stk_frame = {.pc = frame->pc, .sp = frame->a1, .next_pc = frame->a0};
    panic_print_str("\r\nBacktrace:");
    print_backtrace_entry(esp_cpu_process_stack_pc(stk_frame.pc), stk_frame.sp);

    //Check if first frame is valid
    bool corrupted = !(esp_stack_ptr_is_sane(stk_frame.sp) &&
                     esp_ptr_executable((void*)esp_cpu_process_stack_pc(stk_frame.pc)));

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

static void print_registers(const void *f, int core)
{
    XtExcFrame* frame = (XtExcFrame*) f;
    int *regs = (int *)frame;
    int x, y;
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

    for (x = 0; x < 24; x += 4) {
        panic_print_str("\r\n");
        for (y = 0; y < 4; y++) {
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
#if !CONFIG_FREERTOS_UNICORE
        && ((core == 0 && frame->exccause == PANIC_RSN_INTWDT_CPU0) ||
           (core == 1 && frame->exccause == PANIC_RSN_INTWDT_CPU1))
#endif //!CONFIG_FREERTOS_UNICORE
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

static void print_state_for_core(const void *f, int core)
{
    if (!g_panic_abort) {
        print_registers(f, core);
        panic_print_str("\r\n");
    }
    print_backtrace(f, core);
}

static void print_state(const void* f)
{
#if !CONFIG_FREERTOS_UNICORE
    int err_core = f == xt_exc_frames[0] ? 0 : 1;
#else
    int err_core = 0;
#endif

    print_state_for_core(f, err_core);

    panic_print_str("\r\n");

#if !CONFIG_FREERTOS_UNICORE
    // If there are other frame info, print them as well
    for (int i = 0; i < SOC_CPU_CORES_NUM; i++) {
        // `f` is the frame for the offending core, see note above.
        if (err_core != i && xt_exc_frames[i] != NULL) {
            print_state_for_core(xt_exc_frames[i], i);
            panic_print_str("\r\n");
        }
    }
#endif
}

#if CONFIG_IDF_TARGET_ESP32S2
static inline void print_cache_err_details(const void* f)
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
#endif

static void frame_to_panic_info(XtExcFrame *frame, panic_info_t* info, bool pseudo_excause)
{
    info->core = cpu_hal_get_core_id();
    info->exception = PANIC_EXCEPTION_FAULT;
    info->details = NULL;

    if (pseudo_excause) {
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
            "Cache exception",
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
        if(frame->exccause == PANIC_RSN_CACHEERR) {
            info->details = print_cache_err_details;
        }
#endif
    } else {
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

        if (info->reason == reason[0]) {
            info->details = print_illegal_instruction_details;
        }
    }

    info->state = print_state;
    info->addr = ((void*) ((XtExcFrame*) frame)->pc);
    info->frame = frame;
}

static void panic_handler(XtExcFrame *frame, bool pseudo_excause)
{
    /*
     * Setup environment and perform necessary architecture/chip specific
     * steps here prior to the system panic handler.
     * */
    int core_id = cpu_hal_get_core_id();

    // If multiple cores arrive at panic handler, save frames for all of them
    xt_exc_frames[core_id] = frame;

#if !CONFIG_FREERTOS_UNICORE
    // These are cases where both CPUs both go into panic handler. The following code ensures
    // only one core proceeds to the system panic handler.
    if (pseudo_excause) {
        #define BUSY_WAIT_IF_TRUE(b)                { if (b) while(1); }
        // For WDT expiry, pause the non-offending core - offending core handles panic
        BUSY_WAIT_IF_TRUE(frame->exccause == PANIC_RSN_INTWDT_CPU0 && core_id == 1);
        BUSY_WAIT_IF_TRUE(frame->exccause == PANIC_RSN_INTWDT_CPU1 && core_id == 0);

        // For cache error, pause the non-offending core - offending core handles panic
        BUSY_WAIT_IF_TRUE(frame->exccause == PANIC_RSN_CACHEERR && core_id != esp_cache_err_get_cpuid());
    }

    ets_delay_us(1);
    SOC_HAL_STALL_OTHER_CORES();
#endif

#if CONFIG_IDF_TARGET_ESP32
    esp_dport_access_int_abort();
#endif

#if !CONFIG_ESP_PANIC_HANDLER_IRAM
    // Re-enable CPU cache for current CPU if it was disabled
    if (!spi_flash_cache_enabled()) {
        spi_flash_enable_cache(core_id);
        panic_print_str("Re-enable cpu cache.\r\n");
    }
#endif

    if (esp_cpu_in_ocd_debug_mode()) {
        if (frame->exccause == PANIC_RSN_INTWDT_CPU0 ||
            frame->exccause == PANIC_RSN_INTWDT_CPU1) {
            wdt_hal_write_protect_disable(&wdt0_context);
            wdt_hal_handle_intr(&wdt0_context);
            wdt_hal_write_protect_enable(&wdt0_context);
        }
    }

    // Convert architecture exception frame into abstracted panic info
    panic_info_t info;
    frame_to_panic_info(frame, &info, pseudo_excause);

    // Call the system panic handler
    esp_panic_handler(&info);
}

void panicHandler(XtExcFrame *frame)
{
    // This panic handler gets called for when the double exception vector,
    // kernel exception vector gets used; as well as handling interrupt-based
    // faults cache error, wdt expiry. EXCAUSE register gets written with
    // one of PANIC_RSN_* values.
    panic_handler(frame, true);
}

void xt_unhandled_exception(XtExcFrame *frame)
{
    panic_handler(frame, false);
}

static __attribute__((noreturn)) void esp_digital_reset(void)
{
    // make sure all the panic handler output is sent from UART FIFO
    uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);
    // switch to XTAL (otherwise we will keep running from the PLL)

    rtc_clk_cpu_freq_set_xtal();

#if CONFIG_IDF_TARGET_ESP32
    esp_cpu_unstall(PRO_CPU_NUM);
#endif

    // reset the digital part
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_SW_SYS_RST);
    while (true) {
        ;
    }
}

void __attribute__((noreturn)) panic_restart(void)
{
    // If resetting because of a cache error, reset the digital part
    // Make sure that the reset reason is not a generic panic reason as well on ESP32S2,
    // as esp_cache_err_get_cpuid always returns PRO_CPU_NUM
    if (esp_cache_err_get_cpuid() != -1 && esp_reset_reason_get_hint() != ESP_RST_PANIC) {
        esp_digital_reset();
    } else {
        esp_restart_noos();
    }
}