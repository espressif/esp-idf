/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "riscv/rvruntime-frames.h"
#include "esp_private/panic_internal.h"
#include "esp_private/panic_reason.h"
#include "hal/wdt_hal.h"


extern void esp_panic_handler(panic_info_t *info);
volatile bool g_override_illegal_instruction = false;

void __real_esp_panic_handler(panic_info_t *info);
void __real_esp_panic_handler_feed_wdts(void);
void __real_esp_panic_handler_enable_rtc_wdt(uint32_t timeout_ms);
void __real_esp_panic_handler_increment_entry_count(void);
void __real_esp_cpu_stall(int core_id);

static void disable_all_wdts(void)
{
    wdt_hal_context_t wdt0_context = {.inst = WDT_MWDT0, .mwdt_dev = &TIMERG0};
#if SOC_TIMER_GROUPS >= 2
    wdt_hal_context_t wdt1_context = {.inst = WDT_MWDT1, .mwdt_dev = &TIMERG1};
#endif

    //Todo: Refactor to use Interrupt or Task Watchdog API, and a system level WDT context
    //Task WDT is the Main Watchdog Timer of Timer Group 0
    wdt_hal_write_protect_disable(&wdt0_context);
    wdt_hal_disable(&wdt0_context);
    wdt_hal_write_protect_enable(&wdt0_context);

#if SOC_TIMER_GROUPS >= 2
    //Interrupt WDT is the Main Watchdog Timer of Timer Group 1
    wdt_hal_write_protect_disable(&wdt1_context);
    wdt_hal_disable(&wdt1_context);
    wdt_hal_write_protect_enable(&wdt1_context);
#endif

    //Disable RTC WDT
    wdt_hal_context_t rtc_wdt_ctx = RWDT_HAL_CONTEXT_DEFAULT();
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_disable(&rtc_wdt_ctx);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);
}

void return_from_panic_handler(RvExcFrame *frm) __attribute__((noreturn));

/* Memprot test specific IllegalInstruction exception handler:
 * when testing the protection against a code execution, sample code
 * is being injected into various memory regions which produces
 * Illegal instruction on execution attempt. Such a result is expected
 * but it causes system reboot in the standard panic handler.
 * The following variant of panic handling simply returns back to the
 * next instruction and continues normal execution.
 *
 * NOTE: if Illegal instruction comes from a different source than the testing code
 * the behavior is undefined
 * */
void __wrap_esp_panic_handler(panic_info_t *info)
{
    RvExcFrame *frm = (RvExcFrame *)info->frame;
    if ( frm->mcause == MCAUSE_ILLEGAL_INSTRUCTION && g_override_illegal_instruction == true ) {
        /* Return from exception to the return address that called the faulting function.
        */
        frm->mepc = frm->ra;

        /* Restore the CPU state and return from the exception.
        */
        return_from_panic_handler(frm);
    } else {
        __real_esp_panic_handler(info);
    }
}

void __wrap_esp_panic_handler_feed_wdts(void)
{
    if ( g_override_illegal_instruction == true ) {
        disable_all_wdts();
        return;
    } else {
        __real_esp_panic_handler_feed_wdts();
    }
}

void __wrap_esp_panic_handler_enable_rtc_wdt(uint32_t timeout_ms)
{
    if ( g_override_illegal_instruction == true ) {
        return;
    } else {
        __real_esp_panic_handler_enable_rtc_wdt(timeout_ms);
    }
}

void __wrap_esp_panic_handler_increment_entry_count(void)
{
    if ( g_override_illegal_instruction == true ) {
        return;
    } else {
        __real_esp_panic_handler_increment_entry_count();
    }
}

void __wrap_esp_cpu_stall(int core_id)
{
    if ( g_override_illegal_instruction == true ) {
        return;
    } else {
        __real_esp_cpu_stall(core_id);
    }
}
