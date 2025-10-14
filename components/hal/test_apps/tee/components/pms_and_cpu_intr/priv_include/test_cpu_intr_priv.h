/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

/********************************** Utility *********************************/

void test_intr_utils_init(void);
void test_intr_utils_deinit(void);

void test_intr_alloc(int mode, int priority, int intr_src, intr_handler_t func, void *arg);
void test_intr_free_all(void);
