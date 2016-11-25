// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"


extern void bte_main_boot_entry(void *);
extern void bt_app_task_start_up(void);
extern void bt_app_core_start(void);

void pingTask(void *pvParameters)
{
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("ping\n");
    }
}

void app_main()
{
    bt_controller_init();
    xTaskCreatePinnedToCore(&pingTask, "pingTask", 2048, NULL, 5, NULL, 0);
    bt_app_task_start_up();
    // bte_main_boot_entry(bt_app_core_start);
}
