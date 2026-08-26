/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>

int cxx_atomic_use_a(void);
int cxx_atomic_use_b(void);

extern "C" void app_main(void)
{
    // Reference both translation units so the linker keeps their
    // _Lock_policy instantiations (needed for the invalid-policy build check).
    volatile int r = cxx_atomic_use_a() + cxx_atomic_use_b();
    printf("cxx atomic lock policy: %d\n", r);
}
