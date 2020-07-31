// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
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

#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "soc/soc.h"
#include "sdkconfig.h"
#include "esp_attr.h"

#ifdef CONFIG_BT_ENABLED

#define SOC_MEM_BT_DATA_START               0x3ffae6e0
#define SOC_MEM_BT_DATA_END                 0x3ffaff10
#define SOC_MEM_BT_EM_START                 0x3ffb0000
#define SOC_MEM_BT_EM_END                   0x3ffb7cd8
#define SOC_MEM_BT_EM_BTDM0_START           0x3ffb0000
#define SOC_MEM_BT_EM_BTDM0_END             0x3ffb09a8
#define SOC_MEM_BT_EM_BLE_START             0x3ffb09a8
#define SOC_MEM_BT_EM_BLE_END               0x3ffb1ddc
#define SOC_MEM_BT_EM_BTDM1_START           0x3ffb1ddc
#define SOC_MEM_BT_EM_BTDM1_END             0x3ffb2730
#define SOC_MEM_BT_EM_BREDR_START           0x3ffb2730
#define SOC_MEM_BT_EM_BREDR_NO_SYNC_END     0x3ffb6388  //Not calculate with synchronize connection support
#define SOC_MEM_BT_EM_BREDR_END             0x3ffb7cd8  //Calculate with synchronize connection support
#define SOC_MEM_BT_EM_SYNC0_START           0x3ffb6388
#define SOC_MEM_BT_EM_SYNC0_END             0x3ffb6bf8
#define SOC_MEM_BT_EM_SYNC1_START           0x3ffb6bf8
#define SOC_MEM_BT_EM_SYNC1_END             0x3ffb7468
#define SOC_MEM_BT_EM_SYNC2_START           0x3ffb7468
#define SOC_MEM_BT_EM_SYNC2_END             0x3ffb7cd8
#define SOC_MEM_BT_BSS_START                0x3ffb8000
#define SOC_MEM_BT_BSS_END                  0x3ffb9a20
#define SOC_MEM_BT_MISC_START               0x3ffbdb28
#define SOC_MEM_BT_MISC_END                 0x3ffbdb5c

#define SOC_MEM_BT_EM_PER_SYNC_SIZE         0x870

#define SOC_MEM_BT_EM_BREDR_REAL_END        (SOC_MEM_BT_EM_BREDR_NO_SYNC_END + CONFIG_BTDM_CTRL_BR_EDR_MAX_SYNC_CONN_EFF * SOC_MEM_BT_EM_PER_SYNC_SIZE)

#endif //CONFIG_BT_ENABLED

#define SOC_MEMORY_TYPE_NO_PRIOS 3

/* Type descriptor holds a description for a particular type of memory on a particular SoC.
 */
typedef struct {
    const char *name;  ///< Name of this memory type
    uint32_t caps[SOC_MEMORY_TYPE_NO_PRIOS]; ///< Capabilities for this memory type (as a prioritised set)
    bool aliased_iram;   ///< If true, this is data memory that is is also mapped in IRAM
    bool startup_stack; ///< If true, memory of this type is used for ROM stack during startup
} soc_memory_type_desc_t;

/* Constant table of tag descriptors for all this SoC's tags */
extern const soc_memory_type_desc_t soc_memory_types[];
extern const size_t soc_memory_type_count;

/* Region descriptor holds a description for a particular region of memory on a particular SoC.
 */
typedef struct
{
    intptr_t start;  ///< Start address of the region
    size_t size;            ///< Size of the region in bytes
    size_t type;             ///< Type of the region (index into soc_memory_types array)
    intptr_t iram_address; ///< If non-zero, is equivalent address in IRAM
} soc_memory_region_t;

extern const soc_memory_region_t soc_memory_regions[];
extern const size_t soc_memory_region_count;

/* Region descriptor holds a description for a particular region of
   memory reserved on this SoC for a particular use (ie not available
   for stack/heap usage.) */
typedef struct
{
    intptr_t start;
    intptr_t end;
} soc_reserved_region_t;

/* Use this macro to reserved a fixed region of RAM (hardcoded addresses)
 * for a particular purpose.
 *
 * Usually used to mark out memory addresses needed for hardware or ROM code
 * purposes.
 *
 * Don't call this macro from user code which can use normal C static allocation
 * instead.
 *
 * @param START Start address to be reserved.
 * @param END One after the address of the last byte to be reserved. (ie length of
 * the reserved region is (END - START) in bytes.
 * @param NAME Name for the reserved region. Must be a valid variable name,
 * unique to this source file.
 */
#define SOC_RESERVE_MEMORY_REGION(START, END, NAME)     \
    __attribute__((section(".reserved_memory_address"))) __attribute__((used)) \
    static soc_reserved_region_t reserved_region_##NAME = { START, END };

