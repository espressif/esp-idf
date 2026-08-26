/*
 * SPDX-FileCopyrightText: Copyright (c) 1990, 1993 The Regents of the University of California.  All rights reserved.
 *
 * SPDX-License-Identifier: BSD-4-Clause-UC
 *
 * SPDX-FileContributor: 2024 Espressif Systems (Shanghai) CO LTD
 */

#include "strings.h"

int fls(unsigned int mask)
{
    if (mask == 0) {
        return (0);
    }

    return (sizeof(mask) << 3) - __builtin_clz(mask);
}
