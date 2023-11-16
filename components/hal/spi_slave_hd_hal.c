/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for SPI Slave HD

#include <string.h>
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "sdkconfig.h"
#include "soc/spi_periph.h"
#include "soc/lldesc.h"
#include "soc/soc_caps.h"
#include "hal/spi_slave_hd_hal.h"
#include "hal/assert.h"

//This GDMA related part will be introduced by GDMA dedicated APIs in the future. Here we temporarily use macros.
#if SOC_GDMA_SUPPORTED
#include "soc/gdma_struct.h"
#include "hal/gdma_ll.h"
#define spi_dma_ll_tx_restart(dev, chan)                           gdma_ll_tx_restart(&GDMA, chan)
#define spi_dma_ll_rx_restart(dev, chan)                           gdma_ll_rx_restart(&GDMA, chan)
#define spi_dma_ll_rx_reset(dev, chan)                             gdma_ll_rx_reset_channel(&GDMA, chan)
#define spi_dma_ll_tx_reset(dev, chan)                             gdma_ll_tx_reset_channel(&GDMA, chan)
#define spi_dma_ll_rx_enable_burst_data(dev, chan, enable)         gdma_ll_rx_enable_data_burst(&GDMA, chan, enable)
#define spi_dma_ll_tx_enable_burst_data(dev, chan, enable)         gdma_ll_tx_enable_data_burst(&GDMA, chan, enable)
#define spi_dma_ll_rx_enable_burst_desc(dev, chan, enable)         gdma_ll_rx_enable_descriptor_burst(&GDMA, chan, enable)
#define spi_dma_ll_tx_enable_burst_desc(dev, chan, enable)         gdma_ll_tx_enable_descriptor_burst(&GDMA, chan, enable)
#define spi_dma_ll_enable_out_auto_wrback(dev, chan, enable)       gdma_ll_tx_enable_auto_write_back(&GDMA, chan, enable)
#define spi_dma_ll_set_out_eof_generation(dev, chan, enable)       gdma_ll_tx_set_eof_mode(&GDMA, chan, enable)
#define spi_dma_ll_get_out_eof_desc_addr(dev, chan)                gdma_ll_tx_get_eof_desc_addr(&GDMA, chan)
#define spi_dma_ll_get_in_suc_eof_desc_addr(dev, chan)             gdma_ll_rx_get_success_eof_desc_addr(&GDMA, chan)
#define spi_dma_ll_rx_start(dev, chan, addr) do {\
            gdma_ll_rx_set_desc_addr(&GDMA, chan, (uint32_t)addr);\
            gdma_ll_rx_start(&GDMA, chan);\
        } while (0)
#define spi_dma_ll_tx_start(dev, chan, addr) do {\
            gdma_ll_tx_set_desc_addr(&GDMA, chan, (uint32_t)addr);\
            gdma_ll_tx_start(&GDMA, chan);\
        } while (0)
#endif

static void s_spi_slave_hd_hal_dma_init_config(const spi_slave_hd_hal_context_t *hal)
{
    spi_dma_ll_rx_enable_burst_data(hal->dma_in, hal->rx_dma_chan, 1);
    spi_dma_ll_tx_enable_burst_data(hal->dma_out, hal->tx_dma_chan, 1);
    spi_dma_ll_rx_enable_burst_desc(hal->dma_in, hal->rx_dma_chan, 1);
    spi_dma_ll_tx_enable_burst_desc(hal->dma_out, hal->tx_dma_chan, 1);
    spi_dma_ll_enable_out_auto_wrback(hal->dma_out, hal->tx_dma_chan, 1);
    spi_dma_ll_set_out_eof_generation(hal->dma_out, hal->tx_dma_chan, 1);
}

