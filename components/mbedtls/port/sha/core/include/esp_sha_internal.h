/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_SHA_SUPPORT_DMA
/*
 * SHA operations for input data length below the threshold would use the block mode of the SHA peripheral.
 * The SHA peripheral's block mode is faster for smaller lengths of input data as compared to the DMA mode
 * because, the DMA mode involves various overheads like setting up the DMA descriptors.
 *
 * The below threshold numbers vary for some SoCs due to some properties that these SoCs hold,
 * like the cache type involved and its configurations, multi-core systems, DMA architectures, etc.
 *
 * For future SoCs with different and newer properties, this threshold value may require fine-tuning.
 * These values are calculated by performing a SHA operation in purely DMA mode versus purely block
 * mode for various lengths of the input data.
 */
#if CONFIG_IDF_TARGET_ESP32P4
#define SHA_DMA_MODE_THRESHOLD 512
#elif CONFIG_IDF_TARGET_ESP32S3
#define SHA_DMA_MODE_THRESHOLD 256
#else
#define SHA_DMA_MODE_THRESHOLD 128
#endif
#endif /* SOC_SHA_SUPPORT_DMA */

#define SHA_ALIGN_DOWN(num, align)  ((num) & ~((align) - 1))

typedef enum {
    SHA_BLOCK_MODE,
#if SOC_SHA_SUPPORT_DMA
    SHA_DMA_MODE,
#endif /* SOC_SHA_SUPPORT_DMA */
} esp_sha_mode;

static inline esp_sha_mode sha_operation_mode(size_t length)
{
#if SOC_SHA_SUPPORT_DMA
    if (length > SHA_DMA_MODE_THRESHOLD) {
        return SHA_DMA_MODE;
    } else
#endif /* SOC_SHA_SUPPORT_DMA */
    return SHA_BLOCK_MODE;
}

#ifdef __cplusplus
}
#endif
