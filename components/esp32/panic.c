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

#include <xtensa/config/core.h>

#include "esp32/rom/rtc.h"
#include "esp32/rom/uart.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/xtensa_api.h"

#include "soc/uart_periph.h"
#include "soc/gpio_periph.h"
#include "soc/dport_reg.h"
#include "soc/rtc_periph.h"
#include "soc/timer_periph.h"
#include "soc/cpu.h"
#include "soc/rtc.h"
#include "soc/rtc_wdt.h"
#include "soc/soc_memory_layout.h"

#include "esp_private/gdbstub.h"
#include "esp_debug_helpers.h"
#include "esp_private/panic_reason.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_core_dump.h"
#include "esp_spi_flash.h"
#include "esp32/cache_err_int.h"
#include "esp_app_trace.h"
#include "esp_private/system_internal.h"
#include "sdkconfig.h"
#include "esp_ota_ops.h"
#include "driver/timer.h"
#include "hal/timer_ll.h"
#if CONFIG_SYSVIEW_ENABLE
#include "SEGGER_RTT.h"
#endif

#if CONFIG_ESP32_APPTRACE_ONPANIC_HOST_FLUSH_TMO == -1
#define APPTRACE_ONPANIC_HOST_FLUSH_TMO   ESP_APPTRACE_TMO_INFINITE
#else
#define APPTRACE_ONPANIC_HOST_FLUSH_TMO   (1000*CONFIG_ESP32_APPTRACE_ONPANIC_HOST_FLUSH_TMO)
#endif
/*
  Panic handlers; these get called when an unhandled exception occurs or the assembly-level
  task switching / interrupt code runs into an unrecoverable error. The default task stack
  overflow handler and abort handler are also in here.
*/

/*
  Note: The linker script will put everything in this file in IRAM/DRAM, so it also works with flash cache disabled.
*/

#if !CONFIG_ESP32_PANIC_SILENT_REBOOT
//printf may be broken, so we fix our own printing fns...
static void panicPutChar(char c)
{
    while (((READ_PERI_REG(UART_STATUS_REG(CONFIG_ESP_CONSOLE_UART_NUM)) >> UART_TXFIFO_CNT_S)&UART_TXFIFO_CNT) >= 126) ;
    WRITE_PERI_REG(UART_FIFO_REG(CONFIG_ESP_CONSOLE_UART_NUM), c);
}

static void panicPutStr(const char *c)
{
    int x = 0;
    while (c[x] != 0) {
        panicPutChar(c[x]);
        x++;
    }
}

static void panicPutHex(int a)
{
    int x;
    int c;
    for (x = 0; x < 8; x++) {
        c = (a >> 28) & 0xf;
        if (c < 10) {
            panicPutChar('0' + c);
        } else {
            panicPutChar('a' + c - 10);
        }
        a <<= 4;
    }
}

static void panicPutDec(int a)
{
    int n1, n2;
    n1 = a % 10;
    n2 = a / 10;
    if (n2 == 0) {
        panicPutChar(' ');
    } else {
        panicPutChar(n2 + '0');
    }
    panicPutChar(n1 + '0');
}
#else
//No printing wanted. Stub out these functions.
static void panicPutChar(char c) { }
static void panicPutStr(const char *c) { }
static void panicPutHex(int a) { }
static void panicPutDec(int a) { }
#endif

void  __attribute__((weak)) vApplicationStackOverflowHook( TaskHandle_t xTask, signed char *pcTaskName )
{
    panicPutStr("***ERROR*** A stack overflow in task ");
    panicPutStr((char *)pcTaskName);
    panicPutStr(" has been detected.\r\n");
    abort();
}

/* These two weak stubs for esp_reset_reason_{get,set}_hint are used when
 * the application does not call esp_reset_reason() function, and
 * reset_reason.c is not linked into the output file.
 */
void __attribute__((weak)) esp_reset_reason_set_hint(esp_reset_reason_t hint)
{
}

esp_reset_reason_t __attribute__((weak)) esp_reset_reason_get_hint(void)
{
    return ESP_RST_UNKNOWN;
}