/* Return available memory regions for this SoC. Each available memory
 * region is a contiguous piece of memory which is not being used by
 * static data, used by ROM code, or reserved by a component using
 * the SOC_RESERVE_MEMORY_REGION() macro.
 *
 * This result is soc_memory_regions[] minus all regions reserved
 * via the SOC_RESERVE_MEMORY_REGION() macro (which may also split
 * some regions up.)
 *
 * At startup, all available memory returned by this function is
 * registered as heap space.
 *
 * @note OS-level startup function only, not recommended to call from
 * app code.
 *
 * @param regions Pointer to an array for reading available regions into.
 * Size of the array should be at least the result of
 * soc_get_available_memory_region_max_count(). Entries in the array
 * will be ordered by memory address.
 *
 * @return Number of entries copied to 'regions'. Will be no greater than
 * the result of soc_get_available_memory_region_max_count().
 */
size_t soc_get_available_memory_regions(soc_memory_region_t *regions);

/* Return the maximum number of available memory regions which could be
 * returned by soc_get_available_memory_regions(). Used to size the
 * array passed to that function.
 */
size_t soc_get_available_memory_region_max_count(void);

inline static bool IRAM_ATTR esp_ptr_dma_capable(const void *p)
{
    return (intptr_t)p >= SOC_DMA_LOW && (intptr_t)p < SOC_DMA_HIGH;
}

inline static bool IRAM_ATTR esp_ptr_dma_ext_capable(const void *p)
{
#if CONFIG_IDF_TARGET_ESP32S2
    return (intptr_t)p >= SOC_DMA_EXT_LOW && (intptr_t)p < SOC_DMA_EXT_HIGH;
#else
    return false;
#endif
}

inline static bool IRAM_ATTR esp_ptr_word_aligned(const void *p)
{
    return ((intptr_t)p) % 4 == 0;
}

inline static bool IRAM_ATTR esp_ptr_executable(const void *p)
{
    intptr_t ip = (intptr_t) p;
    return (ip >= SOC_IROM_LOW && ip < SOC_IROM_HIGH)
        || (ip >= SOC_IRAM_LOW && ip < SOC_IRAM_HIGH)
        || (ip >= SOC_IROM_MASK_LOW && ip < SOC_IROM_MASK_HIGH)
#if defined(SOC_CACHE_APP_LOW) && defined(CONFIG_FREERTOS_UNICORE)
        || (ip >= SOC_CACHE_APP_LOW && ip < SOC_CACHE_APP_HIGH)
#endif
        || (ip >= SOC_RTC_IRAM_LOW && ip < SOC_RTC_IRAM_HIGH);
}

inline static bool IRAM_ATTR esp_ptr_byte_accessible(const void *p)
{
    intptr_t ip = (intptr_t) p;
    bool r;
    r = (ip >= SOC_BYTE_ACCESSIBLE_LOW && ip < SOC_BYTE_ACCESSIBLE_HIGH);
#if CONFIG_ESP32_ALLOW_RTC_FAST_MEM_AS_HEAP
    /* For ESP32 case, RTC fast memory is accessible to PRO cpu only and hence
     * for single core configuration (where it gets added to system heap) following
     * additional check is required */
    r |= (ip >= SOC_RTC_DRAM_LOW && ip < SOC_RTC_DRAM_HIGH);
#endif
#if CONFIG_SPIRAM
#if CONFIG_SPIRAM_SIZE != -1 // Fixed size, can be more accurate
    r |= (ip >= SOC_EXTRAM_DATA_LOW && ip < (SOC_EXTRAM_DATA_LOW + CONFIG_SPIRAM_SIZE));
#else
    r |= (ip >= SOC_EXTRAM_DATA_LOW && ip < (SOC_EXTRAM_DATA_HIGH));
#endif
#endif
    return r;
}

inline static bool IRAM_ATTR esp_ptr_internal(const void *p) {
    bool r;
    r = ((intptr_t)p >= SOC_MEM_INTERNAL_LOW && (intptr_t)p < SOC_MEM_INTERNAL_HIGH);
    r |= ((intptr_t)p >= SOC_RTC_DATA_LOW && (intptr_t)p < SOC_RTC_DATA_HIGH);
#if CONFIG_ESP32_ALLOW_RTC_FAST_MEM_AS_HEAP
    /* For ESP32 case, RTC fast memory is accessible to PRO cpu only and hence
     * for single core configuration (where it gets added to system heap) following
     * additional check is required */
    r |= ((intptr_t)p >= SOC_RTC_DRAM_LOW && (intptr_t)p < SOC_RTC_DRAM_HIGH);
#endif
    return r;
}


