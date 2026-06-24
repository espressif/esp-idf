/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <stdint.h>

__attribute__((constructor))
void foo(void)
{
    printf("%s\n", __FUNCTION__);
}

__attribute__((constructor(103)))
void init_prio_103(void)
{
    printf("%s\n", __FUNCTION__);
}

__attribute__((constructor(101)))
void init_prio_101(void)
{
    printf("%s\n", __FUNCTION__);
}

__attribute__((constructor(102)))
void init_prio_102(void)
{
    printf("%s\n", __FUNCTION__);
}

__attribute__((constructor))
void bar(void)
{
    printf("%s\n", __FUNCTION__);
}

void preinit_func(void)
{
    printf("%s\n", __FUNCTION__);
}

__attribute__((section(".preinit_array"), used))
uintptr_t test_preinit = (uintptr_t) preinit_func;

void app_main(void)
{
    printf("app_main running\n");
}
