/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "sdkconfig.h"
#include "soc/soc_caps.h"

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


#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
void test_non_cache_iram_reg1_write_violation(void);
void test_non_cache_iram_reg2_write_violation(void);
void test_non_cache_iram_reg3_write_violation(void);
void test_non_cache_iram_reg4_write_violation(void);
#endif

void test_iram_reg4_write_violation(void);

void test_dram_reg1_execute_violation(void);

void test_dram_reg2_execute_violation(void);

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
void test_non_cache_dram_reg1_execute_violation(void);
void test_non_cache_dram_reg2_execute_violation(void);
#endif

void test_rtc_fast_reg1_execute_violation(void);

void test_rtc_fast_reg2_execute_violation(void);

void test_rtc_fast_reg3_execute_violation(void);

void test_rtc_slow_reg1_execute_violation(void);

void test_rtc_slow_reg2_execute_violation(void);

void test_irom_reg_write_violation(void);

void test_spiram_xip_irom_alignment_reg_execute_violation(void);

void test_spiram_xip_drom_alignment_reg_execute_violation(void);

void test_drom_reg_write_violation(void);

void test_drom_reg_execute_violation(void);

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
void test_non_cache_irom_reg_write_violation(void);
void test_non_cache_drom_reg_write_violation(void);
void test_non_cache_drom_reg_execute_violation(void);

void test_non_cache_spiram_xip_irom_alignment_reg_execute_violation(void);
void test_non_cache_spiram_xip_drom_alignment_reg_execute_violation(void);
#endif

void test_invalid_memory_region_write_violation(void);

void test_invalid_memory_region_execute_violation(void);

#ifdef __cplusplus
}
#endif
