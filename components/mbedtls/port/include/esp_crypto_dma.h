/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/dma_types.h"
#include "soc/soc_caps.h"

#if SOC_GDMA_SUPPORTED
#include "soc/gdma_channel.h"
#include "hal/gdma_ll.h"
#endif /* SOC_GDMA_SUPPORTED */

#ifdef __cplusplus
extern "C"
{
#endif


#if (SOC_AES_SUPPORT_DMA) || (SOC_SHA_SUPPORT_DMA)

#if (SOC_AES_GDMA) || (SOC_SHA_GDMA)

#if (SOC_GDMA_TRIG_PERIPH_AES0_BUS == SOC_GDMA_BUS_AHB) || (SOC_GDMA_TRIG_PERIPH_SHA0_BUS == SOC_GDMA_BUS_AHB)
#define DMA_DESC_MEM_ALIGN_SIZE GDMA_LL_AHB_DESC_ALIGNMENT
typedef dma_descriptor_align4_t crypto_dma_desc_t;
#elif (SOC_GDMA_TRIG_PERIPH_AES0_BUS == SOC_GDMA_BUS_AXI) || (SOC_GDMA_TRIG_PERIPH_SHA0_BUS == SOC_GDMA_BUS_AXI)
#define DMA_DESC_MEM_ALIGN_SIZE GDMA_LL_AXI_DESC_ALIGNMENT
typedef dma_descriptor_align8_t crypto_dma_desc_t;
#else
#error "As we support a shared crypto GDMA layer for the AES and the SHA peripheral, both the peripherals must use the same GDMA bus"
#endif /* (SOC_GDMA_TRIG_PERIPH_AES0_BUS == SOC_GDMA_BUS_AHB) || (SOC_GDMA_TRIG_PERIPH_AES0_BUS == SOC_GDMA_BUS_AHB) */

#elif (SOC_AES_CRYPTO_DMA) || (SOC_SHA_CRYPTO_DMA)
#define DMA_DESC_MEM_ALIGN_SIZE GDMA_LL_AHB_DESC_ALIGNMENT
typedef dma_descriptor_align4_t crypto_dma_desc_t;

#endif /* (SOC_AES_GDMA) && (SOC_SHA_GDMA) */

#endif /* (SOC_AES_SUPPORT_DMA) || (SOC_SHA_SUPPORT_DMA) */


#ifdef __cplusplus
}
#endif
