/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "esp_attr.h"

#define TEE_TEST_INT_COUNT 3

uint32_t NOINLINE_ATTR esp_tee_service_add(uint32_t a, uint32_t b);

uint32_t NOINLINE_ATTR esp_tee_service_sub(uint32_t a, uint32_t b);

uint32_t NOINLINE_ATTR esp_tee_service_mul(uint32_t a, uint32_t b);

uint32_t NOINLINE_ATTR esp_tee_service_div(uint32_t a, uint32_t b);

int esp_tee_secure_int_test(void);

int esp_tee_non_secure_int_test(volatile uint32_t* volatile ns_int_count);

int esp_tee_test_int_count(uint32_t *secure_int_count);

int esp_tee_test_store_prohibited(uint32_t type);

int esp_tee_test_illegal_instr(void);

int esp_tee_test_instr_fetch_prohibited(uint32_t type);

void NOINLINE_ATTR dummy_secure_service(int a, int b, int c, int d, int e, int f, int g, int h, int *i);

uint32_t add_in_loop(uint32_t a, uint32_t b, uint32_t iter);

int _ss_esp_tee_test_heap_malloc_write_free(void);

#ifdef __cplusplus
}
#endif