static bool abort_called;

static __attribute__((noreturn)) inline void invoke_abort(void)
{
    abort_called = true;
#if CONFIG_ESP32_APPTRACE_ENABLE
#if CONFIG_SYSVIEW_ENABLE
    SEGGER_RTT_ESP32_FlushNoLock(CONFIG_ESP32_APPTRACE_POSTMORTEM_FLUSH_THRESH, APPTRACE_ONPANIC_HOST_FLUSH_TMO);
#else
    esp_apptrace_flush_nolock(ESP_APPTRACE_DEST_TRAX, CONFIG_ESP32_APPTRACE_POSTMORTEM_FLUSH_THRESH,
                              APPTRACE_ONPANIC_HOST_FLUSH_TMO);
#endif
#endif
    while (1) {
        if (esp_cpu_in_ocd_debug_mode()) {
            __asm__ ("break 0,0");
        }
        *((int *) 0) = 0;
    }
}

void abort(void)
{
#if !CONFIG_ESP32_PANIC_SILENT_REBOOT
    ets_printf("abort() was called at PC 0x%08x on core %d\r\n", (intptr_t)__builtin_return_address(0) - 3, xPortGetCoreID());
#endif
    /* Calling code might have set other reset reason hint (such as Task WDT),
     * don't overwrite that.
     */
    if (esp_reset_reason_get_hint() == ESP_RST_UNKNOWN) {
        esp_reset_reason_set_hint(ESP_RST_PANIC);
    }
    invoke_abort();
}


