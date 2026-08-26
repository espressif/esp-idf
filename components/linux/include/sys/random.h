/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include_next "sys/random.h"

#if __APPLE__
#include <stddef.h>

int getrandom(void *buf, size_t buflen, unsigned int flags);

#endif  // __APPLE__
