#include "hal/spi_slave_hal.h"
#include "hal/spi_ll.h"
#include "soc/soc_caps.h"

//This GDMA related part will be introduced by GDMA dedicated APIs in the future. Here we temporarily use macros.
#if SOC_GDMA_SUPPORTED
#include "soc/gdma_struct.h"
#include "hal/gdma_ll.h"

#define spi_dma_ll_rx_reset(dev, chan)                             gdma_ll_rx_reset_channel(&GDMA, chan)
#define spi_dma_ll_tx_reset(dev, chan)                             gdma_ll_tx_reset_channel(&GDMA, chan);
#define spi_dma_ll_rx_start(dev, chan, addr) do {\
            gdma_ll_rx_set_desc_addr(&GDMA, chan, (uint32_t)addr);\
            gdma_ll_rx_start(&GDMA, chan);\
        } while (0)
#define spi_dma_ll_tx_start(dev, chan, addr) do {\
            gdma_ll_tx_set_desc_addr(&GDMA, chan, (uint32_t)addr);\
            gdma_ll_tx_start(&GDMA, chan);\
        } while (0)
#endif

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
    if (hal->use_dma) {

        //Fill DMA descriptors
        if (hal->rx_buffer) {
            lldesc_setup_link(hal->dmadesc_rx, hal->rx_buffer, ((hal->bitlen + 7) / 8), true);

            //reset dma inlink, this should be reset before spi related reset
            spi_dma_ll_rx_reset(hal->dma_in, hal->rx_dma_chan);
            spi_ll_dma_rx_fifo_reset(hal->dma_in);
            spi_ll_slave_reset(hal->hw);
            spi_ll_infifo_full_clr(hal->hw);

            spi_ll_dma_rx_enable(hal->hw, 1);
            spi_dma_ll_rx_start(hal->dma_in, hal->rx_dma_chan, &hal->dmadesc_rx[0]);
        }
        if (hal->tx_buffer) {
            lldesc_setup_link(hal->dmadesc_tx, hal->tx_buffer, (hal->bitlen + 7) / 8, false);
            //reset dma outlink, this should be reset before spi related reset
            spi_dma_ll_tx_reset(hal->dma_out, hal->tx_dma_chan);
            spi_ll_dma_tx_fifo_reset(hal->dma_out);
            spi_ll_slave_reset(hal->hw);
            spi_ll_outfifo_empty_clr(hal->hw);

            spi_ll_dma_tx_enable(hal->hw, 1);
            spi_dma_ll_tx_start(hal->dma_out, hal->tx_dma_chan, (&hal->dmadesc_tx[0]));
        }
    } else {
        //No DMA. Turn off SPI and copy data to transmit buffers.
        if (hal->tx_buffer) {
            spi_ll_slave_reset(hal->hw);
            spi_ll_write_buffer(hal->hw, hal->tx_buffer, hal->bitlen);
        }

        spi_ll_cpu_tx_fifo_reset(hal->hw);
    }

    spi_ll_slave_set_rx_bitlen(hal->hw, hal->bitlen);
    spi_ll_slave_set_tx_bitlen(hal->hw, hal->bitlen);

#ifdef CONFIG_IDF_TARGET_ESP32
    //SPI Slave mode on ESP32 requires MOSI/MISO enable
    spi_ll_enable_mosi(hal->hw, (hal->rx_buffer == NULL) ? 0 : 1);
    spi_ll_enable_miso(hal->hw, (hal->tx_buffer == NULL) ? 0 : 1);
#endif
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
        spi_ll_read_buffer(hal->hw, hal->rx_buffer, (hal->rcv_bitlen > hal->bitlen) ? hal->bitlen : hal->rcv_bitlen);
    }
}

uint32_t spi_slave_hal_get_rcv_bitlen(spi_slave_hal_context_t *hal)
{
    return hal->rcv_bitlen;
}

#if CONFIG_IDF_TARGET_ESP32
//This workaround is only for esp32
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
#endif //#if CONFIG_IDF_TARGET_ESP32
