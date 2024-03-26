/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "esp_private/panic_internal.h"
#include "soc/soc_memory_types.h"

#if CONFIG_COMPILER_STACK_MIRROR

#define STACK_MIRROR_MAGIC 139871234

/* __cyg_profile_func_enter - gcc inserts a call to this function at the start of every function
 * __cyg_profile_func_exit  - gcc inserts a call to this function at the end of every function
 * no_instrument_function   - This tells GCC to not insert instrumentation calls (would cause an infinite loop) */
void IRAM_ATTR __cyg_profile_func_enter(void *func, void *caller) __attribute__((no_instrument_function));
void IRAM_ATTR __cyg_profile_func_exit(void *func, void *caller) __attribute__((no_instrument_function));
static void IRAM_ATTR print_str(const char* str, bool panic) __attribute__((no_instrument_function));
static void IRAM_ATTR print_entry(uint32_t pc, bool panic) __attribute__((no_instrument_function));

static uint32_t enabled;

static void IRAM_ATTR print_str(const char* str, bool panic)
{
    if (panic) {
        panic_print_str(str);
    } else {
        esp_rom_printf(str);
    }
}

static void IRAM_ATTR print_entry(uint32_t pc, bool panic)
{
    if (panic) {
        panic_print_str(" 0x");
        panic_print_hex(pc);
    } else {
        esp_rom_printf(" 0x%08X", pc);
    }
}

extern void xt_unhandled_exception(XtExcFrame *frame);

void IRAM_ATTR __cyg_profile_func_enter(void *func, void *callsite)
{
    if (enabled == STACK_MIRROR_MAGIC) {
        esp_stack_mirror_t *m = pvTaskGetStackMirrorPointer();
        if (func == xt_unhandled_exception) {
            // dont add to the stack trace after we already hit panic.
            m->panicing = true;
        }
        if (!m->panicing) {
            if (m->depth < CONFIG_COMPILER_STACK_MIRROR_DEPTH) {
                m->backtrace[m->depth] = (uint32_t) func;
            }
            m->depth++;
        }
    }
}

void IRAM_ATTR __cyg_profile_func_exit(void *func, void *callsite)
{
    if (enabled == STACK_MIRROR_MAGIC) {
        esp_stack_mirror_t *m = pvTaskGetStackMirrorPointer();
        if (!m->panicing && m->depth > 0) {
            m->depth--;
        }
    }
}

void IRAM_ATTR stack_mirror_print_backtrace(bool panic)
{
    esp_stack_mirror_t *m = pvTaskGetStackMirrorPointer();

    print_str("\r\nBacktrace (Mirror):", panic);

    if (m == NULL) {

        print_str("\r\nNot Set", panic);

    } else if (esp_ptr_internal(m) == false) {

        print_str("\r\nInvalid Ptr", panic);

    } else if (m->depth == 0) {

        print_str("\r\nEmpty", panic);

    } else {

        uint32_t depth = m->depth;

        if (depth >= CONFIG_COMPILER_STACK_MIRROR_DEPTH) {
            print_str("\r\nBACKTRACE INCOMPLETE", panic);
            depth = CONFIG_COMPILER_STACK_MIRROR_DEPTH;
        }

        for (int i = depth - 1; i >= 0; i--) {

            uint32_t pc = m->backtrace[i];

            print_entry(pc, panic);

            bool corrupted = !esp_ptr_executable((void *)esp_cpu_process_stack_pc(pc));

            if (corrupted) {
                print_str(" |<-CORRUPTED", panic);
                break;
            }
        }
    }

    print_str("\r\n\r\n", panic);

    return;
}

void stack_mirror_enable()
{
    enabled = STACK_MIRROR_MAGIC;
}

#endif // CONFIG_COMPILER_STACK_MIRROR