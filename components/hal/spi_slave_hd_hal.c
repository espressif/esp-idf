// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// The HAL layer for SPI Slave HD

#include <string.h>
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "sdkconfig.h"
#include "soc/spi_periph.h"
#include "soc/lldesc.h"
#include "hal/spi_slave_hd_hal.h"
#include "soc/soc_caps.h"

//This GDMA related part will be introduced by GDMA dedicated APIs in the future. Here we temporarily use macros.
#if SOC_GDMA_SUPPORTED
#include "soc/gdma_struct.h"
#include "hal/gdma_ll.h"

#define spi_dma_ll_rx_reset(dev)                             gdma_ll_rx_reset_channel(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL)
#define spi_dma_ll_tx_reset(dev)                             gdma_ll_tx_reset_channel(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL);
#define spi_dma_ll_rx_enable_burst_data(dev, enable)         gdma_ll_rx_enable_data_burst(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL, enable);
#define spi_dma_ll_tx_enable_burst_data(dev, enable)         gdma_ll_tx_enable_data_burst(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL, enable);
#define spi_dma_ll_rx_enable_burst_desc(dev, enable)         gdma_ll_rx_enable_descriptor_burst(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL, enable);
#define spi_dma_ll_tx_enable_burst_desc(dev, enable)         gdma_ll_tx_enable_descriptor_burst(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL, enable);
#define spi_dma_enable_out_auto_wrback(dev, enable)          gdma_ll_tx_enable_auto_write_back(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL, enable);
#define spi_dma_set_out_eof_generation(dev, enable)          gdma_ll_tx_set_eof_mode(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL, enable);
#define spi_dma_ll_rx_start(dev, addr) do {\
            gdma_ll_rx_set_desc_addr(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL, (uint32_t)addr);\
            gdma_ll_rx_start(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL);\
        } while (0)
#define spi_dma_ll_tx_start(dev, addr) do {\
            gdma_ll_tx_set_desc_addr(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL, (uint32_t)addr);\
            gdma_ll_tx_start(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL);\
        } while (0)
#endif

static void s_spi_slave_hd_hal_dma_init_config(const spi_slave_hd_hal_context_t *hal)
{
    spi_dma_ll_rx_enable_burst_data(hal->dma_in, 1);
    spi_dma_ll_tx_enable_burst_data(hal->dma_out, 1);
    spi_dma_ll_rx_enable_burst_desc(hal->dma_in, 1);
    spi_dma_ll_tx_enable_burst_desc(hal->dma_out, 1);
}

void spi_slave_hd_hal_init(spi_slave_hd_hal_context_t *hal, const spi_slave_hd_hal_config_t *hal_config)
{
    memset(hal, 0, sizeof(spi_slave_hd_hal_context_t));
    spi_dev_t* hw = SPI_LL_GET_HW(hal_config->host_id);
    hal->dev = hw;
    hal->dma_in = hal_config->dma_in;
    hal->dma_out = hal_config->dma_out;

    //Configure slave
    s_spi_slave_hd_hal_dma_init_config(hal);

    spi_ll_slave_hd_init(hw);
    spi_ll_set_addr_bitlen(hw, hal_config->address_bits);
    spi_ll_set_command_bitlen(hw, hal_config->command_bits);
    spi_ll_set_dummy(hw, hal_config->dummy_bits);
    spi_ll_set_rx_lsbfirst(hw, hal_config->rx_lsbfirst);
    spi_ll_set_tx_lsbfirst(hw, hal_config->tx_lsbfirst);
    spi_ll_slave_set_mode(hw, hal_config->mode, (hal_config->dma_chan != 0));

    spi_ll_disable_intr(hw, UINT32_MAX);
    spi_ll_clear_intr(hw, UINT32_MAX);

    spi_ll_set_intr(hw, SPI_LL_INTR_WR_DONE | SPI_LL_INTR_CMD8);

    bool workaround_required = false;
    if (!spi_ll_get_intr(hw, SPI_LL_INTR_WR_DONE)) {
        hal->intr_not_triggered |= SPI_EV_RECV;
        workaround_required = true;
    }
    if (!spi_ll_get_intr(hw, SPI_LL_INTR_CMD8)) {
        hal->intr_not_triggered |= SPI_EV_SEND;
        workaround_required = true;
    }

    if (workaround_required) {
        //Workaround if the previous interrupts are not writable
        spi_ll_set_intr(hw, SPI_LL_INTR_TRANS_DONE);
    }

    spi_ll_slave_hd_set_len_cond(hw,    SPI_LL_TRANS_LEN_COND_WRBUF |
                                        SPI_LL_TRANS_LEN_COND_WRDMA |
                                        SPI_LL_TRANS_LEN_COND_RDBUF |
                                        SPI_LL_TRANS_LEN_COND_RDDMA);

    spi_ll_slave_set_seg_mode(hal->dev, true);
}

