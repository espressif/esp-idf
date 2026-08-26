/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <sys/param.h>
#include "esp_log.h"
#include "bootloader_flash_priv.h"
#include "bootloader_sha.h"
#include "bootloader_sha_flash.h"
#include "spi_flash_mmap.h"
#include "soc/soc_caps.h"
#include "hal/sha_types.h"
#include "rom/sha.h"

ESP_LOG_ATTR_TAG(TAG, "boot_sha");

static esp_err_t bootloader_sha_flash_contents(esp_sha_type type, uint32_t flash_offset, uint32_t len, uint8_t *digest)
{
    if (digest == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Handling firmware images larger than MMU capacity */
    uint32_t mmu_free_pages_count = bootloader_mmap_get_free_pages();
    bootloader_sha_handle_t sha_handle = NULL;

    if (type == SHA2_256) {
        sha_handle = bootloader_sha256_start();
    } else
#if SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384
        if (type == SHA2_384) {
            sha_handle = bootloader_sha512_start(true);
        } else
#endif /* SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384 */
        {
            return ESP_ERR_INVALID_ARG;
        }

    if (sha_handle == NULL) {
        return ESP_ERR_NO_MEM;
    }

    while (len > 0) {
        uint32_t mmu_page_offset = ((flash_offset & MMAP_ALIGNED_MASK) != 0) ? 1 : 0; /* Skip 1st MMU Page if it is already populated */
        uint32_t max_pages = (mmu_free_pages_count > mmu_page_offset) ? (mmu_free_pages_count - mmu_page_offset) : 0;
        if (max_pages == 0) {
            ESP_LOGE(TAG, "No free MMU pages are available");
            if (type == SHA2_256) {
                bootloader_sha256_finish(sha_handle, NULL);
            }
#if SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384
            else if (type == SHA2_384) {
                bootloader_sha512_finish(sha_handle, NULL);
            }
#endif /* SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384 */
            return ESP_ERR_NO_MEM;
        }
        uint32_t max_image_len;
        if (__builtin_mul_overflow(max_pages, SPI_FLASH_MMU_PAGE_SIZE, &max_image_len)) {
            max_image_len = UINT32_MAX;
        }
        uint32_t partial_image_len = MIN(len, max_image_len); /* Read the image that fits in the free MMU pages */

        const void * image = bootloader_mmap(flash_offset, partial_image_len);
        if (image == NULL) {
            if (type == SHA2_256) {
                bootloader_sha256_finish(sha_handle, NULL);
            }
#if SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384
            else if (type == SHA2_384) {
                bootloader_sha512_finish(sha_handle, NULL);
            }
#endif /* SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384 */
            return ESP_FAIL;
        }

        if (type == SHA2_256) {
            bootloader_sha256_data(sha_handle, image, partial_image_len);
        }
#if SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384
        else if (type == SHA2_384) {
            bootloader_sha512_data(sha_handle, image, partial_image_len);
        }
#endif /* SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384 */

        bootloader_munmap(image);

        flash_offset += partial_image_len;
        len -= partial_image_len;
    }

    if (type == SHA2_256) {
        bootloader_sha256_finish(sha_handle, digest);
    }
#if SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384
    else if (type == SHA2_384) {
        bootloader_sha512_finish(sha_handle, digest);
    }
#endif /* SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384 */
    return ESP_OK;
}

esp_err_t bootloader_sha256_flash_contents(uint32_t flash_offset, uint32_t len, uint8_t *digest)
{
    return bootloader_sha_flash_contents(SHA2_256, flash_offset, len, digest);
}

#if SOC_SHA_SUPPORT_SHA384 && SOC_ECDSA_SUPPORT_CURVE_P384
esp_err_t bootloader_sha384_flash_contents(uint32_t flash_offset, uint32_t len, uint8_t *digest)
{
    return bootloader_sha_flash_contents(SHA2_384, flash_offset, len, digest);
}
#endif
