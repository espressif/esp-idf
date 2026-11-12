/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* U-mode code that the tests copy into the memory of a domain. The functions use no global data, and their
 * calls and branches are relative to the program counter, so they run at any address. */

#include <stdint.h>
#include "privilege_code.h"

int privilege_snippet_return_42(uint32_t arg0, uint32_t arg1)
{
    return 42;
}
