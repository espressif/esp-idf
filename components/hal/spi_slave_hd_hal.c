/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
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
#include "soc/ext_mem_defs.h"
#include "hal/spi_slave_hd_hal.h"
#include "hal/assert.h"

//This GDMA related part will be introduced by GDMA dedicated APIs in the future. Here we temporarily use macros.
#if SOC_GDMA_SUPPORTED
#if (SOC_GDMA_TRIG_PERIPH_SPI2_BUS == SOC_GDMA_BUS_AHB) && (SOC_AHB_GDMA_VERSION == 1)
#include "soc/gdma_struct.h"
#include "hal/gdma_ll.h"
#define spi_dma_ll_tx_restart(dev, chan)                           gdma_ll_tx_restart(&GDMA, chan)
#define spi_dma_ll_rx_restart(dev, chan)                           gdma_ll_rx_restart(&GDMA, chan)
#define spi_dma_ll_rx_reset(dev, chan)                             gdma_ll_rx_reset_channel(&GDMA, chan)
#define spi_dma_ll_tx_reset(dev, chan)                             gdma_ll_tx_reset_channel(&GDMA, chan)
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

#elif (SOC_GDMA_TRIG_PERIPH_SPI2_BUS == SOC_GDMA_BUS_AXI)
#include "hal/axi_dma_ll.h"
#define spi_dma_ll_tx_restart(dev, chan)                           axi_dma_ll_tx_restart(&AXI_DMA, chan)
#define spi_dma_ll_rx_restart(dev, chan)                           axi_dma_ll_rx_restart(&AXI_DMA, chan)
#define spi_dma_ll_rx_reset(dev, chan)                             axi_dma_ll_rx_reset_channel(&AXI_DMA, chan)
#define spi_dma_ll_tx_reset(dev, chan)                             axi_dma_ll_tx_reset_channel(&AXI_DMA, chan)
#define spi_dma_ll_enable_out_auto_wrback(dev, chan, enable)       axi_dma_ll_tx_enable_auto_write_back(&AXI_DMA, chan, enable)
#define spi_dma_ll_set_out_eof_generation(dev, chan, enable)       axi_dma_ll_tx_set_eof_mode(&AXI_DMA, chan, enable)
#define spi_dma_ll_get_out_eof_desc_addr(dev, chan)                axi_dma_ll_tx_get_eof_desc_addr(&AXI_DMA, chan)
#define spi_dma_ll_get_in_suc_eof_desc_addr(dev, chan)             axi_dma_ll_rx_get_success_eof_desc_addr(&AXI_DMA, chan)
#define spi_dma_ll_rx_start(dev, chan, addr) do {\
            axi_dma_ll_rx_set_desc_addr(&AXI_DMA, chan, (uint32_t)addr);\
            axi_dma_ll_rx_start(&AXI_DMA, chan);\
        } while (0)
#define spi_dma_ll_tx_start(dev, chan, addr) do {\
            axi_dma_ll_tx_set_desc_addr(&AXI_DMA, chan, (uint32_t)addr);\
            axi_dma_ll_tx_start(&AXI_DMA, chan);\
        } while (0)
#endif
#endif  //SOC_GDMA_SUPPORTED

