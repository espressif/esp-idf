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

#include "esp_err.h"
#include "esp_attr.h"

#include "esp_spi_flash.h"
#include "esp_private/system_internal.h"
#include "esp_private/gdbstub.h"
#include "esp_ota_ops.h"

#if CONFIG_APPTRACE_ENABLE
#include "esp_app_trace.h"
#if CONFIG_SYSVIEW_ENABLE
#include "SEGGER_RTT.h"
#endif
#endif // CONFIG_APPTRACE_ENABLE

#include "esp_core_dump.h"

#include "soc/cpu.h"
#include "soc/rtc.h"
#include "hal/timer_hal.h"
#include "hal/cpu_hal.h"
#include "hal/wdt_types.h"
#include "hal/wdt_hal.h"

#if !CONFIG_ESP_SYSTEM_PANIC_SILENT_REBOOT
#include <string.h>
#include "hal/uart_hal.h"
#endif

#include "esp_private/panic_internal.h"
#include "port/panic_funcs.h"

#include "sdkconfig.h"

#if CONFIG_APPTRACE_ONPANIC_HOST_FLUSH_TMO == -1
#define APPTRACE_ONPANIC_HOST_FLUSH_TMO   ESP_APPTRACE_TMO_INFINITE
#else
#define APPTRACE_ONPANIC_HOST_FLUSH_TMO   (1000*CONFIG_APPTRACE_ONPANIC_HOST_FLUSH_TMO)
#endif

bool g_panic_abort = false;
static char *s_panic_abort_details = NULL;

static wdt_hal_context_t rtc_wdt_ctx = {.inst = WDT_RWDT, .rwdt_dev = &RTCCNTL};

#if !CONFIG_ESP_SYSTEM_PANIC_SILENT_REBOOT

static uart_hal_context_t s_panic_uart = { .dev = CONFIG_ESP_CONSOLE_UART_NUM == 0 ? &UART0 : &UART1 };

void panic_print_char(const char c)
{
    uint32_t sz = 0;
    while(!uart_hal_get_txfifo_len(&s_panic_uart));
    uart_hal_write_txfifo(&s_panic_uart, (uint8_t*) &c, 1, &sz);
}

void panic_print_str(const char *str)
{
    for(int i = 0; str[i] != 0; i++) {
        panic_print_char(str[i]);
    }
}

void panic_print_hex(int h)
{
    int x;
    int c;
    // Does not print '0x', only the digits (8 digits to print)
    for (x = 0; x < 8; x++) {
        c = (h >> 28) & 0xf; // extract the leftmost byte
        if (c < 10) {
            panic_print_char('0' + c);
        } else {
            panic_print_char('a' + c - 10);
        }
        h <<= 4; // move the 2nd leftmost byte to the left, to be extracted next
    }
}

void panic_print_dec(int d)
{
    // can print at most 2 digits!
    int n1, n2;
    n1 = d % 10; // extract ones digit
    n2 = d / 10; // extract tens digit
    if (n2 == 0) {
        panic_print_char(' ');
    } else {
        panic_print_char(n2 + '0');
    }
    panic_print_char(n1 + '0');
}
#endif  // CONFIG_ESP_SYSTEM_PANIC_SILENT_REBOOT

/*
  If watchdogs are enabled, the panic handler runs the risk of getting aborted pre-emptively because
  an overzealous watchdog decides to reset it. On the other hand, if we disable all watchdogs, we run
  the risk of somehow halting in the panic handler and not resetting. That is why this routine kills
  all watchdogs except the timer group 0 watchdog, and it reconfigures that to reset the chip after
  one second.

  We have to do this before we do anything that might cause issues in the WDT interrupt handlers,
  for example stalling the other core on ESP32 may cause the ESP32_ECO3_CACHE_LOCK_FIX
  handler to get stuck.
*/
void esp_panic_handler_reconfigure_wdts(void)
{
    wdt_hal_context_t wdt0_context = {.inst = WDT_MWDT0, .mwdt_dev = &TIMERG0};
    wdt_hal_context_t wdt1_context = {.inst = WDT_MWDT1, .mwdt_dev = &TIMERG1};

    //Todo: Refactor to use Interrupt or Task Watchdog API, and a system level WDT context
    //Reconfigure TWDT (Timer Group 0)
    wdt_hal_init(&wdt0_context, WDT_MWDT0, MWDT0_TICK_PRESCALER, false); //Prescaler: wdt counts in ticks of TG0_WDT_TICK_US
    wdt_hal_write_protect_disable(&wdt0_context);
    wdt_hal_config_stage(&wdt0_context, 0, 1000*1000/MWDT0_TICKS_PER_US, WDT_STAGE_ACTION_RESET_SYSTEM);   //1 second before reset
    wdt_hal_enable(&wdt0_context);
    wdt_hal_write_protect_enable(&wdt0_context);

    //Disable IWDT (Timer Group 1)
    wdt_hal_write_protect_disable(&wdt1_context);
    wdt_hal_disable(&wdt1_context);
    wdt_hal_write_protect_enable(&wdt1_context);
}

