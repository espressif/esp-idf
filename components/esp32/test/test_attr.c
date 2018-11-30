#include "unity.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "soc/soc.h"

static __NOINIT_ATTR uint32_t s_noinit;
static RTC_NOINIT_ATTR uint32_t s_rtc_noinit;
static RTC_DATA_ATTR uint32_t s_rtc_data;
static RTC_RODATA_ATTR uint32_t s_rtc_rodata;
static RTC_FAST_ATTR uint32_t s_rtc_force_fast;
static RTC_SLOW_ATTR uint32_t s_rtc_force_slow;

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


static bool data_in_segment(void *ptr, int *seg_start, int *seg_end)
{
    return ((intptr_t)ptr < (intptr_t)seg_end) && \
           ((intptr_t)ptr >= (intptr_t)seg_start);
}

TEST_CASE("Attributes place variables into correct sections", "[ld]")
{
    TEST_ASSERT(data_in_segment(&s_noinit, &_noinit_start, &_noinit_end));
    TEST_ASSERT(data_in_segment(&s_rtc_noinit, &_rtc_noinit_start, &_rtc_noinit_end));
    TEST_ASSERT(data_in_segment(&s_rtc_data, &_rtc_data_start, &_rtc_data_end));
    TEST_ASSERT(data_in_segment(&s_rtc_rodata, &_rtc_data_start, &_rtc_data_end));
    TEST_ASSERT(data_in_segment(&s_rtc_force_fast, &_rtc_force_fast_start, &_rtc_force_fast_end));
    TEST_ASSERT(data_in_segment(&s_rtc_force_slow, &_rtc_force_slow_start, &_rtc_force_slow_end));

#ifndef CONFIG_ESP32_RTCDATA_IN_FAST_MEM
    TEST_ASSERT(data_in_segment(&s_rtc_data, (int*) SOC_RTC_DATA_LOW, (int*) SOC_RTC_DATA_HIGH));
    TEST_ASSERT(data_in_segment(&s_rtc_rodata, (int*) SOC_RTC_DATA_LOW, (int*) SOC_RTC_DATA_HIGH));
    TEST_ASSERT(data_in_segment(&s_rtc_noinit, (int*) SOC_RTC_DATA_LOW, (int*) SOC_RTC_DATA_HIGH));
#else
    TEST_ASSERT(data_in_segment(&s_rtc_data, (int*) SOC_RTC_DRAM_LOW, (int*) SOC_RTC_DRAM_HIGH));
    TEST_ASSERT(data_in_segment(&s_rtc_rodata, (int*) SOC_RTC_DRAM_LOW, (int*) SOC_RTC_DRAM_HIGH));
    TEST_ASSERT(data_in_segment(&s_rtc_noinit, (int*) SOC_RTC_DRAM_LOW, (int*) SOC_RTC_DRAM_HIGH));
#endif

    TEST_ASSERT(data_in_segment(&s_rtc_force_fast, (int*) SOC_RTC_DRAM_LOW, (int*) SOC_RTC_DRAM_HIGH));
    TEST_ASSERT(data_in_segment(&s_rtc_force_slow, (int*) SOC_RTC_DATA_LOW, (int*) SOC_RTC_DATA_HIGH));
}
