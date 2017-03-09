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


/*
 The cache has an interrupt that can be raised as soon as an access to a cached 
 region (flash, psram) is done without the cache being enabled. We use that here 
 to panic the CPU, which from a debugging perspective is better than grabbing bad 
 data from the bus.
*/


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
#include "driver/timer.h"

#include "soc/dport_reg.h"
#include "esp_int_wdt.h"


void esp_cache_err_int_init() {
    ESP_INTR_DISABLE(ETS_CACHEERR_INUM);
    intr_matrix_set(xPortGetCoreID(), ETS_CACHE_IA_INTR_SOURCE, ETS_CACHEERR_INUM);
    //We do not register a handler for the interrupt because it is interrupt level 4 which
    //is not servicable from C. Instead, xtensa_vectors.S has a call to the panic handler for
    //this interrupt.

    //These (undocumented) settings will enable the ETS_CACHE_IA_INTR_SOURCE behaviour for both PSRAM
    //as well as flash RAM.
    SET_PERI_REG_MASK(DPORT_PRO_DCACHE_DBUG0_REG, 0x3F<<DPORT_PRO_CPU_DISABLED_CACHE_IA_S);
    SET_PERI_REG_MASK(DPORT_CACHE_IA_INT_EN_REG, (0x3F<<22)|(0x3F<<8)|(0x3F<<14)|(0x3F<<0));

    ESP_INTR_ENABLE(ETS_CACHEERR_INUM);
}



