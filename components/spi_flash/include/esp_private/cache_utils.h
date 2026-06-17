/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This header file contains declarations of cache manipulation functions
 * used both in flash_ops.c and flash_mmap.c.
 *
 * These functions are considered internal and are not designed to be called from applications.
 */

// Init mutex protecting access to spi_flash_* APIs
void spi_flash_init_lock(void);

// Take mutex protecting access to spi_flash_* APIs
void spi_flash_op_lock(void);

// Release said mutex
void spi_flash_op_unlock(void);

// Suspend the scheduler on both CPUs, disable cache.
// Contrary to its name this doesn't do anything with interrupts, yet.
// Interrupt disabling capability will be added once we implement
// interrupt allocation API.
void spi_flash_disable_interrupts_caches_and_other_cpu(void);

// Enable cache, enable interrupts (to be added in future), resume scheduler
void spi_flash_enable_interrupts_caches_and_other_cpu(void);

// Disables non-IRAM interrupt handlers on current CPU and caches on both CPUs.
// This function is implied to be called when other CPU is not running or running code from IRAM.
void spi_flash_disable_interrupts_caches_and_other_cpu_no_os(void);

// Enable cache, enable interrupts on current CPU.
// This function is implied to be called when other CPU is not running or running code from IRAM.
void spi_flash_enable_interrupts_caches_no_os(void);

// Mark the pages containing a flash region as having been
// erased or written to. This means the flash cache needs
// to be evicted before these pages can be flash_mmap()ed again,
// as they may contain stale data
//
// Only call this while holding spi_flash_op_lock()
// Returns true if cache was flushed, false otherwise
bool spi_flash_check_and_flush_cache(size_t start_addr, size_t length);

//config cache mode
#if !CONFIG_IDF_TARGET_ESP32
//config instrcutin cache size and cache block size by menuconfig
void esp_config_instruction_cache_mode(void);
//config data cache size and cache block size by menuconfig
void esp_config_data_cache_mode(void);
#endif

#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2
/**
 * @brief   enable cache wrap mode for i/d shared cache
 * @param   icache_wrap_enable  enable cache wrap mode for i/d shared cache
 * @return  ESP_OK on success, ESP_FAIL otherwise
 */
esp_err_t esp_enable_cache_wrap(bool icache_wrap_enable);
#elif CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2
/**
 * @brief  enable cache wrap mode for instruction cache and data cache
 * @param  icache_wrap_enable   enable cache wrap mode for i cache
 * @param  dcache_wrap_enable   enable cache wrap mode for d cache
 * @return ESP_OK on success, ESP_FAIL otherwise
 */
esp_err_t esp_enable_cache_wrap(bool icache_wrap_enable, bool dcache_wrap_enable);
#endif

/** @brief Check at runtime if flash cache is enabled on both CPUs
 *
 * @return true if both CPUs have flash cache enabled, false otherwise.
 */
bool spi_flash_cache_enabled(void);

/**
 * @brief Re-enable cache for the core defined as cpuid parameter.
 *
 * @param cpuid the core number to enable instruction cache for
 */
void spi_flash_enable_cache(uint32_t cpuid);

/**
 * @brief Suspend the Cache access to external memory, will disable branch predictor if supported.
 *
 * @param cpuid       the core number to enable the cache for, meaning less on shared cache.
 * @param saved_state Cache status hold by hal (Used only on ROM impl. in idf, this param unused)
 */
void spi_flash_disable_cache(uint32_t cpuid, uint32_t *saved_state);

/**
 * @brief Resume the Cache access to external memory, will enable branch predictor if supported.
 *
 * @param cpuid       the core number to enable the cache for, meaning less on shared cache.
 * @param saved_state Cache status hold by hal (Used only on ROM impl. in idf, this param unused)
 */
void spi_flash_restore_cache(uint32_t cpuid, uint32_t saved_state);

#ifdef __cplusplus
}
#endif
