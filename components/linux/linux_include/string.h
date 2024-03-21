/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This header file also includes the additional libbsd string.h with functions such as strlcpy(),
 * which are present in the ESP-IDF toolchain and on MacOS, but not on Linux.
 */

#pragma once

#include <bsd/string.h>
#include_next <string.h>
