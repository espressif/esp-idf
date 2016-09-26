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

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "freertos/portmacro.h"
#include "esp_types.h"
#include "esp_system.h"
#include "esp_task.h"
#include "esp_intr.h"
#include "esp_attr.h"
#include "bt.h"

#if CONFIG_BT_ENABLED

/* not for user call, so don't put to include file */
extern void btdm_osi_funcs_register(void *osi_funcs);
extern void btdm_controller_init(void);


#define BT_DEBUG(...)
#define BT_API_CALL_CHECK(info, api_call, ret) \
do{\
    esp_err_t __err = (api_call);\
    if ((ret) != __err) {\
        BT_DEBUG("%s %d %s ret=%d\n", __FUNCTION__, __LINE__, (info), __err);\
        return __err;\
    }\
} while(0)

struct osi_funcs_t {
    xt_handler (*_set_isr)(int n, xt_handler f, void *arg);
    void (*_ints_on)(unsigned int mask);
    void (*_interrupt_disable)(void);
    void (*_interrupt_restore)(void);
    void (*_task_yield)(void);
    void *(*_semphr_create)(uint32_t max, uint32_t init);
    void *(*_semphr_give_from_isr)(void *semphr, void *hptw);
    void *(*_semphr_take)(void *semphr, uint32_t block_time);
    esp_err_t (* _read_efuse_mac)(uint8_t mac[6]);
};

static portMUX_TYPE global_int_mux = portMUX_INITIALIZER_UNLOCKED;

static void IRAM_ATTR interrupt_disable(void)
{
    portENTER_CRITICAL(&global_int_mux);
}

static void IRAM_ATTR interrupt_restore(void)
{
    portEXIT_CRITICAL(&global_int_mux);
}

static void * IRAM_ATTR semphr_take_wrapper(void *semphr, uint32_t block_time)
{
    return (void *)xSemaphoreTake(semphr, block_time);
}

static struct osi_funcs_t osi_funcs = {
    ._set_isr = xt_set_interrupt_handler,
    ._ints_on = xt_ints_on,
    ._interrupt_disable = interrupt_disable,
    ._interrupt_restore = interrupt_restore,
    ._task_yield = vPortYield,
    ._semphr_create = xQueueCreateCountingSemaphore,
    ._semphr_give_from_isr = (void *)xQueueGiveFromISR,
    ._semphr_take = semphr_take_wrapper,
    ._read_efuse_mac = system_efuse_read_mac,
};

static void bt_controller_task(void *pvParam)
{
    btdm_osi_funcs_register(&osi_funcs);
    btdm_controller_init();
}

void bt_controller_init()
{
    xTaskCreatePinnedToCore(bt_controller_task, "btController",
            ESP_TASK_BT_CONTROLLER_STACK, NULL,
            ESP_TASK_BT_CONTROLLER_PRIO, NULL, 0);
}

#endif