inline static bool IRAM_ATTR esp_ptr_external_ram(const void *p) {
    return ((intptr_t)p >= SOC_EXTRAM_DATA_LOW && (intptr_t)p < SOC_EXTRAM_DATA_HIGH);
}

inline static bool IRAM_ATTR esp_ptr_in_iram(const void *p) {
#if CONFIG_IDF_TARGET_ESP32 && CONFIG_FREERTOS_UNICORE
    return ((intptr_t)p >= SOC_CACHE_APP_LOW && (intptr_t)p < SOC_IRAM_HIGH);
#else
    return ((intptr_t)p >= SOC_IRAM_LOW && (intptr_t)p < SOC_IRAM_HIGH);
#endif
}

inline static bool IRAM_ATTR esp_ptr_in_drom(const void *p) {
    return ((intptr_t)p >= SOC_DROM_LOW && (intptr_t)p < SOC_DROM_HIGH);
}

inline static bool IRAM_ATTR esp_ptr_in_dram(const void *p) {
    return ((intptr_t)p >= SOC_DRAM_LOW && (intptr_t)p < SOC_DRAM_HIGH);
}

inline static bool IRAM_ATTR esp_ptr_in_diram_dram(const void *p) {
    return ((intptr_t)p >= SOC_DIRAM_DRAM_LOW && (intptr_t)p < SOC_DIRAM_DRAM_HIGH);
}

inline static bool IRAM_ATTR esp_ptr_in_diram_iram(const void *p) {
    return ((intptr_t)p >= SOC_DIRAM_IRAM_LOW && (intptr_t)p < SOC_DIRAM_IRAM_HIGH);
}

inline static bool IRAM_ATTR esp_ptr_in_rtc_iram_fast(const void *p) {
    return ((intptr_t)p >= SOC_RTC_IRAM_LOW && (intptr_t)p < SOC_RTC_IRAM_HIGH);
}

inline static bool IRAM_ATTR esp_ptr_in_rtc_dram_fast(const void *p) {
    return ((intptr_t)p >= SOC_RTC_DRAM_LOW && (intptr_t)p < SOC_RTC_DRAM_HIGH);
}

inline static bool IRAM_ATTR esp_ptr_in_rtc_slow(const void *p) {
    return ((intptr_t)p >= SOC_RTC_DATA_LOW && (intptr_t)p < SOC_RTC_DATA_HIGH);
}

/* Convert a D/IRAM DRAM pointer to equivalent word address in IRAM

   - Address must be word aligned
   - Address must pass esp_ptr_in_diram_dram() test, or result will be invalid pointer
*/
inline static void * IRAM_ATTR esp_ptr_diram_dram_to_iram(const void *p) {
#if SOC_DIRAM_INVERTED
    return (void *) ( SOC_DIRAM_IRAM_LOW + (SOC_DIRAM_DRAM_HIGH - (intptr_t)p) - 4);
#else
    return (void *) ( SOC_DIRAM_IRAM_LOW + ((intptr_t)p - SOC_DIRAM_DRAM_LOW) );
#endif
}

/* Convert a D/IRAM IRAM pointer to equivalent word address in DRAM

   - Address must be word aligned
   - Address must pass esp_ptr_in_diram_iram() test, or result will be invalid pointer
*/
inline static void * IRAM_ATTR esp_ptr_diram_iram_to_dram(const void *p) {
#if SOC_DIRAM_INVERTED
    return (void *) ( SOC_DIRAM_DRAM_LOW + (SOC_DIRAM_IRAM_HIGH - (intptr_t)p) - 4);
#else
    return (void *) ( SOC_DIRAM_DRAM_LOW + ((intptr_t)p - SOC_DIRAM_IRAM_LOW) );
#endif
}

inline static bool IRAM_ATTR esp_stack_ptr_in_dram(uint32_t sp)
{
    //Check if stack ptr is in between SOC_DRAM_LOW and SOC_DRAM_HIGH, and 16 byte aligned.
    return !(sp < SOC_DRAM_LOW + 0x10 || sp > SOC_DRAM_HIGH - 0x10 || ((sp & 0xF) != 0));
}

#if CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
inline static bool IRAM_ATTR esp_stack_ptr_in_extram(uint32_t sp)
{
    //Check if stack ptr is in between SOC_EXTRAM_DATA_LOW and SOC_EXTRAM_DATA_HIGH, and 16 byte aligned.
    return !(sp < SOC_EXTRAM_DATA_LOW + 0x10 || sp > SOC_EXTRAM_DATA_HIGH - 0x10 || ((sp & 0xF) != 0));
}
#endif

inline static bool IRAM_ATTR esp_stack_ptr_is_sane(uint32_t sp)
{
#if CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
    return (esp_stack_ptr_in_dram(sp) || esp_stack_ptr_in_extram(sp));
#else
    return esp_stack_ptr_in_dram(sp);
#endif
}

