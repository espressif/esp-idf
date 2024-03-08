/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
#define SOC_DCACHE_SUPPORTED  (1)
#endif

/* Functions testing memprot peripheral: memory regions and illegal operations on them */

void test_dcache_read_violation(void);

void test_dcache_write_violation(void);

void test_iram_reg1_write_violation(void);

void test_iram_reg2_write_violation(void);

void test_iram_reg3_write_violation(void);

void test_iram_reg4_write_violation(void);

void test_dram_reg1_execute_violation(void);

void test_dram_reg2_execute_violation(void);

void test_rtc_fast_reg1_execute_violation(void);

void test_rtc_fast_reg2_execute_violation(void);

void test_rtc_fast_reg3_execute_violation(void);

void test_rtc_slow_reg1_execute_violation(void);

void test_rtc_slow_reg2_execute_violation(void);

void test_irom_reg_write_violation(void);

void test_drom_reg_write_violation(void);

void test_drom_reg_execute_violation(void);

#ifdef __cplusplus
}
#endif
