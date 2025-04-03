/*
 * SPDX-FileCopyrightText: 2010-2025 Espressif Systems (Shanghai) CO LTD
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

#ifdef __cplusplus
extern "C" {
#endif

/** Common functions, to be kept in sync with bootloader_memory_utils.h **/

/**
 * @brief Check if the IRAM and DRAM are separate or using the same memory space
 *
 * @return true if the DRAM and IRAM are sharing the same memory space, false otherwise
 */
__attribute__((always_inline))
inline static bool esp_dram_match_iram(void) {
    return ((SOC_DRAM_LOW == SOC_IRAM_LOW) && (SOC_DRAM_HIGH == SOC_IRAM_HIGH));
}

/**
 * @brief Check if the RTC IRAM and RTC DRAM are separate or using the same memory space
 *
 * @return true if the RTC DRAM and RTC IRAM are sharing the same memory space, false otherwise
 */
__attribute__((always_inline))
inline static bool esp_rtc_dram_match_rtc_iram(void) {
#if SOC_RTC_FAST_MEM_SUPPORTED
    return ((SOC_RTC_IRAM_LOW == SOC_RTC_DRAM_LOW) && (SOC_RTC_IRAM_HIGH == SOC_RTC_DRAM_HIGH));
#else
    return false;
#endif
}

/**
 * @brief Check if the pointer is in iram
 *
 * @param p pointer
 *
 * @return true: is in iram; false: not in iram
 */
__attribute__((always_inline))
inline static bool esp_ptr_in_iram(const void *p) {
#if CONFIG_IDF_TARGET_ESP32 && CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    return ((intptr_t)p >= SOC_CACHE_APP_LOW && (intptr_t)p < SOC_IRAM_HIGH);
#else
    return ((intptr_t)p >= SOC_IRAM_LOW && (intptr_t)p < SOC_IRAM_HIGH);
#endif
}

/**
 * @brief Check if the pointer is in dram
 *
 * @param p pointer
 *
 * @return true: is in dram; false: not in dram
 */
__attribute__((always_inline))
inline static bool esp_ptr_in_dram(const void *p) {
    return ((intptr_t)p >= SOC_DRAM_LOW && (intptr_t)p < SOC_DRAM_HIGH);
}

/**
 * @brief Check if the pointer is in diram_dram
 *
 * @param p pointer
 *
 * @return true: is in diram_dram; false: not in diram_dram
 */
__attribute__((always_inline))
inline static bool esp_ptr_in_diram_dram(const void *p) {
    return ((intptr_t)p >= SOC_DIRAM_DRAM_LOW && (intptr_t)p < SOC_DIRAM_DRAM_HIGH);
}

/**
 * @brief Check if the pointer is in diram_iram
 *
 * @param p pointer
 *
 * @return true: is in diram_iram; false: not in diram_iram
 */
__attribute__((always_inline))
inline static bool esp_ptr_in_diram_iram(const void *p) {
// TODO: IDF-5980 esp32c6 D/I RAM share the same address
#if SOC_DIRAM_IRAM_LOW == SOC_DIRAM_DRAM_LOW
    (void)p;
    return false;
#else
    return ((intptr_t)p >= SOC_DIRAM_IRAM_LOW && (intptr_t)p < SOC_DIRAM_IRAM_HIGH);
#endif
}

/**
 * @brief Check if the pointer is in rtc_iram_fast
 *
 * @param p pointer
 *
 * @return true: is in rtc_iram_fast; false: not in rtc_iram_fast
 */
__attribute__((always_inline))
inline static bool esp_ptr_in_rtc_iram_fast(const void *p) {
#if SOC_RTC_FAST_MEM_SUPPORTED
    return ((intptr_t)p >= SOC_RTC_IRAM_LOW && (intptr_t)p < SOC_RTC_IRAM_HIGH);
#else
    (void)p;
    return false;
#endif
}

/**
 * @brief Check if the pointer is in rtc_dram_fast
 *
 * @param p pointer
 *
 * @return true: is in rtc_dram_fast; false: not in rtc_dram_fast
 */
__attribute__((always_inline))
inline static bool esp_ptr_in_rtc_dram_fast(const void *p) {
#if SOC_RTC_FAST_MEM_SUPPORTED
    return ((intptr_t)p >= SOC_RTC_DRAM_LOW && (intptr_t)p < SOC_RTC_DRAM_HIGH);
#else
    (void)p;
    return false;
#endif
}

/**
 * @brief Check if the pointer is in rtc_slow
 *
 * @param p pointer
 *
 * @return true: is in rtc_slow; false: not in rtc_slow
 */
__attribute__((always_inline))
inline static bool esp_ptr_in_rtc_slow(const void *p) {
#if SOC_RTC_SLOW_MEM_SUPPORTED
    return ((intptr_t)p >= SOC_RTC_DATA_LOW && (intptr_t)p < SOC_RTC_DATA_HIGH);
#else
    return false;
#endif
}