void spi_slave_hd_hal_rxdma(spi_slave_hd_hal_context_t *hal, uint8_t *out_buf, size_t len)
{
    lldesc_setup_link(hal->dmadesc_rx, out_buf, len, true);

    spi_ll_dma_fifo_reset(hal->dev);
    spi_dma_ll_rx_reset(hal->dma_in);
    spi_ll_slave_reset(hal->dev);
    spi_ll_infifo_full_clr(hal->dev);
    spi_ll_clear_intr(hal->dev, SPI_LL_INTR_WR_DONE);

    spi_ll_slave_set_rx_bitlen(hal->dev, len * 8);
    spi_ll_dma_rx_enable(hal->dev, 1);
    spi_dma_ll_rx_start(hal->dma_in, &hal->dmadesc_rx[0]);
}

void spi_slave_hd_hal_txdma(spi_slave_hd_hal_context_t *hal, uint8_t *data, size_t len)
{
    lldesc_setup_link(hal->dmadesc_tx, data, len, false);

    spi_ll_dma_fifo_reset(hal->dev);
    spi_dma_ll_tx_reset(hal->dma_out);
    spi_ll_slave_reset(hal->dev);
    spi_ll_outfifo_empty_clr(hal->dev);
    spi_ll_clear_intr(hal->dev, SPI_LL_INTR_CMD8);

    spi_ll_dma_tx_enable(hal->dev, 1);
    spi_dma_ll_tx_start(hal->dma_out, &hal->dmadesc_tx[0]);
}

static spi_ll_intr_t get_event_intr(spi_event_t ev)
{
    spi_ll_intr_t intr = 0;
    if (ev & SPI_EV_BUF_TX) intr |= SPI_LL_INTR_RDBUF;
    if (ev & SPI_EV_BUF_RX) intr |= SPI_LL_INTR_WRBUF;
    if (ev & SPI_EV_SEND) intr |= SPI_LL_INTR_CMD8;
    if (ev & SPI_EV_RECV) intr |= SPI_LL_INTR_WR_DONE;
    if (ev & SPI_EV_CMD9) intr |= SPI_LL_INTR_CMD9;
    if (ev & SPI_EV_CMDA) intr |= SPI_LL_INTR_CMDA;
    if (ev & SPI_EV_TRANS) intr |= SPI_LL_INTR_TRANS_DONE;
    return intr;
}

bool spi_slave_hd_hal_check_clear_event(spi_slave_hd_hal_context_t* hal, spi_event_t ev)
{
    spi_ll_intr_t intr = get_event_intr(ev);
    if (spi_ll_get_intr(hal->dev, intr)) {
        spi_ll_clear_intr(hal->dev, intr);
        return true;
    }
    return false;
}

bool spi_slave_hd_hal_check_disable_event(spi_slave_hd_hal_context_t* hal, spi_event_t ev)
{
    //The trans_done interrupt is used for the workaround when some interrupt is not writable
    spi_ll_intr_t intr = get_event_intr(ev);

    // Workaround for these interrupts not writable
    uint32_t missing_intr = hal->intr_not_triggered & ev;
    if (missing_intr) {
        if ((missing_intr & SPI_EV_RECV) && spi_ll_get_intr(hal->dev, SPI_LL_INTR_WR_DONE)) {
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

void spi_slave_hd_hal_enable_event_intr(spi_slave_hd_hal_context_t* hal, spi_event_t ev)
{
    spi_ll_intr_t intr = get_event_intr(ev);
    spi_ll_enable_intr(hal->dev, intr);
}

void spi_slave_hd_hal_invoke_event_intr(spi_slave_hd_hal_context_t* hal, spi_event_t ev)
{
    spi_ll_intr_t intr = get_event_intr(ev);

    // Workaround for these interrupts not writable
    if (hal->intr_not_triggered & ev & (SPI_EV_RECV | SPI_EV_SEND)) {
        intr |= SPI_LL_INTR_TRANS_DONE;
    }

    spi_ll_enable_intr(hal->dev, intr);
}

void spi_slave_hd_hal_read_buffer(spi_slave_hd_hal_context_t *hal, int addr, uint8_t *out_data, size_t len)
{
    spi_ll_read_buffer_byte(hal->dev, addr, out_data, len);
}

void spi_slave_hd_hal_write_buffer(spi_slave_hd_hal_context_t *hal, int addr, uint8_t *data, size_t len)
{
    spi_ll_write_buffer_byte(hal->dev, addr, data, len);
}

int spi_slave_hd_hal_get_last_addr(spi_slave_hd_hal_context_t *hal)
{
    return spi_ll_slave_hd_get_last_addr(hal->dev);
}

int spi_slave_hd_hal_get_rxlen(spi_slave_hd_hal_context_t *hal)
{
    //this is by -byte
    return spi_ll_slave_get_rx_byte_len(hal->dev);
}

int spi_slave_hd_hal_rxdma_get_len(spi_slave_hd_hal_context_t *hal)
{
    lldesc_t* desc = &hal->dmadesc_rx[0];
    return lldesc_get_received_len(desc, NULL);
}
