#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "unity.h"
#include "esp_pm.h"
#include "esp_clk.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

TEST_CASE("Can dump power management lock stats", "[pm]")
{
    esp_pm_dump_locks(stdout);
}

#ifdef CONFIG_PM_ENABLE

static void switch_freq(int mhz)
{
    rtc_cpu_freq_t max_freq;
    assert(rtc_clk_cpu_freq_from_mhz(mhz, &max_freq));
    esp_pm_config_esp32_t pm_config = {
        .max_cpu_freq = max_freq,
        .min_cpu_freq = RTC_CPU_FREQ_XTAL,
    };
    ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
    printf("Waiting for frequency to be set to %d (%d MHz)...\n", max_freq, mhz);
    while (esp_clk_cpu_freq() / 1000000 != mhz) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("Frequency is %d MHz\n", esp_clk_cpu_freq());
    }
    printf("Frequency is set to %d MHz\n", mhz);
}

TEST_CASE("Can switch frequency using esp_pm_configure", "[pm]")
{
    int orig_freq_mhz = esp_clk_cpu_freq() / 1000000;
    switch_freq(240);
    switch_freq(40);
    switch_freq(160);
    switch_freq(240);
    switch_freq(80);
    switch_freq(40);
    switch_freq(240);
    switch_freq(40);
    switch_freq(80);
    switch_freq(orig_freq_mhz);
}

#endif // CONFIG_PM_ENABLE
