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
#include <string.h>

#include "esp_attr.h"
#include "esp_err.h"
#include "esp_intr.h"
#include "esp_intr_alloc.h"

#include "rom/ets_sys.h"
#include "rom/uart.h"

#include "soc/cpu.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/rtc_cntl_reg.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/portmacro.h"


#define REASON_YIELD (1<<0)

static portMUX_TYPE reasonSpinlock = portMUX_INITIALIZER_UNLOCKED;
static volatile uint32_t reason[ portNUM_PROCESSORS ];


/*
ToDo: There is a small chance the CPU already has yielded when this ISR is serviced. In that case, it's running the intended task but
the ISR will cause it to switch _away_ from it. portYIELD_FROM_ISR will probably just schedule the task again, but have to check that.
*/
static void IRAM_ATTR esp_crosscore_isr(void *arg) {
    uint32_t myReasonVal;
    //A pointer to the correct reason array item is passed to this ISR.
    volatile uint32_t *myReason=arg;

    //Clear the interrupt first.
    if (xPortGetCoreID()==0) {
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_0_REG, 0);
    } else {
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_1_REG, 0);
    }
    //Grab the reason and clear it.
    portENTER_CRITICAL(&reasonSpinlock);
    myReasonVal=*myReason;
    *myReason=0;
    portEXIT_CRITICAL(&reasonSpinlock);

    //Check what we need to do.
    if (myReasonVal&REASON_YIELD) {
        portYIELD_FROM_ISR();
    }
}

//Initialize the crosscore interrupt on this core. Call this once
//on each active core.
void esp_crosscore_int_init() {
    portENTER_CRITICAL(&reasonSpinlock);
    reason[xPortGetCoreID()]=0;
    portEXIT_CRITICAL(&reasonSpinlock);
    esp_err_t err;
    if (xPortGetCoreID()==0) {
        err = esp_intr_alloc(ETS_FROM_CPU_INTR0_SOURCE, ESP_INTR_FLAG_IRAM, esp_crosscore_isr, (void*)&reason[xPortGetCoreID()], NULL);
    } else {
        err = esp_intr_alloc(ETS_FROM_CPU_INTR1_SOURCE, ESP_INTR_FLAG_IRAM, esp_crosscore_isr, (void*)&reason[xPortGetCoreID()], NULL);
    }
    assert(err == ESP_OK);
}

void IRAM_ATTR esp_crosscore_int_send_yield(int coreId) {
    assert(coreId<portNUM_PROCESSORS);
    //Mark the reason we interrupt the other CPU
    portENTER_CRITICAL(&reasonSpinlock);
    reason[coreId]|=REASON_YIELD;
    portEXIT_CRITICAL(&reasonSpinlock);
    //Poke the other CPU.
    if (coreId==0) {
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_0_REG, DPORT_CPU_INTR_FROM_CPU_0);
    } else {
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_1_REG, DPORT_CPU_INTR_FROM_CPU_1);
    }
}

