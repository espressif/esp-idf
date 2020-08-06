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
#include <sys/cdefs.h>
#include <stdatomic.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_compiler.h"
#include "esp_intr_alloc.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "soc/soc_caps.h"
#include "soc/cp_dma_caps.h"
#include "hal/cp_dma_hal.h"
#include "hal/cp_dma_ll.h"
#include "cp_dma.h"
#include "soc/periph_defs.h"

static const char *TAG = "cp_dma";

#define CP_DMA_CHECK(a, msg, tag, ret, ...)                                       \
    do {                                                                          \
        if (unlikely(!(a))) {                                                     \
            ESP_LOGE(TAG, "%s(%d): " msg, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            ret_code = ret;                                                       \
            goto tag;                                                             \
        }                                                                         \
    } while (0)

/**
 * @brief Stream is high level abstraction over descriptor.
 *        It combines the descriptor used by DMA and the callback function registered by user.
 *        The benifit is, we can converter the descriptor address into stream handle.
 */
typedef struct {
    cp_dma_descriptor_t tx_desc;
    cp_dma_isr_cb_t cb;
    void *cb_args;
} cp_dma_out_stream_t;

typedef struct {
    cp_dma_descriptor_t rx_desc;
    cp_dma_isr_cb_t cb;
    void *cb_args;
} cp_dma_in_stream_t;

typedef struct cp_dma_driver_context_s {
    uint32_t max_out_stream;
    uint32_t max_in_stream;
    uint32_t flags;
    cp_dma_hal_context_t hal; // HAL context
    intr_handle_t intr_hdl; // interrupt handle
    portMUX_TYPE spin_lock;
    cp_dma_out_stream_t *out_streams; // pointer to the first out stream
    cp_dma_in_stream_t *in_streams; // pointer to the first in stream
    uint8_t streams[0]; // stream buffer (out streams + in streams), the size if configured by user
} cp_dma_driver_context_t;

static void cp_dma_isr_default_handler(void *arg) IRAM_ATTR;

esp_err_t cp_dma_driver_install(const cp_dma_config_t *config, cp_dma_driver_t *drv_hdl)
{
    esp_err_t ret_code = ESP_OK;
    cp_dma_driver_context_t *cp_dma_driver = NULL;

    CP_DMA_CHECK(config, "configuration can't be null", err, ESP_ERR_INVALID_ARG);
    CP_DMA_CHECK(drv_hdl, "driver handle can't be null", err, ESP_ERR_INVALID_ARG);

    size_t total_malloc_size = sizeof(cp_dma_driver_context_t) + sizeof(cp_dma_out_stream_t) * config->max_out_stream + sizeof(cp_dma_in_stream_t) * config->max_in_stream;
    if (config->flags & CP_DMA_FLAGS_WORK_WITH_CACHE_DISABLED) {
        // to work when cache is disabled, make sure to put driver handle in DRAM
        cp_dma_driver = heap_caps_calloc(1, total_malloc_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    } else {
        cp_dma_driver = calloc(1, total_malloc_size);
    }
    CP_DMA_CHECK(cp_dma_driver, "allocate driver memory failed", err, ESP_ERR_NO_MEM);

    int int_flags = 0;
    if (config->flags & CP_DMA_FLAGS_WORK_WITH_CACHE_DISABLED) {
        int_flags |= ESP_INTR_FLAG_IRAM; // make sure interrupt can still work when cache is disabled
    }
    ret_code = esp_intr_alloc(ETS_DMA_COPY_INTR_SOURCE, int_flags, cp_dma_isr_default_handler, cp_dma_driver, &cp_dma_driver->intr_hdl);
    CP_DMA_CHECK(ret_code == ESP_OK, "allocate intr failed", err, ret_code);

    cp_dma_driver->out_streams = (cp_dma_out_stream_t *)cp_dma_driver->streams;
    cp_dma_driver->in_streams = (cp_dma_in_stream_t *)(cp_dma_driver->streams + config->max_out_stream * sizeof(cp_dma_out_stream_t));
    // HAL layer has no idea about "data stream" but TX/RX descriptors
    // We put all descritprs' addresses into an array, HAL driver will make it a loop during initialization
    {
        cp_dma_descriptor_t *tx_descriptors[config->max_out_stream];
        cp_dma_descriptor_t *rx_descriptors[config->max_in_stream];
        for (int i = 0; i < config->max_out_stream; i++) {
            tx_descriptors[i] = &cp_dma_driver->out_streams[i].tx_desc;
        }
        for (int i = 0; i < config->max_in_stream; i++) {
            rx_descriptors[i] = &cp_dma_driver->in_streams[i].rx_desc;
        }
        cp_dma_hal_init(&cp_dma_driver->hal, tx_descriptors, config->max_out_stream, rx_descriptors, config->max_in_stream);
    } // limit the scope of tx_descriptors and rx_descriptors so that goto can jump after this code block

    cp_dma_driver->spin_lock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    cp_dma_driver->max_in_stream = config->max_in_stream;
    cp_dma_driver->max_out_stream = config->max_out_stream;
    *drv_hdl = cp_dma_driver;

    cp_dma_hal_start(&cp_dma_driver->hal); // enable DMA and interrupt

    return ESP_OK;
err:
    if (cp_dma_driver) {
        if (cp_dma_driver->intr_hdl) {
            esp_intr_free(cp_dma_driver->intr_hdl);
        }
        free(cp_dma_driver);
    }
    if (drv_hdl) {
        *drv_hdl = NULL;
    }
    return ret_code;
}

esp_err_t cp_dma_driver_uninstall(cp_dma_driver_t drv_hdl)
{
    esp_err_t ret_code = ESP_OK;
    CP_DMA_CHECK(drv_hdl, "driver handle can't be null", err, ESP_ERR_INVALID_ARG);

    esp_intr_free(drv_hdl->intr_hdl);
    cp_dma_hal_stop(&drv_hdl->hal);
    cp_dma_hal_deinit(&drv_hdl->hal);
    free(drv_hdl);
    return ESP_OK;
err:
    return ret_code;
}

esp_err_t cp_dma_memcpy(cp_dma_driver_t drv_hdl, void *dst, void *src, size_t n, cp_dma_isr_cb_t cb_isr, void *cb_args)
{
    esp_err_t ret_code = ESP_OK;
    cp_dma_descriptor_t *rx_start_desc = NULL;
    cp_dma_descriptor_t *rx_end_desc = NULL;
    cp_dma_descriptor_t *tx_start_desc = NULL;
    cp_dma_descriptor_t *tx_end_desc = NULL;
    int rx_prepared_size = 0;
    int tx_prepared_size = 0;
    CP_DMA_CHECK(drv_hdl, "driver handle can't be null", err, ESP_ERR_INVALID_ARG);
    // CP_DMA can only access SRAM
    CP_DMA_CHECK(esp_ptr_internal(src) && esp_ptr_internal(dst), "address not in SRAM", err, ESP_ERR_INVALID_ARG);
    CP_DMA_CHECK(n <= SOC_CP_DMA_MAX_BUFFER_SIZE * drv_hdl->max_out_stream, "exceed max num of tx stream", err, ESP_ERR_INVALID_ARG);
    CP_DMA_CHECK(n <= SOC_CP_DMA_MAX_BUFFER_SIZE * drv_hdl->max_in_stream, "exceed max num of rx stream", err, ESP_ERR_INVALID_ARG);
    if (cb_isr && (drv_hdl->flags & CP_DMA_FLAGS_WORK_WITH_CACHE_DISABLED)) {
        CP_DMA_CHECK(esp_ptr_in_iram(cb_isr), "callback(%p) not in IRAM", err, ESP_ERR_INVALID_ARG, cb_isr);
    }

    // Prepare TX and RX descriptor
    portENTER_CRITICAL_SAFE(&drv_hdl->spin_lock);
    // prepare functions will not change internal status of HAL until cp_dma_hal_restart_* are called
    rx_prepared_size = cp_dma_hal_prepare_receive(&drv_hdl->hal, dst, n, &rx_start_desc, &rx_end_desc);
    tx_prepared_size = cp_dma_hal_prepare_transmit(&drv_hdl->hal, src, n, &tx_start_desc, &tx_end_desc);
    if ((rx_prepared_size == n) && (tx_prepared_size == n)) {
        // register user callback to the end-of-frame descriptor (must before we restart RX)
        cp_dma_in_stream_t *data_stream_rx = __containerof(rx_end_desc, cp_dma_in_stream_t, rx_desc);
        data_stream_rx->cb = cb_isr;
        data_stream_rx->cb_args = cb_args;
        // The restart should be called with the exact returned start and end desc from previous successful prepare calls
        cp_dma_hal_restart_rx(&drv_hdl->hal, rx_start_desc, rx_end_desc);
        cp_dma_hal_restart_tx(&drv_hdl->hal, tx_start_desc, tx_end_desc);
    }
    portEXIT_CRITICAL_SAFE(&drv_hdl->spin_lock);

    CP_DMA_CHECK(rx_prepared_size == n, "out of rx descriptor", err, ESP_FAIL);
    // It's unlikely that we have space for rx descriptor but no space for tx descriptor
    // Because in CP_DMA, both tx and rx descriptor should move in the same pace
    CP_DMA_CHECK(tx_prepared_size == n, "out of tx descriptor", err, ESP_FAIL);

    return ESP_OK;
err:
    return ret_code;
}

/**
 * @brief Default ISR handler provided by ESP-IDF
 */
static void cp_dma_isr_default_handler(void *args)
{
    cp_dma_driver_context_t *cp_dma_driver = (cp_dma_driver_context_t *)args;
    cp_dma_in_stream_t *in_stream = NULL;
    cp_dma_descriptor_t *next_desc = NULL;
    bool need_yield = false;
    bool to_continue = false;

    portENTER_CRITICAL_ISR(&cp_dma_driver->spin_lock);
    uint32_t status = cp_dma_hal_get_intr_status(&cp_dma_driver->hal);
    cp_dma_hal_clear_intr_status(&cp_dma_driver->hal, status);
    portEXIT_CRITICAL_ISR(&cp_dma_driver->spin_lock);
    ESP_EARLY_LOGD(TAG, "intr status=0x%x", status);

    // End-Of-Frame on RX side
    if (status & CP_DMA_LL_EVENT_RX_EOF) {
        cp_dma_descriptor_t *eof = (cp_dma_descriptor_t *)cp_dma_ll_get_rx_eof_descriptor_address(cp_dma_driver->hal.dev);
        // traversal all unchecked descriptors
        do {
            portENTER_CRITICAL_ISR(&cp_dma_driver->spin_lock);
            // There is an assumption that the usage of rx descriptors are in the same pace as tx descriptors (this is determined by CP DMA working mechanism)
            // And once the rx descriptor is recycled, the corresponding tx desc is guaranteed to be returned by DMA
            to_continue = cp_dma_hal_get_next_rx_descriptor(&cp_dma_driver->hal, eof, &next_desc);
            portEXIT_CRITICAL_ISR(&cp_dma_driver->spin_lock);
            if (next_desc) {
                in_stream = __containerof(next_desc, cp_dma_in_stream_t, rx_desc);
                // invoke user registered callback if available
                if (in_stream->cb) {
                    cp_dma_event_t e = {.id = CP_DMA_EVENT_M2M_DONE};
                    if (in_stream->cb(cp_dma_driver, &e, in_stream->cb_args)) {
                        need_yield = true;
                    }
                    in_stream->cb = NULL;
                    in_stream->cb_args = NULL;
                }
            }
        } while (to_continue);
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}
