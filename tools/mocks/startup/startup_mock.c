/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include "esp_err.h"
#include "Mockqueue.h"
#include "Mocktask.h"
#include "Mockesp_event.h"
#include <stdio.h>

extern void app_main(void);

int main(int argc, char **argv)
{
    int queue;
    setbuf(stdout, NULL);

    // Mocks are used only as workarounds to build this application
    // without FreeRTOS simulator.
    // The code below presets the mocks to ignore and return
    xQueueSemaphoreTake_IgnoreAndReturn(true);
    xQueueGenericSend_IgnoreAndReturn(true);
    vQueueDelete_Ignore();
    xQueueCreateMutex_IgnoreAndReturn((QueueHandle_t)&queue);
    xTaskGetTickCount_IgnoreAndReturn(0);
    xQueueGenericCreate_IgnoreAndReturn((QueueHandle_t)&queue);
    xTaskCreatePinnedToCore_IgnoreAndReturn((BaseType_t) &queue);
    esp_event_loop_create_default_IgnoreAndReturn(ESP_OK);
    xQueueGiveMutexRecursive_IgnoreAndReturn(true);
    app_main();
    return 0;
}
