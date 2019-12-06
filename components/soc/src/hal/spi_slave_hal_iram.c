#include "hal/spi_slave_hal.h"
#include "hal/spi_ll.h"

bool spi_slave_hal_usr_is_done(spi_slave_hal_context_t* hal)
{
    return spi_ll_usr_is_done(hal->hw);
}

void spi_slave_hal_user_start(const spi_slave_hal_context_t *hal)
{
    spi_ll_clear_int_stat(hal->hw); //clear int bit
    spi_ll_user_start(hal->hw);
}

void spi_slave_hal_prepare_data(const spi_slave_hal_context_t *hal)
{
    spi_ll_slave_reset(hal->hw);
    if (hal->use_dma) {
        spi_ll_reset_dma(hal->hw);

        //Fill DMA descriptors
        if (hal->rx_buffer) {
            lldesc_setup_link(hal->dmadesc_rx, hal->rx_buffer, ((hal->bitlen + 7) / 8), true);
            spi_ll_rxdma_start(hal->hw, &hal->dmadesc_rx[0]);
        }
        if (hal->tx_buffer) {
            lldesc_setup_link(hal->dmadesc_tx, hal->tx_buffer, (hal->bitlen + 7) / 8, false);
            spi_ll_txdma_start(hal->hw, (&hal->dmadesc_tx[0]));
        }
    } else {
        //No DMA. Turn off SPI and copy data to transmit buffers.
        if (hal->tx_buffer) {
            spi_ll_write_buffer(hal->hw, hal->tx_buffer, hal->bitlen);
        }
    }

    spi_ll_slave_set_rx_bitlen(hal->hw, hal->bitlen);
    spi_ll_slave_set_tx_bitlen(hal->hw, hal->bitlen);
    spi_ll_enable_mosi(hal->hw, (hal->tx_buffer == NULL) ? 0 : 1);
    spi_ll_enable_miso(hal->hw, (hal->rx_buffer == NULL) ? 0 : 1);
}

void spi_slave_hal_store_result(spi_slave_hal_context_t *hal)
{
    //when data of cur_trans->length are all sent, the slv_rdata_bit
    //will be the length sent-1 (i.e. cur_trans->length-1 ), otherwise
    //the length sent.
    hal->rcv_bitlen = spi_ll_slave_get_rcv_bitlen(hal->hw);
    if (hal->rcv_bitlen == hal->bitlen - 1) {
        hal->rcv_bitlen++;
    }
    if (!hal->use_dma && hal->rx_buffer) {
        //Copy result out
        spi_ll_read_buffer(hal->hw, hal->rx_buffer, hal->bitlen);
    }
    spi_ll_slave_set_int_type(hal->hw, SPI_LL_INT_TYPE_NORMAL);
}

uint32_t spi_slave_hal_get_rcv_bitlen(spi_slave_hal_context_t *hal)
{
    return hal->rcv_bitlen;
}

bool spi_slave_hal_dma_need_reset(const spi_slave_hal_context_t *hal)
{
    bool ret;
    ret = false;
    if (hal->use_dma && hal->rx_buffer) {
        int i;
        //In case CS goes high too soon, the transfer is aborted while the DMA channel still thinks it's going. This
        //leads to issues later on, so in that case we need to reset the channel. The state can be detected because
        //the DMA system doesn't give back the offending descriptor; the owner is still set to DMA.
        for (i = 0; hal->dmadesc_rx[i].eof == 0 && hal->dmadesc_rx[i].owner == 0; i++) {}
        if (hal->dmadesc_rx[i].owner) {
            ret = true;
        }
    }
    return ret;
}