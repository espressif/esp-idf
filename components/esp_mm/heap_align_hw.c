/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdarg.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_heap_caps.h"
#include "esp_private/esp_cache_private.h"
#include "soc/soc_caps.h"
#if SOC_HAS(GDMA)
#include "hal/gdma_ll.h"
#include "hal/efuse_hal.h"
#endif

#if CONFIG_HEAP_PLACE_FUNCTION_INTO_FLASH
#define HEAP_IRAM_ATTR
#else
#define HEAP_IRAM_ATTR IRAM_ATTR
#endif

#define CAPS_NEEDING_ALIGNMENT (MALLOC_CAP_DMA|MALLOC_CAP_DMA_DESC_AHB|MALLOC_CAP_DMA_DESC_AXI|MALLOC_CAP_CACHE_ALIGNED|MALLOC_CAP_SIMD)

HEAP_IRAM_ATTR void esp_heap_adjust_alignment_to_hw(size_t *p_alignment, size_t *p_size, uint32_t *p_caps)
{
    size_t size = *p_size;
    size_t alignment = *p_alignment;
    uint32_t caps = *p_caps;

    //Bail out early if we don't need alignment
    if (!(caps & CAPS_NEEDING_ALIGNMENT)) {
        return;
    }

#if CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
//Cache functions aren't linked in so we cannot allocate anything needing alignment.
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    //Cannot do this for either internal or external memory.
    *p_caps |= MALLOC_CAP_INVALID;
#else
    //Internal memory DMA alloc is allowed.
    if (caps & MALLOC_CAP_SPIRAM) {
        *p_caps |= MALLOC_CAP_INVALID;
    }
#endif
    return;
#endif

    //Ask cache driver what alignment is applicable here.
    size_t cache_alignment_bytes = 0;
    esp_err_t ret = esp_cache_get_alignment(caps, &cache_alignment_bytes);
    if (ret != ESP_OK) {
        //This is not supposed to happen.
        *p_caps |= MALLOC_CAP_INVALID;
        return;
    }

#if SOC_HAS(AXI_GDMA)
    //Special case: AXI DMA descriptors need to be aligned to 8-byte boundaries.
    if ((caps & MALLOC_CAP_DMA_DESC_AXI) && (cache_alignment_bytes < GDMA_LL_AXI_DESC_ALIGNMENT)) {
        cache_alignment_bytes = GDMA_LL_AXI_DESC_ALIGNMENT;
    }
#endif

    // We assume both DMA alignment and requested alignment are powers of two. We can safely
    // do this because:
    // - DMA alignment in current chips always is a power of two, and is unlikely to ever
    //   be something else,
    // - Requested alignment is checked by heap_caps_aligned_check_args to be a power
    //   of two.
    if (cache_alignment_bytes > alignment) {
        alignment = cache_alignment_bytes;
    }

#if SOC_SIMD_INSTRUCTION_SUPPORTED
    // SIMD instructions preferred data alignment, SOC_SIMD_PREFERRED_DATA_ALIGNMENT, which is also definitely a power of two
    if (caps & MALLOC_CAP_SIMD) {
        alignment = (alignment > SOC_SIMD_PREFERRED_DATA_ALIGNMENT) ? alignment : SOC_SIMD_PREFERRED_DATA_ALIGNMENT;
    }
#endif

#if SOC_HAS(GDMA) && (SOC_PSRAM_DMA_CAPABLE || SOC_DMA_CAN_ACCESS_FLASH)
    if ((caps & MALLOC_CAP_DMA) && efuse_hal_flash_encryption_enabled()) {
        alignment = (alignment > GDMA_LL_GET(ACCESS_ENCRYPTION_MEM_ALIGNMENT)) ? alignment : GDMA_LL_GET(ACCESS_ENCRYPTION_MEM_ALIGNMENT);
    }
#endif

    // Align up `size` to resulting alignment as well.
    size = (size + alignment - 1) & (~(alignment - 1));

    // For the heap allocator itself, there's no difference between data and descriptor DMA; the regions
    // are only marked as DMA-capable.
    if (caps & (MALLOC_CAP_DMA_DESC_AHB | MALLOC_CAP_DMA_DESC_AXI)) {
        caps &= ~(MALLOC_CAP_DMA_DESC_AHB | MALLOC_CAP_DMA_DESC_AXI);
        caps |= MALLOC_CAP_DMA;
    }

    // Workaround: the heap allocator doesn't have regions marked `MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM`
    // so we need to request those without the DMA flag.
    if (caps & MALLOC_CAP_SPIRAM) {
        caps &= ~MALLOC_CAP_DMA;
        //Note: we do not erase any DMA descriptor caps. DMA descriptors cannot be in external
        //memory, so the MALLOC_CAP_SPIRAM|MALLOC_CAP_DMA_DESC_* simply will not return any
        //usable memory.
    }
    // MALLOC_CAP_CACHE_ALIGNED is not a real flag the heap_base component will understand; it
    // only sets alignment (which we handled here)
    caps &= ~ MALLOC_CAP_CACHE_ALIGNED;

    *p_size = size;
    *p_alignment = alignment;
    *p_caps = caps;
}
