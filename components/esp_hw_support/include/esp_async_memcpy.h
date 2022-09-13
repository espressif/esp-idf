/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_etm.h"

/**
 * @brief Type of async memcpy handle
 *
 */
typedef struct async_memcpy_context_t *async_memcpy_t;

/**
 * @brief Type of async memcpy event object
 *
 */
typedef struct {
    void *data; /*!< Event data */
} async_memcpy_event_t;

/**
 * @brief Type of async memcpy interrupt callback function
 *
 * @param mcp_hdl Handle of async memcpy
 * @param event Event object, which contains related data, reserved for future
 * @param cb_args User defined arguments, passed from esp_async_memcpy function
 * @return Whether a high priority task is woken up by the callback function
 *
 * @note User can call OS primitives (semaphore, mutex, etc) in the callback function.
 *       Keep in mind, if any OS primitive wakes high priority task up, the callback should return true.
 */
typedef bool (*async_memcpy_isr_cb_t)(async_memcpy_t mcp_hdl, async_memcpy_event_t *event, void *cb_args);

/**
 * @brief Type of async memcpy configuration
 *
 */
typedef struct {
    uint32_t backlog;          /*!< Maximum number of streams that can be handled simultaneously */
    size_t sram_trans_align;   /*!< DMA transfer alignment (both in size and address) for SRAM memory */
    size_t psram_trans_align;  /*!< DMA transfer alignment (both in size and address) for PSRAM memory */
    uint32_t flags;            /*!< Extra flags to control async memcpy feature */
} async_memcpy_config_t;

/**
 * @brief Default configuration for async memcpy
 *
 */
#define ASYNC_MEMCPY_DEFAULT_CONFIG() \
    {                                 \
        .backlog = 8,                 \
        .sram_trans_align = 0,        \
        .psram_trans_align = 0,       \
        .flags = 0,                   \
    }

/**
 * @brief Install async memcpy driver
 *
 * @param[in] config Configuration of async memcpy
 * @param[out] asmcp Handle of async memcpy that returned from this API. If driver installation is failed, asmcp would be assigned to NULL.
 * @return
 *      - ESP_OK: Install async memcpy driver successfully
 *      - ESP_ERR_INVALID_ARG: Install async memcpy driver failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Install async memcpy driver failed because out of memory
 *      - ESP_FAIL: Install async memcpy driver failed because of other error
 */
esp_err_t esp_async_memcpy_install(const async_memcpy_config_t *config, async_memcpy_t *asmcp);

/**
 * @brief Uninstall async memcpy driver
 *
 * @param[in] asmcp Handle of async memcpy driver that returned from esp_async_memcpy_install
 * @return
 *      - ESP_OK: Uninstall async memcpy driver successfully
 *      - ESP_ERR_INVALID_ARG: Uninstall async memcpy driver failed because of invalid argument
 *      - ESP_FAIL: Uninstall async memcpy driver failed because of other error
 */
esp_err_t esp_async_memcpy_uninstall(async_memcpy_t asmcp);

/**
 * @brief Send an asynchronous memory copy request
 *
 * @note The callback function is invoked in interrupt context, never do blocking jobs in the callback.
 *
 * @param[in] asmcp Handle of async memcpy driver that returned from esp_async_memcpy_install
 * @param[in] dst Destination address (copy to)
 * @param[in] src Source address (copy from)
 * @param[in] n Number of bytes to copy
 * @param[in] cb_isr Callback function, which got invoked in interrupt context. Set to NULL can bypass the callback.
 * @param[in] cb_args User defined argument to be passed to the callback function
 * @return
 *      - ESP_OK: Send memory copy request successfully
 *      - ESP_ERR_INVALID_ARG: Send memory copy request failed because of invalid argument
 *      - ESP_FAIL: Send memory copy request failed because of other error
 */
esp_err_t esp_async_memcpy(async_memcpy_t asmcp, void *dst, void *src, size_t n, async_memcpy_isr_cb_t cb_isr, void *cb_args);

/**
 * @brief Async memory copy specific events that supported by the ETM module
 */
typedef enum {
    ASYNC_MEMCPY_ETM_EVENT_COPY_DONE, /*!< memory copy finished */
} async_memcpy_etm_event_t;

/**
 * @brief Get the ETM event handle for async memcpy done signal
 *
 * @note The created ETM event object can be deleted later by calling `esp_etm_del_event`
 *
 * @param[in] asmcp Handle of async memcpy driver that returned from `esp_async_memcpy_install`
 * @param[in] event_type ETM event type
 * @param[out] out_event Returned ETM event handle
 * @return
 * @return
 *      - ESP_OK: Get ETM event successfully
 *      - ESP_ERR_INVALID_ARG: Get ETM event failed because of invalid argument
 *      - ESP_ERR_NOT_SUPPORTED: Get ETM event failed because the DMA hardware doesn't support ETM submodule
 *      - ESP_FAIL: Get ETM event failed because of other error
 */
esp_err_t esp_async_memcpy_new_etm_event(async_memcpy_t asmcp, async_memcpy_etm_event_t event_type, esp_etm_event_handle_t *out_event);

#ifdef __cplusplus
}
#endif
