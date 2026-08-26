/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 * Second TU in ``main``: only uses CONFIG_IDF_TARGET from sdkconfig—no conditional
 * on the optional symbol defined for configdep_main.c (see ``Kconfig.projbuild``).
 * Used by tests to snapshot this ``.obj`` alongside ``configdep_main``.
 */
#include <stdio.h>
#include "sdkconfig.h"

void unrelated_function(void)
{
    printf("IDF target: %s\n", CONFIG_IDF_TARGET);
}
