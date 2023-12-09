/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "unity.h"
#include "esp_attr.h"

/* Global variables that should be part of the coredump */
COREDUMP_IRAM_DATA_ATTR uint32_t var_iram = 0x42;
COREDUMP_DRAM_ATTR uint32_t var_dram = 0x43;
#if SOC_RTC_MEM_SUPPORTED
COREDUMP_RTC_DATA_ATTR uint32_t var_rtc = 0x44;
COREDUMP_RTC_FAST_ATTR uint32_t var_rtcfast = 0x45;
#endif // SOC_RTC_MEM_SUPPORTED

/* Memory regions to dump, defined at compile time. */
extern int _coredump_dram_start;
extern int _coredump_dram_end;
extern int _coredump_iram_start;
extern int _coredump_iram_end;
#if SOC_RTC_MEM_SUPPORTED
extern int _coredump_rtc_start;
extern int _coredump_rtc_end;
extern int _coredump_rtc_fast_start;
extern int _coredump_rtc_fast_end;
#endif // SOC_RTC_MEM_SUPPORTED

static inline bool is_addr_in_region(void *addr, uint8_t *region, int region_size)
{
    const void *start = (void *) region;
    const void *end = (void *) (region + region_size);
    return addr >= start && addr < end;
}

TEST_CASE("test variables presence in core dump sections", "[espcoredump]")
{
    uint32_t section_start = 0;
    uint32_t section_size = 0;

    /* Check DRAM coredump section */
    section_start = (uint32_t)&_coredump_dram_start;
    section_size = (uint8_t *)&_coredump_dram_end - (uint8_t *)&_coredump_dram_start;
    TEST_ASSERT(section_size > 0);
    TEST_ASSERT(is_addr_in_region(&var_dram, (uint8_t *) section_start, section_size));
#if IRAM_8BIT_ACCESSIBLE
    /* Check IRAM coredump section */
    section_start = (uint32_t)&_coredump_iram_start;
    section_size = (uint8_t *)&_coredump_iram_end - (uint8_t *)&_coredump_iram_start;
    TEST_ASSERT(section_size > 0);
    TEST_ASSERT(is_addr_in_region(&var_iram, (uint8_t *) section_start, section_size));
#endif
#if SOC_RTC_MEM_SUPPORTED
    /* Check RTC coredump section */
    section_start = (uint32_t)&_coredump_rtc_start;
    section_size = (uint8_t *)&_coredump_rtc_end - (uint8_t *)&_coredump_rtc_start;
    TEST_ASSERT(section_size > 0);
    TEST_ASSERT(is_addr_in_region(&var_rtc, (uint8_t *) section_start, section_size));
    /* Check RTC Fast coredump section */
    section_start = (uint32_t)&_coredump_rtc_fast_start;
    section_size = (uint8_t *)&_coredump_rtc_fast_end - (uint8_t *)&_coredump_rtc_fast_start;
    TEST_ASSERT(section_size > 0);
    TEST_ASSERT(is_addr_in_region(&var_rtcfast, (uint8_t *) section_start, section_size));
#endif // SOC_RTC_MEM_SUPPORTED
}