static const char *edesc[] = {
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

#define NUM_EDESCS (sizeof(edesc) / sizeof(char *))

static void commonErrorHandler(XtExcFrame *frame);
static inline void disableAllWdts(void);
static void illegal_instruction_helper(XtExcFrame *frame);

//The fact that we've panic'ed probably means the other CPU is now running wild, possibly
//messing up the serial output, so we stall it here.
static void haltOtherCore(void)
{
    esp_cpu_stall( xPortGetCoreID() == 0 ? 1 : 0 );
}


static void setFirstBreakpoint(uint32_t pc)
{
    asm(
        "wsr.ibreaka0 %0\n" \
        "rsr.ibreakenable a3\n" \
        "movi a4,1\n" \
        "or a4, a4, a3\n" \
        "wsr.ibreakenable a4\n" \
        ::"r"(pc):"a3", "a4");
}

//When interrupt watchdog happen in one core, both cores will be interrupted.
//The core which doesn't trigger the interrupt watchdog will save the frame and return.
//The core which triggers the interrupt watchdog will use the saved frame, and dump frames for both cores.
#if !CONFIG_FREERTOS_UNICORE
static volatile XtExcFrame * other_core_frame = NULL;
#endif //!CONFIG_FREERTOS_UNICORE

void panicHandler(XtExcFrame *frame)
{
    int core_id = xPortGetCoreID();
    //Please keep in sync with PANIC_RSN_* defines
    const char *reasons[] = {
        "Unknown reason",
        "Unhandled debug exception",
        "Double exception",
        "Unhandled kernel exception",
        "Coprocessor exception",
        "Interrupt wdt timeout on CPU0",
        "Interrupt wdt timeout on CPU1",
        "Cache disabled but cached memory region accessed",
    };
    const char *reason = reasons[0];
    //The panic reason is stored in the EXCCAUSE register.
    if (frame->exccause <= PANIC_RSN_MAX) {
        reason = reasons[frame->exccause];
    }

#if !CONFIG_FREERTOS_UNICORE
    //Save frame for other core.
    if ((frame->exccause == PANIC_RSN_INTWDT_CPU0 && core_id == 1) || (frame->exccause == PANIC_RSN_INTWDT_CPU1 && core_id == 0)) {
        other_core_frame = frame;
        while (1);
    }

    //The core which triggers the interrupt watchdog will delay 1 us, so the other core can save its frame.
    if (frame->exccause == PANIC_RSN_INTWDT_CPU0 || frame->exccause == PANIC_RSN_INTWDT_CPU1) {
        ets_delay_us(1);
    }

    if (frame->exccause == PANIC_RSN_CACHEERR && esp_cache_err_get_cpuid() != core_id) {
        // Cache error interrupt will be handled by the panic handler
        // on the other CPU.
        while (1);
    }
#endif //!CONFIG_FREERTOS_UNICORE

    if (frame->exccause == PANIC_RSN_INTWDT_CPU0 || frame->exccause == PANIC_RSN_INTWDT_CPU1) {
        esp_reset_reason_set_hint(ESP_RST_INT_WDT);
    }

    haltOtherCore();
    esp_dport_access_int_abort();
    panicPutStr("Guru Meditation Error: Core ");
    panicPutDec(core_id);
    panicPutStr(" panic'ed (");
    panicPutStr(reason);
    panicPutStr(")\r\n");
    if (frame->exccause == PANIC_RSN_DEBUGEXCEPTION) {
        int debugRsn;
        asm("rsr.debugcause %0":"=r"(debugRsn));
        panicPutStr("Debug exception reason: ");
        if (debugRsn & XCHAL_DEBUGCAUSE_ICOUNT_MASK) {
            panicPutStr("SingleStep ");
        }
        if (debugRsn & XCHAL_DEBUGCAUSE_IBREAK_MASK) {
            panicPutStr("HwBreakpoint ");
        }
        if (debugRsn & XCHAL_DEBUGCAUSE_DBREAK_MASK) {
            //Unlike what the ISA manual says, this core seemingly distinguishes from a DBREAK
            //reason caused by watchdog 0 and one caused by watchdog 1 by setting bit 8 of the
            //debugcause if the cause is watchdog 1 and clearing it if it's watchdog 0.
            if (debugRsn & (1 << 8)) {
#if CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK
                const char *name = pcTaskGetTaskName(xTaskGetCurrentTaskHandleForCPU(core_id));
                panicPutStr("Stack canary watchpoint triggered (");
                panicPutStr(name);
                panicPutStr(") ");
#else
                panicPutStr("Watchpoint 1 triggered ");
#endif
            } else {
                panicPutStr("Watchpoint 0 triggered ");
            }
        }
        if (debugRsn & XCHAL_DEBUGCAUSE_BREAK_MASK) {
            panicPutStr("BREAK instr ");
        }
        if (debugRsn & XCHAL_DEBUGCAUSE_BREAKN_MASK) {
            panicPutStr("BREAKN instr ");
        }
        if (debugRsn & XCHAL_DEBUGCAUSE_DEBUGINT_MASK) {
            panicPutStr("DebugIntr ");
        }
        panicPutStr("\r\n");
    }

    if (esp_cpu_in_ocd_debug_mode()) {
        disableAllWdts();
        if (frame->exccause == PANIC_RSN_INTWDT_CPU0 ||
            frame->exccause == PANIC_RSN_INTWDT_CPU1) {
            timer_group_clr_intr_sta_in_isr(TIMER_GROUP_1, TIMER_INTR_WDT);
        }
#if CONFIG_ESP32_APPTRACE_ENABLE
#if CONFIG_SYSVIEW_ENABLE
        SEGGER_RTT_ESP32_FlushNoLock(CONFIG_ESP32_APPTRACE_POSTMORTEM_FLUSH_THRESH, APPTRACE_ONPANIC_HOST_FLUSH_TMO);
#else
        esp_apptrace_flush_nolock(ESP_APPTRACE_DEST_TRAX, CONFIG_ESP32_APPTRACE_POSTMORTEM_FLUSH_THRESH,
                                  APPTRACE_ONPANIC_HOST_FLUSH_TMO);
#endif
#endif
        setFirstBreakpoint(frame->pc);
        return;
    }
    commonErrorHandler(frame);
}

void xt_unhandled_exception(XtExcFrame *frame)
{
    haltOtherCore();
    esp_dport_access_int_abort();
    if (!abort_called) {
        panicPutStr("Guru Meditation Error: Core ");
        panicPutDec(xPortGetCoreID());
        panicPutStr(" panic'ed (");
        int exccause = frame->exccause;
        if (exccause < NUM_EDESCS) {
            panicPutStr(edesc[exccause]);
        } else {
            panicPutStr("Unknown");
        }
        panicPutStr(")");
        if (esp_cpu_in_ocd_debug_mode()) {
            panicPutStr(" at pc=");
            panicPutHex(frame->pc);
            panicPutStr(". Setting bp and returning..\r\n");
#if CONFIG_ESP32_APPTRACE_ENABLE
#if CONFIG_SYSVIEW_ENABLE
            SEGGER_RTT_ESP32_FlushNoLock(CONFIG_ESP32_APPTRACE_POSTMORTEM_FLUSH_THRESH, APPTRACE_ONPANIC_HOST_FLUSH_TMO);
#else
            esp_apptrace_flush_nolock(ESP_APPTRACE_DEST_TRAX, CONFIG_ESP32_APPTRACE_POSTMORTEM_FLUSH_THRESH,
                                      APPTRACE_ONPANIC_HOST_FLUSH_TMO);
#endif
#endif
            //Stick a hardware breakpoint on the address the handler returns to. This way, the OCD debugger
            //will kick in exactly at the context the error happened.
            setFirstBreakpoint(frame->pc);
            return;
        }
        panicPutStr(". Exception was unhandled.\r\n");
        if (exccause == 0 /* IllegalInstruction */) {
            illegal_instruction_helper(frame);
        }
        esp_reset_reason_set_hint(ESP_RST_PANIC);
    }
    commonErrorHandler(frame);
}

static void illegal_instruction_helper(XtExcFrame *frame)
{
    /* Print out memory around the instruction word */
    uint32_t epc = frame->pc;
    epc = (epc & ~0x3) - 4;

    /* check that the address was sane */
    if (epc < SOC_IROM_MASK_LOW || epc >= SOC_IROM_HIGH) {
        return;
    }
    volatile uint32_t* pepc = (uint32_t*)epc;

    panicPutStr("Memory dump at 0x");
    panicPutHex(epc);
    panicPutStr(": ");

    panicPutHex(*pepc);
    panicPutStr(" ");
    panicPutHex(*(pepc + 1));
    panicPutStr(" ");
    panicPutHex(*(pepc + 2));
    panicPutStr("\r\n");
}

/*
  If watchdogs are enabled, the panic handler runs the risk of getting aborted pre-emptively because
  an overzealous watchdog decides to reset it. On the other hand, if we disable all watchdogs, we run
  the risk of somehow halting in the panic handler and not resetting. That is why this routine kills
  all watchdogs except the timer group 0 watchdog, and it reconfigures that to reset the chip after
  one second.
*/
static void reconfigureAllWdts(void)
{
    timer_ll_wdt_set_protect(&TIMERG0, false);
    timer_ll_wdt_feed(&TIMERG0);
    timer_ll_wdt_init(&TIMERG0);
    timer_ll_wdt_set_tick(&TIMERG0, TG0_WDT_TICK_US); //Prescaler: wdt counts in ticks of TG0_WDT_TICK_US
    //1st stage timeout: reset system
    timer_ll_wdt_set_timeout_behavior(&TIMERG0, 0, TIMER_WDT_RESET_SYSTEM);
    //1 second before reset
    timer_ll_wdt_set_timeout(&TIMERG0, 0, 1000*1000/TG0_WDT_TICK_US);
    timer_ll_wdt_set_enable(&TIMERG0, true);
    timer_ll_wdt_set_protect(&TIMERG0, true);

    //Disable wdt 1
    timer_ll_wdt_set_protect(&TIMERG1, false);
    timer_ll_wdt_set_enable(&TIMERG1, false);
    timer_ll_wdt_set_protect(&TIMERG1, true);
}

/*
  This disables all the watchdogs for when we call the gdbstub.
*/
static inline void disableAllWdts(void)
{
    timer_ll_wdt_set_protect(&TIMERG0, false);
    timer_ll_wdt_set_enable(&TIMERG0, false);
    timer_ll_wdt_set_protect(&TIMERG0, true);

    timer_ll_wdt_set_protect(&TIMERG1, false);
    timer_ll_wdt_set_enable(&TIMERG1, false);
    timer_ll_wdt_set_protect(&TIMERG1, true);
}

#if CONFIG_ESP32_PANIC_PRINT_REBOOT || CONFIG_ESP32_PANIC_SILENT_REBOOT
static void esp_panic_dig_reset(void) __attribute__((noreturn));

static void esp_panic_dig_reset(void)
{
    // make sure all the panic handler output is sent from UART FIFO
    uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);
    // switch to XTAL (otherwise we will keep running from the PLL)
    rtc_clk_cpu_freq_set_xtal();
    // reset the digital part
    esp_cpu_unstall(PRO_CPU_NUM);
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_SW_SYS_RST);
    while (true) {
        ;
    }
}
#endif // CONFIG_ESP32_PANIC_PRINT_REBOOT || CONFIG_ESP32_PANIC_SILENT_REBOOT