/*
  This disables all the watchdogs for when we call the gdbstub.
*/
static inline void disable_all_wdts(void)
{
    wdt_hal_context_t wdt0_context = {.inst = WDT_MWDT0, .mwdt_dev = &TIMERG0};
    wdt_hal_context_t wdt1_context = {.inst = WDT_MWDT1, .mwdt_dev = &TIMERG1};

    //Todo: Refactor to use Interrupt or Task Watchdog API, and a system level WDT context
    //Task WDT is the Main Watchdog Timer of Timer Group 0
    wdt_hal_write_protect_disable(&wdt0_context);
    wdt_hal_disable(&wdt0_context);
    wdt_hal_write_protect_enable(&wdt0_context);

    //Interupt WDT is the Main Watchdog Timer of Timer Group 1
    wdt_hal_write_protect_disable(&wdt1_context);
    wdt_hal_disable(&wdt1_context);
    wdt_hal_write_protect_enable(&wdt1_context);
}

static void print_abort_details(const void *f)
{
    panic_print_str(s_panic_abort_details);
}

// Control arrives from chip-specific panic handler, environment prepared for
// the 'main' logic of panic handling. This means that chip-specific stuff have
// already been done, and panic_info_t has been filled.
void esp_panic_handler(panic_info_t *info)
{
    // The port-level panic handler has already called this, but call it again
    // to reset the TG0WDT period
    esp_panic_handler_reconfigure_wdts();

    // If the exception was due to an abort, override some of the panic info
    if (g_panic_abort) {
        info->description = NULL;
        info->details = s_panic_abort_details ? print_abort_details : NULL;
        info->reason = NULL;
        info->exception = PANIC_EXCEPTION_ABORT;
    }

    /*
     * For any supported chip, the panic handler prints the contents of panic_info_t in the following format:
     *
     *
     * Guru Meditation Error: Core <core> (<exception>). <description>
     * <details>
     *
     * <state>
     *
     * <elf_info>
     *
     *
     * ----------------------------------------------------------------------------------------
     * core - core where exception was triggered
     * exception - what kind of exception occured
     * description - a short description regarding the exception that occured
     * details - more details about the exception
     * state - processor state like register contents, and backtrace
     * elf_info - details about the image currently running
     *
     * NULL fields in panic_info_t are not printed.
     *
     * */
    if (info->reason) {
        panic_print_str("Guru Meditation Error: Core ");
        panic_print_dec(info->core);
        panic_print_str(" panic'ed (");
        panic_print_str(info->reason);
        panic_print_str("). ");
    }

    if (info->description) {
        panic_print_str(info->description);
    }

    panic_print_str("\r\n");

    PANIC_INFO_DUMP(info, details);

    panic_print_str("\r\n");

    // If on-chip-debugger is attached, and system is configured to be aware of this,
    // then only print up to details. Users should be able to probe for the other information
    // in debug mode.
    if (esp_cpu_in_ocd_debug_mode()) {
        panic_print_str("Setting breakpoint at 0x");
        panic_print_hex((uint32_t)info->addr);
        panic_print_str(" and returning...\r\n");
        disable_all_wdts();
#if CONFIG_APPTRACE_ENABLE
#if CONFIG_SYSVIEW_ENABLE
        SEGGER_RTT_ESP32_FlushNoLock(CONFIG_APPTRACE_POSTMORTEM_FLUSH_THRESH, APPTRACE_ONPANIC_HOST_FLUSH_TMO);
#else
        esp_apptrace_flush_nolock(ESP_APPTRACE_DEST_TRAX, CONFIG_APPTRACE_POSTMORTEM_FLUSH_THRESH,
                                  APPTRACE_ONPANIC_HOST_FLUSH_TMO);
#endif
#endif

        cpu_hal_set_breakpoint(0, info->addr); // use breakpoint 0
        return;
    }

    // start panic WDT to restart system if we hang in this handler
    if (!wdt_hal_is_enabled(&rtc_wdt_ctx)) {
        wdt_hal_init(&rtc_wdt_ctx, WDT_RWDT, 0, false);
        uint32_t stage_timeout_ticks = (uint32_t)(7000ULL * rtc_clk_slow_freq_get_hz() / 1000ULL);
        wdt_hal_write_protect_disable(&rtc_wdt_ctx);
        wdt_hal_config_stage(&rtc_wdt_ctx, WDT_STAGE0, stage_timeout_ticks, WDT_STAGE_ACTION_RESET_SYSTEM);
        // 64KB of core dump data (stacks of about 30 tasks) will produce ~85KB base64 data.
        // @ 115200 UART speed it will take more than 6 sec to print them out.
        wdt_hal_enable(&rtc_wdt_ctx);
        wdt_hal_write_protect_enable(&rtc_wdt_ctx);

    }

    esp_panic_handler_reconfigure_wdts(); // Restart WDT again

    PANIC_INFO_DUMP(info, state);
    panic_print_str("\r\n");

    panic_print_str("\r\nELF file SHA256: ");
    char sha256_buf[65];
    esp_ota_get_app_elf_sha256(sha256_buf, sizeof(sha256_buf));
    panic_print_str(sha256_buf);
    panic_print_str("\r\n");

    panic_print_str("\r\n");

#if CONFIG_APPTRACE_ENABLE
    disable_all_wdts();
#if CONFIG_SYSVIEW_ENABLE
    SEGGER_RTT_ESP32_FlushNoLock(CONFIG_APPTRACE_POSTMORTEM_FLUSH_THRESH, APPTRACE_ONPANIC_HOST_FLUSH_TMO);
#else
    esp_apptrace_flush_nolock(ESP_APPTRACE_DEST_TRAX, CONFIG_APPTRACE_POSTMORTEM_FLUSH_THRESH,
                              APPTRACE_ONPANIC_HOST_FLUSH_TMO);
#endif
    esp_panic_handler_reconfigure_wdts(); // restore WDT config
#endif // CONFIG_APPTRACE_ENABLE

#if CONFIG_ESP_SYSTEM_PANIC_GDBSTUB
    disable_all_wdts();
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_disable(&rtc_wdt_ctx);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);
    panic_print_str("Entering gdb stub now.\r\n");
    esp_gdbstub_panic_handler((XtExcFrame*) info->frame);
