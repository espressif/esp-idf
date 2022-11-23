/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "esp_etm.h"
#include "soc/soc_caps.h"
#include "hal/dma_types.h"
#include "freertos/FreeRTOS.h"
#include "esp_async_memcpy.h"

#if SOC_CP_DMA_SUPPORTED
#include "hal/cp_dma_ll.h"
#include "hal/cp_dma_hal.h"
#elif SOC_GDMA_SUPPORTED
#include "esp_private/gdma.h"
#endif


/**
 * @brief Type of async mcp implementation layer context
 *
 */
typedef struct {
#if SOC_CP_DMA_SUPPORTED
    cp_dma_hal_context_t hal; // CP DMA hal
    intr_handle_t intr; // CP DMA interrupt handle
    portMUX_TYPE hal_lock; // CP DMA HAL level spin lock
#elif SOC_GDMA_SUPPORTED
    gdma_channel_handle_t tx_channel;
    gdma_channel_handle_t rx_channel;
#endif
    intptr_t rx_eof_addr;
    size_t sram_trans_align;
    size_t psram_trans_align;
    bool isr_need_yield;      // if current isr needs a yield for higher priority task
} async_memcpy_impl_t;

/**
 * @brief ISR callback function, invoked when RX done event triggered
 *
 * @param impl async mcp implementation layer context pointer
 */
void async_memcpy_isr_on_rx_done_event(async_memcpy_impl_t *impl);

/**
 * @brief Initialize async mcp implementation layer
 *
 * @param impl async mcp implementation layer context pointer
 * @return Always return ESP_OK
 */
esp_err_t async_memcpy_impl_init(async_memcpy_impl_t *impl);

/**
 * @brief Deinitialize async mcp implementation layer
 *
 * @param impl async mcp implementation layer context pointer
 * @return Always return ESP_OK
 */
esp_err_t async_memcpy_impl_deinit(async_memcpy_impl_t *impl);

/**
 * @brief Start async mcp (on implementation layer)
 *
 * @param impl async mcp implementation layer context pointer
 * @param outlink_base base descriptor address for TX DMA channel
 * @param inlink_base base descriptor address for RX DMA channel
 * @return Always return ESP_OK
 */
esp_err_t async_memcpy_impl_start(async_memcpy_impl_t *impl, intptr_t outlink_base, intptr_t inlink_base);

/**
 * @brief Stop async mcp (on implementation layer)
 *
 * @param impl async mcp implementation layer context pointer
 * @return Always return ESP_OK
 */
esp_err_t async_memcpy_impl_stop(async_memcpy_impl_t *impl);

/**
 * @brief Restart async mcp DMA engine
 *
 * @param impl async mcp implementation layer context pointer
 * @return Always return ESP_OK
 */
esp_err_t async_memcpy_impl_restart(async_memcpy_impl_t *impl);

/**
 * @brief Get ETM Event handle
 *
 * @param impl async mcp implementation layer context pointer
 * @param event_type ETM event type
 * @param out_event Returned ETM event handle
 * @return ESP_OK on success, ESP_ERR_NOT_SUPPORTED if not supported in hardware, otherwise failed
 */
esp_err_t async_memcpy_impl_new_etm_event(async_memcpy_impl_t *impl, async_memcpy_etm_event_t event_type, esp_etm_event_handle_t *out_event);

/**
 * @brief check if buffer address is valid
 * @note This is related to underlying target (e.g. on esp32-s2, only buffer located in SRAM is supported)
 *
 * @param impl async mcp implementation layer context pointer
 * @param src Source buffer address
 * @param dst Destination buffer address
 * @return True if both address are valid
 */
bool async_memcpy_impl_is_buffer_address_valid(async_memcpy_impl_t *impl, void *src, void *dst);

#ifdef __cplusplus
}
#endif