void spi_slave_hd_hal_init(spi_slave_hd_hal_context_t *hal, const spi_slave_hd_hal_config_t *hal_config)
{
    spi_dev_t *hw = SPI_LL_GET_HW(hal_config->host_id);
    hal->dev = hw;
    hal->dma_in = hal_config->dma_in;
    hal->dma_out = hal_config->dma_out;
    hal->dma_enabled = hal_config->dma_enabled;
    hal->tx_dma_chan = hal_config->tx_dma_chan;
    hal->rx_dma_chan = hal_config->rx_dma_chan;
    hal->append_mode = hal_config->append_mode;
    hal->rx_cur_desc = hal->dmadesc_rx;
    hal->tx_cur_desc = hal->dmadesc_tx;
    STAILQ_NEXT(&hal->tx_dummy_head.desc, qe) = &hal->dmadesc_tx->desc;
    hal->tx_dma_head = &hal->tx_dummy_head;
    STAILQ_NEXT(&hal->rx_dummy_head.desc, qe) = &hal->dmadesc_rx->desc;
    hal->rx_dma_head = &hal->rx_dummy_head;

    //Configure slave
    if (hal_config->dma_enabled) {
        s_spi_slave_hd_hal_dma_init_config(hal);
    }

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
#if SOC_GDMA_SUPPORTED
        spi_ll_enable_intr(hw, SPI_LL_INTR_CMD7);
#else
        spi_ll_clear_intr(hw, SPI_LL_INTR_OUT_EOF | SPI_LL_INTR_CMD7);
        spi_ll_enable_intr(hw, SPI_LL_INTR_OUT_EOF | SPI_LL_INTR_CMD7);
#endif //SOC_GDMA_SUPPORTED
    }

    spi_ll_slave_hd_set_len_cond(hw,    SPI_LL_TRANS_LEN_COND_WRBUF |
                                        SPI_LL_TRANS_LEN_COND_WRDMA |
                                        SPI_LL_TRANS_LEN_COND_RDBUF |
                                        SPI_LL_TRANS_LEN_COND_RDDMA);

    spi_ll_slave_set_seg_mode(hal->dev, true);
}

uint32_t spi_salve_hd_hal_get_max_bus_size(spi_slave_hd_hal_context_t *hal)
{
    return hal->dma_desc_num * LLDESC_MAX_NUM_PER_DESC;
}

uint32_t spi_slave_hd_hal_get_total_desc_size(spi_slave_hd_hal_context_t *hal, uint32_t bus_size)
{
    //See how many dma descriptors we need
    int dma_desc_ct = (bus_size + LLDESC_MAX_NUM_PER_DESC - 1) / LLDESC_MAX_NUM_PER_DESC;
    if (dma_desc_ct == 0) {
        dma_desc_ct = 1; //default to 4k when max is not given
    }
    hal->dma_desc_num = dma_desc_ct;

    return hal->dma_desc_num * sizeof(spi_slave_hd_hal_desc_append_t);
}

void spi_slave_hd_hal_rxdma(spi_slave_hd_hal_context_t *hal, uint8_t *out_buf, size_t len)
{
    lldesc_setup_link(&hal->dmadesc_rx->desc, out_buf, len, true);

    spi_ll_dma_rx_fifo_reset(hal->dev);
    spi_dma_ll_rx_reset(hal->dma_in, hal->rx_dma_chan);
    spi_ll_slave_reset(hal->dev);
    spi_ll_infifo_full_clr(hal->dev);
    spi_ll_clear_intr(hal->dev, SPI_LL_INTR_CMD7);

    spi_ll_dma_rx_enable(hal->dev, 1);
    spi_dma_ll_rx_start(hal->dma_in, hal->rx_dma_chan, &hal->dmadesc_rx->desc);
}

void spi_slave_hd_hal_txdma(spi_slave_hd_hal_context_t *hal, uint8_t *data, size_t len)
{
    lldesc_setup_link(&hal->dmadesc_tx->desc, data, len, false);

    spi_ll_dma_tx_fifo_reset(hal->dev);
    spi_dma_ll_tx_reset(hal->dma_out, hal->tx_dma_chan);
    spi_ll_slave_reset(hal->dev);
    spi_ll_outfifo_empty_clr(hal->dev);
    spi_ll_clear_intr(hal->dev, SPI_LL_INTR_CMD8);

    spi_ll_dma_tx_enable(hal->dev, 1);
    spi_dma_ll_tx_start(hal->dma_out, hal->tx_dma_chan, &hal->dmadesc_tx->desc);
}

