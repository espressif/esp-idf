/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Utility functions */

void die(const char* msg) __attribute__ ((noreturn));

/* Functions causing an exception/panic in different ways */

void test_abort(void);

void test_abort_cache_disabled(void);

void test_int_wdt(void);

void test_task_wdt_cpu0(void);

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
void test_hw_stack_guard_cpu0(void);
#if !CONFIG_FREERTOS_UNICORE
void test_hw_stack_guard_cpu1(void);
#endif // CONFIG_FREERTOS_UNICORE
#endif // CONFIG_ESP_SYSTEM_HW_STACK_GUARD

#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
void test_panic_extram_stack(void);
#endif

#if !CONFIG_FREERTOS_UNICORE
void test_task_wdt_cpu1(void);
#endif

void test_loadprohibited(void);

void test_storeprohibited(void);

void test_cache_error(void);

void test_int_wdt_cache_disabled(void);

void test_stack_overflow(void);

void test_illegal_instruction(void);

void test_instr_fetch_prohibited(void);

void test_ub(void);

void test_assert(void);

void test_assert_cache_disabled(void);

void test_illegal_access(void);

void test_capture_dram(void);

#ifdef __cplusplus
}
#endif
