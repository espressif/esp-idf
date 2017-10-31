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


#define REASON_YIELD            BIT(0)
#define REASON_FREQ_SWITCH      BIT(1)

static portMUX_TYPE reason_spinlock = portMUX_INITIALIZER_UNLOCKED;
static volatile uint32_t reason[ portNUM_PROCESSORS ];

/*
ToDo: There is a small chance the CPU already has yielded when this ISR is serviced. In that case, it's running the intended task but
the ISR will cause it to switch _away_ from it. portYIELD_FROM_ISR will probably just schedule the task again, but have to check that.
*/
static void esp_crosscore_isr_handle_yield()
{
    portYIELD_FROM_ISR();
}

static void IRAM_ATTR esp_crosscore_isr(void *arg) {
    uint32_t my_reason_val;
    //A pointer to the correct reason array item is passed to this ISR.
    volatile uint32_t *my_reason=arg;

    //Clear the interrupt first.
    if (xPortGetCoreID()==0) {
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_0_REG, 0);
    } else {
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_1_REG, 0);
    }
    //Grab the reason and clear it.
    portENTER_CRITICAL(&reason_spinlock);
    my_reason_val=*my_reason;
    *my_reason=0;
    portEXIT_CRITICAL(&reason_spinlock);

    //Check what we need to do.
    if (my_reason_val & REASON_YIELD) {
        esp_crosscore_isr_handle_yield();
    }
    if (my_reason_val & REASON_FREQ_SWITCH) {
        /* Nothing to do here; the frequency switch event was already
         * handled by a hook in xtensa_vectors.S. Could be used in the future
         * to allow DFS features without the extra latency of the ISR hook.
         */
    }
}

//Initialize the crosscore interrupt on this core. Call this once
//on each active core.
void esp_crosscore_int_init() {
    portENTER_CRITICAL(&reason_spinlock);
    reason[xPortGetCoreID()]=0;
    portEXIT_CRITICAL(&reason_spinlock);
    esp_err_t err;
    if (xPortGetCoreID()==0) {
        err = esp_intr_alloc(ETS_FROM_CPU_INTR0_SOURCE, ESP_INTR_FLAG_IRAM, esp_crosscore_isr, (void*)&reason[0], NULL);
    } else {
        err = esp_intr_alloc(ETS_FROM_CPU_INTR1_SOURCE, ESP_INTR_FLAG_IRAM, esp_crosscore_isr, (void*)&reason[1], NULL);
    }
    assert(err == ESP_OK);
}

static void IRAM_ATTR esp_crosscore_int_send(int core_id, uint32_t reason_mask) {
    assert(core_id<portNUM_PROCESSORS);
    //Mark the reason we interrupt the other CPU
    portENTER_CRITICAL(&reason_spinlock);
    reason[core_id] |= reason_mask;
    portEXIT_CRITICAL(&reason_spinlock);
    //Poke the other CPU.
    if (core_id==0) {
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_0_REG, DPORT_CPU_INTR_FROM_CPU_0);
    } else {
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_1_REG, DPORT_CPU_INTR_FROM_CPU_1);
    }
}

void IRAM_ATTR esp_crosscore_int_send_yield(int core_id)
{
    esp_crosscore_int_send(core_id, REASON_YIELD);
}

void IRAM_ATTR esp_crosscore_int_send_freq_switch(int core_id)
{
    esp_crosscore_int_send(core_id, REASON_FREQ_SWITCH);
}