static void putEntry(uint32_t pc, uint32_t sp)
{
    panicPutStr(" 0x");
    panicPutHex(pc);
    panicPutStr(":0x");
    panicPutHex(sp);
}

static void doBacktrace(XtExcFrame *exc_frame, int depth)
{
    //Initialize stk_frame with first frame of stack
    esp_backtrace_frame_t stk_frame = {.pc = exc_frame->pc, .sp = exc_frame->a1, .next_pc = exc_frame->a0};
    panicPutStr("\r\nBacktrace:");
    putEntry(esp_cpu_process_stack_pc(stk_frame.pc), stk_frame.sp);

    //Check if first frame is valid
    bool corrupted = (esp_stack_ptr_is_sane(stk_frame.sp) &&
                      esp_ptr_executable((void*)esp_cpu_process_stack_pc(stk_frame.pc))) ?
                      false : true;
    uint32_t i = ((depth <= 0) ? INT32_MAX : depth) - 1;    //Account for stack frame that's already printed
    while (i-- > 0 && stk_frame.next_pc != 0 && !corrupted) {
        if (!esp_backtrace_get_next_frame(&stk_frame)) {    //Get next stack frame
            corrupted = true;
        }
        putEntry(esp_cpu_process_stack_pc(stk_frame.pc), stk_frame.sp);
    }

    //Print backtrace termination marker
    if (corrupted) {
        panicPutStr(" |<-CORRUPTED");
    } else if (stk_frame.next_pc != 0) {    //Backtrace continues
        panicPutStr(" |<-CONTINUES");
    }
    panicPutStr("\r\n");
}

