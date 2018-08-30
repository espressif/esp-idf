#include "unity.h"
#include "esp_attr.h"
#include "esp_log.h"

static __NOINIT_ATTR uint32_t s_noinit;
static RTC_NOINIT_ATTR uint32_t s_rtc_noinit;
static RTC_DATA_ATTR uint32_t s_rtc_data;

extern int _rtc_noinit_start;
extern int _rtc_noinit_end;
extern int _rtc_data_start;
extern int _rtc_data_end;
extern int _noinit_start;
extern int _noinit_end;

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
}
