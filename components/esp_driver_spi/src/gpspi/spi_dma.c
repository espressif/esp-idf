/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/spi_dma.h"
#include "hal/spi_ll.h"

#if !SOC_GDMA_SUPPORTED
void spi_dma_enable_burst(spi_dma_chan_handle_t chan_handle, bool data_burst, bool desc_burst)
{
    spi_dma_dev_t *spi_dma = SPI_LL_GET_HW(chan_handle.host_id);

    if (chan_handle.dir == DMA_CHANNEL_DIRECTION_TX) {
        spi_dma_ll_tx_enable_burst_data(spi_dma, chan_handle.chan_id, data_burst);
        spi_dma_ll_tx_enable_burst_desc(spi_dma, chan_handle.chan_id, desc_burst);
    } else {
        spi_dma_ll_rx_enable_burst_data(spi_dma, chan_handle.chan_id, data_burst);
        spi_dma_ll_rx_enable_burst_desc(spi_dma, chan_handle.chan_id, desc_burst);
    }
}

/************************************* IRAM CONTEXT **************************************/

void spi_dma_reset(spi_dma_chan_handle_t chan_handle)
{
    spi_dma_dev_t *spi_dma = SPI_LL_GET_HW(chan_handle.host_id);

    if (chan_handle.dir == DMA_CHANNEL_DIRECTION_TX) {
        spi_dma_ll_tx_reset(spi_dma, chan_handle.chan_id);
    } else {
        spi_dma_ll_rx_reset(spi_dma, chan_handle.chan_id);
    }
}

void spi_dma_start(spi_dma_chan_handle_t chan_handle, void *addr)
{
    spi_dma_dev_t *spi_dma = SPI_LL_GET_HW(chan_handle.host_id);

    if (chan_handle.dir == DMA_CHANNEL_DIRECTION_TX) {
        spi_dma_ll_tx_start(spi_dma, chan_handle.chan_id, (lldesc_t *)addr);
    } else {
        spi_dma_ll_rx_start(spi_dma, chan_handle.chan_id, (lldesc_t *)addr);
    }
}
#endif
