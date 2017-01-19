// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <rom/spi_flash.h>
#include <rom/cache.h>
#include <soc/soc.h>
#include <soc/dport_reg.h>
#include "sdkconfig.h"
#include "esp_ipc.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_spi_flash.h"
#include "esp_log.h"


static void IRAM_ATTR spi_flash_disable_cache(uint32_t cpuid, uint32_t* saved_state);
static void IRAM_ATTR spi_flash_restore_cache(uint32_t cpuid, uint32_t saved_state);

static uint32_t s_flash_op_cache_state[2];

#ifndef CONFIG_FREERTOS_UNICORE
static SemaphoreHandle_t s_flash_op_mutex;
static volatile bool s_flash_op_can_start = false;
static volatile bool s_flash_op_complete = false;
#ifndef NDEBUG
static volatile int s_flash_op_cpu = -1;
#endif

void spi_flash_init_lock()
{
    s_flash_op_mutex = xSemaphoreCreateMutex();
}

void spi_flash_op_lock()
{
    xSemaphoreTake(s_flash_op_mutex, portMAX_DELAY);
}

void spi_flash_op_unlock()
{
    xSemaphoreGive(s_flash_op_mutex);
}

void IRAM_ATTR spi_flash_op_block_func(void* arg)
{
    // Disable scheduler on this CPU
    vTaskSuspendAll();
    // Restore interrupts that aren't located in IRAM
    esp_intr_noniram_disable();
    uint32_t cpuid = (uint32_t) arg;
    // Disable cache so that flash operation can start
    spi_flash_disable_cache(cpuid, &s_flash_op_cache_state[cpuid]);
    s_flash_op_can_start = true;
    while (!s_flash_op_complete) {
        // until we have a way to use interrupts for inter-CPU communication,
        // busy loop here and wait for the other CPU to finish flash operation
    }
    // Flash operation is complete, re-enable cache
    spi_flash_restore_cache(cpuid, s_flash_op_cache_state[cpuid]);
    // Restore interrupts that aren't located in IRAM
    esp_intr_noniram_enable();
    // Re-enable scheduler
    xTaskResumeAll();
}

void IRAM_ATTR spi_flash_disable_interrupts_caches_and_other_cpu()
{
    spi_flash_op_lock();

    const uint32_t cpuid = xPortGetCoreID();
    const uint32_t other_cpuid = (cpuid == 0) ? 1 : 0;
#ifndef NDEBUG
    // For sanity check later: record the CPU which has started doing flash operation
    assert(s_flash_op_cpu == -1);
    s_flash_op_cpu = cpuid;
#endif

    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        // Scheduler hasn't been started yet, it means that spi_flash API is being
        // called from the 2nd stage bootloader or from user_start_cpu0, i.e. from
        // PRO CPU. APP CPU is either in reset or spinning inside user_start_cpu1,
        // which is in IRAM. So it is safe to disable cache for the other_cpuid here.
        assert(other_cpuid == 1);
        spi_flash_disable_cache(other_cpuid, &s_flash_op_cache_state[other_cpuid]);
    } else {
        // Signal to the spi_flash_op_block_task on the other CPU that we need it to
        // disable cache there and block other tasks from executing.
        s_flash_op_can_start = false;
        s_flash_op_complete = false;
        esp_err_t ret = esp_ipc_call(other_cpuid, &spi_flash_op_block_func, (void*) other_cpuid);
        assert(ret == ESP_OK);
        while (!s_flash_op_can_start) {
            // Busy loop and wait for spi_flash_op_block_func to disable cache
            // on the other CPU
        }
        // Disable scheduler on the current CPU
        vTaskSuspendAll();
        // This is guaranteed to run on CPU <cpuid> because the other CPU is now
        // occupied by highest priority task
        assert(xPortGetCoreID() == cpuid);
    }
    // Kill interrupts that aren't located in IRAM
    esp_intr_noniram_disable();
    // Disable cache on this CPU as well
    spi_flash_disable_cache(cpuid, &s_flash_op_cache_state[cpuid]);
}

void IRAM_ATTR spi_flash_enable_interrupts_caches_and_other_cpu()
{
    const uint32_t cpuid = xPortGetCoreID();
    const uint32_t other_cpuid = (cpuid == 0) ? 1 : 0;
#ifndef NDEBUG
    // Sanity check: flash operation ends on the same CPU as it has started
    assert(cpuid == s_flash_op_cpu);
    s_flash_op_cpu = -1;
#endif

    // Re-enable cache on this CPU
    spi_flash_restore_cache(cpuid, s_flash_op_cache_state[cpuid]);

    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        // Scheduler is not running yet — this means we are running on PRO CPU.
        // other_cpuid is APP CPU, and it is either in reset or is spinning in
        // user_start_cpu1, which is in IRAM. So we can simply reenable cache.
        assert(other_cpuid == 1);
        spi_flash_restore_cache(other_cpuid, s_flash_op_cache_state[other_cpuid]);
    } else {
        // Signal to spi_flash_op_block_task that flash operation is complete
        s_flash_op_complete = true;
    }
    // Re-enable non-iram interrupts
    esp_intr_noniram_enable();

    // Resume tasks on the current CPU, if the scheduler has started.
    // NOTE: enabling non-IRAM interrupts has to happen before this,
    // because once the scheduler has started, due to preemption the
    // current task can end up being moved to the other CPU.
    // But esp_intr_noniram_enable has to be called on the same CPU which
    // called esp_intr_noniram_disable
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        xTaskResumeAll();
    }
    // Release API lock
    spi_flash_op_unlock();
}

