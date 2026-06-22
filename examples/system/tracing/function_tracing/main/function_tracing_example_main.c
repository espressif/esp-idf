/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Function Tracing Example
*/

#include <inttypes.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_trace.h"
#include "ft_demo.h"

static const char *TAG = "function-tracing";

static void __attribute__((noinline)) example_workload(uint32_t iteration)
{
    ft_demo_run(iteration);       /* traced: ft_demo_run -> level1 -> level2 */
    ft_demo_secret();             /* excluded by function name: not traced */
    ft_demo_hot_loop(64);         /* excluded by file: not traced */
    ft_demo_quiet_path(8);        /* excluded by file (2nd list entry): not traced */
}

static void example_task(void *arg)
{
    (void)arg;
    uint32_t iteration = 0;

    while (1) {
        example_workload(iteration);
        ESP_LOGI(TAG, "workload iteration %" PRIu32 " on core %d", iteration, xPortGetCoreID());
        iteration++;
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

#if CONFIG_ESP_TRACE_TRANSPORT_APPTRACE
#include "esp_app_trace.h"
esp_trace_open_params_t esp_trace_get_user_params(void)
{
    static esp_apptrace_config_t app_trace_config = APPTRACE_CONFIG_DEFAULT();

    esp_trace_open_params_t trace_params = {
        .core_cfg = NULL,
        .encoder_name = "sysview",
        .encoder_cfg = NULL,
        .transport_name = "apptrace",
        .transport_cfg = &app_trace_config,
    };
    return trace_params;
}
#endif

void app_main(void)
{
    ESP_LOGI(TAG, "Hello from function_tracing example!");

    /* Recording is host-driven: it starts when the SystemView host (OpenOCD
     * "mon esp sysview_mcore start") begins the session. No explicit start call
     * is needed because CONFIG_ESP_TRACE_FUNCTION_TRACE_AUTO_START is enabled. */


    for (int core = 0; core < CONFIG_FREERTOS_NUMBER_OF_CORES; core++) {
        char name[configMAX_TASK_NAME_LEN];
        snprintf(name, sizeof(name), "ft_workload%d", core);
        xTaskCreatePinnedToCore(example_task, name, 4096, NULL, 5, NULL, core);
    }
}
