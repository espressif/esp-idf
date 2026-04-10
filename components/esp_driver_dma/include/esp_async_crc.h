/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Async CRC driver handle
 */
typedef struct async_crc_context_t *async_crc_handle_t;

/**
 * @brief Async CRC event data
 */
typedef struct {
    uint32_t crc_result;  /*!< CRC calculation result */
} async_crc_event_data_t;

/**
 * @brief Type of async CRC interrupt callback function
 *
 * @param crc_hdl Handle of async CRC
 * @param edata Event data object, which contains related data for this event
 * @param cb_args User defined arguments, passed from esp_async_crc_calc function
 * @return Whether a high priority task is woken up by the callback function
 *
 * @note User can call OS primitives (semaphore, mutex, etc) in the callback function.
 *       Keep in mind, if any OS primitive wakes high priority task up, the callback should return true.
 * @note This callback function is invoked in interrupt context (ISR). The following restrictions apply:
 *       - Do not perform blocking operations (e.g., vTaskDelay, xQueueSend with non-zero timeout)
 *       - Keep execution time minimal to avoid impacting system interrupt latency
 *       - Avoid calling non-ISR-safe FreeRTOS functions
 *       - Do not allocate memory or perform heavy computations
 *       - Use only ISR-safe APIs (xQueueSendFromISR, xSemaphoreGiveFromISR, etc.)
 */
typedef bool (*async_crc_isr_cb_t)(async_crc_handle_t crc_hdl, async_crc_event_data_t *edata, void *cb_args);

/**
 * @brief Type of async CRC configuration
 */
typedef struct {
    uint32_t backlog;         /*!< Maximum number of pending CRC requests that can be queued per driver instance.
                                   Higher values use more memory but provide better throughput for bursty workloads. */
    size_t dma_burst_size;    /*!< DMA transfer burst size, in bytes */
} async_crc_config_t;

#if SOC_HAS(AHB_GDMA)
/**
 * @brief Install async CRC driver, with AHB-GDMA as the backend
 *
 * @param[in] config Configuration of async CRC
 * @param[out] crc_hdl Returned driver handle
 * @return
 *      - ESP_OK: Install async CRC driver successfully
 *      - ESP_ERR_INVALID_ARG: Install async CRC driver failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Install async CRC driver failed because out of memory
 *      - ESP_FAIL: Install async CRC driver failed because of other error
 */
esp_err_t esp_async_crc_install_gdma_ahb(const async_crc_config_t *config, async_crc_handle_t *crc_hdl);
#endif // SOC_HAS(AHB_GDMA)

#if SOC_HAS(AXI_GDMA)
/**
 * @brief Install async CRC driver, with AXI-GDMA as the backend
 *
 * @param[in] config Configuration of async CRC
 * @param[out] crc_hdl Returned driver handle
 * @return
 *      - ESP_OK: Install async CRC driver successfully
 *      - ESP_ERR_INVALID_ARG: Install async CRC driver failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Install async CRC driver failed because out of memory
 *      - ESP_FAIL: Install async CRC driver failed because of other error
 */
esp_err_t esp_async_crc_install_gdma_axi(const async_crc_config_t *config, async_crc_handle_t *crc_hdl);
#endif // SOC_HAS(AXI_GDMA)

/**
 * @brief Uninstall async CRC driver
 *
 * @param[in] crc_hdl Handle of async CRC driver that returned from install functions
 * @return
 *      - ESP_OK: Uninstall async CRC driver successfully
 *      - ESP_ERR_INVALID_ARG: Uninstall async CRC driver failed because of invalid argument
 *      - ESP_FAIL: Uninstall async CRC driver failed because of other error
 */
esp_err_t esp_async_crc_uninstall(async_crc_handle_t crc_hdl);

/**
 * @brief CRC calculation parameters
 */
typedef struct {
    uint8_t width;            /*!< CRC bit width: 8, 16, or 32 bits */
    uint32_t polynomial;      /*!< CRC polynomial */
    uint32_t init_value;      /*!< Initial CRC value */
    uint32_t final_xor_value; /*!< Final XOR value */
    bool reverse_input;       /*!< Reverse data bits before processing */
    bool reverse_output;      /*!< Reverse final CRC result */
} async_crc_params_t;

/**
 * @brief Send an asynchronous CRC calculation request
 *
 * @note The callback function is invoked in interrupt context, never do blocking jobs in the callback.
 *
 * @param[in] crc_hdl Handle of async CRC driver that returned from install functions
 * @param[in] data Pointer to data buffer for CRC calculation
 * @param[in] size Size of data in bytes
 * @param[in] params CRC calculation parameters
 * @param[in] cb_isr Callback function, which got invoked in interrupt context. Set to NULL can bypass the callback.
 * @param[in] cb_args User defined argument to be passed to the callback function
 * @return
 *      - ESP_OK: Send CRC calculation request successfully
 *      - ESP_ERR_INVALID_ARG: Send CRC calculation request failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: CRC driver is not in proper state to accept new requests
 *      - ESP_FAIL: Send CRC calculation request failed because of other error
 */
esp_err_t esp_async_crc_calc(async_crc_handle_t crc_hdl, const void *data, size_t size,
                             const async_crc_params_t *params, async_crc_isr_cb_t cb_isr, void *cb_args);

/**
 * @brief Blocking CRC calculation function with timeout
 *
 * @note This function is blocking and should not be called from interrupt context.
 *
 * @param[in] crc_hdl Handle of async CRC driver that returned from install functions
 * @param[in] data Pointer to data buffer for CRC calculation
 * @param[in] size Size of data in bytes
 * @param[in] params CRC calculation parameters
 * @param[in] timeout_ms Timeout in milliseconds:
 *                       - `< 0`: Wait forever (no timeout)
 *                       - `0`: Return immediately (poll once)
 *                       - `> 0`: Wait up to specified milliseconds
 * @param[out] result Pointer to store CRC calculation result
 * @return
 *      - ESP_OK: Calculate CRC successfully
 *      - ESP_ERR_INVALID_ARG: Calculate CRC failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Function called from ISR context or driver in invalid state
 *      - ESP_ERR_TIMEOUT: Operation timed out
 *      - ESP_FAIL: Calculate CRC failed because of other error
 */
esp_err_t esp_crc_calc_blocking(async_crc_handle_t crc_hdl, const void *data, size_t size,
                                const async_crc_params_t *params, int32_t timeout_ms, uint32_t *result);

#ifdef __cplusplus
}
#endif
