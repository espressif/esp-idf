// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "soc/cp_dma_struct.h"

#define CP_DMA_LL_EVENT_RX_DONE       (1 << 0)
#define CP_DMA_LL_EVENT_RX_EOF        (1 << 1)
#define CP_DMA_LL_EVENT_TX_DONE       (1 << 2)
#define CP_DMA_LL_EVENT_TX_EOF        (1 << 3)
#define CP_DMA_LL_EVENT_RX_DESC_ERR   (1 << 4)
#define CP_DMA_LL_EVENT_TX_DESC_ERR   (1 << 5)
#define CP_DMA_LL_EVENT_RX_DESC_EMPTY (1 << 6)
#define CP_DMA_LL_EVENT_TX_TOTAL_EOF  (1 << 7)
#define CP_DMA_LL_EVENT_ALL           (0xFF)

/**
 * Copy DMA firstly reads data to be transferred from internal RAM,
 * stores the data into DMA FIFO via an outlink,
 * and then writes the data to the destination internal RAM via an inlink.
 */

static inline void cp_dma_ll_reset_in_link(cp_dma_dev_t *dev)
{
    dev->dma_conf.dma_in_rst = 1;
    dev->dma_conf.dma_in_rst = 0;
}

static inline void cp_dma_ll_reset_out_link(cp_dma_dev_t *dev)
{
    dev->dma_conf.dma_out_rst = 1;
    dev->dma_conf.dma_out_rst = 0;
}

static inline void cp_dma_ll_reset_fifo(cp_dma_dev_t *dev)
{
    dev->dma_conf.dma_fifo_rst = 1;
    dev->dma_conf.dma_fifo_rst = 0;
}

static inline void cp_dma_ll_reset_cmd_fifo(cp_dma_dev_t *dev)
{
    dev->dma_conf.dma_cmdfifo_rst = 1;
    dev->dma_conf.dma_cmdfifo_rst = 0;
}

static inline void cp_dma_ll_enable_owner_check(cp_dma_dev_t *dev, bool enable)
{
    dev->dma_conf.dma_check_owner = enable;
    dev->dma_conf.dma_out_auto_wrback = 1;
    dev->dma_conf.dma_out_owner = 0;
    dev->dma_conf.dma_in_owner = 0;
}

static inline void cp_dma_ll_enable_clock(cp_dma_dev_t *dev, bool enable)
{
    dev->dma_conf.dma_clk_en = enable;
}

static inline void cp_dma_ll_enable_intr(cp_dma_dev_t *dev, uint32_t mask, bool enable)
{
    if (enable) {
        dev->dma_int_ena.val |= mask;
    } else {
        dev->dma_int_ena.val &= ~mask;
    }
}

static inline __attribute__((always_inline)) uint32_t cp_dma_ll_get_intr_status(cp_dma_dev_t *dev)
{
    return dev->dma_int_st.val;
}

static inline __attribute__((always_inline)) void cp_dma_ll_clear_intr_status(cp_dma_dev_t *dev, uint32_t mask)
{
    dev->dma_int_clr.val = mask;
}

static inline void cp_dma_ll_tx_set_descriptor_base_addr(cp_dma_dev_t *dev, uint32_t address)
{
    dev->dma_out_link.dma_outlink_addr = address;
}

static inline void cp_dma_ll_rx_set_descriptor_base_addr(cp_dma_dev_t *dev, uint32_t address)
{
    dev->dma_in_link.dma_inlink_addr = address;
}

static inline void cp_dma_ll_start_tx(cp_dma_dev_t *dev, bool enable)
{
    if (enable) {
        dev->dma_out_link.dma_outlink_start = 1; // cleared automatically by HW
    } else {
        dev->dma_out_link.dma_outlink_stop = 1; // cleared automatically by HW
    }
}

static inline void cp_dma_ll_start_rx(cp_dma_dev_t *dev, bool enable)
{
    if (enable) {
        dev->dma_in_link.dma_inlink_start = 1; // cleared automatically by HW
    } else {
        dev->dma_in_link.dma_inlink_stop = 1; // cleared automatically by HW
    }
}

static inline void cp_dma_ll_restart_tx(cp_dma_dev_t *dev)
{
    dev->dma_out_link.dma_outlink_restart = 1; // cleared automatically by HW
}

static inline void cp_dma_ll_restart_rx(cp_dma_dev_t *dev)
{
    dev->dma_in_link.dma_inlink_restart = 1; // cleared automatically by HW
}

// get the address of last rx descriptor
static inline uint32_t cp_dma_ll_get_rx_eof_descriptor_address(cp_dma_dev_t *dev)
{
    return dev->dma_in_eof_des_addr.dma_in_suc_eof_des_addr;
}

// get the address of last tx descriptor
static inline uint32_t cp_dma_ll_get_tx_eof_descriptor_address(cp_dma_dev_t *dev)
{
    return dev->dma_out_eof_des_addr.dma_out_eof_des_addr;
}

static inline uint32_t cp_dma_ll_get_tx_status(cp_dma_dev_t *dev)
{
    return dev->dma_out_st.val;
}

static inline uint32_t cp_dma_ll_get_rx_status(cp_dma_dev_t *dev)
{
    return dev->dma_in_st.val;
}

#ifdef __cplusplus
}
#endif
