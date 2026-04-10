/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hal/spi_slave_hal.h"
#include "hal/spi_ll.h"
#include "soc/soc_caps.h"

bool spi_slave_hal_usr_is_done(spi_slave_hal_context_t* hal)
{
    return spi_ll_usr_is_done(hal->hw);
}

void spi_slave_hal_user_start(const spi_slave_hal_context_t *hal)
{
    spi_ll_clear_int_stat(hal->hw); //clear int bit
    spi_ll_user_start(hal->hw);
}

void spi_slave_hal_hw_prepare_rx(spi_dev_t *hw)
{
    spi_ll_dma_rx_fifo_reset(hw);
    spi_ll_infifo_full_clr(hw);
    spi_ll_dma_rx_enable(hw, 1);
}

void spi_slave_hal_hw_prepare_tx(spi_dev_t *hw)
{
    spi_ll_dma_tx_fifo_reset(hw);
    spi_ll_outfifo_empty_clr(hw);
    spi_ll_dma_tx_enable(hw, 1);
}

void spi_slave_hal_hw_reset(spi_slave_hal_context_t *hal)
{
    spi_ll_slave_reset(hal->hw);
}

void spi_slave_hal_hw_fifo_reset(spi_slave_hal_context_t *hal, bool tx_rst, bool rx_rst)
{
    tx_rst ? spi_ll_cpu_tx_fifo_reset(hal->hw) : 0;
    rx_rst ? spi_ll_cpu_rx_fifo_reset(hal->hw) : 0;
}

void spi_slave_hal_push_tx_buffer(spi_slave_hal_context_t *hal)
{
    if (hal->tx_buffer) {
        spi_ll_write_buffer(hal->hw, hal->tx_buffer, hal->bitlen);
    }
}

void spi_slave_hal_set_trans_bitlen(spi_slave_hal_context_t *hal)
{
    spi_ll_slave_set_rx_bitlen(hal->hw, hal->bitlen);
    spi_ll_slave_set_tx_bitlen(hal->hw, hal->bitlen);
}

void spi_slave_hal_enable_data_line(spi_slave_hal_context_t *hal)
{
    spi_ll_enable_mosi(hal->hw, (hal->rx_buffer != NULL));
    spi_ll_enable_miso(hal->hw, (hal->tx_buffer != NULL));
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

bool spi_slave_hal_dma_need_reset(const spi_slave_hal_context_t *hal)
{
    bool ret = false;
#if SPI_LL_SLAVE_NEEDS_RESET_WORKAROUND
    if (hal->use_dma && hal->rx_buffer) {
        int i;
        //In case CS goes high too soon, the transfer is aborted while the DMA channel still thinks it's going. This
        //leads to issues later on, so in that case we need to reset the channel. The state can be detected because
        //the DMA system doesn't give back the offending descriptor; the owner is still set to DMA.
        for (i = 0; hal->dmadesc_rx[i].dw0.suc_eof == 0 && hal->dmadesc_rx[i].dw0.owner == 0; i++) {}
        if (hal->dmadesc_rx[i].dw0.owner) {
            ret = true;
        }
    }
#endif // SPI_LL_SLAVE_NEEDS_RESET_WORKAROUND
    return ret;
}

#if SOC_SPI_SUPPORT_SLAVE_HD_VER2
bool spi_slave_hal_get_intr_status(spi_slave_hal_context_t *hal, uint32_t mask)
{
    return spi_ll_get_intr(hal->hw, mask);
}

void spi_slave_hal_clear_intr_status(spi_slave_hal_context_t *hal, uint32_t mask)
{
    spi_ll_clear_intr(hal->hw, mask);
}
#endif
