
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

#include "soc/soc_caps.h"

#if SOC_APB_BACKUP_DMA
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "esp32c3/rom/apb_backup_dma.h"

static portMUX_TYPE s_apb_backup_dma_mutex = portMUX_INITIALIZER_UNLOCKED;

static void IRAM_ATTR apb_backup_dma_lock(void)
{
    if (xPortInIsrContext()) {
        portENTER_CRITICAL_ISR(&s_apb_backup_dma_mutex);
    } else {
        portENTER_CRITICAL(&s_apb_backup_dma_mutex);
    }
}

static void IRAM_ATTR apb_backup_dma_unlock(void)
{
    if (xPortInIsrContext()) {
        portEXIT_CRITICAL_ISR(&s_apb_backup_dma_mutex);
    } else {
        portEXIT_CRITICAL(&s_apb_backup_dma_mutex);
    }
}

void esp_apb_backup_dma_lock_init(void)
{
    ets_apb_backup_init_lock_func(apb_backup_dma_lock, apb_backup_dma_unlock);
}
#endif
