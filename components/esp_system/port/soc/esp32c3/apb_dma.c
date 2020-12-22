
// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "esp32c3/rom/apb_dma.h"

static portMUX_TYPE apb_backup_mutex = portMUX_INITIALIZER_UNLOCKED;

static void IRAM_ATTR apb_backup_lock(void)
{
    if (xPortInIsrContext()) {
        portENTER_CRITICAL_ISR(&apb_backup_mutex);
    } else {
        portENTER_CRITICAL(&apb_backup_mutex);
    }
}

static void IRAM_ATTR apb_backup_unlock(void)
{
    if (xPortInIsrContext()) {
        portEXIT_CRITICAL_ISR(&apb_backup_mutex);
    } else {
        portEXIT_CRITICAL(&apb_backup_mutex);
    }
}

void esp_apb_backup_lock_init(void)
{
    ets_apb_backup_init_lock_func(apb_backup_lock, apb_backup_unlock);
}
