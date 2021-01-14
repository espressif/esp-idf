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

#include "soc/soc_caps.h"
#include "hal/cp_dma_hal.h"
#include "hal/cp_dma_ll.h"

void cp_dma_hal_init(cp_dma_hal_context_t *hal, const cp_dma_hal_config_t *config)
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
}

void cp_dma_hal_set_desc_base_addr(cp_dma_hal_context_t *hal, intptr_t outlink_base, intptr_t inlink_base)
{
    /* set base address of the first descriptor */
    cp_dma_ll_tx_set_descriptor_base_addr(hal->dev, outlink_base);
    cp_dma_ll_rx_set_descriptor_base_addr(hal->dev, inlink_base);
}

void cp_dma_hal_deinit(cp_dma_hal_context_t *hal)
{
    cp_dma_ll_enable_clock(hal->dev, false);
    hal->dev = NULL;
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

void cp_dma_hal_restart_tx(cp_dma_hal_context_t *hal)
{
    cp_dma_ll_restart_tx(hal->dev);
}

void cp_dma_hal_restart_rx(cp_dma_hal_context_t *hal)
{
    cp_dma_ll_restart_rx(hal->dev);
}
