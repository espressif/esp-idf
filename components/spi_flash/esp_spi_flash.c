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

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <esp_spi_flash.h>
#include <rom/spi_flash.h>
#include <rom/cache.h>
#include <esp_attr.h>
#include <soc/dport_reg.h>
#include "sdkconfig.h"

/*
    Driver for SPI flash read/write/erase operations

    In order to perform some flash operations, we need to make sure both CPUs
    are not running any code from flash for the duration of the flash operation.
    In a single-core setup this is easy: we disable interrupts/scheduler and do
    the flash operation. In the dual-core setup this is slightly more complicated.
    We need to make sure that the other CPU doesn't run any code from flash.

    SPI flash driver starts two tasks (spi_flash_op_block_task), one pinned to
    each CPU. Each task is associated with its own semaphore.

    When SPI flash API is called on CPU A (can be PRO or APP), we wake up the task
    on CPU B by "giving" the semaphore associated with it. Tasks resumes, disables
    cache on CPU B, and acknowledges that it has taken the semaphore by setting
    a flag (s_flash_op_can_start). Flash API function running on CPU A waits for
    this flag to be set. Once the flag is set, it disables cache on CPU A and
    starts flash operation.

    While flash operation is running, interrupts can still run on CPU B. 
    We assume that all interrupt code is placed into RAM.

    Once flash operation is complete, function on CPU A sets another flag,
    s_flash_op_complete, to let the task on CPU B know that it can re-enable
    cache and release the CPU. Then the function on CPU A re-enables the cache on
    CPU A as well and returns control to the calling code. Task on CPU B returns
    to suspended state by "taking" the semaphore.

    Additionally, all API functions are protected with a mutex (s_flash_op_mutex).

    In a single core environment (CONFIG_FREERTOS_UNICORE enabled), we simply
    disable both caches, no inter-CPU communication takes place.
*/

static esp_err_t spi_flash_translate_rc(SpiFlashOpResult rc);
extern void Cache_Flush(int);

#ifndef CONFIG_FREERTOS_UNICORE

static TaskHandle_t s_flash_op_tasks[2];
static SemaphoreHandle_t s_flash_op_mutex;
static SemaphoreHandle_t s_flash_op_sem[2];
static bool s_flash_op_can_start = false;
static bool s_flash_op_complete = false;


// Task whose duty is to block other tasks from running on a given CPU
static void IRAM_ATTR spi_flash_op_block_task(void* arg)
{
    uint32_t cpuid = (uint32_t) arg;
    while (true) {
        // Wait for flash operation to be initiated.
        // This will be indicated by giving the semaphore corresponding to
        // this CPU.
        if (xSemaphoreTake(s_flash_op_sem[cpuid], portMAX_DELAY) != pdTRUE) {
            // TODO: when can this happen?
            abort();
        }
        // Disable cache on this CPU
        Cache_Read_Disable(cpuid);
        // Signal to the flash API function that flash operation can start
        s_flash_op_can_start = true;
        while (!s_flash_op_complete) {
            // until we have a way to use interrupts for inter-CPU communication,
            // busy loop here and wait for the other CPU to finish flash operation
        }
        // Flash operation is complete, re-enable cache
        Cache_Read_Enable(cpuid);
    }
    // TODO: currently this is unreachable code. Introduce spi_flash_uninit 
    // function which will signal to both tasks that they can shut down.
    // Not critical at this point, we don't have a use case for stopping
    // SPI flash driver yet.
    // Also need to delete the semaphore here.
    vTaskDelete(NULL);
}

