/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* U-mode program that the tests load into domains. umode_elf_add_program() in main/CMakeLists.txt builds it,
 * and umode_elf_load() loads it. The first argument selects what it does, see app_basic.h. */

#include <stdbool.h>
#include <stdint.h>
#include "umode_app.h"
#include "app_basic.h"

typedef uint32_t (*operation_t)(uint32_t a, uint32_t b);

static uint32_t add(uint32_t a, uint32_t b)
{
    return a + b;
}

static uint32_t multiply(uint32_t a, uint32_t b)
{
    return a * b;
}

/* .data, with a value that the program checks and changes */
static uint32_t s_data = APP_BASIC_DATA_INITIAL;
/* .bss, starts at zero */
static uint32_t s_counter;
static uint8_t s_buffer[256];
/* Pointers into .rodata and .text, which the loader fills in with R_RISCV_RELATIVE relocations */
static const char *const s_names[] = { "add", "multiply" };
static const operation_t s_operations[] = { add, multiply };

static uint32_t length(const char *text)
{
    uint32_t n = 0;
    while (text[n] != '\0') {
        n++;
    }
    return n;
}

__attribute__((noinline)) static void exit_from_nested_call(uint32_t value)
{
    umode_exit((int)(value + 7));
}

/* Checks that .data and .bss start with their initial values, then changes both */
static int check_data(uint32_t arg)
{
    if (s_data != APP_BASIC_DATA_INITIAL) {
        return APP_BASIC_ERROR_DATA;
    }
    for (uint32_t i = 0; i < sizeof(s_buffer); i++) {
        if (s_buffer[i] != 0) {
            return APP_BASIC_ERROR_BSS;
        }
    }
    s_data = arg;
    for (uint32_t i = 0; i < sizeof(s_buffer); i++) {
        s_buffer[i] = (uint8_t)(arg + i);
    }
    return 0;
}

/* Calls through the pointer tables: the sum or product of 6 and 7, times 100, plus the length of its name */
static int call_table(uint32_t index)
{
    index %= sizeof(s_operations) / sizeof(s_operations[0]);
    return (int)(s_operations[index](6, 7) * 100 + length(s_names[index]));
}

/* Loops forever, until a watchdog fires */
__attribute__((noinline, noreturn)) static void spin(void)
{
    while (true) {
        __asm__ volatile("");
    }
}

/* Loops count times */
__attribute__((noinline)) static void spin_for(uint32_t count)
{
    for (uint32_t i = 0; i < count; i++) {
        __asm__ volatile("");
    }
}

int user_main(uint32_t selector, uint32_t arg)
{
    switch (selector) {
    case APP_BASIC_RETURN:
        return (int)(arg * 3 + 1);
    case APP_BASIC_EXIT_NESTED:
        exit_from_nested_call(arg);
        return APP_BASIC_ERROR_EXIT;
    case APP_BASIC_CHECK_DATA:
        return check_data(arg);
    case APP_BASIC_READ_DATA:
        return (int)(s_data + s_buffer[arg % sizeof(s_buffer)]);
    case APP_BASIC_CALL_TABLE:
        return call_table(arg);
    case APP_BASIC_COUNT:
        return (int)++s_counter;
    case APP_BASIC_LOAD:
        return (int) * (volatile const uint32_t *)arg;
    case APP_BASIC_SPIN:
        spin();
    case APP_BASIC_SPIN_FOR:
        spin_for(arg);
        return 0;
    default:
        return APP_BASIC_ERROR_SELECTOR;
    }
}
