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
    return esp_psram_check_ptr_addr(p);
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
    r |= esp_psram_check_ptr_addr(p);
#endif
#if CONFIG_ESP32S3_DATA_CACHE_16KB
    /* For ESP32-S3, when the DCACHE size is set to 16 kB, the unused 48 kB is
     * added to the heap in 2 blocks of 32 kB (from 0x3FCF0000) and 16 kB
     * (from 0x3C000000 (SOC_DROM_LOW) - 0x3C004000).
     * Though this memory lies in the external memory vaddr, it is no different
     * from the internal RAM in terms of hardware attributes. It is a part of
     * the internal RAM when added to the heap and is byte-accessible .*/
    r |= (ip >= SOC_DROM_LOW && ip < (SOC_DROM_LOW + 0x4000));
#endif
    return r;
}

bool esp_ptr_external_ram(const void *p)
{
#if !SOC_SPIRAM_SUPPORTED
    return false;
#endif  //!SOC_SPIRAM_SUPPORTED
#if CONFIG_SPIRAM
    return esp_psram_check_ptr_addr(p);
#else
    return false;
#endif  //CONFIG_SPIRAM
}

#if CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM
bool esp_stack_ptr_in_extram(uint32_t sp)
{
    //Check if stack ptr is on PSRAM, and 16 byte aligned.
    return (esp_psram_check_ptr_addr((void *)sp) && ((sp & 0xF) == 0));
}
#endif
