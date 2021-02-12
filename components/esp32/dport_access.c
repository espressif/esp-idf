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
#include "esp_intr_alloc.h"
#include "esp32/rom/ets_sys.h"
#include "esp32/rom/uart.h"

#include "soc/cpu.h"
#include "soc/dport_reg.h"
#include "soc/spi_periph.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

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

    /* If this fails then the minimum stack size for this config is too close to running out */
    assert(uxTaskGetStackHighWaterMark(NULL) > 128);

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

/**
 * @brief Read a sequence of DPORT registers to the buffer, SMP-safe version.
 *
 * This implementation uses a method of the pre-reading of the APB register
 * before reading the register of the DPORT, without stall other CPU.
 * There is disable/enable interrupt.
 *
 * @param[out] buff_out  Contains the read data.
 * @param[in]  address   Initial address for reading registers.
 * @param[in]  num_words The number of words.
 */
void IRAM_ATTR esp_dport_access_read_buffer(uint32_t *buff_out, uint32_t address, uint32_t num_words)
{
    DPORT_INTERRUPT_DISABLE();
    for (uint32_t i = 0;  i < num_words; ++i) {
        buff_out[i] = DPORT_SEQUENCE_REG_READ(address + i * 4);
    }
    DPORT_INTERRUPT_RESTORE();
}

/**
 * @brief Read value from register, SMP-safe version.
 *
 * This method uses the pre-reading of the APB register before reading the register of the DPORT.
 * This implementation is useful for reading DORT registers for single reading without stall other CPU.
 * There is disable/enable interrupt.
 *
 * @param reg Register address
 * @return Value
 */
uint32_t IRAM_ATTR esp_dport_access_reg_read(uint32_t reg)
{
#if defined(BOOTLOADER_BUILD) || !defined(CONFIG_ESP32_DPORT_WORKAROUND) || !defined(ESP_PLATFORM)
    return _DPORT_REG_READ(reg);
#else
    uint32_t apb;
    unsigned int intLvl;
    __asm__ __volatile__ (\
                  "rsil %[LVL], "XTSTR(CONFIG_ESP32_DPORT_DIS_INTERRUPT_LVL)"\n"\
                  "movi %[APB], "XTSTR(0x3ff40078)"\n"\
                  "l32i %[APB], %[APB], 0\n"\
                  "l32i %[REG], %[REG], 0\n"\
                  "wsr  %[LVL], "XTSTR(PS)"\n"\
                  "rsync\n"\
                  : [APB]"=a"(apb), [REG]"+a"(reg), [LVL]"=a"(intLvl)\
                  : \
                  : "memory" \
                  );
    return reg;
#endif
}

/**
 * @brief Read value from register, NOT SMP-safe version.
 *
 * This method uses the pre-reading of the APB register before reading the register of the DPORT.
 * There is not disable/enable interrupt.
 * The difference from DPORT_REG_READ() is that the user himself must disable interrupts while DPORT reading.
 * This implementation is useful for reading DORT registers in loop without stall other CPU. Note the usage example.
 * The recommended way to read registers sequentially without stall other CPU
 * is to use the method esp_dport_read_buffer(buff_out, address, num_words). It allows you to read registers in the buffer.
 *
 * \code{c}
 * // This example shows how to use it.
 * { // Use curly brackets to limit the visibility of variables in macros DPORT_INTERRUPT_DISABLE/RESTORE.
 *     DPORT_INTERRUPT_DISABLE(); // Disable interrupt only on current CPU.
 *     for (i = 0; i < max; ++i) {
 *        array[i] = esp_dport_access_sequence_reg_read(Address + i * 4); // reading DPORT registers
 *     }
 *     DPORT_INTERRUPT_RESTORE(); // restore the previous interrupt level
 * }
 * \endcode
 *
 * @param reg Register address
 * @return Value
 */
uint32_t IRAM_ATTR esp_dport_access_sequence_reg_read(uint32_t reg)
{
#if defined(BOOTLOADER_BUILD) || !defined(CONFIG_ESP32_DPORT_WORKAROUND) || !defined(ESP_PLATFORM)
    return _DPORT_REG_READ(reg);
#else
    uint32_t apb;
    __asm__ __volatile__ (\
                  "movi %[APB], "XTSTR(0x3ff40078)"\n"\
                  "l32i %[APB], %[APB], 0\n"\
                  "l32i %[REG], %[REG], 0\n"\
                  : [APB]"=a"(apb), [REG]"+a"(reg)\
                  : \
                  : "memory" \
                  );
    return reg;
#endif
}
