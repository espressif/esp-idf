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



#include "sdkconfig.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_types.h>
#include "esp_err.h"
#include "esp_intr.h"
#include "esp_attr.h"
#include "esp_freertos_hooks.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "driver/timer.h"
#include "xtensa/core-macros.h"

#if CONFIG_SPIRAM_CACHE_WORKAROUND_TEST

void psram_tst_setup() {
    esp_err_t err;
    XTHAL_SET_CCOMPARE(2, XTHAL_GET_CCOUNT()+1000);
    err=esp_intr_alloc(ETS_INTERNAL_TIMER2_INTR_SOURCE, ESP_INTR_FLAG_LEVEL5|ESP_INTR_FLAG_IRAM, NULL, NULL, NULL);
    assert(err==ESP_OK);
}

#endif