static spi_ll_intr_t get_event_intr(spi_slave_hd_hal_context_t *hal, spi_event_t ev)
{
    spi_ll_intr_t intr = 0;
#if CONFIG_IDF_TARGET_ESP32S2
    if ((ev & SPI_EV_SEND) && hal->append_mode) intr |= SPI_LL_INTR_OUT_EOF;
#endif
    if ((ev & SPI_EV_SEND) && !hal->append_mode) intr |= SPI_LL_INTR_CMD8;
    if (ev & SPI_EV_RECV)          intr |= SPI_LL_INTR_CMD7;
    if (ev & SPI_EV_BUF_TX)        intr |= SPI_LL_INTR_RDBUF;
    if (ev & SPI_EV_BUF_RX)        intr |= SPI_LL_INTR_WRBUF;
    if (ev & SPI_EV_CMD9)          intr |= SPI_LL_INTR_CMD9;
    if (ev & SPI_EV_CMDA)          intr |= SPI_LL_INTR_CMDA;
    if (ev & SPI_EV_TRANS)         intr |= SPI_LL_INTR_TRANS_DONE;
    return intr;
}

bool spi_slave_hd_hal_check_clear_event(spi_slave_hd_hal_context_t *hal, spi_event_t ev)
{
    spi_ll_intr_t intr = get_event_intr(hal, ev);
    if (spi_ll_get_intr(hal->dev, intr)) {
        spi_ll_clear_intr(hal->dev, intr);
        return true;
    }
    return false;
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
    lldesc_t *desc = &hal->dmadesc_rx->desc;
    return lldesc_get_received_len(desc, NULL);
}

bool spi_slave_hd_hal_get_tx_finished_trans(spi_slave_hd_hal_context_t *hal, void **out_trans)
{
    if ((uint32_t)&hal->tx_dma_head->desc == spi_dma_ll_get_out_eof_desc_addr(hal->dma_out, hal->tx_dma_chan)) {
        return false;
    }

    hal->tx_dma_head = (spi_slave_hd_hal_desc_append_t *)STAILQ_NEXT(&hal->tx_dma_head->desc, qe);
    *out_trans = hal->tx_dma_head->arg;
    hal->tx_recycled_desc_cnt++;

    return true;
}

bool spi_slave_hd_hal_get_rx_finished_trans(spi_slave_hd_hal_context_t *hal, void **out_trans, size_t *out_len)
{
    if ((uint32_t)&hal->rx_dma_head->desc == spi_dma_ll_get_in_suc_eof_desc_addr(hal->dma_in, hal->rx_dma_chan)) {
        return false;
    }

    hal->rx_dma_head = (spi_slave_hd_hal_desc_append_t *)STAILQ_NEXT(&hal->rx_dma_head->desc, qe);
    *out_trans = hal->rx_dma_head->arg;
    *out_len = hal->rx_dma_head->desc.length;
    hal->rx_recycled_desc_cnt++;

    return true;
}

static void spi_slave_hd_hal_link_append_desc(spi_slave_hd_hal_desc_append_t *dmadesc, const void *data, int len, bool isrx, void *arg)
{
    HAL_ASSERT(len <= LLDESC_MAX_NUM_PER_DESC);     //TODO: Add support for transaction with length larger than 4092, IDF-2660
    int n = 0;
    while (len) {
        int dmachunklen = len;
        if (dmachunklen > LLDESC_MAX_NUM_PER_DESC) {
            dmachunklen = LLDESC_MAX_NUM_PER_DESC;
        }
        if (isrx) {
            //Receive needs DMA length rounded to next 32-bit boundary
            dmadesc[n].desc.size = (dmachunklen + 3) & (~3);
            dmadesc[n].desc.length = (dmachunklen + 3) & (~3);
        } else {
            dmadesc[n].desc.size = dmachunklen;
            dmadesc[n].desc.length = dmachunklen;
        }
        dmadesc[n].desc.buf = (uint8_t *)data;
        dmadesc[n].desc.eof = 0;
        dmadesc[n].desc.sosf = 0;
        dmadesc[n].desc.owner = 1;
        dmadesc[n].desc.qe.stqe_next = &dmadesc[n + 1].desc;
        dmadesc[n].arg = arg;
        len -= dmachunklen;
        data += dmachunklen;
        n++;
    }
    dmadesc[n - 1].desc.eof = 1; //Mark last DMA desc as end of stream.
    dmadesc[n - 1].desc.qe.stqe_next = NULL;
}

