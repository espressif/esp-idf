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

#include <stdlib.h>
#include <assert.h>
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
#include "esp_spi_flash.h"


/*
    Driver for SPI flash read/write/erase operations

    In order to perform some flash operations, we need to make sure both CPUs
    are not running any code from flash for the duration of the flash operation.
    In a single-core setup this is easy: we disable interrupts/scheduler and do
    the flash operation. In the dual-core setup this is slightly more complicated.
    We need to make sure that the other CPU doesn't run any code from flash.


    When SPI flash API is called on CPU A (can be PRO or APP), we start
    spi_flash_op_block_func function on CPU B using esp_ipc_call API. This API
    wakes up high priority task on CPU B and tells it to execute given function,
    in this case spi_flash_op_block_func. This function disables cache on CPU B and
    signals that cache is disabled by setting s_flash_op_can_start flag.
    Then the task on CPU A disables cache as well, and proceeds to execute flash
    operation.

    While flash operation is running, interrupts can still run on CPU B.
    We assume that all interrupt code is placed into RAM.

    Once flash operation is complete, function on CPU A sets another flag,
    s_flash_op_complete, to let the task on CPU B know that it can re-enable
    cache and release the CPU. Then the function on CPU A re-enables the cache on
    CPU A as well and returns control to the calling code.

    Additionally, all API functions are protected with a mutex (s_flash_op_mutex).

    In a single core environment (CONFIG_FREERTOS_UNICORE enabled), we simply
    disable both caches, no inter-CPU communication takes place.
*/

static esp_err_t spi_flash_translate_rc(SpiFlashOpResult rc);
static void IRAM_ATTR spi_flash_disable_cache(uint32_t cpuid, uint32_t* saved_state);
static void IRAM_ATTR spi_flash_restore_cache(uint32_t cpuid, uint32_t saved_state);

static uint32_t s_flash_op_cache_state[2];

#ifndef CONFIG_FREERTOS_UNICORE
static SemaphoreHandle_t s_flash_op_mutex;
static bool s_flash_op_can_start = false;
static bool s_flash_op_complete = false;
#endif //CONFIG_FREERTOS_UNICORE


#ifndef CONFIG_FREERTOS_UNICORE

static void IRAM_ATTR spi_flash_op_block_func(void* arg)
{
    // Disable scheduler on this CPU
    vTaskSuspendAll();
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
    // Re-enable scheduler
    xTaskResumeAll();
}

void spi_flash_init()
{
    s_flash_op_mutex = xSemaphoreCreateMutex();
}

static void IRAM_ATTR spi_flash_disable_interrupts_caches_and_other_cpu()
{
    // Take the API lock
    xSemaphoreTake(s_flash_op_mutex, portMAX_DELAY);

    const uint32_t cpuid = xPortGetCoreID();
    const uint32_t other_cpuid = (cpuid == 0) ? 1 : 0;

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
        esp_ipc_call(other_cpuid, &spi_flash_op_block_func, (void*) other_cpuid);
        while (!s_flash_op_can_start) {
            // Busy loop and wait for spi_flash_op_block_func to disable cache
            // on the other CPU
        }
        // Disable scheduler on CPU cpuid
        vTaskSuspendAll();
        // This is guaranteed to run on CPU <cpuid> because the other CPU is now
        // occupied by highest priority task
        assert(xPortGetCoreID() == cpuid);
    }
    // Disable cache on this CPU as well
    spi_flash_disable_cache(cpuid, &s_flash_op_cache_state[cpuid]);
}

static void IRAM_ATTR spi_flash_enable_interrupts_caches_and_other_cpu()
{
    const uint32_t cpuid = xPortGetCoreID();
    const uint32_t other_cpuid = (cpuid == 0) ? 1 : 0;

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
        // Resume tasks on the current CPU
        xTaskResumeAll();
    }
    // Release API lock
    xSemaphoreGive(s_flash_op_mutex);
}

#else  // CONFIG_FREERTOS_UNICORE

void spi_flash_init()
{
    // No-op in single core mode
}

static void IRAM_ATTR spi_flash_disable_interrupts_caches_and_other_cpu()
{
    vTaskSuspendAll();
    spi_flash_disable_cache(0, &s_flash_op_cache_state[0]);
}

static void IRAM_ATTR spi_flash_enable_interrupts_caches_and_other_cpu()
{
    spi_flash_restore_cache(0, s_flash_op_cache_state[0]);
    xTaskResumeAll();
}

#endif // CONFIG_FREERTOS_UNICORE


