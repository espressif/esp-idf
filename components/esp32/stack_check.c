// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sdkconfig.h"
#include "esp_system.h"

#if CONFIG_STACK_CHECK

#define LOG_LOCAL_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include "esp_log.h"
const static char *TAG = "stack_chk";

void *__stack_chk_guard = NULL;

static void __attribute__ ((constructor))
__esp_stack_guard_setup (void)
{
    ESP_LOGD(TAG, "Intialize random stack guard");
    __stack_chk_guard = (void *)esp_random();
}

void __stack_chk_fail (void)
{
    ets_printf("\r\nStack smashing protect failure!\r\n\r\n");
    abort();
}

#endif
