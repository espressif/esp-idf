/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <unistd.h>
#include <errno.h>
#include "sdkconfig.h"

#ifdef CONFIG_FREERTOS_UNICORE
#define CPU_NUM 1
#else
#define CPU_NUM CONFIG_SOC_CPU_CORES_NUM
#endif

long sysconf(int arg)
{
    switch (arg) {
    case _SC_NPROCESSORS_CONF:
    case _SC_NPROCESSORS_ONLN:
        return CPU_NUM;
    default:
        errno = EINVAL;
        return -1;
    }
}
