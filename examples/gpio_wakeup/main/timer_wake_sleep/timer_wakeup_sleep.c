#include "esp_check.h"
#include "esp_sleep.h"

#define TIMER_SLEEP_TIME_US    (2 * 1000 * 1000)

static const char *TAG = "timer_wakeup_sleep";

esp_err_t example_register_timer_sleep(void)
{
    //ESP_RETURN_ON_ERROR(esp_sleep_enable_timer_wakeup(TIMER_WAKEUP_TIME_US), TAG, "Configure timer as wakeup source failed");
    ESP_LOGI(TAG, "timer sleep is ready");
    return ESP_OK;
}
