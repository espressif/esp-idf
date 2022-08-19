/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This file will be redesigned into MMU driver, to maintain all the external
 * memory contexts including:
 * - Flash
 * - PSRAM
 * - DDR
 *
 * Now only MMU-PSRAM related private APIs
 */

#include <stdint.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "soc/ext_mem_defs.h"
#include "esp_private/mmu_psram_flash.h"

#if CONFIG_IDF_TARGET_ESP32S2
#include "soc/extmem_reg.h"
#include "esp32s2/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "soc/extmem_reg.h"
#include "esp32s3/rom/cache.h"
#endif


#define ALIGN_UP_BY(num, align) (((num) + ((align) - 1)) & ~((align) - 1))
__attribute__((unused)) static const char *TAG = "mmu";
extern int _instruction_reserved_start;
extern int _instruction_reserved_end;
extern int _rodata_reserved_start;
extern int _rodata_reserved_end;


intptr_t mmu_get_psram_vaddr_start(void)
{
#if CONFIG_IDF_TARGET_ESP32S3

    intptr_t rodata_end_aligned = ALIGN_UP_BY((intptr_t)&_rodata_reserved_end, MMU_PAGE_SIZE);
    ESP_EARLY_LOGV(TAG, "rodata_end_aligned is 0x%x bytes", rodata_end_aligned);
    return rodata_end_aligned;

#elif CONFIG_IDF_TARGET_ESP32S2
    return DPORT_CACHE_ADDRESS_LOW;
#else   //CONFIG_IDF_TARGET_ESP32
    return DRAM1_CACHE_ADDRESS_LOW;
#endif
}

intptr_t mmu_get_psram_vaddr_end(void)
{
#if CONFIG_IDF_TARGET_ESP32S3
    return DRAM0_CACHE_ADDRESS_HIGH;
#elif CONFIG_IDF_TARGET_ESP32S2
    return DRAM0_CACHE_ADDRESS_HIGH;
#else   //CONFIG_IDF_TARGET_ESP32
    return DRAM1_CACHE_ADDRESS_HIGH;
#endif
}