void spi_flash_init()
{
    s_flash_op_can_start = false;
    s_flash_op_complete = false;
    s_flash_op_sem[0] = xSemaphoreCreateBinary();
    s_flash_op_sem[1] = xSemaphoreCreateBinary();
    s_flash_op_mutex = xSemaphoreCreateMutex();
    // Start two tasks, one on each CPU, with max priorities
    // TODO: optimize stack usage. Stack size 512 is too small.
    xTaskCreatePinnedToCore(spi_flash_op_block_task, "flash_op_pro", 1024, (void*) 0,
                            configMAX_PRIORITIES - 1, &s_flash_op_tasks[0], 0);
    xTaskCreatePinnedToCore(spi_flash_op_block_task, "flash_op_app", 1024, (void*) 1,
                            configMAX_PRIORITIES - 1, &s_flash_op_tasks[1], 1);
}

static void IRAM_ATTR spi_flash_disable_interrupts_caches_and_other_cpu()
{
    // Take the API lock
    xSemaphoreTake(s_flash_op_mutex, portMAX_DELAY);
    const uint32_t cpuid = xPortGetCoreID();
    uint32_t other_cpuid = !cpuid;
    s_flash_op_can_start = false;
    s_flash_op_complete = false;
    // Signal to the spi_flash_op_block_task on the other CPU that we need it to
    // disable cache there and block other tasks from executing.
    xSemaphoreGive(s_flash_op_sem[other_cpuid]);
    while (!s_flash_op_can_start) {
        // Busy loop and wait for spi_flash_op_block_task to take the semaphore on the
        // other CPU.
    }
    vTaskSuspendAll();
    // Disable cache on this CPU as well
    Cache_Read_Disable(cpuid);
}

static void IRAM_ATTR spi_flash_enable_interrupts_caches_and_other_cpu()
{
    uint32_t cpuid = xPortGetCoreID();
    // Signal to spi_flash_op_block_task that flash operation is complete
    s_flash_op_complete = true;
    // Re-enable cache on this CPU
    Cache_Read_Enable(cpuid);
    // Release API lock
    xSemaphoreGive(s_flash_op_mutex);
    xTaskResumeAll();
}

#else  // CONFIG_FREERTOS_UNICORE

void spi_flash_init()
{
    // No-op in single core mode
}

static void IRAM_ATTR spi_flash_disable_interrupts_caches_and_other_cpu()
{
    vTaskSuspendAll();
    Cache_Read_Disable(0);
    Cache_Read_Disable(1);
}

static void IRAM_ATTR spi_flash_enable_interrupts_caches_and_other_cpu()
{
    Cache_Read_Enable(0);
    Cache_Read_Enable(1);
    xTaskResumeAll();
}

#endif // CONFIG_FREERTOS_UNICORE


esp_err_t IRAM_ATTR spi_flash_erase_sector(uint16_t sec)
{
    spi_flash_disable_interrupts_caches_and_other_cpu();
    SpiFlashOpResult rc;
    rc = SPIUnlock();
    if (rc == SPI_FLASH_RESULT_OK) {
        rc = SPIEraseSector(sec);
    }
    Cache_Flush(0);
    Cache_Flush(1);
    spi_flash_enable_interrupts_caches_and_other_cpu();
    return spi_flash_translate_rc(rc);
}

esp_err_t IRAM_ATTR spi_flash_write(uint32_t dest_addr, const uint32_t *src, uint32_t size)
{
    spi_flash_disable_interrupts_caches_and_other_cpu();
    SpiFlashOpResult rc;
    rc = SPIUnlock();
    if (rc == SPI_FLASH_RESULT_OK) {
        rc = SPIWrite(dest_addr, src, (int32_t) size);
    }
    Cache_Flush(0);
    Cache_Flush(1);
    spi_flash_enable_interrupts_caches_and_other_cpu();
    return spi_flash_translate_rc(rc);
}

esp_err_t IRAM_ATTR spi_flash_read(uint32_t src_addr, uint32_t *dest, uint32_t size)
{
    spi_flash_disable_interrupts_caches_and_other_cpu();
    SpiFlashOpResult rc;
    rc = SPIRead(src_addr, dest, (int32_t) size);
    Cache_Flush(0);
    Cache_Flush(1);
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
