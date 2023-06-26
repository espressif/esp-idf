/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "inttypes.h"
#include "unity.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "soc/soc.h"
#include "esp_system.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp_private/esp_psram_extram.h"
#endif

extern int _rtc_noinit_start;
extern int _rtc_noinit_end;
extern int _rtc_data_start;
extern int _rtc_data_end;
extern int _noinit_start;
extern int _noinit_end;
extern int _rtc_force_fast_start;
extern int _rtc_force_fast_end;
extern int _rtc_force_slow_start;
extern int _rtc_force_slow_end;
extern int _ext_ram_noinit_start;
extern int _ext_ram_noinit_end;
extern int _ext_ram_bss_start;
extern int _ext_ram_bss_end;


//Variables for test: Attributes place variables into correct sections
static __NOINIT_ATTR uint32_t s_noinit;
#if SOC_RTC_MEM_SUPPORTED
static RTC_NOINIT_ATTR uint32_t s_rtc_noinit;
static RTC_DATA_ATTR uint32_t s_rtc_data;
static RTC_RODATA_ATTR uint32_t s_rtc_rodata;
static RTC_FAST_ATTR uint32_t s_rtc_force_fast;
static RTC_SLOW_ATTR uint32_t s_rtc_force_slow;
#endif // SOC_RTC_MEM_SUPPORTED
#if CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY
static EXT_RAM_NOINIT_ATTR uint32_t s_noinit_ext;
#endif

static bool data_in_segment(void *ptr, int *seg_start, int *seg_end)
{
    return ((intptr_t)ptr < (intptr_t)seg_end) && \
           ((intptr_t)ptr >= (intptr_t)seg_start);
}

TEST_CASE("Attributes place variables into correct sections", "[ld]")
{
    TEST_ASSERT(data_in_segment(&s_noinit, &_noinit_start, &_noinit_end));

#if SOC_RTC_MEM_SUPPORTED
    TEST_ASSERT(data_in_segment(&s_rtc_noinit, &_rtc_noinit_start, &_rtc_noinit_end));
    TEST_ASSERT(data_in_segment(&s_rtc_data, &_rtc_data_start, &_rtc_data_end));
    TEST_ASSERT(data_in_segment(&s_rtc_rodata, &_rtc_data_start, &_rtc_data_end));
    TEST_ASSERT(data_in_segment(&s_rtc_force_fast, &_rtc_force_fast_start, &_rtc_force_fast_end));
    TEST_ASSERT(data_in_segment(&s_rtc_force_slow, &_rtc_force_slow_start, &_rtc_force_slow_end));

#if CONFIG_ESP32_RTCDATA_IN_FAST_MEM   || \
    CONFIG_ESP32S2_RTCDATA_IN_FAST_MEM || \
    CONFIG_ESP32S3_RTCDATA_IN_FAST_MEM
    TEST_ASSERT(data_in_segment(&s_rtc_data, (int*) SOC_RTC_DRAM_LOW, (int*) SOC_RTC_DRAM_HIGH));
    TEST_ASSERT(data_in_segment(&s_rtc_rodata, (int*) SOC_RTC_DRAM_LOW, (int*) SOC_RTC_DRAM_HIGH));
    TEST_ASSERT(data_in_segment(&s_rtc_noinit, (int*) SOC_RTC_DRAM_LOW, (int*) SOC_RTC_DRAM_HIGH));
#else
    TEST_ASSERT(data_in_segment(&s_rtc_data, (int*) SOC_RTC_DATA_LOW, (int*) SOC_RTC_DATA_HIGH));
    TEST_ASSERT(data_in_segment(&s_rtc_rodata, (int*) SOC_RTC_DATA_LOW, (int*) SOC_RTC_DATA_HIGH));
    TEST_ASSERT(data_in_segment(&s_rtc_noinit, (int*) SOC_RTC_DATA_LOW, (int*) SOC_RTC_DATA_HIGH));
#endif

    TEST_ASSERT(data_in_segment(&s_rtc_force_fast, (int*) SOC_RTC_DRAM_LOW, (int*) SOC_RTC_DRAM_HIGH));
    TEST_ASSERT(data_in_segment(&s_rtc_force_slow, (int*) SOC_RTC_DATA_LOW, (int*) SOC_RTC_DATA_HIGH));
#endif // SOC_RTC_MEM_SUPPORTED

#if CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY
    TEST_ASSERT(data_in_segment(&s_noinit_ext, &_ext_ram_noinit_start, &_ext_ram_noinit_end));
#endif
}

#if CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY

#define TEST_BUFFER_SIZE (16*1024/4)
static EXT_RAM_NOINIT_ATTR uint32_t s_noinit_buffer[TEST_BUFFER_SIZE];

static void write_spiram_and_reset(void)
{
    // Fill the noinit buffer
    printf("Filling buffer\n");
    for (uint32_t i = 0; i < TEST_BUFFER_SIZE; i++) {
        s_noinit_buffer[i] = i ^ 0x55555555U;
    }
    printf("Flushing cache\n");
    // Flush the cache out to SPIRAM before resetting.
    esp_psram_extram_writeback_cache();

    printf("Restarting\n");
    // Reset to test that noinit memory is left intact.
    esp_restart();
}

static void check_spiram_contents(void)
{
    // Confirm that the memory contents are still what we expect
    uint32_t error_count = 0;
    for (uint32_t i = 0; i < TEST_BUFFER_SIZE; i++) {
        if (s_noinit_buffer[i] != (i ^ 0x55555555U)) {
            error_count++;
        }
    }
    printf("Found %" PRIu32 " memory errors\n", error_count);
    TEST_ASSERT(error_count == 0);
}

TEST_CASE_MULTIPLE_STAGES("Spiram test noinit memory", "[psram][ld]", write_spiram_and_reset, check_spiram_contents);

#endif // CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY


#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
#define TEST_BSS_NUM    (256 * 1024)
static EXT_RAM_BSS_ATTR uint32_t s_bss_buffer[TEST_BSS_NUM];

TEST_CASE("Test variables placed in external .bss segment", "[psram][ld]")
{
    for (int i = 0; i < TEST_BSS_NUM; i++) {
        TEST_ASSERT(data_in_segment(&s_bss_buffer[i], &_ext_ram_bss_start, &_ext_ram_bss_end));
        TEST_ASSERT_EQUAL(0, s_bss_buffer[i]);
    }
}
#endif  //#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
