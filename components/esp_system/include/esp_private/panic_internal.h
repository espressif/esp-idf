/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "esp_macros.h"
#include "soc/soc_caps.h"

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

extern bool g_panic_abort;
extern char *g_panic_abort_details;
extern void *g_exc_frames[SOC_CPU_CORES_NUM];

// Function to print longer amounts of information such as the details
// and backtrace field of panic_info_t. These functions should limit themselves
// to printing to the console and should do other more involved processing,
// and must be aware that the main logic in panic.c has a watchdog timer active.
typedef void (*panic_info_dump_fn_t)(const void* frame);

// Non architecture specific exceptions (generally valid for all targets).
// Can be used to convey to the main logic what exception is being
// dealt with to perform some actions, without knowing the underlying
// architecture/chip-specific exception.
typedef enum {
    PANIC_EXCEPTION_DEBUG,
    PANIC_EXCEPTION_IWDT,
    PANIC_EXCEPTION_TWDT,
    PANIC_EXCEPTION_ABORT,
    PANIC_EXCEPTION_FAULT,                  // catch-all for all types of faults
} panic_exception_t;

typedef struct {
    int core;                               // core which triggered panic
    panic_exception_t exception;            // non-architecture-specific exception code
    const char* reason;                     // exception string
    const char* description;                // short description of the exception
    panic_info_dump_fn_t details;           // more details on the exception
    panic_info_dump_fn_t state;             // processor state, usually the contents of the registers
    const void* addr;                       // instruction address that triggered the exception
    const void* frame;                      // reference to the frame
    bool pseudo_excause;                    // flag indicating that exception cause has special meaning
} panic_info_t;

#define PANIC_INFO_DUMP(info, dump_fn)      {if ((info)->dump_fn) (*(info)->dump_fn)((info->frame));}

// Create own print functions, since printf might be broken, and can be silenced
// when CONFIG_ESP_SYSTEM_PANIC_SILENT_REBOOT
#if !CONFIG_ESP_SYSTEM_PANIC_SILENT_REBOOT
void panic_print_char(char c);
void panic_print_str(const char *str);
void panic_print_dec(int d);
void panic_print_hex(int h);
#else
#define panic_print_char(c)     ESP_UNUSED(c)
#define panic_print_str(str)    ESP_UNUSED(str)
#define panic_print_dec(d)      ESP_UNUSED(d)
#define panic_print_hex(h)      ESP_UNUSED(h)
#endif

void __attribute__((__noreturn__)) panic_abort(const char *details);

void panic_arch_fill_info(void *frame, panic_info_t *info);

void panic_soc_fill_info(void *frame, panic_info_t *info);

bool panic_soc_check_pseudo_cause(void *f, panic_info_t *info);

void panic_print_registers(const void *frame, int core);

void panic_print_backtrace(const void *frame, int core);

uint32_t panic_get_address(const void* frame);

void panic_set_address(void *frame, uint32_t addr);

uint32_t panic_get_cause(const void* frame);

#ifdef __cplusplus
}
#endif
