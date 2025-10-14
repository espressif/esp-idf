/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdarg.h>
#include "esp_rom_sys.h"

#if CONFIG_SECURE_TEE_DEBUG_MODE
#define tee_panic_print(format, ...) esp_rom_printf(DRAM_STR(format), ##__VA_ARGS__)
#else
#define tee_panic_print(format, ...)
#endif

void panic_print_backtrace(const void *f, int depth);

void panic_print_registers(const void *f, int core);

void panic_print_rsn(const void *f, int core, const char *rsn);

void panic_print_exccause(const void *f, int core);

void panic_print_isrcause(const void *f, int core);
