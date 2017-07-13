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

#include "rom/rtc.h"
#include "rom/uart.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/xtensa_api.h"

#include "soc/uart_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/dport_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "soc/cpu.h"
#include "soc/rtc.h"

#include "esp_gdbstub.h"
#include "esp_panic.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_core_dump.h"
#include "esp_spi_flash.h"
#include "esp_cache_err_int.h"
#include "esp_app_trace.h"
#include "esp_system.h"
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
    while (((READ_PERI_REG(UART_STATUS_REG(CONFIG_CONSOLE_UART_NUM)) >> UART_TXFIFO_CNT_S)&UART_TXFIFO_CNT) >= 126) ;
    WRITE_PERI_REG(UART_FIFO_REG(CONFIG_CONSOLE_UART_NUM), c);
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

static bool abort_called;

static __attribute__((noreturn)) inline void invoke_abort()
{
    abort_called = true;
#if CONFIG_ESP32_APPTRACE_ENABLE
#if CONFIG_SYSVIEW_ENABLE
    SEGGER_RTT_ESP32_FlushNoLock(CONFIG_ESP32_APPTRACE_POSTMORTEM_FLUSH_TRAX_THRESH, APPTRACE_ONPANIC_HOST_FLUSH_TMO);
#else
    esp_apptrace_flush_nolock(ESP_APPTRACE_DEST_TRAX, CONFIG_ESP32_APPTRACE_POSTMORTEM_FLUSH_TRAX_THRESH,
                            APPTRACE_ONPANIC_HOST_FLUSH_TMO);
#endif
#endif
    while(1) {
        if (esp_cpu_in_ocd_debug_mode()) {
            __asm__ ("break 0,0");
        }
        *((int*) 0) = 0;
    }
}

