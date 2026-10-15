/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_efuse.h"
#include "esp_memory_utils.h"
#include "esp_private/esp_mspi_align.h"
#if CONFIG_SPIRAM
#include "esp_psram.h"
#endif /* CONFIG_SPIRAM */
#include "soc/soc_caps.h"

#define MSPI_FLASH_ENC_ALIGNMENT  SOC_MEMSPI_ENCRYPTION_ALIGNMENT
#define MSPI_PSRAM_ECC_ALIGNMENT  SOC_MEMSPI_ENCRYPTION_ALIGNMENT

size_t esp_mspi_get_alignment(const void *ptr)
{
    size_t alignment = 1;
    bool generic_query = ptr == NULL;
    bool __attribute__((unused)) is_psram = esp_ptr_external_ram(ptr);
    bool is_drom = esp_ptr_in_drom(ptr);
    bool is_psram_enc = false;

#if CONFIG_SPIRAM
    is_psram_enc = is_psram && !esp_psram_ptr_is_no_enc(ptr);
#endif /* CONFIG_SPIRAM */

    if ((generic_query || is_drom || is_psram_enc) && esp_efuse_is_flash_encryption_enabled()) {
        alignment = MAX(alignment, MSPI_FLASH_ENC_ALIGNMENT);
    }

#if CONFIG_SPIRAM_ECC_ENABLE
    if (generic_query || is_psram) {
        alignment = MAX(alignment, MSPI_PSRAM_ECC_ALIGNMENT);
    }
#endif

    return alignment;
}

bool esp_mspi_buffer_alignment_satisfied(const void *ptr, size_t size)
{
    // Zero-length is not a valid MSPI transfer, so it never satisfies the check.
    if (ptr == NULL || size == 0) {
        return false;
    }

    size_t alignment = esp_mspi_get_alignment(ptr);
    if (alignment <= 1) {
        return true;
    }
    uintptr_t addr = (uintptr_t)ptr;
    return ((addr & (alignment - 1)) == 0) && ((size & (alignment - 1)) == 0);
}
