/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_attr.h"

#define ESP_TEE_TEST_INTR_ITER  (4)

void NOINLINE_ATTR esp_tee_test_dummy_sec_srv(int a, int b, int c, int d, int e, int f, int g, int h, int *i);

#ifdef __cplusplus
}
#endif
