/* Low_power_management example : Automatic light and deep sleep implementation

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this software is distributed on an "AS
   IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/

/*
   This example shows how to use power saving mode
   It uses the automatic light and deep sleep functions implemented in the pm_esp32.c file.
   Depending on the type of lock acquired or released, it switches between different modes : Active,
   Light sleep and Deep sleep. The wakeup source is the timer defined in the pm_esp32.c file.
   It is important to note that CONFIG_PM_ENABLE and CONFIG_FREERTOS_USE_TICKLESS_IDLE should be set
   to 1 in order to enable automatic light and deep sleep (cf make menuconfig).
*/

#include "esp32/rom/uart.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#ifdef CONFIG_PM_ENABLE
static esp_pm_lock_handle_t s_lock_handle_light_sleep;
static esp_pm_lock_handle_t s_lock_handle_deep_sleep;

/* Deep sleep enter time is saved in the RTC memory */
static RTC_DATA_ATTR struct timeval s_deep_sleep_enter_time;
#endif

static const char* TAG = "LOW_POWER_MANAGEMENT";

/**
 * @brief Switch to different power saving modes
 *
 * Depending on the lock, the chip goes into active, light sleep or deep sleep mode.
 *
 */
static void power_save(void);

static void power_save(void)
{
#if CONFIG_PM_ENABLE
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE

    /* If the the wakeup source is the timer, log the time spent in deep sleep. Skipped on boot */
    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER) {
        struct timeval now;
        gettimeofday(&now, NULL);
        int deep_sleep_time_ms = (now.tv_sec - s_deep_sleep_enter_time.tv_sec) * 1000
            + (now.tv_usec - s_deep_sleep_enter_time.tv_usec) / 1000;

        ESP_LOGI(TAG,
            "Wake up from deep sleep due to timer. Time spent in deep sleep: "
            "%dms\n",
            deep_sleep_time_ms);
    }

    /* ---------------------------- Active mode ----------------------- */

    ESP_ERROR_CHECK(esp_pm_lock_acquire(s_lock_handle_light_sleep));

    /* Light sleep lock is acquired to prevent the chip going into light sleep or lower mode */
    ESP_LOGI(TAG, "Entering ACTIVE MODE : all the features of the chip are active.\n");
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    /* ---------------------------- Light sleep ----------------------- */

    ESP_ERROR_CHECK(esp_pm_lock_acquire(s_lock_handle_deep_sleep));

    /* Deep sleep lock is acquired before releasing light sleep lock handle to allow the chip going
     * into light sleep */
    ESP_ERROR_CHECK(esp_pm_lock_release(s_lock_handle_light_sleep));
    ESP_LOGI(TAG, "Entering LIGHT SLEEP");

    /* To make sure the complete line is printed before entering sleep mode, need to wait until UART
     * TX FIFO is empty */
    uart_tx_wait_idle(CONFIG_CONSOLE_UART_NUM);

    /* Save the time in microseconds before going into light sleep */
    int64_t light_sleep_enter_time_us = esp_timer_get_time();
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    /* Save the time in microseconds when exiting light sleep */
    int64_t light_sleep_exit_time_us = esp_timer_get_time();

    ESP_LOGI(TAG,
        "Returned from light sleep due to timer. Time spent in light sleep "
        "%lld ms\n",
        (light_sleep_exit_time_us - light_sleep_enter_time_us) / 1000);

    /* ---------------------------- Deep sleep ----------------------- */

    ESP_ERROR_CHECK(esp_pm_lock_release(s_lock_handle_deep_sleep));

    /* Deep sleep lock is released to allow the chip going into deep sleep */
    ESP_LOGI(TAG, "Entering DEEP SLEEP");
    uart_tx_wait_idle(CONFIG_CONSOLE_UART_NUM);

    /* Save the time in microseconds before going into deep sleep */
    gettimeofday(&s_deep_sleep_enter_time, NULL);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
#endif
#endif
}

void app_main()
{
#if CONFIG_PM_ENABLE
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE

    /* Initialize a lock handle for light sleep */
    if (s_lock_handle_light_sleep == NULL) {
        esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "Light Sleep", &s_lock_handle_light_sleep);
    }

    /* Initialize a lock handle for deep sleep */
    if (s_lock_handle_deep_sleep == NULL) {
        esp_pm_lock_create(ESP_PM_NO_DEEP_SLEEP, 0, "Deep Sleep", &s_lock_handle_deep_sleep);
    }

    /* Automatic light and deep sleep are enabled if tickless idle support is enabled. */
    esp_pm_config_esp32_t pm_config = {
        .light_sleep_enable = true,
        .deep_sleep_enable = true,
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));

    ESP_LOGI(TAG, "Entering different power saving modes\n");
    power_save();
#endif /* CONFIG_FREERTOS_USE_TICKLESS_IDLE */
#endif /* CONFIG_PM_ENABLE */
}
