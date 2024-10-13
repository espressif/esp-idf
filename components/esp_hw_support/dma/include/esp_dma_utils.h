/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_heap_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DMA Mem info
 */
typedef struct {
    int extra_heap_caps;               ///< extra heap caps based on MALLOC_CAP_DMA
    size_t dma_alignment_bytes;        ///< DMA alignment
} esp_dma_mem_info_t;

/**
 * @brief Helper function for malloc a DMA capable memory buffer
 *
 * @note This API will take care of the cache alignment internally,
 *       you will need to set `esp_dma_mem_info_t: dma_alignment_bytes`
 *       with either the custom alignment or DMA alignment of used peripheral driver.
 *
 * @param[in]  size          Size in bytes, the amount of memory to allocate
 * @param[in]  dma_mem_info  DMA and memory info, see `esp_dma_mem_info_t`
 * @param[out] out_ptr       A pointer to the memory allocated successfully
 * @param[out] actual_size   Actual size for allocation in bytes, when the size you specified doesn't meet the DMA alignment requirements, this value might be bigger than the size you specified. Set null if you don't care this value.
 *
 * @return
 *        - ESP_OK:
 *        - ESP_ERR_INVALID_ARG: Invalid argument
 *        - ESP_ERR_NO_MEM:      No enough memory for allocation
 */
esp_err_t esp_dma_capable_malloc(size_t size, const esp_dma_mem_info_t *dma_mem_info, void **out_ptr, size_t *actual_size);

/**
 * @brief Helper function for calloc a DMA capable memory buffer
 *
 * @param[in]  calloc_num    Number of elements to allocate
 * @param[in]  size          Size in bytes, the amount of memory to allocate
 * @param[in]  dma_mem_info  DMA and memory info, see `esp_dma_mem_info_t`
 * @param[out] out_ptr       A pointer to the memory allocated successfully
 * @param[out] actual_size   Actual size for allocation in bytes, when the size you specified doesn't meet the DMA alignment requirements, this value might be bigger than the size you specified. Set null if you don't care this value.
 *
 * @return
 *        - ESP_OK:
 *        - ESP_ERR_INVALID_ARG: Invalid argument
 *        - ESP_ERR_NO_MEM:      No enough memory for allocation
 */
esp_err_t esp_dma_capable_calloc(size_t calloc_num, size_t size, const esp_dma_mem_info_t *dma_mem_info, void **out_ptr, size_t *actual_size);

/**
 * @brief Helper function to check if a DMA buffer pointer and size meet both hardware alignment requirements and custom alignment requirements
 *
 * @param[in]  ptr           Pointer to the buffer
 * @param[in]  size          Size of the buffer
 * @param[in]  dma_mem_info  DMA and memory info, see `esp_dma_mem_info_t`
 *
 * @return
 *        - True:  Buffer is aligned
 *        - False: Buffer is not aligned, or buffer is not DMA capable
 */
bool esp_dma_is_buffer_alignment_satisfied(const void *ptr, size_t size, esp_dma_mem_info_t dma_mem_info);

/**
 * @brief Needed info to get GDMA alignment
 */
typedef struct {
    bool is_desc;               ///< allocate DMA descriptor
    bool on_psram;              ///< allocate DMA from the PSRAM
} dma_alignment_info_t;

//-----------------------Deprecated APIs-----------------------//
/**
 * DMA malloc flags
 */
/**
 * @brief Memory is in PSRAM
 */
#define ESP_DMA_MALLOC_FLAG_PSRAM        BIT(0)

/**
 * @note This API will use MAX alignment requirement
 */
esp_err_t esp_dma_malloc(size_t size, uint32_t flags, void **out_ptr, size_t *actual_size)
__attribute__((deprecated("esp_dma_malloc is deprecated, please use esp_dma_capable_malloc")));

/**
 * @note This API will use MAX alignment requirement
 */
esp_err_t esp_dma_calloc(size_t n, size_t size, uint32_t flags, void **out_ptr, size_t *actual_size)
__attribute__((deprecated("esp_dma_calloc is deprecated, please use esp_dma_capable_calloc")));

/**
 * @brief DMA buffer location
 */
typedef enum {
    ESP_DMA_BUF_LOCATION_INTERNAL,    ///< DMA buffer is in internal memory
    ESP_DMA_BUF_LOCATION_PSRAM,       ///< DMA buffer is in PSRAM
    ESP_DMA_BUF_LOCATION_AUTO,        ///< Auto detect buffer location, under this condition API will loop to search the buffer location
} esp_dma_buf_location_t;

/**
 * @note This API will use MAX alignment requirement
 */
bool esp_dma_is_buffer_aligned(const void *ptr, size_t size, esp_dma_buf_location_t location)
__attribute__((deprecated("esp_dma_is_buffer_aligned is deprecated, please use esp_dma_is_buffer_alignment_satisfied")));

#ifdef __cplusplus
}
#endif
