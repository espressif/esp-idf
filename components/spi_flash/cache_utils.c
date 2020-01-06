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
#if CONFIG_IDF_TARGET_ESP32
#include <esp32/rom/spi_flash.h>
#include <esp32/rom/cache.h>
#elif CONFIG_IDF_TARGET_ESP32S2BETA
#include "esp32s2beta/rom/spi_flash.h"
#include "esp32s2beta/rom/cache.h"
#endif
#include <soc/soc.h>
#include <soc/dport_reg.h>
#include "sdkconfig.h"
#include "esp_ipc.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_spi_flash.h"
#include "esp_log.h"

static __attribute__((unused)) const char* TAG = "cache";

#define DPORT_CACHE_BIT(cpuid, regid) DPORT_ ## cpuid ## regid

#define DPORT_CACHE_MASK(cpuid) (DPORT_CACHE_BIT(cpuid, _CACHE_MASK_OPSDRAM) | DPORT_CACHE_BIT(cpuid, _CACHE_MASK_DROM0) | \
                                DPORT_CACHE_BIT(cpuid, _CACHE_MASK_DRAM1) | DPORT_CACHE_BIT(cpuid, _CACHE_MASK_IROM0) | \
                                DPORT_CACHE_BIT(cpuid, _CACHE_MASK_IRAM1) | DPORT_CACHE_BIT(cpuid, _CACHE_MASK_IRAM0) )

#define DPORT_CACHE_VAL(cpuid) (~(DPORT_CACHE_BIT(cpuid, _CACHE_MASK_DROM0) | \
                                        DPORT_CACHE_BIT(cpuid, _CACHE_MASK_DRAM1) | \
                                        DPORT_CACHE_BIT(cpuid, _CACHE_MASK_IRAM0)))

#define DPORT_CACHE_GET_VAL(cpuid) (cpuid == 0) ? DPORT_CACHE_VAL(PRO) : DPORT_CACHE_VAL(APP)
#define DPORT_CACHE_GET_MASK(cpuid) (cpuid == 0) ? DPORT_CACHE_MASK(PRO) : DPORT_CACHE_MASK(APP)

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

void spi_flash_init_lock(void)
{
    s_flash_op_mutex = xSemaphoreCreateRecursiveMutex();
    assert(s_flash_op_mutex != NULL);
}

void spi_flash_op_lock(void)
{
    xSemaphoreTakeRecursive(s_flash_op_mutex, portMAX_DELAY);
}

void spi_flash_op_unlock(void)
{
    xSemaphoreGiveRecursive(s_flash_op_mutex);
}
/*
 If you're going to modify this, keep in mind that while the flash caches of the pro and app
 cpu are separate, the psram cache is *not*. If one of the CPUs returns from a flash routine
 with its cache enabled but the other CPUs cache is not enabled yet, you will have problems
 when accessing psram from the former CPU.
*/

void IRAM_ATTR spi_flash_op_block_func(void* arg)
{
    // Disable scheduler on this CPU
    vTaskSuspendAll();
    // Restore interrupts that aren't located in IRAM
    esp_intr_noniram_disable();
    uint32_t cpuid = (uint32_t) arg;
    // s_flash_op_complete flag is cleared on *this* CPU, otherwise the other
    // CPU may reset the flag back to false before IPC task has a chance to check it
    // (if it is preempted by an ISR taking non-trivial amount of time)
    s_flash_op_complete = false;
    s_flash_op_can_start = true;
    while (!s_flash_op_complete) {
        // busy loop here and wait for the other CPU to finish flash operation
    }
    // Flash operation is complete, re-enable cache
    spi_flash_restore_cache(cpuid, s_flash_op_cache_state[cpuid]);
    // Restore interrupts that aren't located in IRAM
    esp_intr_noniram_enable();
    // Re-enable scheduler
    xTaskResumeAll();
}

