/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "sdkconfig.h"
#include "unity.h"
#include "esp_private/sd_host_buffer.h"
#include "soc/soc.h"
#include "esp_heap_caps.h"
#include "esp_memory_utils.h"
#include "esp_psram.h"

#if SOC_RTC_FAST_MEM_SUPPORTED
TEST_CASE("SDMMC rejects an RTC fast RAM DMA buffer", "[sdmmc]")
{
    const size_t buffer_size = 512;
    void *internal_dma_buf = heap_caps_malloc(buffer_size, MALLOC_CAP_DMA | MALLOC_CAP_CACHE_ALIGNED);
    const size_t offset = 0x20;
    const void *rtc_fast_ram_buf = (const void *)(SOC_RTC_DRAM_LOW + offset);
    TEST_ASSERT_NOT_NULL(internal_dma_buf);

    TEST_ASSERT_TRUE(sd_host_check_buffer_alignment(NULL, internal_dma_buf, buffer_size));
    TEST_ASSERT_FALSE(sd_host_check_buffer_alignment(NULL, rtc_fast_ram_buf, buffer_size));
    TEST_ASSERT_FALSE(sd_host_check_buffer_alignment(NULL, (const void *)(UINTPTR_MAX - 255), buffer_size));

    heap_caps_free(internal_dma_buf);
}
#endif

#if CONFIG_SPIRAM && !SOC_SDMMC_PSRAM_DMA_CAPABLE
TEST_CASE("SDMMC rejects a PSRAM buffer when PSRAM is not DMA capable", "[sdmmc]")
{
    const size_t buffer_size = 512;
    if (!esp_psram_is_initialized()) {
        TEST_IGNORE_MESSAGE("PSRAM is not available");
    }

    void *psram_buf = heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_CACHE_ALIGNED);
    TEST_ASSERT_NOT_NULL(psram_buf);
    TEST_ASSERT_TRUE(esp_ptr_external_ram(psram_buf));

    TEST_ASSERT_FALSE(sd_host_check_buffer_alignment(NULL, psram_buf, buffer_size));

    heap_caps_free(psram_buf);
}
#endif
