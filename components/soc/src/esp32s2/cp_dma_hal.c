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

#include "hal/cp_dma_hal.h"
#include "hal/cp_dma_ll.h"
#include "soc/cp_dma_caps.h"

#define MCP_DESCRIPTOR_BUFFER_OWNER_DMA (1)
#define MCP_DESCRIPTOR_BUFFER_OWNER_CPU (0)

void cp_dma_hal_init(cp_dma_hal_context_t *hal, cp_dma_descriptor_t *tx_descriptors[], uint32_t tx_desc_num, cp_dma_descriptor_t *rx_descriptors[], uint32_t rx_desc_num)
{
    hal->dev = &CP_DMA;
    cp_dma_ll_enable_clock(hal->dev, true);
    cp_dma_ll_reset_in_link(hal->dev);
    cp_dma_ll_reset_out_link(hal->dev);
    cp_dma_ll_reset_cmd_fifo(hal->dev);
    cp_dma_ll_reset_fifo(hal->dev);
    cp_dma_ll_enable_intr(hal->dev, UINT32_MAX, false);
    cp_dma_ll_clear_intr_status(hal->dev, UINT32_MAX);
    cp_dma_ll_enable_owner_check(hal->dev, true);

    // circle TX descriptors
    for (int i = 0; i < tx_desc_num; i++) {
        tx_descriptors[i]->dw0.owner = MCP_DESCRIPTOR_BUFFER_OWNER_CPU;
        tx_descriptors[i]->next = tx_descriptors[i + 1];
    }
    tx_descriptors[tx_desc_num - 1]->next = tx_descriptors[0];

    // circle RX descriptors
    for (int i = 0; i < rx_desc_num; i++) {
        rx_descriptors[i]->dw0.owner = MCP_DESCRIPTOR_BUFFER_OWNER_CPU;
        rx_descriptors[i]->next = rx_descriptors[i + 1];
    }
    rx_descriptors[rx_desc_num - 1]->next = rx_descriptors[0];

    // set the start of each descriptor chain
    hal->tx_desc = tx_descriptors[0];
    hal->rx_desc = rx_descriptors[0];

    /* set base address of the first descriptor */
    cp_dma_ll_tx_set_descriptor_base_addr(hal->dev, (uint32_t)hal->tx_desc);
    cp_dma_ll_rx_set_descriptor_base_addr(hal->dev, (uint32_t)hal->rx_desc);

    hal->next_rx_desc_to_check = rx_descriptors[0];
}

void cp_dma_hal_deinit(cp_dma_hal_context_t *hal)
{
    cp_dma_ll_enable_clock(hal->dev, false);
    hal->dev = NULL;
    hal->tx_desc = NULL;
    hal->rx_desc = NULL;
}

void cp_dma_hal_start(cp_dma_hal_context_t *hal)
{
    // enable DMA engine
    cp_dma_ll_start_rx(hal->dev, true);
    cp_dma_ll_start_tx(hal->dev, true);
    // enable RX EOF interrupt
    cp_dma_ll_enable_intr(hal->dev, CP_DMA_LL_EVENT_RX_EOF, true);
}

void cp_dma_hal_stop(cp_dma_hal_context_t *hal)
{
    // disable interrupt
    cp_dma_ll_enable_intr(hal->dev, CP_DMA_LL_EVENT_RX_EOF, false);
    cp_dma_ll_enable_intr(hal->dev, CP_DMA_LL_EVENT_TX_EOF, false);

    // disable DMA
    cp_dma_ll_start_rx(hal->dev, false);
    cp_dma_ll_start_tx(hal->dev, false);
}

uint32_t cp_dma_hal_get_intr_status(cp_dma_hal_context_t *hal)
{
    return cp_dma_ll_get_intr_status(hal->dev);
}

void cp_dma_hal_clear_intr_status(cp_dma_hal_context_t *hal, uint32_t mask)
{
    cp_dma_ll_clear_intr_status(hal->dev, mask);
}