/* Convert a D/IRAM DRAM pointer to equivalent word address in IRAM

   - Address must be word aligned
   - Address must pass esp_ptr_in_diram_dram() test, or result will be invalid pointer
*/
__attribute__((always_inline))
inline static void * esp_ptr_diram_dram_to_iram(const void *p) {
#if SOC_DIRAM_INVERTED
    return (void *) ( SOC_DIRAM_IRAM_LOW + (SOC_DIRAM_DRAM_HIGH - (intptr_t)p) - 4);
#else
    return (void *) ( SOC_DIRAM_IRAM_LOW + ((intptr_t)p - SOC_DIRAM_DRAM_LOW) );
#endif
}

/* Convert a RTC DRAM pointer to equivalent word address in RTC IRAM

   - Address must be word aligned
   - Address must pass esp_ptr_in_rtc_dram_fast() test, or result will be invalid pointer
*/
__attribute__((always_inline))
inline static void * esp_ptr_rtc_dram_to_iram(const void *p) {
    intptr_t ptr = (intptr_t)p;
#if SOC_RTC_FAST_MEM_SUPPORTED && (SOC_RTC_IRAM_LOW != SOC_RTC_DRAM_LOW)
    return (void *) ( SOC_RTC_IRAM_LOW + (ptr - SOC_RTC_DRAM_LOW) );
#else
    return (void *) ptr;
#endif
}

/* Convert a D/IRAM IRAM pointer to equivalent word address in DRAM

   - Address must be word aligned
   - Address must pass esp_ptr_in_diram_iram() test, or result will be invalid pointer
*/
__attribute__((always_inline))
inline static void * esp_ptr_diram_iram_to_dram(const void *p) {
#if SOC_DIRAM_INVERTED
    return (void *) ( SOC_DIRAM_DRAM_LOW + (SOC_DIRAM_IRAM_HIGH - (intptr_t)p) - 4);
#else
    return (void *) ( SOC_DIRAM_DRAM_LOW + ((intptr_t)p - SOC_DIRAM_IRAM_LOW) );
#endif
}

#if SOC_MEM_TCM_SUPPORTED
/**
 * @brief Check if the pointer is in TCM
 *
 * @param p pointer
 *
 * @return true: is in TCM; false: not in TCM
 */
__attribute__((always_inline))
inline static bool esp_ptr_in_tcm(const void *p) {
    return ((intptr_t)p >= SOC_TCM_LOW && (intptr_t)p < SOC_TCM_HIGH);
}
#endif  //#if SOC_MEM_TCM_SUPPORTED

/** End of common functions to be kept in sync with bootloader_memory_utils.h **/
/** Add app-specific functions below **/

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
bool esp_ptr_executable(const void *p);

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

#if CONFIG_ESP32S3_DATA_CACHE_16KB
    /* For ESP32-S3, when the DCACHE size is set to 16 kB, the unused 48 kB is
     * added to the heap in 2 blocks of 32 kB (from 0x3FCF0000) and 16 kB
     * (from 0x3C000000 (SOC_DROM_LOW) - 0x3C004000).
     * Though this memory lies in the external memory vaddr, it is no different
     * from the internal RAM in terms of hardware attributes and it is a part of
     * the internal RAM when added to the heap.*/
    r |= ((intptr_t)p >= SOC_DROM_LOW && (intptr_t)p < (SOC_DROM_LOW + 0x4000));
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
    int32_t drom_start_addr = SOC_DROM_LOW;
#if CONFIG_ESP32S3_DATA_CACHE_16KB
    /* For ESP32-S3, when the DCACHE size is set to 16 kB, the unused 48 kB is
     * added to the heap in 2 blocks of 32 kB (from 0x3FCF0000) and 16 kB
     * (from 0x3C000000 (SOC_DROM_LOW) - 0x3C004000).
     * The drom_start_addr has to be moved by 0x4000 (16kB) to accommodate
     * this addition. */
    drom_start_addr += 0x4000;
#endif

    return ((intptr_t)p >= drom_start_addr && (intptr_t)p < SOC_DROM_HIGH);
}


/**
 * @brief Check if the given pointer is in ROM
 *
 * @param ptr Pointer to check
 *
 * @return true if `ptr` points to ROM, false else
 */
__attribute__((always_inline))
inline static bool esp_ptr_in_rom(const void *p)
{
    intptr_t ip = (intptr_t) p;
    return
    /**
     * The following DROM macros are only defined on RISC-V targets, moreover, to prevent
     * the compiler from generating a `logical-op` warning, make sure the macros are
     * distinct. */
#if CONFIG_IDF_TARGET_ARCH_RISCV && SOC_DROM_MASK_LOW != SOC_IROM_MASK_LOW
        (ip >= SOC_DROM_MASK_LOW && ip < SOC_DROM_MASK_HIGH) ||
#endif
        (ip >= SOC_IROM_MASK_LOW && ip < SOC_IROM_MASK_HIGH);
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

#if CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM
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
#if CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM
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
