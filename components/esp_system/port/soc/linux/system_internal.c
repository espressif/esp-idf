/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * "inner" restart function on POSIX/Linux just exits.
*/
void esp_restart_noos(void)
{
    printf("restart triggered on Linux, hence exiting\n");
    exit(0); // TODO: other exit value?
}
