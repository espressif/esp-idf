/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for SPI Slave HD

#include <string.h>
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "soc/lldesc.h"
#include "soc/soc_caps.h"
#include "soc/soc.h"   //for SOC_NON_CACHEABLE_OFFSET_SRAM
#include "hal/spi_slave_hd_hal.h"
#include "hal/assert.h"

void spi_slave_hd_hal_init(spi_slave_hd_hal_context_t *hal, const spi_slave_hd_hal_config_t *hal_config)
{
    spi_dev_t *hw = SPI_LL_GET_HW(hal_config->host_id);
    hal->dev = hw;
    hal->dma_enabled = hal_config->dma_enabled;
    hal->append_mode = hal_config->append_mode;
    hal->tx_cur_desc = hal->dmadesc_tx;
    hal->rx_cur_desc = hal->dmadesc_rx;
    hal->tx_dma_head = hal->dmadesc_tx + hal->dma_desc_num - 1;
    hal->rx_dma_head = hal->dmadesc_rx + hal->dma_desc_num - 1;

    spi_ll_slave_hd_init(hw);
    spi_ll_set_addr_bitlen(hw, hal_config->address_bits);
    spi_ll_set_command_bitlen(hw, hal_config->command_bits);
    spi_ll_set_dummy(hw, hal_config->dummy_bits);
    spi_ll_set_rx_lsbfirst(hw, hal_config->rx_lsbfirst);
    spi_ll_set_tx_lsbfirst(hw, hal_config->tx_lsbfirst);
    spi_ll_slave_set_mode(hw, hal_config->mode, (hal_config->dma_enabled));

    spi_ll_disable_intr(hw, UINT32_MAX);
    spi_ll_clear_intr(hw, UINT32_MAX);
    if (!hal_config->append_mode) {
        spi_ll_set_intr(hw, SPI_LL_INTR_CMD7 | SPI_LL_INTR_CMD8);

        bool workaround_required = false;
        if (!spi_ll_get_intr(hw, SPI_LL_INTR_CMD7)) {
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
    } else {
#if !SOC_GDMA_SUPPORTED
        spi_ll_enable_intr(hw, SPI_LL_INTR_CMD7 | SPI_LL_INTR_CMD8);
#endif //SOC_GDMA_SUPPORTED
    }

    spi_ll_slave_hd_set_len_cond(hw,    SPI_LL_TRANS_LEN_COND_WRBUF |
                                 SPI_LL_TRANS_LEN_COND_WRDMA |
                                 SPI_LL_TRANS_LEN_COND_RDBUF |
                                 SPI_LL_TRANS_LEN_COND_RDDMA);

    spi_ll_slave_set_seg_mode(hal->dev, true);
}

#if SOC_NON_CACHEABLE_OFFSET_SRAM
#include "hal/cache_ll.h"
#define ADDR_DMA_2_CPU(addr)   ((typeof(addr))CACHE_LL_L2MEM_NON_CACHE_ADDR(addr))
#define ADDR_CPU_2_DMA(addr)   ((typeof(addr))CACHE_LL_L2MEM_CACHE_ADDR(addr))
#else
#define ADDR_DMA_2_CPU(addr)   (addr)
#define ADDR_CPU_2_DMA(addr)   (addr)
#endif

static int s_desc_get_received_len_addr(spi_dma_desc_t* head, spi_dma_desc_t** out_next, void **out_buff_head)
{
    spi_dma_desc_t* desc_cpu = ADDR_DMA_2_CPU(head);
    int len = 0;
    if (out_buff_head) {
        *out_buff_head = desc_cpu->buffer;
    }
    while (head) {
        len += desc_cpu->dw0.length;
        bool eof = desc_cpu->dw0.suc_eof;
        desc_cpu = ADDR_DMA_2_CPU(desc_cpu->next);
        head = head->next;
        if (eof) {
            break;
        }
    }
    if (out_next) {
        *out_next = head;
    }
    return len;
}

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

int spi_slave_hd_hal_rxdma_seg_get_len(spi_slave_hd_hal_context_t *hal)
{
    spi_dma_desc_t *desc = hal->dmadesc_rx->desc;
    return s_desc_get_received_len_addr(desc, NULL, NULL);
}

bool spi_slave_hd_hal_get_tx_finished_trans(spi_slave_hd_hal_context_t *hal, void **out_trans, void **real_buff_addr)
{
    if ((uint32_t)hal->tx_dma_head->desc == hal->current_eof_addr) {
        return false;
    }

    //find used paired desc-trans by desc addr
    hal->tx_dma_head++;
    if (hal->tx_dma_head >= hal->dmadesc_tx + hal->dma_desc_num) {
        hal->tx_dma_head = hal->dmadesc_tx;
    }
    *out_trans = hal->tx_dma_head->arg;
    s_desc_get_received_len_addr(hal->tx_dma_head->desc, NULL, real_buff_addr);
    return true;
}

bool spi_slave_hd_hal_get_rx_finished_trans(spi_slave_hd_hal_context_t *hal, void **out_trans, void **real_buff_addr, size_t *out_len)
{
    if ((uint32_t)hal->rx_dma_head->desc == hal->current_eof_addr) {
        return false;
    }

    //find used paired desc-trans by desc addr
    hal->rx_dma_head++;
    if (hal->rx_dma_head >= hal->dmadesc_rx + hal->dma_desc_num) {
        hal->rx_dma_head = hal->dmadesc_rx;
    }
    *out_trans = hal->rx_dma_head->arg;
    *out_len = s_desc_get_received_len_addr(hal->rx_dma_head->desc, NULL, real_buff_addr);
    return true;
}
