#include <stdio.h>
#include <math.h>
#include "unity.h"
#include "driver/adc.h"
#include <time.h>
#include <sys/time.h>
#include "soc/rtc_cntl_reg.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "sdkconfig.h"


#if portNUM_PROCESSORS == 2

// https://github.com/espressif/arduino-esp32/issues/120
TEST_CASE("Reading RTC registers on APP CPU doesn't affect clock", "[newlib]")
{
    // This runs on APP CPU:
    void time_adc_test_task(void* arg)
    {
        for (int i = 0; i < 200000; ++i) {
            // wait for 20us, reading one of RTC registers
            uint32_t ccount = xthal_get_ccount();
            while (xthal_get_ccount() - ccount < 20 * CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ) {
                volatile uint32_t val = REG_READ(RTC_CNTL_STATE0_REG);
                (void) val;
            }
        }
        SemaphoreHandle_t * p_done = (SemaphoreHandle_t *) arg;
        xSemaphoreGive(*p_done);
        vTaskDelay(1);
        vTaskDelete(NULL);
    }

    SemaphoreHandle_t done = xSemaphoreCreateBinary();
    xTaskCreatePinnedToCore(&time_adc_test_task, "time_adc", 4096, &done, 5, NULL, 1);

    // This runs on PRO CPU:
    for (int i = 0; i < 4; ++i) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        struct timeval tv_stop;
        gettimeofday(&tv_stop, NULL);
        float time_sec = tv_stop.tv_sec - tv_start.tv_sec + 1e-6f * (tv_stop.tv_usec - tv_start.tv_usec);
        printf("(0) time taken: %f sec\n", time_sec);
        TEST_ASSERT_TRUE(fabs(time_sec - 1.0f) < 0.1);
    }
    TEST_ASSERT_TRUE(xSemaphoreTake(done, 5000 / portTICK_RATE_MS));
}

#endif // portNUM_PROCESSORS == 2
