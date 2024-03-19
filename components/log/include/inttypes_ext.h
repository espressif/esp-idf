/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/*
 * This header provides PRIuSIZE, a printf formatter macro for size_t which is similar to the macros
 * PRIuX defined in inttypes.h. It is necessary because size_t has different width on different
 * platforms (e.g., Linux and ESP32). The alternative would be always casting parameters
 * to a certain type in printf and logging functions.
 */

#if __SIZE_WIDTH__ == 64 // __SIZE_WIDTH__ is defined by both GCC and Clang
#define PRIuSIZE "lu"
#else
#define PRIuSIZE "u"
#endif
