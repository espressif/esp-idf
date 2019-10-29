#include <unistd.h>
#include "unity.h"
#include "soc/rtc_cntl_reg.h"
#include "esp32/ulp.h"
#include "ulp_test_app.h"


extern const uint8_t ulp_test_app_bin_start[] asm("_binary_ulp_test_app_bin_start");
extern const uint8_t ulp_test_app_bin_end[]   asm("_binary_ulp_test_app_bin_end");


TEST_CASE("jumps condition", "[ulp]")
{
    esp_err_t err = ulp_load_binary(0, ulp_test_app_bin_start,
            (ulp_test_app_bin_end - ulp_test_app_bin_start) / sizeof(uint32_t));
    TEST_ESP_OK(err);

    REG_CLR_BIT(RTC_CNTL_INT_RAW_REG, RTC_CNTL_ULP_CP_INT_RAW);
    TEST_ESP_OK(ulp_run(&ulp_test_jumps - RTC_SLOW_MEM));
    usleep(10000);

    TEST_ASSERT_NOT_EQUAL(0, REG_GET_BIT(RTC_CNTL_INT_RAW_REG, RTC_CNTL_ULP_CP_INT_RAW));
    TEST_ASSERT_EQUAL(0, ulp_jumps_fail & UINT16_MAX);
    TEST_ASSERT_EQUAL(1, ulp_jumps_pass & UINT16_MAX);
}