void abort()
{
#if !CONFIG_ESP32_PANIC_SILENT_REBOOT
    ets_printf("abort() was called at PC 0x%08x on core %d\r\n", (intptr_t)__builtin_return_address(0) - 3, xPortGetCoreID());
#endif
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

//The fact that we've panic'ed probably means the other CPU is now running wild, possibly
//messing up the serial output, so we stall it here.
static void haltOtherCore()
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
    if (frame->exccause == PANIC_RSN_CACHEERR && esp_cache_err_get_cpuid() != core_id) {
        // Cache error interrupt will be handled by the panic handler
        // on the other CPU.
        return;
    }
    haltOtherCore();
    esp_dport_access_int_deinit();
    panicPutStr("Guru Meditation Error: Core ");
    panicPutDec(core_id);
    panicPutStr(" panic'ed (");
    panicPutStr(reason);
    panicPutStr(")\r\n");
    if (frame->exccause == PANIC_RSN_DEBUGEXCEPTION) {
        int debugRsn;
        asm("rsr.debugcause %0":"=r"(debugRsn));
        panicPutStr("Debug exception reason: ");
        if (debugRsn&XCHAL_DEBUGCAUSE_ICOUNT_MASK) panicPutStr("SingleStep ");
        if (debugRsn&XCHAL_DEBUGCAUSE_IBREAK_MASK) panicPutStr("HwBreakpoint ");
        if (debugRsn&XCHAL_DEBUGCAUSE_DBREAK_MASK) {
            //Unlike what the ISA manual says, this core seemingly distinguishes from a DBREAK
            //reason caused by watchdog 0 and one caused by watchdog 1 by setting bit 8 of the
            //debugcause if the cause is watchdog 1 and clearing it if it's watchdog 0.
            if (debugRsn&(1<<8)) {
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
        if (debugRsn&XCHAL_DEBUGCAUSE_BREAK_MASK) panicPutStr("BREAK instr ");
        if (debugRsn&XCHAL_DEBUGCAUSE_BREAKN_MASK) panicPutStr("BREAKN instr ");
        if (debugRsn&XCHAL_DEBUGCAUSE_DEBUGINT_MASK) panicPutStr("DebugIntr ");
        panicPutStr("\r\n");
    }

    if (esp_cpu_in_ocd_debug_mode()) {
#if CONFIG_ESP32_APPTRACE_ENABLE
#if CONFIG_SYSVIEW_ENABLE
        SEGGER_RTT_ESP32_FlushNoLock(CONFIG_ESP32_APPTRACE_POSTMORTEM_FLUSH_TRAX_THRESH, APPTRACE_ONPANIC_HOST_FLUSH_TMO);
#else
        esp_apptrace_flush_nolock(ESP_APPTRACE_DEST_TRAX, CONFIG_ESP32_APPTRACE_POSTMORTEM_FLUSH_TRAX_THRESH,
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
    esp_dport_access_int_deinit();
    if (!abort_called) {
        panicPutStr("Guru Meditation Error of type ");
        int exccause = frame->exccause;
        if (exccause < NUM_EDESCS) {
            panicPutStr(edesc[exccause]);
        } else {
            panicPutStr("Unknown");
        }
        panicPutStr(" occurred on core ");
        panicPutDec(xPortGetCoreID());
        if (esp_cpu_in_ocd_debug_mode()) {
            panicPutStr(" at pc=");
            panicPutHex(frame->pc);
            panicPutStr(". Setting bp and returning..\r\n");
#if CONFIG_ESP32_APPTRACE_ENABLE
#if CONFIG_SYSVIEW_ENABLE
            SEGGER_RTT_ESP32_FlushNoLock(CONFIG_ESP32_APPTRACE_POSTMORTEM_FLUSH_TRAX_THRESH, APPTRACE_ONPANIC_HOST_FLUSH_TMO);
#else
            esp_apptrace_flush_nolock(ESP_APPTRACE_DEST_TRAX, CONFIG_ESP32_APPTRACE_POSTMORTEM_FLUSH_TRAX_THRESH,
                                    APPTRACE_ONPANIC_HOST_FLUSH_TMO);
#endif
#endif
            //Stick a hardware breakpoint on the address the handler returns to. This way, the OCD debugger
            //will kick in exactly at the context the error happened.
            setFirstBreakpoint(frame->pc);
            return;
        }
        panicPutStr(". Exception was unhandled.\r\n");
    }
    commonErrorHandler(frame);
}


/*
  If watchdogs are enabled, the panic handler runs the risk of getting aborted pre-emptively because
  an overzealous watchdog decides to reset it. On the other hand, if we disable all watchdogs, we run
  the risk of somehow halting in the panic handler and not resetting. That is why this routine kills
  all watchdogs except the timer group 0 watchdog, and it reconfigures that to reset the chip after
  one second.
*/
static void reconfigureAllWdts()
{
    TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed = 1;
    TIMERG0.wdt_config0.sys_reset_length = 7;           //3.2uS
    TIMERG0.wdt_config0.cpu_reset_length = 7;           //3.2uS
    TIMERG0.wdt_config0.stg0 = TIMG_WDT_STG_SEL_RESET_SYSTEM; //1st stage timeout: reset system
    TIMERG0.wdt_config1.clk_prescale = 80 * 500;        //Prescaler: wdt counts in ticks of 0.5mS
    TIMERG0.wdt_config2 = 2000;                         //1 second before reset
    TIMERG0.wdt_config0.en = 1;
    TIMERG0.wdt_wprotect = 0;
    //Disable wdt 1
    TIMERG1.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
    TIMERG1.wdt_config0.en = 0;
    TIMERG1.wdt_wprotect = 0;
}

/*
  This disables all the watchdogs for when we call the gdbstub.
*/
static inline void disableAllWdts()
{
    TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_config0.en = 0;
    TIMERG0.wdt_wprotect = 0;
    TIMERG1.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
    TIMERG1.wdt_config0.en = 0;
    TIMERG1.wdt_wprotect = 0;
}

static void esp_panic_wdt_start()
{
    if (REG_GET_BIT(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_EN)) {
        return;
    }
    WRITE_PERI_REG(RTC_CNTL_WDTWPROTECT_REG, RTC_CNTL_WDT_WKEY_VALUE);
    WRITE_PERI_REG(RTC_CNTL_WDTFEED_REG, 1);
    REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_SYS_RESET_LENGTH, 7);
    REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_CPU_RESET_LENGTH, 7);
    REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_STG0, RTC_WDT_STG_SEL_RESET_SYSTEM);
    // 64KB of core dump data (stacks of about 30 tasks) will produce ~85KB base64 data.
    // @ 115200 UART speed it will take more than 6 sec to print them out.
    WRITE_PERI_REG(RTC_CNTL_WDTCONFIG1_REG, rtc_clk_slow_freq_get_hz() * 7);
    REG_SET_BIT(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_EN);
    WRITE_PERI_REG(RTC_CNTL_WDTWPROTECT_REG, 0);
}

void esp_panic_wdt_stop()
{
    WRITE_PERI_REG(RTC_CNTL_WDTWPROTECT_REG, RTC_CNTL_WDT_WKEY_VALUE);
    WRITE_PERI_REG(RTC_CNTL_WDTFEED_REG, 1);
    REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_STG0, RTC_WDT_STG_SEL_OFF);
    REG_CLR_BIT(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_EN);
    WRITE_PERI_REG(RTC_CNTL_WDTWPROTECT_REG, 0);
}

