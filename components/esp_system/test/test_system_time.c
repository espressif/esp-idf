#include <stdio.h>
#include "unity.h"

#include "esp_private/system_internal.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/clk.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/clk.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/clk.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/clk.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/clk.h"
#endif

TEST_CASE("Test effect of rtc clk calibration compensation on system time", "[esp_system]")
{
    uint32_t prev_cal = esp_clk_slowclk_cal_get();
    int64_t t1 = esp_system_get_time();

    // Modify calibration value
    esp_clk_slowclk_cal_set(prev_cal/2);

    // Internally, the origin point of rtc clk has been adjusted
    // so that t2 > t1 remains true
    int64_t t2 = esp_system_get_time();

    TEST_ASSERT_GREATER_THAN(t1, t2);

    // Restore calibration value
    esp_clk_slowclk_cal_set(prev_cal);

    t2 = esp_system_get_time();

    TEST_ASSERT_GREATER_THAN(t1, t2);
}
