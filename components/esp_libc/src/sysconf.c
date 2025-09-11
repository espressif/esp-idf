/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include "esp_err.h"
#include "esp_log.h"
#include "sdkconfig.h"

static const char *TAG = "sysconf";

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

// pathconf
long fpathconf(int fildes, int name)
{
    if (name == _PC_PATH_MAX) {
        return PATH_MAX;
    }
    ESP_LOGW(TAG, "fpathconf: unsupported name %d", name);
    errno = EINVAL;
    return -1;
}

long pathconf(const char *path, int name)
{
    if (name == _PC_PATH_MAX) {
        return PATH_MAX;
    }
    ESP_LOGW(TAG, "pathconf: unsupported name %d", name);
    errno = EINVAL;
    return -1;
}
