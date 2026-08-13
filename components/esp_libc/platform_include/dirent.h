/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/* picolibc's <dirent.h> declares posix_getdents() with ssize_t without
 * pulling the type in itself.
 */
#include <sys/types.h>

#include_next <dirent.h>
