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
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "esp_debug_helpers.h"
#include "soc/periph_defs.h"

#include "hal/cpu_hal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
#include "soc/dport_reg.h"
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32H2
#include "soc/system_reg.h"
#endif

#define REASON_YIELD            BIT(0)
#define REASON_FREQ_SWITCH      BIT(1)

#if !CONFIG_IDF_TARGET_ESP32C3 && !CONFIG_IDF_TARGET_ESP32H2
#define REASON_PRINT_BACKTRACE  BIT(2)
#endif

static portMUX_TYPE reason_spinlock = portMUX_INITIALIZER_UNLOCKED;
static volatile uint32_t reason[portNUM_PROCESSORS];

/*
ToDo: There is a small chance the CPU already has yielded when this ISR is serviced. In that case, it's running the intended task but
the ISR will cause it to switch _away_ from it. portYIELD_FROM_ISR will probably just schedule the task again, but have to check that.
*/
static inline void IRAM_ATTR esp_crosscore_isr_handle_yield(void)
{
    portYIELD_FROM_ISR();
}

static void IRAM_ATTR esp_crosscore_isr(void *arg) {
    uint32_t my_reason_val;
    //A pointer to the correct reason array item is passed to this ISR.
    volatile uint32_t *my_reason=arg;

    //Clear the interrupt first.
#if CONFIG_IDF_TARGET_ESP32
    if (cpu_hal_get_core_id()==0) {
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_0_REG, 0);
    } else {
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_1_REG, 0);
    }
#elif CONFIG_IDF_TARGET_ESP32S2
    DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_0_REG, 0);
#elif CONFIG_IDF_TARGET_ESP32S3
    if (cpu_hal_get_core_id()==0) {
        WRITE_PERI_REG(SYSTEM_CPU_INTR_FROM_CPU_0_REG, 0);
    } else {
        WRITE_PERI_REG(SYSTEM_CPU_INTR_FROM_CPU_1_REG, 0);
    }
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
    WRITE_PERI_REG(SYSTEM_CPU_INTR_FROM_CPU_0_REG, 0);
#endif

    //Grab the reason and clear it.
    portENTER_CRITICAL_ISR(&reason_spinlock);
    my_reason_val=*my_reason;
    *my_reason=0;
    portEXIT_CRITICAL_ISR(&reason_spinlock);

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
#if !CONFIG_IDF_TARGET_ESP32C3 && !CONFIG_IDF_TARGET_ESP32H2 // IDF-2986
    if (my_reason_val & REASON_PRINT_BACKTRACE) {
        esp_backtrace_print(100);
    }
#endif
}

//Initialize the crosscore interrupt on this core. Call this once
//on each active core.
void esp_crosscore_int_init(void) {
    portENTER_CRITICAL(&reason_spinlock);
    reason[cpu_hal_get_core_id()]=0;
    portEXIT_CRITICAL(&reason_spinlock);
    esp_err_t err __attribute__((unused)) = ESP_OK;
#if portNUM_PROCESSORS > 1
    if (cpu_hal_get_core_id()==0) {
        err = esp_intr_alloc(ETS_FROM_CPU_INTR0_SOURCE, ESP_INTR_FLAG_IRAM, esp_crosscore_isr, (void*)&reason[0], NULL);
    } else {
        err = esp_intr_alloc(ETS_FROM_CPU_INTR1_SOURCE, ESP_INTR_FLAG_IRAM, esp_crosscore_isr, (void*)&reason[1], NULL);
    }
#else
    err = esp_intr_alloc(ETS_FROM_CPU_INTR0_SOURCE, ESP_INTR_FLAG_IRAM, esp_crosscore_isr, (void*)&reason[0], NULL);
#endif
    ESP_ERROR_CHECK(err);
}

static void IRAM_ATTR esp_crosscore_int_send(int core_id, uint32_t reason_mask) {
    assert(core_id<portNUM_PROCESSORS);
    //Mark the reason we interrupt the other CPU
    portENTER_CRITICAL_ISR(&reason_spinlock);
    reason[core_id] |= reason_mask;
    portEXIT_CRITICAL_ISR(&reason_spinlock);
    //Poke the other CPU.
#if CONFIG_IDF_TARGET_ESP32
    if (core_id==0) {
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_0_REG, DPORT_CPU_INTR_FROM_CPU_0);
    } else {
        DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_1_REG, DPORT_CPU_INTR_FROM_CPU_1);
    }
#elif CONFIG_IDF_TARGET_ESP32S2
    DPORT_WRITE_PERI_REG(DPORT_CPU_INTR_FROM_CPU_0_REG, DPORT_CPU_INTR_FROM_CPU_0);
#elif CONFIG_IDF_TARGET_ESP32S3
    if (core_id==0) {
        WRITE_PERI_REG(SYSTEM_CPU_INTR_FROM_CPU_0_REG, SYSTEM_CPU_INTR_FROM_CPU_0);
    } else {
        WRITE_PERI_REG(SYSTEM_CPU_INTR_FROM_CPU_1_REG, SYSTEM_CPU_INTR_FROM_CPU_1);
    }
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
    WRITE_PERI_REG(SYSTEM_CPU_INTR_FROM_CPU_0_REG, SYSTEM_CPU_INTR_FROM_CPU_0);
#endif
}

void IRAM_ATTR esp_crosscore_int_send_yield(int core_id)
{
    esp_crosscore_int_send(core_id, REASON_YIELD);
}

void IRAM_ATTR esp_crosscore_int_send_freq_switch(int core_id)
{
    esp_crosscore_int_send(core_id, REASON_FREQ_SWITCH);
}

#if !CONFIG_IDF_TARGET_ESP32C3 && !CONFIG_IDF_TARGET_ESP32H2
void IRAM_ATTR esp_crosscore_int_send_print_backtrace(int core_id)
{
    esp_crosscore_int_send(core_id, REASON_PRINT_BACKTRACE);
}
#endif
