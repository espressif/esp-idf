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

#include <stddef.h>
#include "esp_err.h"

/**
 * @brief Handle of CP_DMA driver
 *
 */
typedef struct cp_dma_driver_context_s *cp_dma_driver_t;

/**
 * @brief CP_DMA event ID
 *
 */
typedef enum {
    CP_DMA_EVENT_M2M_DONE, /*!< One or more memory copy transactions are done */
} cp_dma_event_id_t;

/**
 * @brief Type defined for CP_DMA event object (including event ID, event data)
 *
 */
typedef struct {
    cp_dma_event_id_t id; /*!< Event ID */
    void *data;           /*!< Event data */
} cp_dma_event_t;

/**
 * @brief Type defined for cp_dma ISR callback function
 *
 * @param drv_hdl Handle of CP_DMA driver
 * @param event Event object, which contains the event ID, event data, and so on
 * @param cb_args User defined arguments for the callback function. It's passed in cp_dma_memcpy function
 * @return Whether a high priority task is woken up by the callback function
 *
 */
typedef bool (*cp_dma_isr_cb_t)(cp_dma_driver_t drv_hdl, cp_dma_event_t *event, void *cb_args);

/**
 * @brief Type defined for configuration of CP_DMA driver
 *
 */
typedef struct {
    uint32_t max_out_stream; /*!< maximum number of out link streams that can work simultaneously */
    uint32_t max_in_stream;  /*!< maximum number of in link streams that can work simultaneously */
    uint32_t flags;          /*!< Extra flags to control some special behaviour of CP_DMA, OR'ed of CP_DMA_FLAGS_xxx macros */
} cp_dma_config_t;

#define CP_DMA_FLAGS_WORK_WITH_CACHE_DISABLED (1 << 0) /*!< CP_DMA can work even when cache is diabled */

/**
 * @brief Default configuration for CP_DMA driver
 *
 */
#define CP_DMA_DEFAULT_CONFIG() \
    {                           \
        .max_out_stream = 8,    \
        .max_in_stream = 8,     \
        .flags = 0,             \
    }

/**
 * @brief Install CP_DMA driver
 *
 * @param[in] config Configuration of CP_DMA driver
 * @param[out] drv_hdl Returned handle of CP_DMA driver or NULL if driver installation failed
 * @return
 *      - ESP_OK: Install CP_DMA driver successfully
 *      - ESP_ERR_INVALID_ARG: Install CP_DMA driver failed because of some invalid argument
 *      - ESP_ERR_NO_MEM: Install CP_DMA driver failed because there's no enough capable memory
 *      - ESP_FAIL: Install CP_DMA driver failed because of other error
 */
esp_err_t cp_dma_driver_install(const cp_dma_config_t *config, cp_dma_driver_t *drv_hdl);

/**
 * @brief Uninstall CP_DMA driver
 *
 * @param[in] drv_hdl Handle of CP_DMA driver that returned from cp_dma_driver_install
 * @return
 *      - ESP_OK: Uninstall CP_DMA driver successfully
 *      - ESP_ERR_INVALID_ARG: Uninstall CP_DMA driver failed because of some invalid argument
 *      - ESP_FAIL: Uninstall CP_DMA driver failed because of other error
 */
esp_err_t cp_dma_driver_uninstall(cp_dma_driver_t drv_hdl);

/**
 * @brief Send an asynchronous memory copy request
 *
 * @param[in] drv_hdl Handle of CP_DMA driver that returned from cp_dma_driver_install
 * @param[in] dst Destination address (copy to)
 * @param[in] src Source address (copy from)
 * @param[in] n Number of bytes to copy
 * @param[in] cb_isr Callback function, which got invoked in ISR context. A NULL pointer here can bypass the callback.
 * @param[in] cb_args User defined argument to be passed to the callback function
 * @return
 *      - ESP_OK: Send memcopy request successfully
 *      - ESP_ERR_INVALID_ARG: Send memcopy request failed because of some invalid argument
 *      - ESP_FAIL: Send memcopy request failed because of other error
 *
 * @note The callback function is invoked in ISR context, please never handle heavy load in the callback.
 *       The default ISR handler is placed in IRAM, please place callback function in IRAM as well by applying IRAM_ATTR to it.
 */
esp_err_t cp_dma_memcpy(cp_dma_driver_t drv_hdl, void *dst, void *src, size_t n, cp_dma_isr_cb_t cb_isr, void *cb_args);

#ifdef __cplusplus
}
#endif
