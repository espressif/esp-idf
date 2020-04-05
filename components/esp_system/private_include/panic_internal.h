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

#pragma once

#include <stdint.h>

#include "port/panic_funcs.h"

#include "sdkconfig.h"

extern bool g_panic_abort;

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
#define panic_print_char(c)
#define panic_print_str(str)
#define panic_print_dec(d)
#define panic_print_hex(h)
#endif

void __attribute__((noreturn)) panic_abort(const char *details);