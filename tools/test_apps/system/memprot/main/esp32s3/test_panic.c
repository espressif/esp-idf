/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "xtensa_context.h"
#include "esp_private/panic_internal.h"
#include "hal/wdt_hal.h"

extern void esp_panic_handler(panic_info_t *info);
extern volatile bool g_override_illegal_instruction;

void __real_esp_panic_handler(panic_info_t *info);
void __real_esp_panic_handler_reconfigure_wdts(void);
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
    //Interupt WDT is the Main Watchdog Timer of Timer Group 1
    wdt_hal_write_protect_disable(&wdt1_context);
    wdt_hal_disable(&wdt1_context);
    wdt_hal_write_protect_enable(&wdt1_context);
#endif
}

/* Memprot test specific IllegalInstruction exception handler:
 * when testing the protection against a code execution, sample code
 * is being injected into various memory regions which produces
 * EXCCAUSE_ILLEGAL on execution attempt. Such a result is expected
 * but it causes system reboot in the standard panic handler.
 * The following variant of panic handling simply returns back to the
 * next instruction and continues normal execution.
 *
 * NOTE: if EXCCAUSE_ILLEGAL comes from a different source than the testing code
 * the behavior is undefined
 * */
void __wrap_esp_panic_handler(panic_info_t *info)
{
    XtExcFrame *frm = (XtExcFrame *)info->frame;
    if ( frm->exccause == EXCCAUSE_ILLEGAL && g_override_illegal_instruction == true ) {
        frm->pc = frm->a0;
        return;
    } else {
        __real_esp_panic_handler(info);
    }
}

void __wrap_esp_panic_handler_reconfigure_wdts(void)
{
    if ( g_override_illegal_instruction == true ) {
        disable_all_wdts();
        return;
    } else {
        __real_esp_panic_handler_reconfigure_wdts();
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