/*
 * Dump registers and do backtrace.
 */
static void commonErrorHandler_dump(XtExcFrame *frame, int core_id)
{
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
    if (!abort_called) {
        panicPutStr("Core");
        panicPutDec(core_id);
        panicPutStr(" register dump:\r\n");

        for (x = 0; x < 24; x += 4) {
            for (y = 0; y < 4; y++) {
                if (sdesc[x + y][0] != 0) {
                    panicPutStr(sdesc[x + y]);
                    panicPutStr(": 0x");
                    panicPutHex(regs[x + y + 1]);
                    panicPutStr("  ");
                }
            }
            panicPutStr("\r\n");
        }

        if (xPortInterruptedFromISRContext()
#if !CONFIG_FREERTOS_UNICORE
            && other_core_frame != frame
#endif //!CONFIG_FREERTOS_UNICORE
            ) {
            //If the core which triggers the interrupt watchdog was in ISR context, dump the epc registers.
            uint32_t __value;
            panicPutStr("Core");
            panicPutDec(core_id);
            panicPutStr(" was running in ISR context:\r\n");

            __asm__("rsr.epc1 %0" : "=a"(__value));
            panicPutStr("EPC1    : 0x");
            panicPutHex(__value);

            __asm__("rsr.epc2 %0" : "=a"(__value));
            panicPutStr("  EPC2    : 0x");
            panicPutHex(__value);

            __asm__("rsr.epc3 %0" : "=a"(__value));
            panicPutStr("  EPC3    : 0x");
            panicPutHex(__value);

            __asm__("rsr.epc4 %0" : "=a"(__value));
            panicPutStr("  EPC4    : 0x");
            panicPutHex(__value);

            panicPutStr("\r\n");
        }

    }

    panicPutStr("\r\nELF file SHA256: ");
    char sha256_buf[65];
    esp_ota_get_app_elf_sha256(sha256_buf, sizeof(sha256_buf));
    panicPutStr(sha256_buf);
    panicPutStr("\r\n");

    /* With windowed ABI backtracing is easy, let's do it. */
    doBacktrace(frame, 100);

    panicPutStr("\r\n");
}

