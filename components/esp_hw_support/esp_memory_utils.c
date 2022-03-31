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


//TODO: IDF-4855, replace PSRAM related address region into PSRAM private APIs

bool esp_ptr_dma_ext_capable(const void *p)
{
#ifdef SOC_PSRAM_DMA_CAPABLE
    return (intptr_t)p >= SOC_DMA_EXT_LOW && (intptr_t)p < SOC_DMA_EXT_HIGH;
#else
    return false;
#endif
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
#if CONFIG_SPIRAM_SIZE != -1 // Fixed size, can be more accurate
    r |= (ip >= SOC_EXTRAM_DATA_LOW && ip < (SOC_EXTRAM_DATA_LOW + CONFIG_SPIRAM_SIZE));
#else
    r |= (ip >= SOC_EXTRAM_DATA_LOW && ip < (SOC_EXTRAM_DATA_HIGH));
#endif
#endif
    return r;
}

bool esp_ptr_external_ram(const void *p) {
#if SOC_SPIRAM_SUPPORTED
    return ((intptr_t)p >= SOC_EXTRAM_DATA_LOW && (intptr_t)p < SOC_EXTRAM_DATA_HIGH);
#else
    return false; // SoC has no external RAM
#endif
}

#if CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
bool esp_stack_ptr_in_extram(uint32_t sp)
{
    //Check if stack ptr is in between SOC_EXTRAM_DATA_LOW and SOC_EXTRAM_DATA_HIGH, and 16 byte aligned.
    return !(sp < SOC_EXTRAM_DATA_LOW + 0x10 || sp > SOC_EXTRAM_DATA_HIGH - 0x10 || ((sp & 0xF) != 0));
}
#endif
