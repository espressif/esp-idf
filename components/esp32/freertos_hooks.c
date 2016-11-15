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
#include "esp_attr.h"
#include "esp_freertos_hooks.h"

//We use just a static array here because it's not expected many components will need
//an idle or tick hook.
#define MAX_HOOKS 8

static esp_freertos_idle_cb_t idle_cb[MAX_HOOKS]={0};
static esp_freertos_tick_cb_t tick_cb[MAX_HOOKS]={0};

void IRAM_ATTR esp_vApplicationTickHook() 
{
    int n;
    for (n=0; n<MAX_HOOKS; n++) {
        if (tick_cb[n]!=NULL) {
            tick_cb[n]();
        }
    }
}

void esp_vApplicationIdleHook() 
{
    bool doWait=true;
    bool r;
    int n;
    for (n=0; n<MAX_HOOKS; n++) {
        if (idle_cb[n]!=NULL) {
            r=idle_cb[n]();
            if (!r) doWait=false;
        }
    }
    if (doWait) {
        //Wait for whatever interrupt comes next... this should save some power.
        asm("waiti 0");
    }
}


esp_err_t esp_register_freertos_idle_hook(esp_freertos_idle_cb_t new_idle_cb) 
{
    int n;
    for (n=0; n<MAX_HOOKS; n++) {
        if (idle_cb[n]==NULL) {
            idle_cb[n]=new_idle_cb;
            return ESP_OK;
        }
    }
    return ESP_ERR_NO_MEM;
}

esp_err_t esp_register_freertos_tick_hook(esp_freertos_tick_cb_t new_tick_cb) 
{
    int n;
    for (n=0; n<MAX_HOOKS; n++) {
        if (tick_cb[n]==NULL) {
            tick_cb[n]=new_tick_cb;
            return ESP_OK;
        }
    }
    return ESP_ERR_NO_MEM;
}

void esp_deregister_freertos_idle_hook(esp_freertos_idle_cb_t old_idle_cb)
{
    int n;
    for (n=0; n<MAX_HOOKS; n++) {
        if (idle_cb[n]==old_idle_cb) idle_cb[n]=NULL;
    }
}

void esp_deregister_freertos_tick_hook(esp_freertos_tick_cb_t old_tick_cb)
{
    int n;
    for (n=0; n<MAX_HOOKS; n++) {
        if (tick_cb[n]==old_tick_cb) tick_cb[n]=NULL;
    }
}

