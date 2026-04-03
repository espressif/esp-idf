/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * @brief  GDMA channel handle type for ASRC hardware
 */
typedef void *asrc_hw_gdma_channel_handle_t;

/**
 * @brief  GDMA descriptor link list handle type for ASRC hardware
 */
typedef void *asrc_hw_gdma_link_list_handle_t;

/**
 * @brief  GDMA event type
 */
typedef enum {
    ASRC_HW_GDMA_RECV_FRAME_DONE = (1 << 0),  /*!< GDMA receive frame done event */
} asrc_hw_gdma_evt_enum_t;

/**
 * @brief  GDMA event structure
 */
typedef struct {
    asrc_hw_gdma_evt_enum_t  gdma_evt;  /*!< GDMA event from interrupt */
} asrc_hw_gdma_evt_t;

/**
 * @brief  Create GDMA channels for ASRC hardware
 *
 * @param[in]   asrc_idx             ASRC hardware index
 * @param[in]   user_data            User context passed to GDMA callbacks
 * @param[in]   max_data_burst_size  Maximum data burst size
 * @param[out]  dma_tx_chan          Returned GDMA TX channel handle
 * @param[out]  dma_rx_chan          Returned GDMA RX channel handle
 *
 * @return
 *       - ESP_OK  Create successful
 *       - Others  Create failed
 */
esp_err_t asrc_hw_gdma_create_channel(int asrc_idx, void *user_data, uint16_t max_data_burst_size,
                                      asrc_hw_gdma_channel_handle_t *dma_tx_chan, asrc_hw_gdma_channel_handle_t *dma_rx_chan);

/**
 * @brief  Create or recreate GDMA descriptor link list if needed
 *
 * @param[in]     byte_cnt      Total transfer size in bytes
 * @param[inout]  list_hd       Pointer to descriptor list handle (will be updated if new list is created)
 * @param[inout]  max_desc_num  Pointer to maximum descriptor number (will be updated if new list is created)
 *
 * @return
 *       - ESP_OK  Create successful
 *       - Others  Create failed
 */
esp_err_t asrc_hw_gdma_create_link_list(uint32_t byte_cnt, asrc_hw_gdma_link_list_handle_t *list_hd, uint32_t *max_desc_num);

/**
 * @brief  Mount buffers to GDMA descriptor link list
 *
 * @param[in]  list_hd   Descriptor list handle (must be created by asrc_hw_gdma_create_link_list)
 * @param[in]  desc_num  Number of descriptors to mount
 * @param[in]  buf       Pointer to DMA buffer
 * @param[in]  byte_cnt  Total transfer size in bytes
 *
 * @return
 *       - ESP_OK  Mount successful
 *       - Others  Mount failed
 */
esp_err_t asrc_hw_gdma_mount_link_list(asrc_hw_gdma_link_list_handle_t list_hd, uint32_t desc_num, uint8_t *buf, uint32_t byte_cnt);

/**
 * @brief  Start GDMA channel for ASRC hardware
 *
 * @param[in]  dma_chan  GDMA channel handle
 * @param[in]  desc      GDMA descriptor link list handle
 *
 * @return
 *       - ESP_OK  Start successful
 *       - Others  Start failed
 */
esp_err_t asrc_hw_gdma_start_channel(asrc_hw_gdma_channel_handle_t dma_chan, asrc_hw_gdma_link_list_handle_t desc);

/**
 * @brief  Free GDMA descriptor link list
 *
 * @param[in]  list_hd  Descriptor list handle to free
 */
void asrc_hw_gdma_free_link_list(asrc_hw_gdma_link_list_handle_t list_hd);

/**
 * @brief  Deinitialize GDMA channels for ASRC hardware
 *
 * @param[in]  dma_tx_chan  GDMA TX channel handle
 * @param[in]  dma_rx_chan  GDMA RX channel handle
 */
void asrc_hw_gdma_destroy_channel(asrc_hw_gdma_channel_handle_t dma_tx_chan, asrc_hw_gdma_channel_handle_t dma_rx_chan);

/**
 * @brief  Get buffer alignment for a given heap capability
 *
 * @param[in]   heap_caps      Heap capability to check
 * @param[out]  out_alignment  Returned buffer alignment
 *
 * @return
 *       - ESP_OK  Alignment retrieval successful
 *       - Others  Alignment retrieval failed
 */
esp_err_t asrc_hw_get_buffer_alignment(uint32_t heap_caps, size_t *out_alignment);

/**
 * @brief  Check if buffer alignment is valid for ASRC hardware
 *
 * @param[in]  buffer       Pointer to buffer to check
 * @param[in]  buffer_size  Buffer size in bytes
 *
 * @return
 *       - ESP_OK  Buffer alignment is valid
 *       - Others  Buffer alignment is invalid
 */
esp_err_t asrc_hw_check_buffer_alignment(uint8_t *buffer, uint32_t buffer_size);

/**
 * @brief  Cache sync from CPU to memory
 *
 * @param[in]  buffer       Pointer to buffer to sync
 * @param[in]  buffer_size  Buffer size in bytes
 *
 * @return
 *       - ESP_OK  Cache sync successful
 *       - Others  Cache sync failed
 */
esp_err_t asrc_hw_cache_msync_c2m(uint8_t *buffer, uint32_t buffer_size);

/**
 * @brief  Cache sync from memory to CPU
 *
 * @param[in]  buffer       Pointer to buffer to sync
 * @param[in]  buffer_size  Buffer size in bytes
 *
 * @return
 *       - ESP_OK  Cache sync successful
 *       - Others  Cache sync failed
 */
esp_err_t asrc_hw_cache_msync_m2c(uint8_t *buffer, uint32_t buffer_size);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
