#include "unity.h"
#include "esp_deep_sleep.h"
#include "driver/rtc_io.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TEST_CASE("esp_deepsleep works", "[deepsleep]")
{
    esp_deep_sleep(2000000);
}

static void deep_sleep_task(void* arg)
{
    esp_deep_sleep_start();
}

static void do_deep_sleep_from_app_cpu()
{
    xTaskCreatePinnedToCore(&deep_sleep_task, "ds", 2048, NULL, 5, NULL, 1);

    // keep running some non-IRAM code
    vTaskSuspendAll();
    while(true) {
        ;
    }
}

TEST_CASE("can wake up from deep sleep using timer", "[deepsleep]")
{
    esp_deep_sleep_enable_timer_wakeup(2000000);
    esp_deep_sleep_start();
}

TEST_CASE("go into deep sleep from APP CPU and wake up using timer", "[deepsleep]")
{
    esp_deep_sleep_enable_timer_wakeup(2000000);
    do_deep_sleep_from_app_cpu();
}


TEST_CASE("can wake up from deep sleep using ext0 (13 high)", "[deepsleep]")
{
    ESP_ERROR_CHECK(rtc_gpio_init(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pullup_dis(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pulldown_en(GPIO_NUM_13));
    ESP_ERROR_CHECK(esp_deep_sleep_enable_ext0_wakeup(GPIO_NUM_13, 1));
    esp_deep_sleep_start();
}

TEST_CASE("can wake up from deep sleep using ext0 (13 low)", "[deepsleep]")
{
    ESP_ERROR_CHECK(rtc_gpio_init(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pullup_en(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pulldown_dis(GPIO_NUM_13));
    ESP_ERROR_CHECK(esp_deep_sleep_enable_ext0_wakeup(GPIO_NUM_13, 0));
    esp_deep_sleep_start();
}

TEST_CASE("can wake up from deep sleep using ext1 (13 high)", "[deepsleep]")
{
    ESP_ERROR_CHECK(rtc_gpio_init(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pullup_dis(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pulldown_en(GPIO_NUM_13));
    ESP_ERROR_CHECK(esp_deep_sleep_enable_ext1_wakeup(BIT(GPIO_NUM_13), ESP_EXT1_WAKEUP_ANY_HIGH));
    esp_deep_sleep_start();
}

TEST_CASE("can wake up from deep sleep using ext1 (13 low)", "[deepsleep]")
{
    ESP_ERROR_CHECK(rtc_gpio_init(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pullup_en(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pulldown_dis(GPIO_NUM_13));
    ESP_ERROR_CHECK(esp_deep_sleep_enable_ext1_wakeup(BIT(GPIO_NUM_13), ESP_EXT1_WAKEUP_ALL_LOW));
    esp_deep_sleep_start();
}
