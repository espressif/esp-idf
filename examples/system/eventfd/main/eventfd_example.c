/* eventfd example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <sys/select.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include "esp_vfs_eventfd.h"
#include "esp_timer.h"
#include "driver/gptimer.h"

#define TIMER_RESOLUTION      1000000 // 1MHz, 1 tick = 1us
#define TIMER_INTERVAL_US     2500000 // 2.5s
#define PROGRESS_INTERVAL_MS  3500
#define TIMER_SIGNAL          1
#define PROGRESS_SIGNAL       2
/* Print the signals value a bit before 5 seconds, else, `select` would run again
 * and the following print may happen 2 seconds (timeout value) later. */
#define PRINT_INTERVAL_MSEC    4990

static const char *TAG = "eventfd_example";

static int s_timer_fd;
static int s_progress_fd;
static TaskHandle_t s_worker_handle;
static gptimer_handle_t s_gptimer;

static bool eventfd_timer_isr_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    uint64_t signal = TIMER_SIGNAL;
    ssize_t val = write(s_timer_fd, &signal, sizeof(signal));
    assert(val == sizeof(signal));

    return true;
}

static void eventfd_timer_init(void)
{
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = TIMER_RESOLUTION,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &s_gptimer));

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = TIMER_INTERVAL_US,
        .flags.auto_reload_on_alarm = true,
    };
    gptimer_event_callbacks_t cbs = {
        .on_alarm = eventfd_timer_isr_callback,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(s_gptimer, &cbs, NULL));
    ESP_ERROR_CHECK(gptimer_set_alarm_action(s_gptimer, &alarm_config));
    ESP_ERROR_CHECK(gptimer_enable(s_gptimer));
    ESP_ERROR_CHECK(gptimer_start(s_gptimer));
}

static void worker_task(void *arg)
{
    /* Wait for the collector to be ready. */
    ulTaskNotifyTake(true, portMAX_DELAY);

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(PROGRESS_INTERVAL_MS));
        uint64_t signal = PROGRESS_SIGNAL;
        ssize_t val = write(s_progress_fd, &signal, sizeof(signal));
        assert(val == sizeof(signal));
    }
    vTaskDelete(NULL);
}

static void collector_task(void *arg)
{
    esp_vfs_eventfd_config_t config = ESP_VFS_EVENTD_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_vfs_eventfd_register(&config));

    s_timer_fd = eventfd(0, EFD_SUPPORT_ISR);
    s_progress_fd = eventfd(0, 0);
    assert(s_timer_fd > 0);
    assert(s_progress_fd > 0);

    int maxFd = s_progress_fd > s_timer_fd ? s_progress_fd : s_timer_fd;
    int select_timeout_count = 0;
    int timer_trigger_count = 0;
    int progress_trigger_count = 0;

    /* Notify the worker we are ready to catch the signals */
    assert( xTaskNotifyGive(s_worker_handle) == pdPASS );
    uint64_t start = esp_timer_get_time();
    uint64_t previous = start;

    while (1) {
        struct timeval timeout;
        uint64_t signal;

        timeout.tv_sec = 2;
        timeout.tv_usec = 0;

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(s_timer_fd, &readfds);
        FD_SET(s_progress_fd, &readfds);

        int num_triggered = select(maxFd + 1, &readfds, NULL, NULL, &timeout);
        assert(num_triggered >= 0);

        ESP_LOGI(TAG, "Elapsed since test start: %lld ms", (esp_timer_get_time() - start) / 1000);

        if (FD_ISSET(s_progress_fd, &readfds)) {
            ssize_t ret = read(s_progress_fd, &signal, sizeof(signal));
            assert(ret == sizeof(signal));
            assert(signal == PROGRESS_SIGNAL);
            progress_trigger_count++;
            ESP_LOGI(TAG, "Progress fd event triggered");
        }
        if (FD_ISSET(s_timer_fd, &readfds)) {
            ssize_t ret = read(s_timer_fd, &signal, sizeof(signal));
            assert(ret == sizeof(signal));
            assert(signal == TIMER_SIGNAL);
            timer_trigger_count++;
            ESP_LOGI(TAG, "TimerEvent fd event triggered");
        }
        if (num_triggered == 0) {
            select_timeout_count++;
            ESP_LOGI(TAG, "Select timeout");
        }

        /* Print information about received events every PRINT_INTERVAL_MSEC milliseconds. */
        const uint64_t current = esp_timer_get_time();
        const uint64_t elapsed = current - previous;
        if (elapsed >= PRINT_INTERVAL_MSEC * 1000) {
            ESP_LOGI(TAG, "=================================");
            ESP_LOGI(TAG, "Select timeouted for %d times", select_timeout_count);
            ESP_LOGI(TAG, "Timer triggered for %d times", timer_trigger_count);
            ESP_LOGI(TAG, "Progress triggered for %d times", progress_trigger_count);
            ESP_LOGI(TAG, "=================================");
            previous = current;
        }
    }

    gptimer_stop(s_gptimer);
    gptimer_disable(s_gptimer);
    gptimer_del_timer(s_gptimer);
    close(s_timer_fd);
    close(s_progress_fd);
    esp_vfs_eventfd_unregister();
    vTaskDelete(NULL);
}

void app_main(void)
{
    eventfd_timer_init();
    /* Save the handle for this task as we will need to notify it */
    xTaskCreate(worker_task, "worker_task", 4 * 1024, NULL, 5, &s_worker_handle);
    xTaskCreate(collector_task, "collector_task", 4 * 1024, NULL, 5, NULL);
}
