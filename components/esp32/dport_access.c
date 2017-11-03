// Copyright 2010-2017 Espressif Systems (Shanghai) PTE LTD
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
 * DPORT access is used for do protection when dual core access DPORT internal register and APB register via DPORT simultaneously
 * This function will be initialize after FreeRTOS startup.
 * When cpu0 want to access DPORT register, it should notify cpu1 enter in high-priority interrupt for be mute. When cpu1 already in high-priority interrupt,
 * cpu0 can access DPORT register. Currently, cpu1 will wait for cpu0 finish access and exit high-priority interrupt. 
 */

#include <stdint.h>
#include <string.h>

#include <sdkconfig.h>
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_intr.h"
#include "rom/ets_sys.h"
#include "rom/uart.h"

#include "soc/cpu.h"
#include "soc/dport_reg.h"
#include "soc/spi_reg.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/portmacro.h"

#include "xtensa/core-macros.h"

#ifndef CONFIG_FREERTOS_UNICORE
static portMUX_TYPE g_dport_mux = portMUX_INITIALIZER_UNLOCKED;

#define DPORT_CORE_STATE_IDLE        0
#define DPORT_CORE_STATE_RUNNING     1
static uint32_t volatile dport_core_state[portNUM_PROCESSORS];      //cpu is already run

/* these global variables are accessed from interrupt vector, hence not declared as static */
uint32_t volatile dport_access_start[portNUM_PROCESSORS];      //dport register could be accessed
uint32_t volatile dport_access_end[portNUM_PROCESSORS];        //dport register is accessed over

static uint32_t volatile dport_access_ref[portNUM_PROCESSORS];        //dport access reference

#ifdef DPORT_ACCESS_BENCHMARK
#define DPORT_ACCESS_BENCHMARK_STORE_NUM
static uint32_t ccount_start[portNUM_PROCESSORS];
static uint32_t ccount_end[portNUM_PROCESSORS];
static uint32_t ccount_margin[portNUM_PROCESSORS][DPORT_ACCESS_BENCHMARK_STORE_NUM];
static uint32_t ccount_margin_cnt;
#endif


static BaseType_t oldInterruptLevel[2];
#endif // CONFIG_FREERTOS_UNICORE

/* stall other cpu that this cpu is pending to access dport register start */
void IRAM_ATTR esp_dport_access_stall_other_cpu_start(void)
{
#ifndef CONFIG_FREERTOS_UNICORE
    if (dport_core_state[0] == DPORT_CORE_STATE_IDLE
        || dport_core_state[1] == DPORT_CORE_STATE_IDLE) {
        return;
    }

    BaseType_t intLvl = portENTER_CRITICAL_NESTED();

    int cpu_id = xPortGetCoreID();

#ifdef DPORT_ACCESS_BENCHMARK
    ccount_start[cpu_id] = XTHAL_GET_CCOUNT();
#endif

    if (dport_access_ref[cpu_id] == 0) {
        portENTER_CRITICAL_ISR(&g_dport_mux);

        oldInterruptLevel[cpu_id]=intLvl;

        dport_access_start[cpu_id] = 0;
        dport_access_end[cpu_id] = 0;

        if (cpu_id == 0) {
            _DPORT_REG_WRITE(DPORT_CPU_INTR_FROM_CPU_3_REG, DPORT_CPU_INTR_FROM_CPU_3); //interrupt on cpu1
        } else {
            _DPORT_REG_WRITE(DPORT_CPU_INTR_FROM_CPU_2_REG, DPORT_CPU_INTR_FROM_CPU_2); //interrupt on cpu0
        }

        while (!dport_access_start[cpu_id]) {};

        REG_READ(SPI_DATE_REG(3));  //just read a APB register sure that the APB-bus is idle
    }

    dport_access_ref[cpu_id]++;

    if (dport_access_ref[cpu_id] > 1) {
        /* Interrupts are already disabled by the parent, we're nested here. */
        portEXIT_CRITICAL_NESTED(intLvl);
    }
#endif /* CONFIG_FREERTOS_UNICORE */
}

