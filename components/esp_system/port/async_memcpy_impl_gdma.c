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

#include "freertos/FreeRTOS.h"
#include "soc/periph_defs.h"
#include "soc/soc_memory_layout.h"
#include "soc/soc_caps.h"
#include "hal/gdma_ll.h"
#include "hal/gdma_hal.h"
#include "driver/periph_ctrl.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_async_memcpy_impl.h"

IRAM_ATTR static void async_memcpy_impl_default_isr_handler(void *args)
{
    async_memcpy_impl_t *mcp_impl = (async_memcpy_impl_t *)args;

    portENTER_CRITICAL_ISR(&mcp_impl->hal_lock);
    uint32_t status = gdma_ll_get_interrupt_status(mcp_impl->hal.dev, SOC_GDMA_M2M_DMA_CHANNEL);
    gdma_ll_clear_interrupt_status(mcp_impl->hal.dev, SOC_GDMA_M2M_DMA_CHANNEL, status);
    portEXIT_CRITICAL_ISR(&mcp_impl->hal_lock);

    // End-Of-Frame on RX side
    if (status & GDMA_LL_EVENT_RX_SUC_EOF) {
        async_memcpy_isr_on_rx_done_event(mcp_impl);
    }

    if (mcp_impl->isr_need_yield) {
        mcp_impl->isr_need_yield = false;
        portYIELD_FROM_ISR();
    }
}

esp_err_t async_memcpy_impl_allocate_intr(async_memcpy_impl_t *impl, int int_flags, intr_handle_t *intr)
{
    return esp_intr_alloc(ETS_DMA_CH0_INTR_SOURCE, int_flags, async_memcpy_impl_default_isr_handler, impl, intr);
}

esp_err_t async_memcpy_impl_init(async_memcpy_impl_t *impl, dma_descriptor_t *outlink_base, dma_descriptor_t *inlink_base)
{
    impl->hal_lock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    impl->hal.dev = &GDMA;
    periph_module_enable(PERIPH_GDMA_MODULE);
    gdma_ll_enable_clock(impl->hal.dev, true);
    gdma_ll_tx_reset_channel(impl->hal.dev, SOC_GDMA_M2M_DMA_CHANNEL);
    gdma_ll_rx_reset_channel(impl->hal.dev, SOC_GDMA_M2M_DMA_CHANNEL);
    gdma_ll_enable_interrupt(impl->hal.dev, SOC_GDMA_M2M_DMA_CHANNEL, UINT32_MAX, true);
    gdma_ll_clear_interrupt_status(impl->hal.dev, SOC_GDMA_M2M_DMA_CHANNEL, UINT32_MAX);
    gdma_ll_enable_m2m_mode(impl->hal.dev, SOC_GDMA_M2M_DMA_CHANNEL, true);
    gdma_ll_tx_enable_auto_write_back(impl->hal.dev, SOC_GDMA_M2M_DMA_CHANNEL, true);
    gdma_ll_tx_enable_owner_check(impl->hal.dev, SOC_GDMA_M2M_DMA_CHANNEL, true);
    gdma_ll_rx_enable_owner_check(impl->hal.dev, SOC_GDMA_M2M_DMA_CHANNEL, true);
    gdma_ll_tx_set_desc_addr(impl->hal.dev, SOC_GDMA_M2M_DMA_CHANNEL, (uint32_t)outlink_base);
    gdma_ll_rx_set_desc_addr(impl->hal.dev, SOC_GDMA_M2M_DMA_CHANNEL, (uint32_t)inlink_base);
    return ESP_OK;
}

esp_err_t async_memcpy_impl_deinit(async_memcpy_impl_t *impl)
{
    periph_module_disable(PERIPH_GDMA_MODULE);
    return ESP_OK;
}

esp_err_t async_memcpy_impl_start(async_memcpy_impl_t *impl)
{
    gdma_ll_rx_start(impl->hal.dev, SOC_GDMA_M2M_DMA_CHANNEL);
    gdma_ll_tx_start(impl->hal.dev, SOC_GDMA_M2M_DMA_CHANNEL);
    gdma_ll_enable_interrupt(impl->hal.dev, SOC_GDMA_M2M_DMA_CHANNEL, GDMA_LL_EVENT_RX_SUC_EOF, true);
    return ESP_OK;
}

esp_err_t async_memcpy_impl_stop(async_memcpy_impl_t *impl)
{
    gdma_ll_enable_interrupt(impl->hal.dev, SOC_GDMA_M2M_DMA_CHANNEL, GDMA_LL_EVENT_RX_SUC_EOF, false);
    gdma_ll_rx_stop(impl->hal.dev, SOC_GDMA_M2M_DMA_CHANNEL);
    gdma_ll_tx_stop(impl->hal.dev, SOC_GDMA_M2M_DMA_CHANNEL);
    return ESP_OK;
}

esp_err_t async_memcpy_impl_restart(async_memcpy_impl_t *impl)
{
    gdma_ll_rx_restart(impl->hal.dev, SOC_GDMA_M2M_DMA_CHANNEL);
    gdma_ll_tx_restart(impl->hal.dev, SOC_GDMA_M2M_DMA_CHANNEL);
    return ESP_OK;
}

bool async_memcpy_impl_is_buffer_address_valid(async_memcpy_impl_t *impl, void *src, void *dst)
{
    return true;
}

dma_descriptor_t *async_memcpy_impl_get_rx_suc_eof_descriptor(async_memcpy_impl_t *impl)
{
    return (dma_descriptor_t *)gdma_ll_rx_get_success_eof_desc_addr(impl->hal.dev, SOC_GDMA_M2M_DMA_CHANNEL);
}