void IRAM_ATTR spi_flash_disable_interrupts_caches_and_other_cpu(void)
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
        // Temporarily raise current task priority to prevent a deadlock while
        // waiting for IPC task to start on the other CPU
        int old_prio = uxTaskPriorityGet(NULL);
        vTaskPrioritySet(NULL, configMAX_PRIORITIES - 1);
        // Signal to the spi_flash_op_block_task on the other CPU that we need it to
        // disable cache there and block other tasks from executing.
        s_flash_op_can_start = false;
        esp_err_t ret = esp_ipc_call(other_cpuid, &spi_flash_op_block_func, (void*) other_cpuid);
        assert(ret == ESP_OK);
        while (!s_flash_op_can_start) {
            // Busy loop and wait for spi_flash_op_block_func to disable cache
            // on the other CPU
        }
        // Disable scheduler on the current CPU
        vTaskSuspendAll();
        // Can now set the priority back to the normal one
        vTaskPrioritySet(NULL, old_prio);
        // This is guaranteed to run on CPU <cpuid> because the other CPU is now
        // occupied by highest priority task
        assert(xPortGetCoreID() == cpuid);
    }
    // Kill interrupts that aren't located in IRAM
    esp_intr_noniram_disable();
    // This CPU executes this routine, with non-IRAM interrupts and the scheduler
    // disabled. The other CPU is spinning in the spi_flash_op_block_func task, also
    // with non-iram interrupts and the scheduler disabled. None of these CPUs will
    // touch external RAM or flash this way, so we can safely disable caches.
    spi_flash_disable_cache(cpuid, &s_flash_op_cache_state[cpuid]);
    spi_flash_disable_cache(other_cpuid, &s_flash_op_cache_state[other_cpuid]);
}

