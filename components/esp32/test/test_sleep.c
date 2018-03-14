#include "unity.h"
#include <sys/time.h>
#include "esp_sleep.h"
#include "driver/rtc_io.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ESP_EXT0_WAKEUP_LEVEL_LOW 0
#define ESP_EXT0_WAKEUP_LEVEL_HIGH 1

TEST_CASE("esp_deepsleep works", "[deepsleep][reset=DEEPSLEEP_RESET]")
{
    esp_deep_sleep(2000000);
}

static void deep_sleep_task(void *arg)
{
    esp_deep_sleep_start();
}

static void do_deep_sleep_from_app_cpu()
{
    xTaskCreatePinnedToCore(&deep_sleep_task, "ds", 2048, NULL, 5, NULL, 1);

    // keep running some non-IRAM code
    vTaskSuspendAll();

    while (true) {
        ;
    }
}

TEST_CASE("wake up using timer", "[deepsleep][reset=DEEPSLEEP_RESET]")
{
    esp_sleep_enable_timer_wakeup(2000000);
    esp_deep_sleep_start();
}

TEST_CASE("wake up from light sleep using timer", "[deepsleep]")
{
    esp_sleep_enable_timer_wakeup(2000000);
    struct timeval tv_start, tv_stop;
    gettimeofday(&tv_start, NULL);
    esp_light_sleep_start();
    gettimeofday(&tv_stop, NULL);
    float dt = (tv_stop.tv_sec - tv_start.tv_sec) * 1e3f +
               (tv_stop.tv_usec - tv_start.tv_usec) * 1e-3f;
    TEST_ASSERT_INT32_WITHIN(500, 2000, (int) dt);
}

TEST_CASE("wake up disable timer for ext0 wakeup (13 low)", "[deepsleep][ignore]")
{
    // Setup timer to wakeup with timeout
    esp_sleep_enable_timer_wakeup(2000000);
    struct timeval tv_start, tv_stop;
    gettimeofday(&tv_start, NULL);
    esp_light_sleep_start();
    gettimeofday(&tv_stop, NULL);
    float dt = (tv_stop.tv_sec - tv_start.tv_sec) * 1e3f +
               (tv_stop.tv_usec - tv_start.tv_usec) * 1e-3f;
    printf("Timer sleep time = %d\r\n", (int)dt);
    
    TEST_ASSERT_INT32_WITHIN(500, 2000, (int) dt);
    
    // Setup ext0 configuration to wake up  
    ESP_ERROR_CHECK(rtc_gpio_init(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pullup_en(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pulldown_dis(GPIO_NUM_13));
    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, ESP_EXT0_WAKEUP_LEVEL_LOW));
    
    // Disable timer wakeup trigger to wakeup from ext0 source
    // instead of timer wakeup
    ESP_ERROR_CHECK(esp_sleep_disable_timer_wakeup());
    printf("Waiting low level on GPIO_13\r\n");
    
    gettimeofday(&tv_start, NULL);
    esp_light_sleep_start();
    gettimeofday(&tv_stop, NULL);
    
    dt = (tv_stop.tv_sec - tv_start.tv_sec) * 1e3f +
               (tv_stop.tv_usec - tv_start.tv_usec) * 1e-3f;
    printf("Ext0 sleep time = %d\r\n", (int)dt);
    
    // Check error message 
    esp_err_t err_code = esp_sleep_disable_timer_wakeup();
    TEST_ASSERT(err_code == ESP_ERR_INVALID_STATE);
}

#ifndef CONFIG_FREERTOS_UNICORE
TEST_CASE("enter deep sleep on APP CPU and wake up using timer", "[deepsleep][reset=DEEPSLEEP_RESET]")
{
    esp_sleep_enable_timer_wakeup(2000000);
    do_deep_sleep_from_app_cpu();
}
#endif


TEST_CASE("wake up using ext0 (13 high)", "[deepsleep][ignore]")
{
    ESP_ERROR_CHECK(rtc_gpio_init(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pullup_dis(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pulldown_en(GPIO_NUM_13));
    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, ESP_EXT0_WAKEUP_LEVEL_HIGH));
    esp_deep_sleep_start();
}

TEST_CASE("wake up using ext0 (13 low)", "[deepsleep][ignore]")
{
    ESP_ERROR_CHECK(rtc_gpio_init(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pullup_en(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pulldown_dis(GPIO_NUM_13));
    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, ESP_EXT0_WAKEUP_LEVEL_LOW));
    esp_deep_sleep_start();
}

TEST_CASE("wake up using ext1 when RTC_PERIPH is off (13 high)", "[deepsleep][ignore]")
{
    // This test needs external pulldown
    ESP_ERROR_CHECK(rtc_gpio_init(GPIO_NUM_13));
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(BIT(GPIO_NUM_13), ESP_EXT1_WAKEUP_ANY_HIGH));
    esp_deep_sleep_start();
}

TEST_CASE("wake up using ext1 when RTC_PERIPH is off (13 low)", "[deepsleep][ignore]")
{
    // This test needs external pullup
    ESP_ERROR_CHECK(rtc_gpio_init(GPIO_NUM_13));
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(BIT(GPIO_NUM_13), ESP_EXT1_WAKEUP_ALL_LOW));
    esp_deep_sleep_start();
}

TEST_CASE("wake up using ext1 when RTC_PERIPH is on (13 high)", "[deepsleep][ignore]")
{
    ESP_ERROR_CHECK(rtc_gpio_init(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pullup_dis(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pulldown_en(GPIO_NUM_13));
    ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON));
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(BIT(GPIO_NUM_13), ESP_EXT1_WAKEUP_ANY_HIGH));
    esp_deep_sleep_start();
}

TEST_CASE("wake up using ext1 when RTC_PERIPH is on (13 low)", "[deepsleep][ignore]")
{
    ESP_ERROR_CHECK(rtc_gpio_init(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pullup_en(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pulldown_dis(GPIO_NUM_13));
    ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON));
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(BIT(GPIO_NUM_13), ESP_EXT1_WAKEUP_ALL_LOW));
    esp_deep_sleep_start();
}
