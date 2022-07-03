/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "esp_attr.h"
#include "esp_memory_utils.h"
#if CONFIG_SPIRAM
#include "esp_private/esp_psram_extram.h"
#endif


bool esp_ptr_dma_ext_capable(const void *p)
{
#if !SOC_PSRAM_DMA_CAPABLE
    return false;
#endif  //!SOC_PSRAM_DMA_CAPABLE
#if CONFIG_SPIRAM
    intptr_t vaddr_start = 0;
    intptr_t vaddr_end = 0;
    esp_psram_extram_get_mapped_range(&vaddr_start, &vaddr_end);
    return (intptr_t)p >= vaddr_start && (intptr_t)p < vaddr_end;
#else
    return false;
#endif  //CONFIG_SPIRAM
}

bool esp_ptr_byte_accessible(const void *p)
{
    intptr_t ip = (intptr_t) p;
    bool r;
    r = (ip >= SOC_BYTE_ACCESSIBLE_LOW && ip < SOC_BYTE_ACCESSIBLE_HIGH);
#if CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
    /* For ESP32 case, RTC fast memory is accessible to PRO cpu only and hence
     * for single core configuration (where it gets added to system heap) following
     * additional check is required */
    r |= (ip >= SOC_RTC_DRAM_LOW && ip < SOC_RTC_DRAM_HIGH);
#endif
#if CONFIG_SPIRAM
    intptr_t vaddr_start = 0;
    intptr_t vaddr_end = 0;
    esp_psram_extram_get_mapped_range(&vaddr_start, &vaddr_end);
    r |= (ip >= vaddr_start && ip < vaddr_end);
#endif
    return r;
}

bool esp_ptr_external_ram(const void *p)
{
#if !SOC_SPIRAM_SUPPORTED
    return false;
#endif  //!SOC_SPIRAM_SUPPORTED
#if CONFIG_SPIRAM
    intptr_t vaddr_start = 0;
    intptr_t vaddr_end = 0;
    esp_psram_extram_get_mapped_range(&vaddr_start, &vaddr_end);
    return (intptr_t)p >= vaddr_start && (intptr_t)p < vaddr_end;
#else
    return false;
#endif  //CONFIG_SPIRAM
}

#if CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
bool esp_stack_ptr_in_extram(uint32_t sp)
{
    intptr_t vaddr_start = 0;
    intptr_t vaddr_end = 0;
    esp_psram_extram_get_mapped_range(&vaddr_start, &vaddr_end);
    //Check if stack ptr is in between SOC_EXTRAM_DATA_LOW and SOC_EXTRAM_DATA_HIGH, and 16 byte aligned.
    return !(sp < vaddr_start + 0x10 || sp > vaddr_end - 0x10 || ((sp & 0xF) != 0));
}
#endif