void IRAM_ATTR spi_flash_enable_interrupts_caches_and_other_cpu(void)
{
    const uint32_t cpuid = xPortGetCoreID();
    const uint32_t other_cpuid = (cpuid == 0) ? 1 : 0;
#ifndef NDEBUG
    // Sanity check: flash operation ends on the same CPU as it has started
    assert(cpuid == s_flash_op_cpu);
    // More sanity check: if scheduler isn't started, only CPU0 can call this.
    assert(!(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED && cpuid != 0));
    s_flash_op_cpu = -1;
#endif

    // Re-enable cache on both CPUs. After this, cache (flash and external RAM) should work again.
    spi_flash_restore_cache(cpuid, s_flash_op_cache_state[cpuid]);
    spi_flash_restore_cache(other_cpuid, s_flash_op_cache_state[other_cpuid]);

    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
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

void IRAM_ATTR spi_flash_disable_interrupts_caches_and_other_cpu_no_os(void)
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

void IRAM_ATTR spi_flash_enable_interrupts_caches_no_os(void)
{
    const uint32_t cpuid = xPortGetCoreID();

    // Re-enable cache on this CPU
    spi_flash_restore_cache(cpuid, s_flash_op_cache_state[cpuid]);
    // Re-enable non-iram interrupts
    esp_intr_noniram_enable();
}

#else // CONFIG_FREERTOS_UNICORE

void spi_flash_init_lock(void)
{
}

void spi_flash_op_lock(void)
{
    vTaskSuspendAll();
}

void spi_flash_op_unlock(void)
{
    xTaskResumeAll();
}


void IRAM_ATTR spi_flash_disable_interrupts_caches_and_other_cpu(void)
{
    spi_flash_op_lock();
    esp_intr_noniram_disable();
    spi_flash_disable_cache(0, &s_flash_op_cache_state[0]);
}

void IRAM_ATTR spi_flash_enable_interrupts_caches_and_other_cpu(void)
{
    spi_flash_restore_cache(0, s_flash_op_cache_state[0]);
    esp_intr_noniram_enable();
    spi_flash_op_unlock();
}

void IRAM_ATTR spi_flash_disable_interrupts_caches_and_other_cpu_no_os(void)
{
    // Kill interrupts that aren't located in IRAM
    esp_intr_noniram_disable();
    // Disable cache on this CPU as well
    spi_flash_disable_cache(0, &s_flash_op_cache_state[0]);
}

void IRAM_ATTR spi_flash_enable_interrupts_caches_no_os(void)
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
static void IRAM_ATTR spi_flash_disable_cache(uint32_t cpuid, uint32_t* saved_state)
{
#if CONFIG_IDF_TARGET_ESP32
    uint32_t ret = 0;
    const uint32_t cache_mask = DPORT_CACHE_GET_MASK(cpuid);
    if (cpuid == 0) {
        ret |= DPORT_GET_PERI_REG_BITS2(DPORT_PRO_CACHE_CTRL1_REG, cache_mask, 0);
        while (DPORT_GET_PERI_REG_BITS2(DPORT_PRO_DCACHE_DBUG0_REG, DPORT_PRO_CACHE_STATE, DPORT_PRO_CACHE_STATE_S) != 1) {
            ;
        }
        DPORT_SET_PERI_REG_BITS(DPORT_PRO_CACHE_CTRL_REG, 1, 0, DPORT_PRO_CACHE_ENABLE_S);
    }
#if !CONFIG_FREERTOS_UNICORE
    else {
        ret |= DPORT_GET_PERI_REG_BITS2(DPORT_APP_CACHE_CTRL1_REG, cache_mask, 0);
        while (DPORT_GET_PERI_REG_BITS2(DPORT_APP_DCACHE_DBUG0_REG, DPORT_APP_CACHE_STATE, DPORT_APP_CACHE_STATE_S) != 1) {
            ;
        }
        DPORT_SET_PERI_REG_BITS(DPORT_APP_CACHE_CTRL_REG, 1, 0, DPORT_APP_CACHE_ENABLE_S);
    }
#endif
    *saved_state = ret;
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    *saved_state = Cache_Suspend_ICache();
    if (!Cache_Drom0_Using_ICache()) {
        *(saved_state + 1) = Cache_Suspend_DCache();
    }
#endif
}

static void IRAM_ATTR spi_flash_restore_cache(uint32_t cpuid, uint32_t saved_state)
{
#if CONFIG_IDF_TARGET_ESP32
    const uint32_t cache_mask = DPORT_CACHE_GET_MASK(cpuid);
    if (cpuid == 0) {
        DPORT_SET_PERI_REG_BITS(DPORT_PRO_CACHE_CTRL_REG, 1, 1, DPORT_PRO_CACHE_ENABLE_S);
        DPORT_SET_PERI_REG_BITS(DPORT_PRO_CACHE_CTRL1_REG, cache_mask, saved_state, 0);
    }
#if !CONFIG_FREERTOS_UNICORE
    else {
        DPORT_SET_PERI_REG_BITS(DPORT_APP_CACHE_CTRL_REG, 1, 1, DPORT_APP_CACHE_ENABLE_S);
        DPORT_SET_PERI_REG_BITS(DPORT_APP_CACHE_CTRL1_REG, cache_mask, saved_state, 0);
    }
#endif
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    Cache_Resume_ICache(saved_state);
    if (!Cache_Drom0_Using_ICache()) {
        Cache_Resume_DCache(s_flash_op_cache_state[1]);
    }
#endif
}

IRAM_ATTR bool spi_flash_cache_enabled(void)
{
#if CONFIG_IDF_TARGET_ESP32
    bool result = (DPORT_REG_GET_BIT(DPORT_PRO_CACHE_CTRL_REG, DPORT_PRO_CACHE_ENABLE) != 0);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    bool result = (DPORT_REG_GET_BIT(DPORT_PRO_ICACHE_CTRL_REG, DPORT_PRO_ICACHE_ENABLE) != 0);
    if (!Cache_Drom0_Using_ICache()) {
        result = result && (DPORT_REG_GET_BIT(DPORT_PRO_DCACHE_CTRL_REG, DPORT_PRO_DCACHE_ENABLE) != 0);
    }
#endif
#if portNUM_PROCESSORS == 2
    result = result && (DPORT_REG_GET_BIT(DPORT_APP_CACHE_CTRL_REG, DPORT_APP_CACHE_ENABLE) != 0);
#endif
    return result;
}

#if CONFIG_IDF_TARGET_ESP32S2BETA
IRAM_ATTR void esp_config_instruction_cache_mode(void)
{
    cache_size_t cache_size;
    cache_ways_t cache_ways;
    cache_line_size_t cache_line_size;

#if CONFIG_ESP32S2_INSTRUCTION_CACHE_8KB
    Cache_Allocate_SRAM(CACHE_MEMORY_ICACHE_LOW, CACHE_MEMORY_INVALID, CACHE_MEMORY_INVALID, CACHE_MEMORY_INVALID);
    cache_size = CACHE_SIZE_8KB;
#else
    Cache_Allocate_SRAM(CACHE_MEMORY_ICACHE_LOW, CACHE_MEMORY_ICACHE_HIGH, CACHE_MEMORY_INVALID, CACHE_MEMORY_INVALID);
    cache_size = CACHE_SIZE_16KB;
#endif
#if CONFIG_ESP32S2_INSTRUCTION_CACHE_4WAYS
    cache_ways = CACHE_4WAYS_ASSOC;
#else
    cache_ways = CACHE_8WAYS_ASSOC;
#endif
#if CONFIG_ESP32S2_INSTRUCTION_CACHE_LINE_16B
    cache_line_size = CACHE_LINE_SIZE_16B;
#elif CONFIG_ESP32S2_INSTRUCTION_CACHE_LINE_32B
    cache_line_size = CACHE_LINE_SIZE_32B;
#else
    cache_line_size = CACHE_LINE_SIZE_64B;
#endif
    ESP_EARLY_LOGI(TAG, "Instruction cache \t: size %dKB, %dWays, cache line size %dByte", cache_size == CACHE_SIZE_8KB ? 8 : 16,cache_ways == CACHE_4WAYS_ASSOC ? 4: 8, cache_line_size == CACHE_LINE_SIZE_16B ? 16 : (cache_line_size == CACHE_LINE_SIZE_32B ? 32 : 64));
    Cache_Suspend_ICache();
    Cache_Set_ICache_Mode(cache_size, cache_ways, cache_line_size);
    Cache_Invalidate_ICache_All();
    Cache_Resume_ICache(0);
}

IRAM_ATTR void esp_config_data_cache_mode(void)
{
    cache_size_t cache_size;
    cache_ways_t cache_ways;
    cache_line_size_t cache_line_size;

#if CONFIG_ESP32S2_INSTRUCTION_CACHE_8KB
#if CONFIG_ESP32S2_DATA_CACHE_8KB
    Cache_Allocate_SRAM(CACHE_MEMORY_ICACHE_LOW, CACHE_MEMORY_DCACHE_LOW, CACHE_MEMORY_INVALID, CACHE_MEMORY_INVALID);
    cache_size = CACHE_SIZE_8KB;
#else
    Cache_Allocate_SRAM(CACHE_MEMORY_ICACHE_LOW, CACHE_MEMORY_DCACHE_LOW, CACHE_MEMORY_DCACHE_HIGH, CACHE_MEMORY_INVALID);
    cache_size = CACHE_SIZE_16KB;
#endif
#else
#if CONFIG_ESP32S2_DATA_CACHE_8KB
    Cache_Allocate_SRAM(CACHE_MEMORY_ICACHE_LOW, CACHE_MEMORY_ICACHE_HIGH, CACHE_MEMORY_DCACHE_LOW, CACHE_MEMORY_INVALID);
    cache_size = CACHE_SIZE_8KB;
#else
    Cache_Allocate_SRAM(CACHE_MEMORY_ICACHE_LOW, CACHE_MEMORY_ICACHE_HIGH, CACHE_MEMORY_DCACHE_LOW, CACHE_MEMORY_DCACHE_HIGH);
    cache_size = CACHE_SIZE_16KB;
#endif
#endif

#if CONFIG_ESP32S2_DATA_CACHE_4WAYS
    cache_ways = CACHE_4WAYS_ASSOC;
#else
    cache_ways = CACHE_8WAYS_ASSOC;
#endif
#if CONFIG_ESP32S2_DATA_CACHE_LINE_16B
    cache_line_size = CACHE_LINE_SIZE_16B;
#elif CONFIG_ESP32S2_DATA_CACHE_LINE_32B
    cache_line_size = CACHE_LINE_SIZE_32B;
#else
    cache_line_size = CACHE_LINE_SIZE_64B;
#endif
    ESP_EARLY_LOGI(TAG, "Data cache \t\t: size %dKB, %dWays, cache line size %dByte", cache_size == CACHE_SIZE_8KB ? 8 : 16, cache_ways == CACHE_4WAYS_ASSOC ? 4: 8, cache_line_size == CACHE_LINE_SIZE_16B ? 16 : (cache_line_size == CACHE_LINE_SIZE_32B ? 32 : 64));
    Cache_Set_DCache_Mode(cache_size, cache_ways, cache_line_size);
    Cache_Invalidate_DCache_All();
}

void esp_switch_rodata_to_dcache(void)
{
    REG_CLR_BIT(DPORT_PRO_DCACHE_CTRL1_REG, DPORT_PRO_DCACHE_MASK_DROM0);
    Cache_Drom0_Source_DCache();
    MMU_Drom_ICache_Unmap();
    REG_SET_BIT(DPORT_PRO_ICACHE_CTRL1_REG, DPORT_PRO_ICACHE_MASK_DROM0);
    ESP_EARLY_LOGI(TAG, "Switch rodata load path to data cache.");
}

static IRAM_ATTR void esp_enable_cache_flash_wrap(bool icache, bool dcache)
{
    uint32_t i_autoload, d_autoload;
    if (icache) {
        i_autoload = Cache_Suspend_ICache();
    }
    if (dcache) {
        d_autoload = Cache_Suspend_DCache();
    }
    REG_SET_BIT(DPORT_PRO_CACHE_WRAP_AROUND_CTRL_REG, DPORT_PRO_CACHE_FLASH_WRAP_AROUND);
    if (icache) {
        Cache_Resume_ICache(i_autoload);
    }
    if (dcache) {
        Cache_Resume_DCache(d_autoload);
    }
}

#if CONFIG_ESP32S2_SPIRAM_SUPPORT
static IRAM_ATTR void esp_enable_cache_spiram_wrap(bool icache, bool dcache)
{
    uint32_t i_autoload, d_autoload;
    if (icache) {
        i_autoload = Cache_Suspend_ICache();
    }
    if (dcache) {
        d_autoload = Cache_Suspend_DCache();
    }
    REG_SET_BIT(DPORT_PRO_CACHE_WRAP_AROUND_CTRL_REG, DPORT_PRO_CACHE_SRAM_RD_WRAP_AROUND);
    if (icache) {
        Cache_Resume_ICache(i_autoload);
    }
    if (dcache) {
        Cache_Resume_DCache(d_autoload);
    }
}
#endif

esp_err_t esp_enable_cache_wrap(bool icache_wrap_enable, bool dcache_wrap_enable)
{
    int icache_wrap_size = 0, dcache_wrap_size = 0;
    int flash_wrap_sizes[2]={-1, -1}, spiram_wrap_sizes[2]={-1, -1};
    int flash_wrap_size = 0, spiram_wrap_size = 0;
    int flash_count = 0, spiram_count = 0;
    int i;
    bool flash_spiram_wrap_together, flash_support_wrap = true, spiram_support_wrap = true;
    if (icache_wrap_enable) {
#if CONFIG_ESP32S2_INSTRUCTION_CACHE_LINE_16B
        icache_wrap_size = 16;
#elif CONFIG_ESP32S2_INSTRUCTION_CACHE_LINE_32B
        icache_wrap_size = 32;
#else
        icache_wrap_size = 64;
#endif
    }
    if (dcache_wrap_enable) {
#if CONFIG_ESP32S2_DATA_CACHE_LINE_16B
        dcache_wrap_size = 16;
#elif CONFIG_ESP32S2_DATA_CACHE_LINE_32B
        dcache_wrap_size = 32;
#else
        dcache_wrap_size = 64;
#endif
    }

    uint32_t instruction_use_spiram = 0;
    uint32_t rodata_use_spiram = 0;
#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
extern uint32_t esp_spiram_instruction_access_enabled();
    instruction_use_spiram = esp_spiram_instruction_access_enabled();
#endif
#if CONFIG_SPIRAM_RODATA
extern uint32_t esp_spiram_rodata_access_enabled();
    rodata_use_spiram = esp_spiram_rodata_access_enabled();
#endif

    if (instruction_use_spiram) {
        spiram_wrap_sizes[0] = icache_wrap_size;
    } else {
        flash_wrap_sizes[0] = icache_wrap_size;
    }
    if (rodata_use_spiram) {
        if (Cache_Drom0_Using_ICache()) {
            spiram_wrap_sizes[0] = icache_wrap_size;
        } else {
            spiram_wrap_sizes[1] = dcache_wrap_size;
            flash_wrap_sizes[1] = dcache_wrap_size;
        }
#ifdef CONFIG_EXT_RODATA_SUPPORT
        spiram_wrap_sizes[1] = dcache_wrap_size;
#endif
    } else {
        if (Cache_Drom0_Using_ICache()) {
            flash_wrap_sizes[0] = icache_wrap_size;
        } else {
            flash_wrap_sizes[1] = dcache_wrap_size;
        }
#ifdef CONFIG_EXT_RODATA_SUPPORT
        flash_wrap_sizes[1] = dcache_wrap_size;
#endif
    }
#ifdef CONFIG_ESP32S2_SPIRAM_SUPPORT
    spiram_wrap_sizes[1] = dcache_wrap_size;
#endif
    for (i = 0; i < 2; i++) {
        if (flash_wrap_sizes[i] != -1) {
            flash_count++;
            flash_wrap_size = flash_wrap_sizes[i];
        }
    }
    for (i = 0; i < 2; i++) {
        if (spiram_wrap_sizes[i] != -1) {
            spiram_count++;
            spiram_wrap_size = spiram_wrap_sizes[i];
        }
    }
    if (flash_count + spiram_count <= 2) {
        flash_spiram_wrap_together = false;
    } else {
        flash_spiram_wrap_together = true;
    }
    ESP_EARLY_LOGI(TAG, "flash_count=%d, size=%d, spiram_count=%d, size=%d,together=%d", flash_count, flash_wrap_size, spiram_count, spiram_wrap_size, flash_spiram_wrap_together);
    if (flash_count > 1 && flash_wrap_sizes[0] != flash_wrap_sizes[1]) {
        ESP_EARLY_LOGW(TAG, "Flash wrap with different length %d and %d, abort wrap.", flash_wrap_sizes[0], flash_wrap_sizes[1]);
        if (spiram_wrap_size == 0) {
            return ESP_FAIL;
        }
        if (flash_spiram_wrap_together) {
            ESP_EARLY_LOGE(TAG, "Abort spiram wrap because flash wrap length not fixed.");
            return ESP_FAIL;
        }
    }
    if (spiram_count > 1 && spiram_wrap_sizes[0] != spiram_wrap_sizes[1]) {
        ESP_EARLY_LOGW(TAG, "SPIRAM wrap with different length %d and %d, abort wrap.", spiram_wrap_sizes[0], spiram_wrap_sizes[1]);
        if (flash_wrap_size == 0) {
            return ESP_FAIL;
        }
        if (flash_spiram_wrap_together) {
            ESP_EARLY_LOGW(TAG, "Abort flash wrap because spiram wrap length not fixed.");
            return ESP_FAIL;
        }
    }

    if (flash_spiram_wrap_together && flash_wrap_size != spiram_wrap_size) {
        ESP_EARLY_LOGW(TAG, "SPIRAM has different wrap length with flash, %d and %d, abort wrap.", spiram_wrap_size, flash_wrap_size);
        return ESP_FAIL;
    }

extern bool spi_flash_support_wrap_size(uint32_t wrap_size);
    if (!spi_flash_support_wrap_size(flash_wrap_size)) {
        flash_support_wrap = false;
        ESP_EARLY_LOGW(TAG, "Flash do not support wrap size %d.", flash_wrap_size);
    }

#ifdef CONFIG_ESP32S2_SPIRAM_SUPPORT
extern bool psram_support_wrap_size(uint32_t wrap_size);
    if (!psram_support_wrap_size(spiram_wrap_size)) {
        spiram_support_wrap = false;
        ESP_EARLY_LOGW(TAG, "SPIRAM do not support wrap size %d.", spiram_wrap_size);
    }
#endif

    if (flash_spiram_wrap_together && !(flash_support_wrap && spiram_support_wrap)) {
        ESP_EARLY_LOGW(TAG, "Flash and SPIRAM should support wrap together.");
        return ESP_FAIL;
    }

extern esp_err_t spi_flash_enable_wrap(uint32_t wrap_size);
    if (flash_support_wrap && flash_wrap_size > 0) {
        ESP_EARLY_LOGI(TAG, "Flash wrap enabled.");
        spi_flash_enable_wrap(flash_wrap_size);
        esp_enable_cache_flash_wrap((flash_wrap_sizes[0] > 0), (flash_wrap_sizes[1] > 0));
    }
#if CONFIG_ESP32S2_SPIRAM_SUPPORT
extern esp_err_t psram_enable_wrap(uint32_t wrap_size);
    if (spiram_support_wrap && spiram_wrap_size > 0) {
        ESP_EARLY_LOGI(TAG, "SPIRAM wrap enabled.");
        psram_enable_wrap(spiram_wrap_size);
        esp_enable_cache_spiram_wrap((spiram_wrap_sizes[0] > 0), (spiram_wrap_sizes[1] > 0));
    }
#endif

    return ESP_OK;

}
#endif

void IRAM_ATTR spi_flash_enable_cache(uint32_t cpuid)
{
#if CONFIG_IDF_TARGET_ESP32
    uint32_t cache_value = DPORT_CACHE_GET_VAL(cpuid);
    cache_value &= DPORT_CACHE_GET_MASK(cpuid);

    // Re-enable cache on this CPU
    spi_flash_restore_cache(cpuid, cache_value);
#else
    spi_flash_restore_cache(0, 0); // TODO cache_value should be non-zero
#endif
}