static void s_spi_slave_hd_hal_dma_init_config(const spi_slave_hd_hal_context_t *hal)
{
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
    hal->tx_cur_desc = hal->dmadesc_tx;
    hal->rx_cur_desc = hal->dmadesc_rx;
    hal->tx_dma_head = hal->dmadesc_tx + hal->dma_desc_num -1;
    hal->rx_dma_head = hal->dmadesc_rx + hal->dma_desc_num -1;

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

#if SOC_NON_CACHEABLE_OFFSET
#define ADDR_DMA_2_CPU(addr)   ((typeof(addr))((uint32_t)(addr) + 0x40000000))
#define ADDR_CPU_2_DMA(addr)   ((typeof(addr))((uint32_t)(addr) - 0x40000000))
#else
#define ADDR_DMA_2_CPU(addr)   (addr)
#define ADDR_CPU_2_DMA(addr)   (addr)
#endif

static void s_spi_hal_dma_desc_setup_link(spi_dma_desc_t *dmadesc, const void *data, int len, bool is_rx)
{
    dmadesc = ADDR_DMA_2_CPU(dmadesc);
    int n = 0;
    while (len) {
        int dmachunklen = len;
        if (dmachunklen > DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED) {
            dmachunklen = DMA_DESCRIPTOR_BUFFER_MAX_SIZE_4B_ALIGNED;
        }
        if (is_rx) {
            //Receive needs DMA length rounded to next 32-bit boundary
            dmadesc[n].dw0.size = (dmachunklen + 3) & (~3);
            dmadesc[n].dw0.length = (dmachunklen + 3) & (~3);
        } else {
            dmadesc[n].dw0.size = dmachunklen;
            dmadesc[n].dw0.length = dmachunklen;
        }
        dmadesc[n].buffer = (uint8_t *)data;
        dmadesc[n].dw0.suc_eof = 0;
        dmadesc[n].dw0.owner = 1;
        dmadesc[n].next = ADDR_CPU_2_DMA(&dmadesc[n + 1]);
        len -= dmachunklen;
        data += dmachunklen;
        n++;
    }
    dmadesc[n - 1].dw0.suc_eof = 1; //Mark last DMA desc as end of stream.
    dmadesc[n - 1].next = NULL;
}

static int s_desc_get_received_len_addr(spi_dma_desc_t* head, spi_dma_desc_t** out_next, void **out_buff_head)
{
    spi_dma_desc_t* desc_cpu = ADDR_DMA_2_CPU(head);
    int len = 0;
    if (out_buff_head) {
        *out_buff_head = desc_cpu->buffer;
    }
    while(head) {
        len += desc_cpu->dw0.length;
        bool eof = desc_cpu->dw0.suc_eof;
        desc_cpu = ADDR_DMA_2_CPU(desc_cpu->next);
        head = head->next;
        if (eof) break;
    }
    if (out_next) {
        *out_next = head;
    }
    return len;
}

void spi_slave_hd_hal_rxdma(spi_slave_hd_hal_context_t *hal, uint8_t *out_buf, size_t len)
{
    s_spi_hal_dma_desc_setup_link(hal->dmadesc_rx->desc, out_buf, len, true);

    spi_ll_dma_rx_fifo_reset(hal->dev);
    spi_dma_ll_rx_reset(hal->dma_in, hal->rx_dma_chan);
    spi_ll_slave_reset(hal->dev);
    spi_ll_infifo_full_clr(hal->dev);
    spi_ll_clear_intr(hal->dev, SPI_LL_INTR_CMD7);

    spi_ll_dma_rx_enable(hal->dev, 1);
    spi_dma_ll_rx_start(hal->dma_in, hal->rx_dma_chan, (lldesc_t *)hal->dmadesc_rx->desc);
}

void spi_slave_hd_hal_txdma(spi_slave_hd_hal_context_t *hal, uint8_t *data, size_t len)
{
    s_spi_hal_dma_desc_setup_link(hal->dmadesc_tx->desc, data, len, false);

    spi_ll_dma_tx_fifo_reset(hal->dev);
    spi_dma_ll_tx_reset(hal->dma_out, hal->tx_dma_chan);
    spi_ll_slave_reset(hal->dev);
    spi_ll_outfifo_empty_clr(hal->dev);
    spi_ll_clear_intr(hal->dev, SPI_LL_INTR_CMD8);

    spi_ll_dma_tx_enable(hal->dev, 1);
    spi_dma_ll_tx_start(hal->dma_out, hal->tx_dma_chan, (lldesc_t *)hal->dmadesc_tx->desc);
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
    spi_dma_desc_t *desc = hal->dmadesc_rx->desc;
    return s_desc_get_received_len_addr(desc, NULL, NULL);
}

bool spi_slave_hd_hal_get_tx_finished_trans(spi_slave_hd_hal_context_t *hal, void **out_trans, void **real_buff_addr)
{
    uint32_t desc_now = spi_dma_ll_get_out_eof_desc_addr(hal->dma_out, hal->tx_dma_chan);
    if ((uint32_t)hal->tx_dma_head->desc == desc_now) {
        return false;
    }

    //find used paired desc-trans by desc addr
    hal->tx_dma_head++;
    if (hal->tx_dma_head >= hal->dmadesc_tx + hal->dma_desc_num) {
        hal->tx_dma_head = hal->dmadesc_tx;
    }
    *out_trans = hal->tx_dma_head->arg;
    s_desc_get_received_len_addr(hal->tx_dma_head->desc, NULL, real_buff_addr);
    hal->tx_recycled_desc_cnt++;
    return true;
}

bool spi_slave_hd_hal_get_rx_finished_trans(spi_slave_hd_hal_context_t *hal, void **out_trans, void **real_buff_addr, size_t *out_len)
{
    uint32_t desc_now = spi_dma_ll_get_in_suc_eof_desc_addr(hal->dma_in, hal->rx_dma_chan);
    if ((uint32_t)hal->rx_dma_head->desc == desc_now) {
        return false;
    }

    //find used paired desc-trans by desc addr
    hal->rx_dma_head++;
    if (hal->rx_dma_head >= hal->dmadesc_rx + hal->dma_desc_num) {
        hal->rx_dma_head = hal->dmadesc_rx;
    }
    *out_trans = hal->rx_dma_head->arg;
    *out_len = s_desc_get_received_len_addr(hal->rx_dma_head->desc, NULL, real_buff_addr);
    hal->rx_recycled_desc_cnt++;
    return true;
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

    s_spi_hal_dma_desc_setup_link(hal->tx_cur_desc->desc, data, len, false);
    hal->tx_cur_desc->arg = arg;

    if (!hal->tx_dma_started) {
        hal->tx_dma_started = true;
        //start a link
        hal->tx_dma_tail = hal->tx_cur_desc;
        spi_ll_dma_tx_fifo_reset(hal->dma_out);
        spi_ll_outfifo_empty_clr(hal->dev);
        spi_dma_ll_tx_reset(hal->dma_out, hal->tx_dma_chan);
        spi_ll_dma_tx_enable(hal->dev, 1);
        spi_dma_ll_tx_start(hal->dma_out, hal->tx_dma_chan, (lldesc_t *)hal->tx_cur_desc->desc);
    } else {
        //there is already a consecutive link
        ADDR_DMA_2_CPU(hal->tx_dma_tail->desc)->next = hal->tx_cur_desc->desc;
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

    s_spi_hal_dma_desc_setup_link(hal->rx_cur_desc->desc, data, len, false);
    hal->rx_cur_desc->arg = arg;

    if (!hal->rx_dma_started) {
        hal->rx_dma_started = true;
        //start a link
        hal->rx_dma_tail = hal->rx_cur_desc;
        spi_dma_ll_rx_reset(hal->dma_in, hal->rx_dma_chan);
        spi_ll_dma_rx_fifo_reset(hal->dma_in);
        spi_ll_infifo_full_clr(hal->dev);
        spi_ll_dma_rx_enable(hal->dev, 1);
        spi_dma_ll_rx_start(hal->dma_in, hal->rx_dma_chan, (lldesc_t *)hal->rx_cur_desc->desc);
    } else {
        //there is already a consecutive link
        ADDR_DMA_2_CPU(hal->rx_dma_tail->desc)->next = hal->rx_cur_desc->desc;
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
