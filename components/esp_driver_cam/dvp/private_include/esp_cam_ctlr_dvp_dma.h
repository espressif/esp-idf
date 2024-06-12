/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/dma_types.h"
#include "esp_private/gdma.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_CAM_CTLR_DVP_DMA_DESC_BUFFER_MAX_SIZE   DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED

/**
 * @brief DVP DMA description object
 */
typedef dma_descriptor_align8_t esp_cam_ctlr_dvp_dma_desc_t;

/**
 * @brief DVP DMA object
 */
typedef struct esp_cam_ctlr_dvp_dma {
    gdma_channel_handle_t dma_chan;                 /*!< DVP DMA channel handle */
    size_t size;                                    /*!< DVP DMA buffer size */
    esp_cam_ctlr_dvp_dma_desc_t *desc;              /*!< DVP DMA description buffer */
    size_t desc_count;                              /*!< DVP DMA description count */
    size_t desc_size;                               /*!< DVP DMA description buffer size in byte */
} esp_cam_ctlr_dvp_dma_t;

/**
 * @brief Initialize DVP DMA object
 *
 * @param dma  DVP DMA object pointer
 * @param burst_size DVP DMA burst transmission block size
 * @param size DVP DMA buffer size
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
esp_err_t esp_cam_ctlr_dvp_dma_init(esp_cam_ctlr_dvp_dma_t *dma, uint32_t burst_size, size_t size);

/**
 * @brief De-initialize DVP DMA object
 *
 * @param dma DVP DMA object pointer
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
esp_err_t esp_cam_ctlr_dvp_dma_deinit(esp_cam_ctlr_dvp_dma_t *dma);

/**
 * @brief Set DVP DMA descriptor address and start engine
 *
 * @param dma    DVP DMA object pointer
 * @param buffer DVP DMA buffer pointer
 * @param size   DVP DMA buffer size
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
esp_err_t esp_cam_ctlr_dvp_dma_start(esp_cam_ctlr_dvp_dma_t *dma, uint8_t *buffer, size_t size);

/**
 * @brief Stop DVP DMA engine
 *
 * @param dma DVP DMA object pointer
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
esp_err_t esp_cam_ctlr_dvp_dma_stop(esp_cam_ctlr_dvp_dma_t *dma);

/**
 * @brief Reset DVP DMA FIFO and internal finite state machine
 *
 * @param dma DVP DMA object pointer
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
esp_err_t esp_cam_ctlr_dvp_dma_reset(esp_cam_ctlr_dvp_dma_t *dma);

/**
 * @brief Get DMA received data size
 *
 * @param dma DVP DMA object pointer
 *
 * @return DMA received data size
 */
size_t esp_cam_ctlr_dvp_dma_get_recv_size(esp_cam_ctlr_dvp_dma_t *dma);

#ifdef __cplusplus
}
#endif
