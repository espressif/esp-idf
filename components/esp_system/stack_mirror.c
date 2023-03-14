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
#include "esp_cpu_utils.h"

#if CONFIG_COMPILER_STACK_MIRROR

#define STACK_MIRROR_TLS_INDEX 1

#define STACK_MIRROR_MAGIC 128975098

/* This function attribute tells GCC to call the given function
   before every function call in the program. */
void __cyg_profile_func_enter(void *func, void *caller) __attribute__((no_instrument_function));
void __cyg_profile_func_exit(void *func, void *caller) __attribute__((no_instrument_function));

typedef struct {
    uint32_t pc;
    uint32_t sp;
} frame_t;

typedef struct {
    uint32_t magic;
    uint32_t depth;
    frame_t backtrace[CONFIG_COMPILER_STACK_MIRROR_DEPTH];
} stack_mirror_t;

static void IRAM_ATTR print_str(const char* str, bool panic)
{
    if (panic) {
        panic_print_str(str);
    } else {
        esp_rom_printf(str);
    }
}

static void IRAM_ATTR print_entry(uint32_t pc, uint32_t sp, bool panic)
{
    if (panic) {
        panic_print_str(" 0x");
        panic_print_hex(pc);
        panic_print_str(":0x");
        panic_print_hex(sp);
    } else {
        esp_rom_printf(" 0x%08X:0x%08X", pc, sp);
    }
}


static void delete(int tlsIndex, void* val)
{
    free(val);
}

static void push_frame(uint32_t pc, uint32_t sp)
{
    stack_mirror_t* m = (stack_mirror_t*) 
        pvTaskGetThreadLocalStoragePointer(NULL, STACK_MIRROR_TLS_INDEX);
    if (!m) {
        m = (stack_mirror_t*) heap_caps_calloc(sizeof(stack_mirror_t), 1, MALLOC_CAP_INTERNAL);
        m->magic = STACK_MIRROR_MAGIC;
        if (m) {
            vTaskSetThreadLocalStoragePointerAndDelCallback(NULL, STACK_MIRROR_TLS_INDEX, m, delete);
        }
    }
    if (m) {
        if (m->depth < CONFIG_COMPILER_STACK_MIRROR_DEPTH) {
            m->backtrace[m->depth].pc = pc;
            m->backtrace[m->depth].sp = sp;
        }
        m->depth++;
    }
}

static void pop_frame()
{
    stack_mirror_t* m = (stack_mirror_t*) 
        pvTaskGetThreadLocalStoragePointer(NULL, STACK_MIRROR_TLS_INDEX);
    if (m) {
        m->depth--;
    }
}

void __cyg_profile_func_enter(void *func, void *caller)
{
    uint32_t pc = (uint32_t) __builtin_return_address(0);
    uint32_t sp = (uint32_t) __builtin_frame_address(0);
    push_frame(pc, sp);
}

void __cyg_profile_func_exit(void *func, void *caller)
{
    pop_frame();
}

void stack_mirror_print_backtrace(bool panic)
{
    stack_mirror_t* m = (stack_mirror_t*) 
        pvTaskGetThreadLocalStoragePointer(NULL, STACK_MIRROR_TLS_INDEX);

    print_str("\r\n\r\nBacktrace (Mirror):", panic);

    if (m == NULL) {

        print_str("\r\nNot Set", panic);

    } else if (esp_ptr_internal(m) == false) {

        print_str("\r\nInvalid Ptr", panic);

    } else if (m->depth == 0) {

        print_str("\r\nEmpty", panic);

    } else if (m->magic != STACK_MIRROR_MAGIC) {

        print_str("\r\nInvalid Magic", panic);

    } else {

        if (m->depth >= CONFIG_COMPILER_STACK_MIRROR_DEPTH) {
            print_str("\r\nBACKTRACE INCOMPLETE", panic);
        }

        for (int i = m->depth - 1; i >= 0; i--) {

            uint32_t pc = m->backtrace[i].pc;
            uint32_t sp = m->backtrace[i].pc;

            print_entry(pc, sp, panic);

            bool corrupted = !(
                esp_stack_ptr_is_sane(sp) && 
                esp_ptr_executable((void *)esp_cpu_process_stack_pc(pc))
            );

            if (corrupted) {
                print_str(" |<-CORRUPTED", panic);
                break;
            }
        }
    }

    print_str("\r\n\r\n", panic);

    return;
}

#endif // CONFIG_COMPILER_STACK_MIRROR