SpiFlashOpResult IRAM_ATTR spi_flash_unlock()
{
    static bool unlocked = false;
    if (!unlocked) {
        SpiFlashOpResult rc = SPIUnlock();
        if (rc != SPI_FLASH_RESULT_OK) {
            return rc;
        }
        unlocked = true;
    }
    return SPI_FLASH_RESULT_OK;
}

esp_err_t IRAM_ATTR spi_flash_erase_sector(uint16_t sec)
{
    spi_flash_disable_interrupts_caches_and_other_cpu();
    SpiFlashOpResult rc;
    rc = spi_flash_unlock();
    if (rc == SPI_FLASH_RESULT_OK) {
        rc = SPIEraseSector(sec);
    }
    spi_flash_enable_interrupts_caches_and_other_cpu();
    return spi_flash_translate_rc(rc);
}

esp_err_t IRAM_ATTR spi_flash_write(uint32_t dest_addr, const uint32_t *src, uint32_t size)
{
    spi_flash_disable_interrupts_caches_and_other_cpu();
    SpiFlashOpResult rc;
    rc = spi_flash_unlock();
    if (rc == SPI_FLASH_RESULT_OK) {
        rc = SPIWrite(dest_addr, src, (int32_t) size);
    }
    spi_flash_enable_interrupts_caches_and_other_cpu();
    return spi_flash_translate_rc(rc);
}

esp_err_t IRAM_ATTR spi_flash_read(uint32_t src_addr, uint32_t *dest, uint32_t size)
{
    spi_flash_disable_interrupts_caches_and_other_cpu();
    SpiFlashOpResult rc;
    rc = SPIRead(src_addr, dest, (int32_t) size);
    spi_flash_enable_interrupts_caches_and_other_cpu();
    return spi_flash_translate_rc(rc);
}

static esp_err_t spi_flash_translate_rc(SpiFlashOpResult rc)
{
    switch (rc) {
    case SPI_FLASH_RESULT_OK:
        return ESP_OK;
    case SPI_FLASH_RESULT_TIMEOUT:
        return ESP_ERR_FLASH_OP_TIMEOUT;
    case SPI_FLASH_RESULT_ERR:
    default:
        return ESP_ERR_FLASH_OP_FAIL;
    }
}

static const uint32_t cache_mask  = DPORT_APP_CACHE_MASK_OPSDRAM | DPORT_APP_CACHE_MASK_DROM0 |
        DPORT_APP_CACHE_MASK_DRAM1 | DPORT_APP_CACHE_MASK_IROM0 |
        DPORT_APP_CACHE_MASK_IRAM1 | DPORT_APP_CACHE_MASK_IRAM0;

static void IRAM_ATTR spi_flash_disable_cache(uint32_t cpuid, uint32_t* saved_state)
{
    uint32_t ret = 0;
    if (cpuid == 0) {
        ret |= GET_PERI_REG_BITS2(PRO_CACHE_CTRL1_REG, cache_mask, 0);
        while (GET_PERI_REG_BITS2(PRO_DCACHE_DBUG_REG0, DPORT_PRO_CACHE_STATE, DPORT_PRO_CACHE_STATE_S) != 1) {
            ;
        }
        SET_PERI_REG_BITS(PRO_CACHE_CTRL_REG, 1, 0, DPORT_PRO_CACHE_ENABLE_S);
    } else {
        ret |= GET_PERI_REG_BITS2(APP_CACHE_CTRL1_REG, cache_mask, 0);
        while (GET_PERI_REG_BITS2(APP_DCACHE_DBUG_REG0, DPORT_APP_CACHE_STATE, DPORT_APP_CACHE_STATE_S) != 1) {
            ;
        }
        SET_PERI_REG_BITS(APP_CACHE_CTRL_REG, 1, 0, DPORT_APP_CACHE_ENABLE_S);
    }
    *saved_state = ret;
}

static void IRAM_ATTR spi_flash_restore_cache(uint32_t cpuid, uint32_t saved_state)
{
    if (cpuid == 0) {
        SET_PERI_REG_BITS(PRO_CACHE_CTRL_REG, 1, 1, DPORT_PRO_CACHE_ENABLE_S);
        SET_PERI_REG_BITS(PRO_CACHE_CTRL1_REG, cache_mask, saved_state, 0);
    } else {
        SET_PERI_REG_BITS(APP_CACHE_CTRL_REG, 1, 1, DPORT_APP_CACHE_ENABLE_S);
        SET_PERI_REG_BITS(APP_CACHE_CTRL1_REG, cache_mask, saved_state, 0);
    }
}
