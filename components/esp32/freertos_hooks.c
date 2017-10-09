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


#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "esp_attr.h"
#include "esp_freertos_hooks.h"

#include "sdkconfig.h"
#include "esp_pm.h"
#include "pm_impl.h"

//We use just a static array here because it's not expected many components will need
//an idle or tick hook.
#define MAX_HOOKS 8

static portMUX_TYPE hooks_spinlock = portMUX_INITIALIZER_UNLOCKED;
static esp_freertos_idle_cb_t idle_cb[portNUM_PROCESSORS][MAX_HOOKS]={0};
static esp_freertos_tick_cb_t tick_cb[portNUM_PROCESSORS][MAX_HOOKS]={0};

void IRAM_ATTR esp_vApplicationTickHook() 
{
    int n;
    int core = xPortGetCoreID();
    for (n=0; n<MAX_HOOKS; n++) {
        if (tick_cb[core][n]!=NULL) {
            tick_cb[core][n]();
        }
    }
}

void esp_vApplicationIdleHook() 
{
    bool can_go_idle=true;
    int core = xPortGetCoreID();
    for (int n = 0; n < MAX_HOOKS; n++) {
        if (idle_cb[core][n] != NULL && !idle_cb[core][n]()) {
            can_go_idle = false;
        }
    }
    if (!can_go_idle) {
        return;
    }

#ifdef CONFIG_PM_ENABLE
    esp_pm_impl_idle_hook();
#endif
    asm("waiti 0");
}

esp_err_t esp_register_freertos_idle_hook_for_cpu(esp_freertos_idle_cb_t new_idle_cb, UBaseType_t cpuid)
{
    if(cpuid >= portNUM_PROCESSORS){
        return ESP_ERR_INVALID_ARG;
    }
    portENTER_CRITICAL(&hooks_spinlock);
    for(int n = 0; n < MAX_HOOKS; n++){
        if (idle_cb[cpuid][n]==NULL) {
            idle_cb[cpuid][n]=new_idle_cb;
            portEXIT_CRITICAL(&hooks_spinlock);
            return ESP_OK;
        }
    }
    portEXIT_CRITICAL(&hooks_spinlock);
    return ESP_ERR_NO_MEM;
}

esp_err_t esp_register_freertos_idle_hook(esp_freertos_idle_cb_t new_idle_cb)
{
    return esp_register_freertos_idle_hook_for_cpu(new_idle_cb, xPortGetCoreID());
}

esp_err_t esp_register_freertos_tick_hook_for_cpu(esp_freertos_tick_cb_t new_tick_cb, UBaseType_t cpuid)
{
    if(cpuid >= portNUM_PROCESSORS){
        return ESP_ERR_INVALID_ARG;
    }
    portENTER_CRITICAL(&hooks_spinlock);
    for(int n = 0; n < MAX_HOOKS; n++){
        if (tick_cb[cpuid][n]==NULL) {
            tick_cb[cpuid][n]=new_tick_cb;
            portEXIT_CRITICAL(&hooks_spinlock);
            return ESP_OK;
        }
    }
    portEXIT_CRITICAL(&hooks_spinlock);
    return ESP_ERR_NO_MEM;
}

esp_err_t esp_register_freertos_tick_hook(esp_freertos_tick_cb_t new_tick_cb) 
{
    return esp_register_freertos_tick_hook_for_cpu(new_tick_cb, xPortGetCoreID());
}

void esp_deregister_freertos_idle_hook_for_cpu(esp_freertos_idle_cb_t old_idle_cb, UBaseType_t cpuid)
{
    portENTER_CRITICAL(&hooks_spinlock);
    if(cpuid >= portNUM_PROCESSORS){
        return;
    }
    for(int n = 0; n < MAX_HOOKS; n++){
        if(idle_cb[cpuid][n] == old_idle_cb) idle_cb[cpuid][n] = NULL;
    }
    portEXIT_CRITICAL(&hooks_spinlock);
}

void esp_deregister_freertos_idle_hook(esp_freertos_idle_cb_t old_idle_cb)
{
    portENTER_CRITICAL(&hooks_spinlock);
    for(int m = 0; m < portNUM_PROCESSORS; m++) {
        esp_deregister_freertos_idle_hook_for_cpu(old_idle_cb, m);
    }
    portEXIT_CRITICAL(&hooks_spinlock);
}

void esp_deregister_freertos_tick_hook_for_cpu(esp_freertos_tick_cb_t old_tick_cb, UBaseType_t cpuid)
{
    portENTER_CRITICAL(&hooks_spinlock);
    if(cpuid >= portNUM_PROCESSORS){
        return;
    }
    for(int n = 0; n < MAX_HOOKS; n++){
        if(tick_cb[cpuid][n] == old_tick_cb) tick_cb[cpuid][n] = NULL;
    }
    portEXIT_CRITICAL(&hooks_spinlock);
}

void esp_deregister_freertos_tick_hook(esp_freertos_tick_cb_t old_tick_cb)
{
    portENTER_CRITICAL(&hooks_spinlock);
    for(int m = 0; m < portNUM_PROCESSORS; m++){
        esp_deregister_freertos_tick_hook_for_cpu(old_tick_cb, m);
    }
    portEXIT_CRITICAL(&hooks_spinlock);
}