static void esp_panic_dig_reset() __attribute__((noreturn));

static void esp_panic_dig_reset()
{
    // make sure all the panic handler output is sent from UART FIFO
    uart_tx_wait_idle(CONFIG_CONSOLE_UART_NUM);
    // switch to XTAL (otherwise we will keep running from the PLL)
    rtc_clk_cpu_freq_set(RTC_CPU_FREQ_XTAL);
    // reset the digital part
    esp_cpu_unstall(PRO_CPU_NUM);
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_SW_SYS_RST);
    while (true) {
        ;
    }
}

static void putEntry(uint32_t pc, uint32_t sp)
{
    if (pc & 0x80000000) {
        pc = (pc & 0x3fffffff) | 0x40000000;
    }
    panicPutStr(" 0x");
    panicPutHex(pc);
    panicPutStr(":0x");
    panicPutHex(sp);
}

static void doBacktrace(XtExcFrame *frame)
{
    uint32_t i = 0, pc = frame->pc, sp = frame->a1;
    panicPutStr("\r\nBacktrace:");
    /* Do not check sanity on first entry, PC could be smashed. */
    putEntry(pc, sp);
    pc = frame->a0;
    while (i++ < 100) {
        uint32_t psp = sp;
        if (!esp_stack_ptr_is_sane(sp) || i++ > 100) {
            break;
        }
        sp = *((uint32_t *) (sp - 0x10 + 4));
        putEntry(pc - 3, sp); // stack frame addresses are return addresses, so subtract 3 to get the CALL address
        pc = *((uint32_t *) (psp - 0x10));
        if (pc < 0x40000000) {
            break;
        }
    }
    panicPutStr("\r\n\r\n");
}

