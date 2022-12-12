/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "soc/periph_defs.h"
#include "soc/soc_memory_layout.h"
#include "hal/cp_dma_hal.h"
#include "hal/cp_dma_ll.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_etm.h"
#include "esp_async_memcpy_impl.h"

IRAM_ATTR static void async_memcpy_impl_default_isr_handler(void *args)
{
    async_memcpy_impl_t *mcp_impl = (async_memcpy_impl_t *)args;

    portENTER_CRITICAL_ISR(&mcp_impl->hal_lock);
    uint32_t status = cp_dma_hal_get_intr_status(&mcp_impl->hal);
    cp_dma_hal_clear_intr_status(&mcp_impl->hal, status);
    portEXIT_CRITICAL_ISR(&mcp_impl->hal_lock);

    // End-Of-Frame on RX side
    if (status & CP_DMA_LL_EVENT_RX_EOF) {
        mcp_impl->rx_eof_addr = cp_dma_ll_get_rx_eof_descriptor_address(mcp_impl->hal.dev);
        async_memcpy_isr_on_rx_done_event(mcp_impl);
    }

    if (mcp_impl->isr_need_yield) {
        mcp_impl->isr_need_yield = false;
        portYIELD_FROM_ISR();
    }
}

esp_err_t async_memcpy_impl_init(async_memcpy_impl_t *impl)
{
    esp_err_t ret = ESP_OK;

    impl->hal_lock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    cp_dma_hal_config_t config = {};
    cp_dma_hal_init(&impl->hal, &config);

    ret = esp_intr_alloc(ETS_DMA_COPY_INTR_SOURCE, ESP_INTR_FLAG_IRAM, async_memcpy_impl_default_isr_handler, impl, &impl->intr);
    return ret;
}

esp_err_t async_memcpy_impl_deinit(async_memcpy_impl_t *impl)
{
    esp_err_t ret = ESP_OK;

    cp_dma_hal_deinit(&impl->hal);
    ret = esp_intr_free(impl->intr);
    return ret;
}

esp_err_t async_memcpy_impl_start(async_memcpy_impl_t *impl, intptr_t outlink_base, intptr_t inlink_base)
{
    cp_dma_hal_set_desc_base_addr(&impl->hal, outlink_base, inlink_base);
    cp_dma_hal_start(&impl->hal); // enable DMA and interrupt
    return ESP_OK;
}

esp_err_t async_memcpy_impl_stop(async_memcpy_impl_t *impl)
{
    cp_dma_hal_stop(&impl->hal); // disable DMA and interrupt
    return ESP_OK;
}

esp_err_t async_memcpy_impl_restart(async_memcpy_impl_t *impl)
{
    cp_dma_hal_restart_rx(&impl->hal);
    cp_dma_hal_restart_tx(&impl->hal);
    return ESP_OK;
}

esp_err_t async_memcpy_impl_new_etm_event(async_memcpy_impl_t *impl, async_memcpy_etm_event_t event_type, esp_etm_event_handle_t *out_event)
{
    (void)impl;
    (void)event_type;
    (void)out_event;
    return ESP_ERR_NOT_SUPPORTED;
}

bool async_memcpy_impl_is_buffer_address_valid(async_memcpy_impl_t *impl, void *src, void *dst)
{
    // CP_DMA can only access SRAM
    return esp_ptr_internal(src) && esp_ptr_internal(dst);
}
