/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "stdbool.h"
#include "hal/spi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !SOC_GDMA_SUPPORTED
/**
 * @brief Enumeration of SPI_DMA channel direction
 */
typedef enum {
    DMA_CHANNEL_DIRECTION_TX, /*!< DMA channel direction: TX */
    DMA_CHANNEL_DIRECTION_RX, /*!< DMA channel direction: RX */
} spi_dma_chan_dir_t;

typedef struct {
    spi_host_device_t  host_id;
    spi_dma_chan_dir_t dir;
    int chan_id;
} spi_dma_chan_handle_t;

/**
 * Enable/Disable data/desc burst for spi_dma channel
 *
 * @param chan_handle   Context of the spi_dma channel.
 * @param data_burst    enable or disable data burst
 * @param desc_burst    enable or disable desc burst
 */
void spi_dma_enable_burst(spi_dma_chan_handle_t chan_handle, bool data_burst, bool desc_burst);

/**
 * Re-trigger a HW pre-load to pick up appended linked descriptor
 *
 * @param chan_handle   Context of the spi_dma channel.
 */
void spi_dma_append(spi_dma_chan_handle_t chan_handle);

/**
 * Reset dma channel for spi_dma
 *
 * @param chan_handle   Context of the spi_dma channel.
 */
void spi_dma_reset(spi_dma_chan_handle_t chan_handle);

/**
 * Start dma channel for spi_dma
 *
 * @param chan_handle   Context of the spi_dma channel.
 * @param addr          Addr of linked dma descriptor to mount
 */
void spi_dma_start(spi_dma_chan_handle_t chan_handle, void *addr);

/**
 * Get EOF descriptor for a dma channel
 *
 * @param chan_handle   Context of the spi_dma channel.
 */
uint32_t spi_dma_get_eof_desc(spi_dma_chan_handle_t chan_handle);

#endif  //!SOC_GDMA_SUPPORTED

#ifdef __cplusplus
}
#endif
