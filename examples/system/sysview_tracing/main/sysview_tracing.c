/* Application Trace to Host Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/timer.h"


static const char *TAG = "example";

typedef struct {
    int          group;
    int          timer;
    int          count;
    TaskHandle_t thnd;
} example_event_data_t;


#if CONFIG_SYSVIEW_ENABLE
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

    U8* pPayload = SEGGER_SYSVIEW_PREPARE_PACKET(aPacket);
    pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, val);   // Add the parameter to the packet
    SEGGER_SYSVIEW_SendPacket(&aPacket[0], pPayload, s_example_sysview_module.EventOffset + id);
}

#endif

#else

#define SYSVIEW_EXAMPLE_SEND_EVENT_START()
#define SYSVIEW_EXAMPLE_SEND_EVENT_END(_val_)
#define SYSVIEW_EXAMPLE_WAIT_EVENT_START()
#define SYSVIEW_EXAMPLE_WAIT_EVENT_END(_val_)

#endif


static void example_timer_init(int timer_group, int timer_idx, uint32_t period)
{
    timer_config_t config;
    uint64_t alarm_val = (period * (TIMER_BASE_CLK / 1000000UL)) / 2;

    config.alarm_en = 1;
    config.auto_reload = 1;
    config.counter_dir = TIMER_COUNT_UP;
    config.divider = 2;     //Range is 2 to 65536
    config.intr_type = TIMER_INTR_LEVEL;
    config.counter_en = TIMER_PAUSE;
    /*Configure timer*/
    timer_init(timer_group, timer_idx, &config);
    /*Stop timer counter*/
    timer_pause(timer_group, timer_idx);
    /*Load counter value */
    timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL);
    /*Set alarm value*/
    timer_set_alarm_value(timer_group, timer_idx, alarm_val);
    /*Enable timer interrupt*/
    timer_enable_intr(timer_group, timer_idx);
}

static void example_timer_isr(void *arg)
{
    example_event_data_t *tim_arg = (example_event_data_t *)arg;

    if (tim_arg->thnd != NULL) {
        if (tim_arg->count++ < 10) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            SYSVIEW_EXAMPLE_SEND_EVENT_START();
            if (xTaskNotifyFromISR(tim_arg->thnd, tim_arg->count, eSetValueWithOverwrite, &xHigherPriorityTaskWoken) != pdPASS) {
                ESP_EARLY_LOGE(TAG, "Failed to notify task %p", tim_arg->thnd);
            } else {
                SYSVIEW_EXAMPLE_SEND_EVENT_END(tim_arg->count);
                if (xHigherPriorityTaskWoken == pdTRUE) {
                    portYIELD_FROM_ISR();
                }
            }
        }
    }
    // re-start timer
    timer_group_intr_clr_in_isr(tim_arg->group, tim_arg->timer);
    timer_group_enable_alarm_in_isr(tim_arg->group, tim_arg->timer);
}

static void example_task(void *p)
{
    example_event_data_t *arg = (example_event_data_t *) p;
    timer_isr_handle_t inth;

    ESP_LOGI(TAG, "%p: run task", xTaskGetCurrentTaskHandle());

    esp_err_t res = timer_isr_register(arg->group, arg->timer, example_timer_isr, arg, 0, &inth);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "%p: failed to register timer ISR", xTaskGetCurrentTaskHandle());
    } else {
        res = timer_start(arg->group, arg->timer);
        if (res != ESP_OK) {
            ESP_LOGE(TAG, "%p: failed to start timer", xTaskGetCurrentTaskHandle());
        }
    }

    while (1) {
        uint32_t event_val;
        SYSVIEW_EXAMPLE_WAIT_EVENT_START();
        xTaskNotifyWait(0, 0, &event_val, portMAX_DELAY);
        SYSVIEW_EXAMPLE_WAIT_EVENT_END(event_val);
        ESP_LOGI(TAG, "Task[%p]: received event %d", xTaskGetCurrentTaskHandle(), event_val);
    }
}

void app_main(void)
{
    static example_event_data_t event_data[portNUM_PROCESSORS] = {
        {
            .group = TIMER_GROUP_1,
            .timer = TIMER_0,
        },
#if CONFIG_FREERTOS_UNICORE == 0
        {
            .group = TIMER_GROUP_1,
            .timer = TIMER_1,
        },
#endif
    };

#if CONFIG_SYSVIEW_ENABLE && CONFIG_USE_CUSTOM_EVENT_ID
    // Currently OpenOCD does not support requesting module info from target. So do the following...
    // Wait untill SystemView module receives START command from host,
    // after that data can be sent to the host using onboard API,
    // so user module description does not need to be requested by OpenOCD itself.
    while(!SEGGER_SYSVIEW_Started()) {
        vTaskDelay(1);
    }
    SEGGER_SYSVIEW_RegisterModule(&s_example_sysview_module);
#endif

    example_timer_init(TIMER_GROUP_1, TIMER_0, 2000);
    example_timer_init(TIMER_GROUP_1, TIMER_1, 4000);

    xTaskCreatePinnedToCore(example_task, "svtrace0", 2048, &event_data[0], 3, &event_data[0].thnd, 0);
    ESP_LOGI(TAG, "Created task %p", event_data[0].thnd);
#if CONFIG_FREERTOS_UNICORE == 0
    xTaskCreatePinnedToCore(example_task, "svtrace1", 2048, &event_data[1], 3, &event_data[1].thnd, 1);
    ESP_LOGI(TAG, "Created task %p", event_data[1].thnd);
#endif
}