/*
  We arrive here after a panic or unhandled exception, when no OCD is detected. Dump the registers to the
  serial port and either jump to the gdb stub, halt the CPU or reboot.
*/
static __attribute__((noreturn)) void commonErrorHandler(XtExcFrame *frame)
{

    int core_id = xPortGetCoreID();
    // start panic WDT to restart system if we hang in this handler
    if (!rtc_wdt_is_on()) {
        rtc_wdt_protect_off();
        rtc_wdt_disable();
        rtc_wdt_set_length_of_reset_signal(RTC_WDT_SYS_RESET_SIG, RTC_WDT_LENGTH_3_2us);
        rtc_wdt_set_length_of_reset_signal(RTC_WDT_CPU_RESET_SIG, RTC_WDT_LENGTH_3_2us);
        rtc_wdt_set_stage(RTC_WDT_STAGE0, RTC_WDT_STAGE_ACTION_RESET_SYSTEM);
        // 64KB of core dump data (stacks of about 30 tasks) will produce ~85KB base64 data.
        // @ 115200 UART speed it will take more than 6 sec to print them out.
        rtc_wdt_set_time(RTC_WDT_STAGE0, 7000);
        rtc_wdt_enable();
        rtc_wdt_protect_on();
    }

    //Feed the watchdogs, so they will give us time to print out debug info
    reconfigureAllWdts();

    commonErrorHandler_dump(frame, core_id);
#if !CONFIG_FREERTOS_UNICORE
    if (other_core_frame != NULL) {
        commonErrorHandler_dump((XtExcFrame *)other_core_frame, (core_id ? 0 : 1));
    }
#endif //!CONFIG_FREERTOS_UNICORE

#if CONFIG_ESP32_APPTRACE_ENABLE
    disableAllWdts();
#if CONFIG_SYSVIEW_ENABLE
    SEGGER_RTT_ESP32_FlushNoLock(CONFIG_ESP32_APPTRACE_POSTMORTEM_FLUSH_THRESH, APPTRACE_ONPANIC_HOST_FLUSH_TMO);
#else
    esp_apptrace_flush_nolock(ESP_APPTRACE_DEST_TRAX, CONFIG_ESP32_APPTRACE_POSTMORTEM_FLUSH_THRESH,
                              APPTRACE_ONPANIC_HOST_FLUSH_TMO);
#endif
    reconfigureAllWdts();
#endif

#if !CONFIG_ESP_PANIC_HANDLER_IRAM
    // Re-enable CPU cache for current CPU if it was disabled
    if (!spi_flash_cache_enabled()) {
        spi_flash_enable_cache(core_id);
        panicPutStr("Re-enable cpu cache.\r\n");
    }
#endif

#if CONFIG_ESP32_PANIC_GDBSTUB
    disableAllWdts();
    rtc_wdt_disable();
    panicPutStr("Entering gdb stub now.\r\n");
    esp_gdbstub_panic_handler(frame);
#else
#if CONFIG_ESP32_ENABLE_COREDUMP
    static bool s_dumping_core;
    if (s_dumping_core) {
        panicPutStr("Re-entered core dump! Exception happened during core dump!\r\n");
    } else {
        disableAllWdts();
        s_dumping_core = true;
#if CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH
        esp_core_dump_to_flash(frame);
#endif
#if CONFIG_ESP32_ENABLE_COREDUMP_TO_UART && !CONFIG_ESP32_PANIC_SILENT_REBOOT
        esp_core_dump_to_uart(frame);
#endif
        s_dumping_core = false;
        reconfigureAllWdts();
    }
#endif /* CONFIG_ESP32_ENABLE_COREDUMP */
    rtc_wdt_disable();
#if CONFIG_ESP32_PANIC_PRINT_REBOOT || CONFIG_ESP32_PANIC_SILENT_REBOOT
    panicPutStr("Rebooting...\r\n");
    if (frame->exccause != PANIC_RSN_CACHEERR) {
        esp_restart_noos();
    } else {
        // The only way to clear invalid cache access interrupt is to reset the digital part
        esp_panic_dig_reset();
    }
#else
    disableAllWdts();
    panicPutStr("CPU halted.\r\n");
    while (1);
#endif /* CONFIG_ESP32_PANIC_PRINT_REBOOT || CONFIG_ESP32_PANIC_SILENT_REBOOT */
#endif /* CONFIG_ESP32_PANIC_GDBSTUB */
}


