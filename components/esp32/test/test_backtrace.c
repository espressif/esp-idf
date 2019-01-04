/*
 * Note: Currently, the backtraces must still be checked manually. Therefore,
 * these test cases should always pass.
 * Todo: Automate the checking of backtrace addresses.
 */
#include <stdlib.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/xtensa_api.h"
#include "esp_intr_alloc.h"

#define SW_ISR_LEVEL_1          7
#define SW_ISR_LEVEL_3          29
#define RECUR_DEPTH             3
#define ACTION_ABORT            -1
#define ACTION_INT_WDT          -2

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

static void level_three_isr (void *arg)
{
    xt_set_intclear(1 << SW_ISR_LEVEL_3);                           //Clear interrupt
    recursive_func(RECUR_DEPTH, backtrace_trigger_source);         //Abort at the max recursive depth
}

static void level_one_isr(void *arg)
{
    xt_set_intclear(1 << SW_ISR_LEVEL_1);           //Clear interrupt
    recursive_func(RECUR_DEPTH, SW_ISR_LEVEL_3);    //Trigger nested interrupt max recursive depth
}

TEST_CASE("Test backtrace from abort", "[reset_reason][reset=abort,SW_CPU_RESET]")
{
    //Allocate level one and three SW interrupts
    esp_intr_alloc(ETS_INTERNAL_SW0_INTR_SOURCE, 0, level_one_isr, NULL, NULL);     //Level 1 SW intr
    esp_intr_alloc(ETS_INTERNAL_SW1_INTR_SOURCE, 0, level_three_isr, NULL, NULL);   //Level 3 SW intr
    backtrace_trigger_source = ACTION_ABORT;
    recursive_func(RECUR_DEPTH, SW_ISR_LEVEL_1);    //Trigger lvl 1 SW interrupt at max recursive depth
}

TEST_CASE("Test backtrace from interrupt watchdog timeout", "[reset_reason][reset=Interrupt wdt timeout on CPU0,SW_CPU_RESET]")
{
    //Allocate level one and three SW interrupts
    esp_intr_alloc(ETS_INTERNAL_SW0_INTR_SOURCE, 0, level_one_isr, NULL, NULL);     //Level 1 SW intr
    esp_intr_alloc(ETS_INTERNAL_SW1_INTR_SOURCE, 0, level_three_isr, NULL, NULL);   //Level 3 SW intr
    backtrace_trigger_source = ACTION_INT_WDT;
    recursive_func(RECUR_DEPTH, SW_ISR_LEVEL_1);    //Trigger lvl 1 SW interrupt at max recursive depth
}