esp_err_t spi_slave_hd_hal_txdma_append(spi_slave_hd_hal_context_t *hal, uint8_t *data, size_t len, void *arg)
{
    //Check if there are enough available DMA descriptors for software to use
    int num_required = (len + LLDESC_MAX_NUM_PER_DESC - 1) / LLDESC_MAX_NUM_PER_DESC;
    int not_recycled_desc_num = hal->tx_used_desc_cnt - hal->tx_recycled_desc_cnt;
    int available_desc_num = hal->dma_desc_num - not_recycled_desc_num;
    if (num_required > available_desc_num) {
        return ESP_ERR_INVALID_STATE;
    }

    spi_slave_hd_hal_link_append_desc(hal->tx_cur_desc, data, len, false, arg);

    if (!hal->tx_dma_started) {
        hal->tx_dma_started = true;
        //start a link
        hal->tx_dma_tail = hal->tx_cur_desc;
        spi_ll_dma_tx_fifo_reset(hal->dma_out);
        spi_ll_outfifo_empty_clr(hal->dev);
        spi_dma_ll_tx_reset(hal->dma_out, hal->tx_dma_chan);
        spi_ll_dma_tx_enable(hal->dev, 1);
        spi_dma_ll_tx_start(hal->dma_out, hal->tx_dma_chan, &hal->tx_cur_desc->desc);
    } else {
        //there is already a consecutive link
        STAILQ_NEXT(&hal->tx_dma_tail->desc, qe) = &hal->tx_cur_desc->desc;
        hal->tx_dma_tail = hal->tx_cur_desc;
        spi_dma_ll_tx_restart(hal->dma_out, hal->tx_dma_chan);
    }

    //Move the current descriptor pointer according to the number of the linked descriptors
    for (int i = 0; i < num_required; i++) {
        hal->tx_used_desc_cnt++;
        hal->tx_cur_desc++;
        if (hal->tx_cur_desc == hal->dmadesc_tx + hal->dma_desc_num) {
            hal->tx_cur_desc = hal->dmadesc_tx;
        }
    }

    return ESP_OK;
}

esp_err_t spi_slave_hd_hal_rxdma_append(spi_slave_hd_hal_context_t *hal, uint8_t *data, size_t len, void *arg)
{
    //Check if there are enough available dma descriptors for software to use
    int num_required = (len + LLDESC_MAX_NUM_PER_DESC - 1) / LLDESC_MAX_NUM_PER_DESC;
    int not_recycled_desc_num = hal->rx_used_desc_cnt - hal->rx_recycled_desc_cnt;
    int available_desc_num = hal->dma_desc_num - not_recycled_desc_num;
    if (num_required > available_desc_num) {
        return ESP_ERR_INVALID_STATE;
    }

    spi_slave_hd_hal_link_append_desc(hal->rx_cur_desc, data, len, false, arg);

    if (!hal->rx_dma_started) {
        hal->rx_dma_started = true;
        //start a link
        hal->rx_dma_tail = hal->rx_cur_desc;
        spi_dma_ll_rx_reset(hal->dma_in, hal->rx_dma_chan);
        spi_ll_dma_rx_fifo_reset(hal->dma_in);
        spi_ll_infifo_full_clr(hal->dev);
        spi_ll_dma_rx_enable(hal->dev, 1);
        spi_dma_ll_rx_start(hal->dma_in, hal->rx_dma_chan, &hal->rx_cur_desc->desc);
    } else {
        //there is already a consecutive link
        STAILQ_NEXT(&hal->rx_dma_tail->desc, qe) = &hal->rx_cur_desc->desc;
        hal->rx_dma_tail = hal->rx_cur_desc;
        spi_dma_ll_rx_restart(hal->dma_in, hal->rx_dma_chan);
    }

    //Move the current descriptor pointer according to the number of the linked descriptors
    for (int i = 0; i < num_required; i++) {
        hal->rx_used_desc_cnt++;
        hal->rx_cur_desc++;
        if (hal->rx_cur_desc == hal->dmadesc_rx + hal->dma_desc_num) {
            hal->rx_cur_desc = hal->dmadesc_rx;
        }
    }

    return ESP_OK;
}
