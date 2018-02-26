#include "unity.h"
#include "esp_system.h"
#include "rom/rtc.h"
#include "esp_log.h"

// This is a test sequence to test behavior of .rtc_noinit and .noinit sections.
// The values placed into .rtc_noinit section go to RTC SLOW Memory segment and
// keep their value after reset and deep sleep. Use new added attribute macro
// RTC_NOINIT_ATTR for this behavior. The second macro - __NOINIT_ATTR places value
// into .noinit section which goes to SRAM and will not be initialized after reset.

#define RTC_NOINIT_PATTERN 0xAAAAAAAA
#define _NOINIT_PATTERN 0x55555555

static __NOINIT_ATTR uint32_t noinit_data;
static RTC_NOINIT_ATTR uint32_t rtc_noinit_data;

extern int _rtc_noinit_start;
extern int _rtc_noinit_end;
extern int _noinit_start;
extern int _noinit_end;

// Pointers to the values
uint32_t *noinit_val_addr = (uint32_t*)&noinit_data;
uint32_t *rtc_noinit_val_addr = (uint32_t*)&rtc_noinit_data;

static const char* tag = "noinit_UnitTestMain";

static esp_err_t check_data_seg(uint32_t *value_address, \
                                        uint32_t *seg_start, uint32_t *seg_end)
{
    esp_err_t result = ESP_FAIL;
    if (((uint32_t)value_address <= (uint32_t)seg_end) && \
        ((uint32_t)value_address >= (uint32_t)seg_start)){
        result = ESP_OK;
    }
    return result;
}

static void setup_attributes(void)
{
    rtc_noinit_data = RTC_NOINIT_PATTERN;
    noinit_data = _NOINIT_PATTERN;
}

static void init_attributes(void)
{
    setup_attributes();
    printf("noinit_data = 0x%X \n", (uint32_t)*noinit_val_addr);
    printf("rtc_noinit_data = 0x%X \n", (uint32_t)*rtc_noinit_val_addr);
    TEST_ASSERT(*noinit_val_addr == noinit_data);
    TEST_ASSERT(*rtc_noinit_val_addr == rtc_noinit_data);
}

static void reset_reason_power_on(void)
{
    printf("This test case checks behavior of noinit variables POWERON_RESET sequence. \n");
    RESET_REASON reason = rtc_get_reset_reason(0);
    ESP_LOGI(tag, "POWERON_RESET reset values = (0x%X), (0x%X), reset reason=(%d)\n", \
                (uint32_t)*noinit_val_addr, (uint32_t)*rtc_noinit_val_addr, (uint16_t)reason);
    TEST_ASSERT((reason == POWERON_RESET) || (reason == RTCWDT_RTC_RESET));

    init_attributes();
    TEST_ASSERT(check_data_seg(noinit_val_addr, \
                                    (uint32_t*)&_noinit_start, \
                                    (uint32_t*)&_noinit_end) == ESP_OK);
    TEST_ASSERT(check_data_seg(rtc_noinit_val_addr, \
                                    (uint32_t*)&_rtc_noinit_start, \
                                    (uint32_t*)&_rtc_noinit_end) == ESP_OK);
    TEST_ASSERT(_NOINIT_PATTERN == *noinit_val_addr);
    TEST_ASSERT(RTC_NOINIT_PATTERN == *rtc_noinit_val_addr);

    printf("Next test case will check SOFTWARE_RESET behavior. \n");
    esp_restart();
}

static void reset_reason_sw_reset(void)
{
    printf("This test case checks behavior of noinit variables after software reset sequence. \n");
    RESET_REASON reason = rtc_get_reset_reason(0);
    ESP_LOGI(tag, "SW_CPU_RESET reset values = (0x%X), (0x%X), reset reason=(%d)\n", \
                (uint32_t)*noinit_val_addr, (uint32_t)*rtc_noinit_val_addr, (uint16_t)reason);
    TEST_ASSERT(reason == SW_CPU_RESET);
    TEST_ASSERT(check_data_seg(noinit_val_addr, \
                                    (uint32_t*)&_noinit_start, \
                                    (uint32_t*)&_noinit_end) == ESP_OK);
    TEST_ASSERT(check_data_seg(rtc_noinit_val_addr, \
                                    (uint32_t*)&_rtc_noinit_start, \
                                    (uint32_t*)&_rtc_noinit_end) == ESP_OK);
    // The ROM bootloader behavior may apply to this assert.
    // TEST_ASSERT(0x55555555 == *noinit_val_addr);
    TEST_ASSERT(RTC_NOINIT_PATTERN == *rtc_noinit_val_addr);
    printf("Go to deep sleep to check DEEP_SLEEP_RESET behavior. \n");
    esp_sleep_enable_timer_wakeup(2000000);
    esp_deep_sleep_start();
}

static void reset_reason_deep_sleep(void)
{
    printf("This test case checks behavior of noinit variables after deep sleep reset. \n");
    RESET_REASON reason = rtc_get_reset_reason(0);
    ESP_LOGI(tag, "DEEP_SLEEP_RESET reset values = (0x%X), (0x%X), reset reason=(%d)\n", \
                (uint32_t)*noinit_val_addr, (uint32_t)*rtc_noinit_val_addr, (uint16_t)reason);
    TEST_ASSERT(reason == DEEPSLEEP_RESET);
    TEST_ASSERT(check_data_seg(noinit_val_addr, \
                                    (uint32_t*)&_noinit_start, \
                                    (uint32_t*)&_noinit_end) == ESP_OK);
    TEST_ASSERT(check_data_seg(rtc_noinit_val_addr, \
                                    (uint32_t*)&_rtc_noinit_start, \
                                    (uint32_t*)&_rtc_noinit_end) == ESP_OK);
    TEST_ASSERT(RTC_NOINIT_PATTERN == *rtc_noinit_val_addr);
    printf("The noinit test cases are done.. \n");
}

// The lines below are required to suppress GCC warnings about casting of function pointers
// in unity macro expansion. These warnings may be treated as errors during automated test.
#pragma GCC diagnostic push  // required for GCC
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
// The multiple stages test case to check values after certain reset reason
TEST_CASE_MULTIPLE_STAGES("NOINIT attributes behavior", 
                            "[restart][reset=SW_CPU_RESET, DEEPSLEEP_RESET]", 
                            reset_reason_power_on, reset_reason_sw_reset, reset_reason_deep_sleep);
#pragma GCC diagnostic pop   // require GCC