#else
#if CONFIG_ESP32_ENABLE_COREDUMP
    static bool s_dumping_core;
    if (s_dumping_core) {
        panic_print_str("Re-entered core dump! Exception happened during core dump!\r\n");
    } else {
        disable_all_wdts();
        s_dumping_core = true;
#if CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH
        esp_core_dump_to_flash(info);
#endif
#if CONFIG_ESP32_ENABLE_COREDUMP_TO_UART && !CONFIG_ESP_SYSTEM_PANIC_SILENT_REBOOT
        esp_core_dump_to_uart(info);
#endif
        s_dumping_core = false;

        esp_panic_handler_reconfigure_wdts();
    }
#endif /* CONFIG_ESP32_ENABLE_COREDUMP */
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_disable(&rtc_wdt_ctx);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);
#if CONFIG_ESP_SYSTEM_PANIC_PRINT_REBOOT || CONFIG_ESP_SYSTEM_PANIC_SILENT_REBOOT

    if (esp_reset_reason_get_hint() == ESP_RST_UNKNOWN) {
        switch (info->exception)
        {
        case PANIC_EXCEPTION_IWDT:
            esp_reset_reason_set_hint(ESP_RST_INT_WDT);
            break;
        case PANIC_EXCEPTION_TWDT:
            esp_reset_reason_set_hint(ESP_RST_TASK_WDT);
            break;
        case PANIC_EXCEPTION_ABORT:
        case PANIC_EXCEPTION_FAULT:
        default:
            esp_reset_reason_set_hint(ESP_RST_PANIC);
            break; // do not touch the previously set reset reason hint
        }
    }

    panic_print_str("Rebooting...\r\n");
    panic_restart();
#else
    disable_all_wdts();
    panic_print_str("CPU halted.\r\n");
    while (1);
#endif /* CONFIG_ESP_SYSTEM_PANIC_PRINT_REBOOT || CONFIG_ESP_SYSTEM_PANIC_SILENT_REBOOT */
#endif /* CONFIG_ESP_SYSTEM_PANIC_GDBSTUB */
}

void __attribute__((noreturn)) panic_abort(const char *details)
{
    g_panic_abort = true;
    s_panic_abort_details = (char*) details;

#if CONFIG_APPTRACE_ENABLE
#if CONFIG_SYSVIEW_ENABLE
    SEGGER_RTT_ESP32_FlushNoLock(CONFIG_APPTRACE_POSTMORTEM_FLUSH_THRESH, APPTRACE_ONPANIC_HOST_FLUSH_TMO);
#else
    esp_apptrace_flush_nolock(ESP_APPTRACE_DEST_TRAX, CONFIG_APPTRACE_POSTMORTEM_FLUSH_THRESH,
                              APPTRACE_ONPANIC_HOST_FLUSH_TMO);
#endif
#endif

    *((int *) 0) = 0; // NOLINT(clang-analyzer-core.NullDereference) should be an invalid operation on targets
    while(1);
}

/* Weak versions of reset reason hint functions.
 * If these weren't provided, reset reason code would be linked into the app
 * even if the app never called esp_reset_reason().
 */
void IRAM_ATTR __attribute__((weak)) esp_reset_reason_set_hint(esp_reset_reason_t hint)
{
}

esp_reset_reason_t IRAM_ATTR  __attribute__((weak)) esp_reset_reason_get_hint(void)
{
    return ESP_RST_UNKNOWN;
}