int cp_dma_hal_prepare_transmit(cp_dma_hal_context_t *hal, void *buffer, size_t len, cp_dma_descriptor_t **start_desc, cp_dma_descriptor_t **end_desc)
{
    uint32_t prepared_length = 0;
    uint8_t *buf = (uint8_t *)buffer;
    cp_dma_descriptor_t *desc = hal->tx_desc; // descriptor iterator
    cp_dma_descriptor_t *start = desc;
    cp_dma_descriptor_t *end = desc;

    while (len > SOC_CP_DMA_MAX_BUFFER_SIZE) {
        if (desc->dw0.owner != MCP_DESCRIPTOR_BUFFER_OWNER_DMA) {
            desc->dw0.eof = 0; // not the end of the transaction
            desc->dw0.size = SOC_CP_DMA_MAX_BUFFER_SIZE;
            desc->dw0.length = SOC_CP_DMA_MAX_BUFFER_SIZE;
            desc->buffer = &buf[prepared_length];
            desc = desc->next; // move to next descriptor
            prepared_length += SOC_CP_DMA_MAX_BUFFER_SIZE;
            len -= SOC_CP_DMA_MAX_BUFFER_SIZE;
        } else {
            // out of TX descriptors
            goto _exit;
        }
    }
    if (len) {
        if (desc->dw0.owner != MCP_DESCRIPTOR_BUFFER_OWNER_DMA) {
            end = desc; // the last descriptor used
            desc->dw0.eof = 1; // end of the transaction
            desc->dw0.size = len;
            desc->dw0.length = len;
            desc->buffer = &buf[prepared_length];
            desc = desc->next; // move to next descriptor
            prepared_length += len;
        } else {
            // out of TX descriptors
            goto _exit;
        }
    }

    *start_desc = start;
    *end_desc = end;
_exit:
    return prepared_length;
}

int cp_dma_hal_prepare_receive(cp_dma_hal_context_t *hal, void *buffer, size_t size, cp_dma_descriptor_t **start_desc, cp_dma_descriptor_t **end_desc)
{
    uint32_t prepared_length = 0;
    uint8_t *buf = (uint8_t *)buffer;
    cp_dma_descriptor_t *desc = hal->rx_desc; // descriptor iterator
    cp_dma_descriptor_t *start = desc;
    cp_dma_descriptor_t *end = desc;

    while (size > SOC_CP_DMA_MAX_BUFFER_SIZE) {
        if (desc->dw0.owner != MCP_DESCRIPTOR_BUFFER_OWNER_DMA) {
            desc->dw0.size = SOC_CP_DMA_MAX_BUFFER_SIZE;
            desc->buffer = &buf[prepared_length];
            desc = desc->next; // move to next descriptor
            prepared_length += SOC_CP_DMA_MAX_BUFFER_SIZE;
            size -= SOC_CP_DMA_MAX_BUFFER_SIZE;
        } else {
            // out of TX descriptors
            goto _exit;
        }
    }
    if (size) {
        if (desc->dw0.owner != MCP_DESCRIPTOR_BUFFER_OWNER_DMA) {
            end = desc; // the last descriptor used
            desc->dw0.size = size;
            desc->buffer = &buf[prepared_length];
            desc = desc->next; // move to next descriptor
            prepared_length += size;
        } else {
            // out of TX descriptors
            goto _exit;
        }
    }

    *start_desc = start;
    *end_desc = end;
_exit:
    return prepared_length;
}

void cp_dma_hal_restart_tx(cp_dma_hal_context_t *hal, cp_dma_descriptor_t *start_desc, cp_dma_descriptor_t *end_desc)
{
    // Give descriptor owner to DMA
    cp_dma_descriptor_t *desc = start_desc;
    while (desc != end_desc) {
        desc->dw0.owner = MCP_DESCRIPTOR_BUFFER_OWNER_DMA;
        desc = desc->next;
    }
    desc->dw0.owner = MCP_DESCRIPTOR_BUFFER_OWNER_DMA;
    hal->tx_desc = end_desc->next; // update the next available descriptor in HAL
    cp_dma_ll_restart_tx(hal->dev);
}

void cp_dma_hal_restart_rx(cp_dma_hal_context_t *hal, cp_dma_descriptor_t *start_desc, cp_dma_descriptor_t *end_desc)
{
    // Give descriptor owner to DMA
    cp_dma_descriptor_t *desc = start_desc;
    while (desc != end_desc) {
        desc->dw0.owner = MCP_DESCRIPTOR_BUFFER_OWNER_DMA;
        desc = desc->next;
    }
    desc->dw0.owner = MCP_DESCRIPTOR_BUFFER_OWNER_DMA;
    hal->rx_desc = end_desc->next; // update the next available descriptor in HAL
    cp_dma_ll_restart_rx(hal->dev);
}

bool cp_dma_hal_get_next_rx_descriptor(cp_dma_hal_context_t *hal, cp_dma_descriptor_t *eof_desc, cp_dma_descriptor_t **next_desc)
{
    cp_dma_descriptor_t *next = hal->next_rx_desc_to_check;
    // additional check, to avoid potential interrupt got triggered by mistake
    if (next->dw0.owner == MCP_DESCRIPTOR_BUFFER_OWNER_CPU) {
        hal->next_rx_desc_to_check = hal->next_rx_desc_to_check->next;
        *next_desc = next;
        // return if we need to continue
        return eof_desc == next ? false : true;
    } else {
        *next_desc = NULL;
        return false;
    }
}
