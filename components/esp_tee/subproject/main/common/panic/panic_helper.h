/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

void panic_print_backtrace(const void *f, int depth);

void panic_print_registers(const void *f, int core);

void panic_print_exccause(const void *f, int core);

void panic_print_isrcause(const void *f, int core);
