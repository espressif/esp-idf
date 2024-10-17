/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdatomic.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "basic_freertos_smp_usage.h"

#define SHARE_RES_THREAD_NUM     2
#define ITERATION_NUMBER         100000


// declare a static global integer as a protected shared resource that is accessible to multiple tasks
static volatile int s_global_num = 0;
static atomic_int s_atomic_global_num;
static SemaphoreHandle_t s_mutex;
static portMUX_TYPE s_spinlock = portMUX_INITIALIZER_UNLOCKED;
static volatile bool timed_out;
const static char *TAG = "lock example";

// Take a mutex to protect the shared resource. If mutex is already taken, this task will be blocked until it is available;
// when the mutex is available, FreeRTOS will reschedule this task and this task can further access the shared resource
static void inc_num_mutex_iter(void *arg)
{
    int core_id = esp_cpu_get_core_id();
    int64_t start_time, end_time, duration = 0;
    start_time = esp_timer_get_time();
    while (s_global_num < ITERATION_NUMBER) {
        if (xSemaphoreTake(s_mutex, portMAX_DELAY) == pdTRUE) {
            s_global_num++;
            xSemaphoreGive(s_mutex);
        }
    }
    end_time = esp_timer_get_time();
    duration = end_time - start_time;
    ESP_LOGI(TAG, "mutex task took %lld us on core%d", duration, core_id);

    vTaskDelete(NULL);
}

// Enter a critical section and take a spinlock to protect the shared resource. If the spinlock is already taken, this task busy-wait here until it is available.
// In contrast to the mutex, when in a critical section interrupts are disabled, which means nothing will interrupt the task and the freertos scheduler will never run
// and reschedule the task.
static void inc_num_spinlock_iter(void *arg)
{
    int core_id = esp_cpu_get_core_id();
    int64_t start_time, end_time, duration = 0;
    start_time = esp_timer_get_time();
    while (s_global_num < ITERATION_NUMBER) {
        portENTER_CRITICAL(&s_spinlock);
        s_global_num++;
        portEXIT_CRITICAL(&s_spinlock);
    }
    end_time = esp_timer_get_time();
    duration = end_time - start_time;
    ESP_LOGI(TAG, "spinlock task took %lld us on core%d", duration, core_id);

    vTaskDelete(NULL);
}

static void inc_num_atomic_iter(void *arg)
{
    int core_id = esp_cpu_get_core_id();
    int64_t start_time, end_time, duration = 0;
    start_time = esp_timer_get_time();
    while (atomic_load(&s_atomic_global_num) < ITERATION_NUMBER) {
        atomic_fetch_add(&s_atomic_global_num, 1);
    }
    end_time = esp_timer_get_time();
    duration = end_time - start_time;
    ESP_LOGI(TAG, "atomic task took %lld us on core%d", duration, core_id);

    vTaskDelete(NULL);
}

static void inc_num_mutex(void *arg)
{
    int task_index = (int)arg;
    ESP_LOGI(TAG, "mutex task %d created", task_index);

    while (!timed_out) {
        xSemaphoreTake(s_mutex, portMAX_DELAY); // == pdTRUE

        int core_id = esp_cpu_get_core_id();
        ESP_LOGI(TAG, "task%d read value = %d on core #%d", task_index, s_global_num, core_id);
        s_global_num++;
        // delay for 500 ms
        vTaskDelay(pdMS_TO_TICKS(500));
        xSemaphoreGive(s_mutex);
        ESP_LOGI(TAG, "task%d set value = %d", task_index, s_global_num);
    }

    vTaskDelete(NULL);
}


/* Lock example: show how to use mutex and spinlock to protect shared resources

Firstly, a shared resource `s_global_num` is protected by a mutex and there are 2 tasks,
whose task function is `inc_num_mutex_iter`, take turns to access and increase this number.
Once the number value reaches 100000, the time duration from starting running till the
current time is measured and recorded, then both these 2 tasks will be deleted.

Next, `s_global_num` is reset and there are another 2 tasks, calling task function
`inc_num_spinlock_iter`, that access and increase this shared resource until it reaches
100000, under the protection of a spinlock. The expected result is these 2 tasks will have
less time overhead in comparison with the previous 2 tasks because they involve less context
switching for task execution.

After that, another 2 tasks are create to complete the same
addition job, but the shared resource is an atomic type integer. It should have a shorter
running time than the spinlock tasks, because atomic operation is a kind of look-free implementation
and it saves the time of entering and exiting the critical section.

Note: if this example runs on single core, only 1 task of each type will be created.

Finally, it illustrates show the shared resource `s_global_num` is protected by a mutex
and in turn accessed by multiple tasks. */
int comp_lock_entry_func(int argc, char **argv)
{
    s_global_num = 0;
    int thread_id;
    int core_id;

    timed_out = false;

    // create mutex
    s_mutex = xSemaphoreCreateMutex();
    if (s_mutex == NULL) {
        ESP_LOGE(TAG, SEM_CREATE_ERR_STR);
        return 1;
    }

    // create 2 tasks accessing a shared resource protected by mutex
    for (core_id = 0; core_id < CONFIG_FREERTOS_NUMBER_OF_CORES; core_id++) {
        xTaskCreatePinnedToCore(inc_num_mutex_iter, NULL, 4096, NULL, TASK_PRIO_3, NULL, core_id);
    }

    // reset s_global_num
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    s_global_num = 0;
    // create 2 tasks accessing a shared resource protected by spinlock
    for (core_id = 0; core_id < CONFIG_FREERTOS_NUMBER_OF_CORES; core_id++) {
        xTaskCreatePinnedToCore(inc_num_spinlock_iter, NULL, 4096, NULL, TASK_PRIO_3, NULL, core_id);
    }

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    // create 2 tasks accessing an atomic shared resource
    atomic_init(&s_atomic_global_num, 0);
    for (core_id = 0; core_id < CONFIG_FREERTOS_NUMBER_OF_CORES; core_id++) {
        xTaskCreatePinnedToCore(inc_num_atomic_iter, NULL, 4096, NULL, TASK_PRIO_3, NULL, core_id);
    }

    // reset s_global_num
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    s_global_num = 0;
    // create 2 tasks to increase a shared number in turn
    for (thread_id = 0; thread_id < SHARE_RES_THREAD_NUM; thread_id++) {
        xTaskCreatePinnedToCore(inc_num_mutex, NULL, 4096, (void *)thread_id, TASK_PRIO_3, NULL, tskNO_AFFINITY);
    }

    // time out and stop running after 5 seconds
    vTaskDelay(pdMS_TO_TICKS(COMP_LOOP_PERIOD));
    timed_out = true;
    // delay to let tasks finish the last loop
    vTaskDelay(1500 / portTICK_PERIOD_MS);
    return 0;
}
