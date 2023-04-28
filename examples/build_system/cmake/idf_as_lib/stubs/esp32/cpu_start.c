/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdbool.h>
#include <setjmp.h>

extern void app_main(void);
jmp_buf buf;

int main(void)
{
    setjmp(buf);
    app_main();
}
