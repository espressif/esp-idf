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

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "soc/soc_caps.h"
#include "hal/dma_types.h"
#include "freertos/FreeRTOS.h"

#if SOC_CP_DMA_SUPPORTED
#include "hal/cp_dma_ll.h"
#include "hal/cp_dma_hal.h"
#elif SOC_GDMA_SUPPORTED
#include "hal/gdma_ll.h"
#include "hal/gdma_hal.h"
#endif

/**
 * @brief Type of async mcp implementation layer context
 *
 */
typedef struct {
    portMUX_TYPE hal_lock;    // spin lock for HAL object
#if SOC_CP_DMA_SUPPORTED
    cp_dma_hal_context_t hal; // CP DMA hal
#elif SOC_GDMA_SUPPORTED
    gdma_hal_context_t hal;   // General DMA hal
#endif
    bool isr_need_yield;      // if current isr needs a yield for higher priority task
} async_memcpy_impl_t;

/**
 * @brief ISR callback function, invoked when RX done event triggered
 *
 * @param impl async mcp implementation layer context pointer
 */
void async_memcpy_isr_on_rx_done_event(async_memcpy_impl_t *impl);

/**
 * @brief Allocate interrupt handle, register default isr handler
 *
 * @param impl async mcp implementation layer context pointer
 * @param int_flags interrupt flags
 * @param intr Returned interrupt handle
 * @return
 *      - ESP_OK: Allocate interrupt handle successfully
 *      - ESP_ERR_INVALID_ARG: Allocate interrupt handle failed because of invalid argument
 *      - ESP_FAIL: Allocate interrupt handle failed because of other error
 */
esp_err_t async_memcpy_impl_allocate_intr(async_memcpy_impl_t *impl, int int_flags, intr_handle_t *intr);

/**
 * @brief Initialize async mcp implementation layer
 *
 * @param impl async mcp implementation layer context pointer
 * @param outlink_base Pointer to the first TX descriptor
 * @param inlink_base Pointer to the first RX descriptor
 * @return Always return ESP_OK
 */
esp_err_t async_memcpy_impl_init(async_memcpy_impl_t *impl, dma_descriptor_t *outlink_base, dma_descriptor_t *inlink_base);

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
 * @return Always return ESP_OK
 */
esp_err_t async_memcpy_impl_start(async_memcpy_impl_t *impl);

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
 * @brief check if buffer address is valid
 * @note This is related to underlying target (e.g. on esp32-s2, only buffer located in SRAM is supported)
 *
 * @param impl async mcp implementation layer context pointer
 * @param src Source buffer address
 * @param dst Destination buffer address
 * @return True if both address are valid
 */
bool async_memcpy_impl_is_buffer_address_valid(async_memcpy_impl_t *impl, void *src, void *dst);

/**
 * @brief Get the EOF RX descriptor address
 *
 * @param impl async mcp implementation layer context pointer
 * @return Pointer to the EOF RX descriptor
 */
dma_descriptor_t *async_memcpy_impl_get_rx_suc_eof_descriptor(async_memcpy_impl_t *impl);