/* stall other cpu that this cpu is pending to access dport register end */
void IRAM_ATTR esp_dport_access_stall_other_cpu_end(void)
{
#ifndef CONFIG_FREERTOS_UNICORE
    int cpu_id = xPortGetCoreID();
    
    if (dport_core_state[0] == DPORT_CORE_STATE_IDLE
            || dport_core_state[1] == DPORT_CORE_STATE_IDLE) {
        return;
    }

    if (dport_access_ref[cpu_id] == 0) {
        assert(0);
    }

    dport_access_ref[cpu_id]--;

    if (dport_access_ref[cpu_id] == 0) {
        dport_access_end[cpu_id] = 1;

        portEXIT_CRITICAL_ISR(&g_dport_mux);

        portEXIT_CRITICAL_NESTED(oldInterruptLevel[cpu_id]);
    }

#ifdef DPORT_ACCESS_BENCHMARK
    ccount_end[cpu_id] = XTHAL_GET_CCOUNT();
    ccount_margin[cpu_id][ccount_margin_cnt] = ccount_end[cpu_id] - ccount_start[cpu_id];
    ccount_margin_cnt = (ccount_margin_cnt + 1)&(DPORT_ACCESS_BENCHMARK_STORE_NUM - 1);
#endif
#endif /* CONFIG_FREERTOS_UNICORE */
}

void IRAM_ATTR esp_dport_access_stall_other_cpu_start_wrap(void)
{
    DPORT_STALL_OTHER_CPU_START();
}

void IRAM_ATTR esp_dport_access_stall_other_cpu_end_wrap(void)
{
    DPORT_STALL_OTHER_CPU_END();
}

#ifndef CONFIG_FREERTOS_UNICORE
static void dport_access_init_core(void *arg)
{
    int core_id = 0;
    uint32_t intr_source = ETS_FROM_CPU_INTR2_SOURCE;


    core_id = xPortGetCoreID();
    if (core_id == 1) {
        intr_source = ETS_FROM_CPU_INTR3_SOURCE;
    }

    ESP_INTR_DISABLE(ETS_DPORT_INUM);
    intr_matrix_set(core_id, intr_source, ETS_DPORT_INUM);
    ESP_INTR_ENABLE(ETS_DPORT_INUM);

    dport_access_ref[core_id] = 0;
    dport_access_start[core_id] = 0;
    dport_access_end[core_id] = 0;
    dport_core_state[core_id] = DPORT_CORE_STATE_RUNNING;

    vTaskDelete(NULL);
}
#endif

/*  Defer initialisation until after scheduler is running */
void esp_dport_access_int_init(void)
{
#ifndef CONFIG_FREERTOS_UNICORE
    portBASE_TYPE res = xTaskCreatePinnedToCore(&dport_access_init_core, "dport", configMINIMAL_STACK_SIZE, NULL, 5, NULL, xPortGetCoreID());
    assert(res == pdTRUE);
#endif
}

void IRAM_ATTR esp_dport_access_int_pause(void)
{
#ifndef CONFIG_FREERTOS_UNICORE
    portENTER_CRITICAL_ISR(&g_dport_mux);
    dport_core_state[0] = DPORT_CORE_STATE_IDLE;
    dport_core_state[1] = DPORT_CORE_STATE_IDLE;
    portEXIT_CRITICAL_ISR(&g_dport_mux);
#endif
}

//Used in panic code: the enter_critical stuff may be messed up so we just stop everything without checking the mux.
void IRAM_ATTR esp_dport_access_int_abort(void)
{
#ifndef CONFIG_FREERTOS_UNICORE
    dport_core_state[0] = DPORT_CORE_STATE_IDLE;
    dport_core_state[1] = DPORT_CORE_STATE_IDLE;
#endif
}

void IRAM_ATTR esp_dport_access_int_resume(void)
{
#ifndef CONFIG_FREERTOS_UNICORE
    portENTER_CRITICAL_ISR(&g_dport_mux);
    dport_core_state[0] = DPORT_CORE_STATE_RUNNING;
    dport_core_state[1] = DPORT_CORE_STATE_RUNNING;
    portEXIT_CRITICAL_ISR(&g_dport_mux);
#endif
}