/*
  We arrive here after a panic or unhandled exception, when no OCD is detected. Dump the registers to the
  serial port and either jump to the gdb stub, halt the CPU or reboot.
*/
static __attribute__((noreturn)) void commonErrorHandler(XtExcFrame *frame)
{
    int *regs = (int *)frame;
    int x, y;
    const char *sdesc[] = {
        "PC      ", "PS      ", "A0      ", "A1      ", "A2      ", "A3      ", "A4      ", "A5      ",
        "A6      ", "A7      ", "A8      ", "A9      ", "A10     ", "A11     ", "A12     ", "A13     ",
        "A14     ", "A15     ", "SAR     ", "EXCCAUSE", "EXCVADDR", "LBEG    ", "LEND    ", "LCOUNT  "
    };

    // start panic WDT to restart system if we hang in this handler
    esp_panic_wdt_start();

    //Feed the watchdogs, so they will give us time to print out debug info
    reconfigureAllWdts();

    /* only dump registers for 'real' crashes, if crashing via abort()
       the register window is no longer useful.
    */
    if (!abort_called) {
        panicPutStr("Register dump:\r\n");

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
    }

    /* With windowed ABI backtracing is easy, let's do it. */
    doBacktrace(frame);

#if CONFIG_ESP32_APPTRACE_ENABLE
    disableAllWdts();
#if CONFIG_SYSVIEW_ENABLE
    SEGGER_RTT_ESP32_FlushNoLock(CONFIG_ESP32_APPTRACE_POSTMORTEM_FLUSH_TRAX_THRESH, APPTRACE_ONPANIC_HOST_FLUSH_TMO);
#else
    esp_apptrace_flush_nolock(ESP_APPTRACE_DEST_TRAX, CONFIG_ESP32_APPTRACE_POSTMORTEM_FLUSH_TRAX_THRESH,
                            APPTRACE_ONPANIC_HOST_FLUSH_TMO);
#endif
    reconfigureAllWdts();
#endif

#if CONFIG_ESP32_PANIC_GDBSTUB
    disableAllWdts();
    esp_panic_wdt_stop();
    panicPutStr("Entering gdb stub now.\r\n");
    esp_gdbstub_panic_handler(frame);
#else
#if CONFIG_ESP32_ENABLE_COREDUMP
    disableAllWdts();
#if CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH
    esp_core_dump_to_flash(frame);
#endif
#if CONFIG_ESP32_ENABLE_COREDUMP_TO_UART && !CONFIG_ESP32_PANIC_SILENT_REBOOT
    esp_core_dump_to_uart(frame);
#endif
    reconfigureAllWdts();
#endif /* CONFIG_ESP32_ENABLE_COREDUMP */
    esp_panic_wdt_stop();
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
    if (no<0 || no>1) return ESP_ERR_INVALID_ARG;
    if (flags&(~0xC0000000)) return ESP_ERR_INVALID_ARG;
    int dbreakc=0x3F;
    //We support watching 2^n byte values, from 1 to 64. Calculate the mask for that.
    for (x=0; x<7; x++) {
        if (size==(1<<x)) break;
        dbreakc<<=1;
    }
    if (x==7) return ESP_ERR_INVALID_ARG;
    //Mask mask and add in flags.
    dbreakc=(dbreakc&0x3f)|flags;

    if (no==0) {
        asm volatile(
            "wsr.dbreaka0 %0\n" \
            "wsr.dbreakc0 %1\n" \
            ::"r"(adr),"r"(dbreakc));
    } else {
        asm volatile(
            "wsr.dbreaka1 %0\n" \
            "wsr.dbreakc1 %1\n" \
            ::"r"(adr),"r"(dbreakc));
    }
    return ESP_OK;
}

void esp_clear_watchpoint(int no)
{
    //Setting a dbreakc register to 0 makes it trigger on neither load nor store, effectively disabling it.
    int dbreakc=0;
    if (no==0) {
        asm volatile(
            "wsr.dbreakc0 %0\n" \
            ::"r"(dbreakc));
    } else {
        asm volatile(
            "wsr.dbreakc1 %0\n" \
            ::"r"(dbreakc));
    }
}

void _esp_error_check_failed(esp_err_t rc, const char *file, int line, const char *function, const char *expression)
{
    ets_printf("ESP_ERROR_CHECK failed: esp_err_t 0x%x at 0x%08x\n", rc, (intptr_t)__builtin_return_address(0) - 3);
    if (spi_flash_cache_enabled()) { // strings may be in flash cache
        ets_printf("file: \"%s\" line %d\nfunc: %s\nexpression: %s\n", file, line, function, expression);
    }
    invoke_abort();
}
