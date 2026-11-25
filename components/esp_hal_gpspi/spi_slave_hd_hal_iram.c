/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for SPI Slave HD, functions used by the driver ISR.
// Kept in a separate file so that the whole object, including any out-of-line copies of the
// LL inline functions, can be placed in IRAM with CONFIG_SPI_SLAVE_ISR_IN_IRAM.

#include "soc/soc_caps.h"
#include "hal/spi_slave_hd_hal.h"

void spi_slave_hd_hal_hw_prepare_rx(spi_slave_hd_hal_context_t *hal)
{
    spi_ll_dma_rx_fifo_reset(hal->dev);
    spi_ll_infifo_full_clr(hal->dev);
    spi_ll_dma_rx_enable(hal->dev, 1);
}

void spi_slave_hd_hal_hw_prepare_tx(spi_slave_hd_hal_context_t *hal)
{
    spi_ll_dma_tx_fifo_reset(hal->dev);
    spi_ll_outfifo_empty_clr(hal->dev);
    spi_ll_dma_tx_enable(hal->dev, 1);
}

void spi_slave_hd_hal_rxdma(spi_slave_hd_hal_context_t *hal)
{
    spi_ll_slave_reset(hal->dev);
    spi_ll_clear_intr(hal->dev, SPI_LL_INTR_CMD7);
    spi_slave_hd_hal_hw_prepare_rx(hal);
}

void spi_slave_hd_hal_txdma(spi_slave_hd_hal_context_t *hal)
{
    spi_ll_slave_reset(hal->dev);
    spi_ll_clear_intr(hal->dev, SPI_LL_INTR_CMD8);
    spi_slave_hd_hal_hw_prepare_tx(hal);
}

static spi_ll_intr_t get_event_intr(spi_slave_hd_hal_context_t *hal, spi_event_t ev)
{
    spi_ll_intr_t intr = 0;
    if (ev & SPI_EV_SEND) {
        intr |= SPI_LL_INTR_CMD8;
    }
    if (ev & SPI_EV_RECV) {
        intr |= SPI_LL_INTR_CMD7;
    }
    if (ev & SPI_EV_BUF_TX) {
        intr |= SPI_LL_INTR_RDBUF;
    }
    if (ev & SPI_EV_BUF_RX) {
        intr |= SPI_LL_INTR_WRBUF;
    }
    if (ev & SPI_EV_CMD9) {
        intr |= SPI_LL_INTR_CMD9;
    }
    if (ev & SPI_EV_CMDA) {
        intr |= SPI_LL_INTR_CMDA;
    }
    if (ev & SPI_EV_TRANS) {
        intr |= SPI_LL_INTR_TRANS_DONE;
    }
    return intr;
}

bool spi_slave_hd_hal_check_clear_intr(spi_slave_hd_hal_context_t *hal, uint32_t mask)
{
    if (spi_ll_get_intr(hal->dev, mask)) {
        spi_ll_clear_intr(hal->dev, mask);
        return true;
    }
    return false;
}

bool spi_slave_hd_hal_check_clear_event(spi_slave_hd_hal_context_t *hal, spi_event_t ev)
{
    return spi_slave_hd_hal_check_clear_intr(hal, get_event_intr(hal, ev));
}

bool spi_slave_hd_hal_check_disable_event(spi_slave_hd_hal_context_t *hal, spi_event_t ev)
{
    //The trans_done interrupt is used for the workaround when some interrupt is not writable
    spi_ll_intr_t intr = get_event_intr(hal, ev);

    // Workaround for these interrupts not writable
    uint32_t missing_intr = hal->intr_not_triggered & ev;
    if (missing_intr) {
        if ((missing_intr & SPI_EV_RECV) && spi_ll_get_intr(hal->dev, SPI_LL_INTR_CMD7)) {
            hal->intr_not_triggered &= ~SPI_EV_RECV;
        }
        if ((missing_intr & SPI_EV_SEND) && spi_ll_get_intr(hal->dev, SPI_LL_INTR_CMD8)) {
            hal->intr_not_triggered &= ~SPI_EV_SEND;
        }
        if (spi_ll_get_intr(hal->dev, SPI_LL_INTR_TRANS_DONE)) {
            spi_ll_disable_intr(hal->dev, SPI_LL_INTR_TRANS_DONE);
        }
    }

    if (spi_ll_get_intr(hal->dev, intr)) {
        spi_ll_disable_intr(hal->dev, intr);
        return true;
    }
    return false;
}

void spi_slave_hd_hal_enable_event_intr(spi_slave_hd_hal_context_t *hal, spi_event_t ev)
{
    spi_ll_intr_t intr = get_event_intr(hal, ev);
    spi_ll_enable_intr(hal->dev, intr);
}

void spi_slave_hd_hal_invoke_event_intr(spi_slave_hd_hal_context_t *hal, spi_event_t ev)
{
    spi_ll_intr_t intr = get_event_intr(hal, ev);

    // Workaround for these interrupts not writable
    if (hal->intr_not_triggered & ev & (SPI_EV_RECV | SPI_EV_SEND)) {
        intr |= SPI_LL_INTR_TRANS_DONE;
    }

    spi_ll_enable_intr(hal->dev, intr);
}
