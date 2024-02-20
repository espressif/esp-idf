/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This is a helper include for fls(), which is present in the ESP-IDF toolchains and MacOS, but not on Linux.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int fls(unsigned int value);

#ifdef __cplusplus
}
#endif

#include_next <strings.h>
