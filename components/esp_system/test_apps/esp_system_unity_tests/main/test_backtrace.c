/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 * Note: Currently, the backtraces must still be checked manually. Therefore,
 * these test cases should always pass.
 * Todo: Automate the checking of backtrace addresses.
 */
#include <stdlib.h>
#include "unity.h"
#include "test_utils.h"

#if __XTENSA__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "xtensa_api.h" // Replace with interrupt allocator API (IDF-3891)
#include "esp_debug_helpers.h"
#include "esp_intr_alloc.h"
#include "esp_rom_sys.h"
#include "esp_rom_uart.h"
#include "hal/misc.h"

#define SW_ISR_LEVEL_1          7
#define SW_ISR_LEVEL_3          29
#define RECUR_DEPTH             3
#define ACTION_ABORT            -1
#define ACTION_INT_WDT          -2
#define TASK_PRIORITY           5

// Set to (-1) for abort(), (-2) for interrupt watchdog
static int backtrace_trigger_source;

/*
 * Recursive functions to generate a longer call stack. When the max specified
 * recursion depth is reached, the following actions can be taken.
 */
static void __attribute__((__noinline__)) recursive_func(int recur_depth, int action)
{
    if (recur_depth > 1) {
        recursive_func(recur_depth - 1, action);
    } else if (action >= 0) {
        xt_set_intset(1 << action);
    } else if (action == ACTION_ABORT) {
        abort();
        // Todo: abort() causes problems in GDB Stub backtrace due to being 'non returning'.
    } else if (action == ACTION_INT_WDT) {
        portDISABLE_INTERRUPTS();
        while (1) {
            ;
        }
    }
}

static void level_three_isr(void *arg)
{
    xt_set_intclear(1 << SW_ISR_LEVEL_3);                           //Clear interrupt
    recursive_func(RECUR_DEPTH, backtrace_trigger_source);         //Abort at the max recursive depth
}

static void level_one_isr(void *arg)
{
    xt_set_intclear(1 << SW_ISR_LEVEL_1);           //Clear interrupt
    recursive_func(RECUR_DEPTH, SW_ISR_LEVEL_3);    //Trigger nested interrupt max recursive depth
}

static void do_abort(void)
{
    //Allocate level one and three SW interrupts
    esp_intr_alloc(ETS_INTERNAL_SW0_INTR_SOURCE, 0, level_one_isr, NULL, NULL);     //Level 1 SW intr
    esp_intr_alloc(ETS_INTERNAL_SW1_INTR_SOURCE, 0, level_three_isr, NULL, NULL);   //Level 3 SW intr
    backtrace_trigger_source = ACTION_ABORT;
    recursive_func(RECUR_DEPTH, SW_ISR_LEVEL_1);    //Trigger lvl 1 SW interrupt at max recursive depth
}

static void check_reset_reason_panic(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_PANIC, esp_reset_reason());
}

TEST_CASE_MULTIPLE_STAGES("Test backtrace from abort", "[reset_reason][reset=abort,SW_CPU_RESET]",
                          do_abort,
                          check_reset_reason_panic)

static void do_wdt_timeout(void)
{
    //Allocate level one and three SW interrupts
    esp_intr_alloc(ETS_INTERNAL_SW0_INTR_SOURCE, 0, level_one_isr, NULL, NULL);     //Level 1 SW intr
    esp_intr_alloc(ETS_INTERNAL_SW1_INTR_SOURCE, 0, level_three_isr, NULL, NULL);   //Level 3 SW intr
    backtrace_trigger_source = ACTION_INT_WDT;
    recursive_func(RECUR_DEPTH, SW_ISR_LEVEL_1);    //Trigger lvl 1 SW interrupt at max recursive depth
}

static void check_reset_reason_int_wdt(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_INT_WDT, esp_reset_reason());
}

TEST_CASE_MULTIPLE_STAGES("Test backtrace from interrupt watchdog timeout", "[reset_reason][reset=Interrupt wdt timeout on CPU0,SW_CPU_RESET]",
                          do_wdt_timeout,
                          check_reset_reason_int_wdt)

static void write_char_crash(char c)
{
    esp_rom_output_putc(c);
    hal_memset((void *)0x00000001, 0, 1);
}

static void do_rom_crash(void)
{
    esp_rom_install_channel_putc(1, write_char_crash);
    esp_rom_printf("foo");
}

TEST_CASE_MULTIPLE_STAGES("Test backtrace with a ROM function", "[reset_reason][reset=StoreProhibited,SW_CPU_RESET]",
                          do_rom_crash,
                          check_reset_reason_panic)

#define NUM_TEST_FUNCS      2

static void backtrace_suspend_func(void *arg)
{
    // Simply suspend and wait to be deleted
    vTaskSuspend(NULL);
}

TEST_CASE("Test esp_backtrace_print_all_tasks()", "[esp_system]")
{
    TaskHandle_t task_handles[NUM_TEST_FUNCS];

    for (int i = 0; i < NUM_TEST_FUNCS; i++) {
        // Create multiple unpinned tasks at higher priorities
        xTaskCreate(backtrace_suspend_func, "trace_func", 2048, NULL, UNITY_FREERTOS_PRIORITY + i + 1, &task_handles[i]);
    }
    // Short delay to allow tasks to suspend
    vTaskDelay(10);
    // Print backtraces of all tasks
    esp_backtrace_print_all_tasks(3);

    // Clean up tasks
    for (int i = 0; i < NUM_TEST_FUNCS; i++) {
        vTaskDelete(task_handles[i]);
    }
}

#endif
