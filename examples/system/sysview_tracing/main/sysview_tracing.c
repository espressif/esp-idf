/* Application Trace to Host Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "sdkconfig.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gptimer.h"

static const char *TAG = "example";

#if CONFIG_APPTRACE_SV_ENABLE
#if !CONFIG_USE_CUSTOM_EVENT_ID

#define SYSVIEW_EXAMPLE_SEND_EVENT_ID     0
#define SYSVIEW_EXAMPLE_WAIT_EVENT_ID     1

#define SYSVIEW_EXAMPLE_SEND_EVENT_START()  SEGGER_SYSVIEW_OnUserStart(SYSVIEW_EXAMPLE_SEND_EVENT_ID)
#define SYSVIEW_EXAMPLE_SEND_EVENT_END(_val_)    SEGGER_SYSVIEW_OnUserStop(SYSVIEW_EXAMPLE_SEND_EVENT_ID)
#define SYSVIEW_EXAMPLE_WAIT_EVENT_START()  SEGGER_SYSVIEW_OnUserStart(SYSVIEW_EXAMPLE_WAIT_EVENT_ID)
#define SYSVIEW_EXAMPLE_WAIT_EVENT_END(_val_)    SEGGER_SYSVIEW_OnUserStop(SYSVIEW_EXAMPLE_WAIT_EVENT_ID)

#else

#define SYSVIEW_EXAMPLE_SEND_EVENT_START_ID     0
#define SYSVIEW_EXAMPLE_SEND_EVENT_END_ID       1
#define SYSVIEW_EXAMPLE_WAIT_EVENT_START_ID     2
#define SYSVIEW_EXAMPLE_WAIT_EVENT_END_ID       3
#define SYSVIEW_EXAMPLE_EVENT_MAX               4

#define SYSVIEW_EXAMPLE_SEND_EVENT_START()  example_sysview_event_send(SYSVIEW_EXAMPLE_SEND_EVENT_START_ID, 0)
#define SYSVIEW_EXAMPLE_SEND_EVENT_END(_val_)    example_sysview_event_send(SYSVIEW_EXAMPLE_SEND_EVENT_END_ID, _val_)
#define SYSVIEW_EXAMPLE_WAIT_EVENT_START()  example_sysview_event_send(SYSVIEW_EXAMPLE_WAIT_EVENT_START_ID, 0)
#define SYSVIEW_EXAMPLE_WAIT_EVENT_END(_val_)    example_sysview_event_send(SYSVIEW_EXAMPLE_WAIT_EVENT_END_ID, _val_)

static void example_sysview_module_send_desc(void);

static SEGGER_SYSVIEW_MODULE s_example_sysview_module = {
    .sModule = "example_sysview_module",
    .NumEvents = SYSVIEW_EXAMPLE_EVENT_MAX,
    .pfSendModuleDesc = example_sysview_module_send_desc,
};

static void example_sysview_module_send_desc(void)
{
    SEGGER_SYSVIEW_RecordModuleDescription(&s_example_sysview_module, "Example SystemView User Module");
}

static void example_sysview_event_send(uint32_t id, uint32_t val)
{
    U8  aPacket[SEGGER_SYSVIEW_INFO_SIZE + SEGGER_SYSVIEW_QUANTA_U32];

    U8 *pPayload = SEGGER_SYSVIEW_PREPARE_PACKET(aPacket);
    pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, val);   // Add the parameter to the packet
    SEGGER_SYSVIEW_SendPacket(&aPacket[0], pPayload, s_example_sysview_module.EventOffset + id);
}

#endif // !CONFIG_USE_CUSTOM_EVENT_ID

#else

#define SYSVIEW_EXAMPLE_SEND_EVENT_START()
#define SYSVIEW_EXAMPLE_SEND_EVENT_END(_val_)
#define SYSVIEW_EXAMPLE_WAIT_EVENT_START()
#define SYSVIEW_EXAMPLE_WAIT_EVENT_END(_val_)

#endif // CONFIG_APPTRACE_SV_ENABLE

typedef struct {
    gptimer_handle_t gptimer;
    int count;
    TaskHandle_t thnd;
    uint64_t period;
    char task_name[32];
} example_event_data_t;

static bool example_timer_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    example_event_data_t *tim_arg = (example_event_data_t *)user_ctx;
    bool need_yield = false;

    if (tim_arg->thnd != NULL) {
        if (tim_arg->count++ < 10) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            SYSVIEW_EXAMPLE_SEND_EVENT_START();
            if (xTaskNotifyFromISR(tim_arg->thnd, tim_arg->count, eSetValueWithOverwrite, &xHigherPriorityTaskWoken) != pdPASS) {
                ESP_EARLY_LOGE(TAG, "Failed to notify task %p", tim_arg->thnd);
            } else {
                SYSVIEW_EXAMPLE_SEND_EVENT_END(tim_arg->count);
                if (xHigherPriorityTaskWoken == pdTRUE) {
                    need_yield = true;
                }
            }
        }
    }
    return need_yield;
}

static void example_task(void *p)
{
    uint32_t event_val;
    example_event_data_t *arg = (example_event_data_t *) p;
    ESP_LOGI(TAG, "%p: run task", xTaskGetCurrentTaskHandle());
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = arg->period,
        .flags.auto_reload_on_alarm = true,
    };
    // This task is pinned to a specific core, to the interrupt will also be install to that core
    gptimer_event_callbacks_t cbs = {
        .on_alarm = example_timer_alarm_cb,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(arg->gptimer, &cbs, arg));
    ESP_ERROR_CHECK(gptimer_set_alarm_action(arg->gptimer, &alarm_config));
    ESP_ERROR_CHECK(gptimer_enable(arg->gptimer));
    ESP_ERROR_CHECK(gptimer_start(arg->gptimer));
    while (1) {
        SYSVIEW_EXAMPLE_WAIT_EVENT_START();
        xTaskNotifyWait(0, 0, &event_val, portMAX_DELAY);
        SYSVIEW_EXAMPLE_WAIT_EVENT_END(event_val);
        ESP_LOGI(TAG, "Task[%p]: received event %"PRIu32, xTaskGetCurrentTaskHandle(), event_val);
    }
}

void app_main(void)
{
    static example_event_data_t event_data[CONFIG_FREERTOS_NUMBER_OF_CORES];

#if CONFIG_APPTRACE_SV_ENABLE && CONFIG_USE_CUSTOM_EVENT_ID
    // Currently OpenOCD does not support requesting module info from target. So do the following...
    // Wait until SystemView module receives START command from host,
    // after that data can be sent to the host using onboard API,
    // so user module description does not need to be requested by OpenOCD itself.
    while (!SEGGER_SYSVIEW_Started()) {
        vTaskDelay(1);
    }
    SEGGER_SYSVIEW_RegisterModule(&s_example_sysview_module);
#endif

    for (int i = 0; i < CONFIG_FREERTOS_NUMBER_OF_CORES; i++) {
        gptimer_config_t timer_config = {
            .clk_src = GPTIMER_CLK_SRC_DEFAULT,
            .direction = GPTIMER_COUNT_UP,
            .resolution_hz = 1000000,
        };
        ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &event_data[i].gptimer));
        event_data[i].period = 1000000 * (i + 1);
    }

    for (int i = 0; i < CONFIG_FREERTOS_NUMBER_OF_CORES; i++) {
        sprintf(event_data->task_name, "svtrace%d", i);
        xTaskCreatePinnedToCore(example_task, event_data->task_name, 4096, &event_data[i], 3, &event_data[i].thnd, i);
        ESP_LOGI(TAG, "Created task %p", event_data[i].thnd);
    }
}
