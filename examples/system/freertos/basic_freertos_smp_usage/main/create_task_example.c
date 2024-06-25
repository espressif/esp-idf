/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "basic_freertos_smp_usage.h"


#define SPIN_ITER   350000  //actual CPU cycles consumed will depend on compiler optimization
#define CORE0       0
// only define xCoreID CORE1 as 1 if this is a multiple core processor target, else define it as tskNO_AFFINITY
#define CORE1       ((CONFIG_FREERTOS_NUMBER_OF_CORES > 1) ? 1 : tskNO_AFFINITY)


static volatile bool timed_out;
const static char *TAG = "create task example";

static void spin_iteration(int spin_iter_num)
{
    for (int i = 0; i < spin_iter_num; i++) {
        __asm__ __volatile__("NOP");
    }
}

static void spin_task(void *arg)
{
    // convert arg pointer from void type to int type then dereference it
    int task_id = (int)arg;
    ESP_LOGI(TAG, "created task#%d", task_id);
    while (!timed_out) {
        int core_id = esp_cpu_get_core_id();
        ESP_LOGI(TAG, "task#%d is running on core#%d", task_id, core_id);
        // consume some CPU cycles to keep Core#0 a little busy, so task3 has opportunity to be scheduled on Core#1
        spin_iteration(SPIN_ITER);
        vTaskDelay(pdMS_TO_TICKS(150));
    }

    vTaskDelete(NULL);
}

// Creating task example: show how to create pinned and unpinned tasks on CPU cores
int comp_creating_task_entry_func(int argc, char **argv)
{
    timed_out = false;
    // pin 2 tasks on same core and observe in-turn execution,
    // and pin another task on the other core to observe "simultaneous" execution
    int task_id0 = 0, task_id1 = 1, task_id2 = 2, task_id3 = 3;
    xTaskCreatePinnedToCore(spin_task, "pinned_task0_core0", 4096, (void*)task_id0, TASK_PRIO_3, NULL, CORE0);
    xTaskCreatePinnedToCore(spin_task, "pinned_task1_core0", 4096, (void*)task_id1, TASK_PRIO_3, NULL, CORE0);
    xTaskCreatePinnedToCore(spin_task, "pinned_task2_core1", 4096, (void*)task_id2, TASK_PRIO_3, NULL, CORE1);
    // Create a unpinned task with xCoreID = tskNO_AFFINITY, which can be scheduled on any core, hopefully it can be observed that the scheduler moves the task between the different cores according to the workload
    xTaskCreatePinnedToCore(spin_task, "unpinned_task", 4096, (void*)task_id3, TASK_PRIO_2, NULL, tskNO_AFFINITY);

    // time out and stop running after 5 seconds
    vTaskDelay(pdMS_TO_TICKS(COMP_LOOP_PERIOD));
    timed_out = true;
    // delay to let tasks finish the last loop
    vTaskDelay(500 / portTICK_PERIOD_MS);
    return 0;
}
