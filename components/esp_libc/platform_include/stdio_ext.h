/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#if CONFIG_LIBC_NEWLIB
#include_next <stdio_ext.h>
#else
#include <stdio-bufio.h>
#endif
