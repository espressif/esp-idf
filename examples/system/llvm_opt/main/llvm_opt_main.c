/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>

#include "sdkconfig.h"

void app_main(void)
{
#if CONFIG_COMPILER_LLVM_MEMCPY_OPTIMIZATION
    printf("LLVM optimization for ESP32-P4 memcpy: enabled in menuconfig\n");
#else
    printf("LLVM optimization for ESP32-P4 memcpy: not enabled (select it in menuconfig)\n");
#endif
    printf("This component is built with ENABLE_LLVM_OPT.\n");
}
