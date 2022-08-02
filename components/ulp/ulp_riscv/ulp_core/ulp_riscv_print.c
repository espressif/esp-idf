/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ulp_riscv_print.h"

typedef struct {
    putc_fn_t putc; // Putc function of the underlying driver, e.g. UART
    void *putc_ctx; // Context passed to the putc function
} ulp_riscv_print_ctx_t;

static ulp_riscv_print_ctx_t s_print_ctx;

void ulp_riscv_print_install(putc_fn_t putc, void * putc_ctx)
{
    s_print_ctx.putc_ctx = putc_ctx;
    s_print_ctx.putc = putc;
}

void ulp_riscv_print_str(const char *str)
{
    if (!s_print_ctx.putc) {
        return;
    }

    for (int i = 0; str[i] != 0; i++) {
        s_print_ctx.putc(s_print_ctx.putc_ctx ,str[i]);
    }
}

void ulp_riscv_print_hex(int h)
{
    int x;
    int c;

    if (!s_print_ctx.putc) {
        return;
    }

    // Does not print '0x', only the digits (8 digits to print)
    for (x = 0; x < 8; x++) {
        c = (h >> 28) & 0xf; // extract the leftmost byte
        if (c < 10) {
            s_print_ctx.putc(s_print_ctx.putc_ctx ,'0' + c);
        } else {
            s_print_ctx.putc(s_print_ctx.putc_ctx ,'a' + c - 10);
        }
        h <<= 4; // move the 2nd leftmost byte to the left, to be extracted next
    }
}
