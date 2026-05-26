/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "esp_private/crosscore_int.h"

/*
 * "inner" restart function on POSIX/Linux just exits.
*/
void esp_restart_noos(void)
{
    printf("restart triggered on Linux, hence exiting\n");
    exit(0); // TODO: other exit value?
}

/*
 * Cross-core interrupts do not exist on the single-core Linux simulator.
 */
void esp_crosscore_int_init(void)
{
}