void IRAM_ATTR spi_flash_disable_interrupts_caches_and_other_cpu_no_os()
{
    const uint32_t cpuid = xPortGetCoreID();
    const uint32_t other_cpuid = (cpuid == 0) ? 1 : 0;

    // do not care about other CPU, it was halted upon entering panic handler
    spi_flash_disable_cache(other_cpuid, &s_flash_op_cache_state[other_cpuid]);
    // Kill interrupts that aren't located in IRAM
    esp_intr_noniram_disable();
    // Disable cache on this CPU as well
    spi_flash_disable_cache(cpuid, &s_flash_op_cache_state[cpuid]);
}

void IRAM_ATTR spi_flash_enable_interrupts_caches_no_os()
{
    const uint32_t cpuid = xPortGetCoreID();

    // Re-enable cache on this CPU
    spi_flash_restore_cache(cpuid, s_flash_op_cache_state[cpuid]);
    // Re-enable non-iram interrupts
    esp_intr_noniram_enable();
}

#else // CONFIG_FREERTOS_UNICORE

void spi_flash_init_lock()
{
}

void spi_flash_op_lock()
{
    vTaskSuspendAll();
}

void spi_flash_op_unlock()
{
    xTaskResumeAll();
}


void IRAM_ATTR spi_flash_disable_interrupts_caches_and_other_cpu()
{
    esp_intr_noniram_disable();
    spi_flash_op_lock();
    spi_flash_disable_cache(0, &s_flash_op_cache_state[0]);
}

void IRAM_ATTR spi_flash_enable_interrupts_caches_and_other_cpu()
{
    spi_flash_restore_cache(0, s_flash_op_cache_state[0]);
    spi_flash_op_unlock();
    esp_intr_noniram_enable();
}

void IRAM_ATTR spi_flash_disable_interrupts_caches_and_other_cpu_no_os()
{
    // Kill interrupts that aren't located in IRAM
    esp_intr_noniram_disable();
    // Disable cache on this CPU as well
    spi_flash_disable_cache(0, &s_flash_op_cache_state[0]);
}

void IRAM_ATTR spi_flash_enable_interrupts_caches_no_os()
{
    // Re-enable cache on this CPU
    spi_flash_restore_cache(0, s_flash_op_cache_state[0]);
    // Re-enable non-iram interrupts
    esp_intr_noniram_enable();
}

#endif // CONFIG_FREERTOS_UNICORE

/**
 * The following two functions are replacements for Cache_Read_Disable and Cache_Read_Enable
 * function in ROM. They are used to work around a bug where Cache_Read_Disable requires a call to
 * Cache_Flush before Cache_Read_Enable, even if cached data was not modified.
 */

static const uint32_t cache_mask  = DPORT_APP_CACHE_MASK_OPSDRAM | DPORT_APP_CACHE_MASK_DROM0 |
        DPORT_APP_CACHE_MASK_DRAM1 | DPORT_APP_CACHE_MASK_IROM0 |
        DPORT_APP_CACHE_MASK_IRAM1 | DPORT_APP_CACHE_MASK_IRAM0;

static void IRAM_ATTR spi_flash_disable_cache(uint32_t cpuid, uint32_t* saved_state)
{
    uint32_t ret = 0;
    if (cpuid == 0) {
        ret |= GET_PERI_REG_BITS2(DPORT_PRO_CACHE_CTRL1_REG, cache_mask, 0);
        while (GET_PERI_REG_BITS2(DPORT_PRO_DCACHE_DBUG0_REG, DPORT_PRO_CACHE_STATE, DPORT_PRO_CACHE_STATE_S) != 1) {
            ;
        }
        SET_PERI_REG_BITS(DPORT_PRO_CACHE_CTRL_REG, 1, 0, DPORT_PRO_CACHE_ENABLE_S);
    } else {
        ret |= GET_PERI_REG_BITS2(DPORT_APP_CACHE_CTRL1_REG, cache_mask, 0);
        while (GET_PERI_REG_BITS2(DPORT_APP_DCACHE_DBUG0_REG, DPORT_APP_CACHE_STATE, DPORT_APP_CACHE_STATE_S) != 1) {
            ;
        }
        SET_PERI_REG_BITS(DPORT_APP_CACHE_CTRL_REG, 1, 0, DPORT_APP_CACHE_ENABLE_S);
    }
    *saved_state = ret;
}

static void IRAM_ATTR spi_flash_restore_cache(uint32_t cpuid, uint32_t saved_state)
{
    if (cpuid == 0) {
        SET_PERI_REG_BITS(DPORT_PRO_CACHE_CTRL_REG, 1, 1, DPORT_PRO_CACHE_ENABLE_S);
        SET_PERI_REG_BITS(DPORT_PRO_CACHE_CTRL1_REG, cache_mask, saved_state, 0);
    } else {
        SET_PERI_REG_BITS(DPORT_APP_CACHE_CTRL_REG, 1, 1, DPORT_APP_CACHE_ENABLE_S);
        SET_PERI_REG_BITS(DPORT_APP_CACHE_CTRL1_REG, cache_mask, saved_state, 0);
    }
}