void esp_set_breakpoint_if_jtag(void *fn)
{
    if (esp_cpu_in_ocd_debug_mode()) {
        setFirstBreakpoint((uint32_t)fn);
    }
}


esp_err_t esp_set_watchpoint(int no, void *adr, int size, int flags)
{
    int x;
    if (no < 0 || no > 1) {
        return ESP_ERR_INVALID_ARG;
    }
    if (flags & (~0xC0000000)) {
        return ESP_ERR_INVALID_ARG;
    }
    int dbreakc = 0x3F;
    //We support watching 2^n byte values, from 1 to 64. Calculate the mask for that.
    for (x = 0; x < 7; x++) {
        if (size == (1 << x)) {
            break;
        }
        dbreakc <<= 1;
    }
    if (x == 7) {
        return ESP_ERR_INVALID_ARG;
    }
    //Mask mask and add in flags.
    dbreakc = (dbreakc & 0x3f) | flags;

    if (no == 0) {
        asm volatile(
            "wsr.dbreaka0 %0\n" \
            "wsr.dbreakc0 %1\n" \
            ::"r"(adr), "r"(dbreakc));
    } else {
        asm volatile(
            "wsr.dbreaka1 %0\n" \
            "wsr.dbreakc1 %1\n" \
            ::"r"(adr), "r"(dbreakc));
    }
    return ESP_OK;
}

void esp_clear_watchpoint(int no)
{
    //Setting a dbreakc register to 0 makes it trigger on neither load nor store, effectively disabling it.
    int dbreakc = 0;
    if (no == 0) {
        asm volatile(
            "wsr.dbreakc0 %0\n" \
            ::"r"(dbreakc));
    } else {
        asm volatile(
            "wsr.dbreakc1 %0\n" \
            ::"r"(dbreakc));
    }
}

static void esp_error_check_failed_print(const char *msg, esp_err_t rc, const char *file, int line, const char *function, const char *expression)
{
    ets_printf("%s failed: esp_err_t 0x%x", msg, rc);
#ifdef CONFIG_ESP_ERR_TO_NAME_LOOKUP
    ets_printf(" (%s)", esp_err_to_name(rc));
#endif //CONFIG_ESP_ERR_TO_NAME_LOOKUP
    ets_printf(" at 0x%08x\n", (intptr_t)__builtin_return_address(0) - 3);
    if (spi_flash_cache_enabled()) { // strings may be in flash cache
        ets_printf("file: \"%s\" line %d\nfunc: %s\nexpression: %s\n", file, line, function, expression);
    }
}

void _esp_error_check_failed_without_abort(esp_err_t rc, const char *file, int line, const char *function, const char *expression)
{
    esp_error_check_failed_print("ESP_ERROR_CHECK_WITHOUT_ABORT", rc, file, line, function, expression);
}

void _esp_error_check_failed(esp_err_t rc, const char *file, int line, const char *function, const char *expression)
{
    esp_error_check_failed_print("ESP_ERROR_CHECK", rc, file, line, function, expression);
    invoke_abort();
}
