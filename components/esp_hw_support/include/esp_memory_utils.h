/*
 * SPDX-FileCopyrightText: 2010-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"
#include "esp_attr.h"
#include "bootloader_memory_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Check if the pointer is dma capable
 *
 * @param p pointer
 *
 * @return true: capable; false: not capable
 */
__attribute__((always_inline))
inline static bool esp_ptr_dma_capable(const void *p)
{
    return (intptr_t)p >= SOC_DMA_LOW && (intptr_t)p < SOC_DMA_HIGH;
}

/**
 * @brief Check if the pointer is in external ram dma capable region
 *
 * @param p pointer
 *
 * @return true: capable; false: not capable
 */
bool esp_ptr_dma_ext_capable(const void *p);

/**
 * @brief Check if the pointer is word aligned
 *
 * @param p pointer
 *
 * @return true: aligned; false: not aligned
 */
__attribute__((always_inline))
inline static bool esp_ptr_word_aligned(const void *p)
{
    return ((intptr_t)p) % 4 == 0;
}

/**
 * @brief Check if the pointer is executable
 *
 * @param p pointer
 *
 * @return true: is executable; false: not executable
 */
__attribute__((always_inline))
inline static bool esp_ptr_executable(const void *p)
{
    intptr_t ip = (intptr_t) p;
    return (ip >= SOC_IROM_LOW && ip < SOC_IROM_HIGH)
        || (ip >= SOC_IRAM_LOW && ip < SOC_IRAM_HIGH)
        || (ip >= SOC_IROM_MASK_LOW && ip < SOC_IROM_MASK_HIGH)
#if defined(SOC_CACHE_APP_LOW) && defined(CONFIG_FREERTOS_UNICORE)
        || (ip >= SOC_CACHE_APP_LOW && ip < SOC_CACHE_APP_HIGH)
#endif
#if SOC_RTC_FAST_MEM_SUPPORTED
        || (ip >= SOC_RTC_IRAM_LOW && ip < SOC_RTC_IRAM_HIGH)
#endif
    ;
}

/**
 * @brief Check if the pointer is byte accessible
 *
 * @param p pointer
 *
 * @return true: is byte accessible; false: not byte accessible
 */
bool esp_ptr_byte_accessible(const void *p);

/**
 * @brief Check if the pointer is in internal ram
 *
 * @param p pointer
 *
 * @return true: is in internal ram; false: not in internal ram
 */
__attribute__((always_inline))
inline static bool esp_ptr_internal(const void *p) {
    bool r;
    r = ((intptr_t)p >= SOC_MEM_INTERNAL_LOW && (intptr_t)p < SOC_MEM_INTERNAL_HIGH);

#if SOC_RTC_SLOW_MEM_SUPPORTED
    r |= ((intptr_t)p >= SOC_RTC_DATA_LOW && (intptr_t)p < SOC_RTC_DATA_HIGH);
#endif

#if CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
    /* For ESP32 case, RTC fast memory is accessible to PRO cpu only and hence
     * for single core configuration (where it gets added to system heap) following
     * additional check is required */
    r |= ((intptr_t)p >= SOC_RTC_DRAM_LOW && (intptr_t)p < SOC_RTC_DRAM_HIGH);
#endif
    return r;
}

/**
 * @brief Check if the pointer is in external ram
 *
 * @param p pointer
 *
 * @return true: is in external ram; false: not in external ram
 */
bool esp_ptr_external_ram(const void *p);

/**
 * @brief Check if the pointer is in drom
 *
 * @param p pointer
 *
 * @return true: is in drom; false: not in drom
 */
__attribute__((always_inline))
inline static bool esp_ptr_in_drom(const void *p) {
    return ((intptr_t)p >= SOC_DROM_LOW && (intptr_t)p < SOC_DROM_HIGH);
}

/**
 * @brief Check if the stack pointer is in dram
 *
 * @param sp stack pointer
 *
 * @return true: is in dram; false: not in dram
 */
__attribute__((always_inline))
inline static bool esp_stack_ptr_in_dram(uint32_t sp)
{
    //Check if stack ptr is in between SOC_DRAM_LOW and SOC_DRAM_HIGH, and 16 byte aligned.
    return !(sp < SOC_DRAM_LOW + 0x10 || sp > SOC_DRAM_HIGH - 0x10 || ((sp & 0xF) != 0));
}

#if CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
/**
 * @brief Check if the stack pointer is in external ram
 *
 * @param sp stack pointer
 *
 * @return true: is in external ram; false: not in external ram
 */
bool esp_stack_ptr_in_extram(uint32_t sp);
#endif

/**
 * @brief Check if the stack pointer is sane
 *
 * @param sp stack pointer
 *
 * @return true: is in sane; false: not in sane
 */
__attribute__((always_inline))
inline static bool esp_stack_ptr_is_sane(uint32_t sp)
{
    return esp_stack_ptr_in_dram(sp)
#if CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
        || esp_stack_ptr_in_extram(sp)
#endif
#if CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
        || esp_ptr_in_rtc_dram_fast((void*) sp)
#endif
        ;
}

#ifdef __cplusplus
}
#endif
