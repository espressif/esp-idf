/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "driver/rmt_common.h"
#include "driver/rmt_encoder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Group of RMT TX callbacks
 * @note The callbacks are all running under ISR environment
 * @note When CONFIG_RMT_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       The variables used in the function should be in the SRAM as well.
 */
typedef struct {
    rmt_tx_done_callback_t on_trans_done; /*!< Event callback, invoked when transmission is finished */
} rmt_tx_event_callbacks_t;

/**
 * @brief RMT TX channel specific configuration
 */
typedef struct {
    gpio_num_t gpio_num;        /*!< GPIO number used by RMT TX channel. Set to -1 if unused */
    rmt_clock_source_t clk_src; /*!< Clock source of RMT TX channel, channels in the same group must use the same clock source */
    uint32_t resolution_hz;     /*!< Channel clock resolution, in Hz */
    size_t mem_block_symbols;   /*!< Size of memory block, in number of `rmt_symbol_word_t`, must be an even.
                                     In the DMA mode, this field controls the DMA buffer size, it can be set to a large value;
                                     In the normal mode, this field controls the number of RMT memory block that will be used by the channel. */
    size_t trans_queue_depth;   /*!< Depth of internal transfer queue, increase this value can support more transfers pending in the background */
    int intr_priority;          /*!< RMT interrupt priority,
                                     if set to 0, the driver will try to allocate an interrupt with a relative low priority (1,2,3) */
    struct {
        uint32_t invert_out: 1;   /*!< Whether to invert the RMT channel signal before output to GPIO pad */
        uint32_t with_dma: 1;     /*!< If set, the driver will allocate an RMT channel with DMA capability */
        uint32_t io_loop_back: 1; /*!< The signal output from the GPIO will be fed to the input path as well */
        uint32_t io_od_mode: 1;   /*!< Configure the GPIO as open-drain mode */
    } flags;                      /*!< TX channel config flags */
} rmt_tx_channel_config_t;

/**
 * @brief RMT transmit specific configuration
 */
typedef struct {
    int loop_count; /*!< Specify the times of transmission in a loop, -1 means transmitting in an infinite loop */
    struct {
        uint32_t eot_level : 1;         /*!< Set the output level for the "End Of Transmission" */
        uint32_t queue_nonblocking : 1; /*!< If set, when the transaction queue is full, driver will not block the thread but return directly */
    } flags;                            /*!< Transmit specific config flags */
} rmt_transmit_config_t;

/**
 * @brief Synchronous manager configuration
 */
typedef struct {
    const rmt_channel_handle_t *tx_channel_array; /*!< Array of TX channels that are about to be managed by a synchronous controller */
    size_t array_size;                            /*!< Size of the `tx_channel_array` */
} rmt_sync_manager_config_t;

/**
 * @brief Create a RMT TX channel
 *
 * @param[in] config TX channel configurations
 * @param[out] ret_chan Returned generic RMT channel handle
 * @return
 *      - ESP_OK: Create RMT TX channel successfully
 *      - ESP_ERR_INVALID_ARG: Create RMT TX channel failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create RMT TX channel failed because out of memory
 *      - ESP_ERR_NOT_FOUND: Create RMT TX channel failed because all RMT channels are used up and no more free one
 *      - ESP_ERR_NOT_SUPPORTED: Create RMT TX channel failed because some feature is not supported by hardware, e.g. DMA feature is not supported by hardware
 *      - ESP_FAIL: Create RMT TX channel failed because of other error
 */
esp_err_t rmt_new_tx_channel(const rmt_tx_channel_config_t *config, rmt_channel_handle_t *ret_chan);

/**
 * @brief Transmit data by RMT TX channel
 *
 * @note This function constructs a transaction descriptor then pushes to a queue.
 *       The transaction will not start immediately if there's another one under processing.
 *       Based on the setting of `rmt_transmit_config_t::queue_nonblocking`,
 *       if there're too many transactions pending in the queue, this function can block until it has free slot,
 *       otherwise just return quickly.
 * @note The data to be transmitted will be encoded into RMT symbols by the specific `encoder`.
 *
 * @param[in] tx_channel RMT TX channel that created by `rmt_new_tx_channel()`
 * @param[in] encoder RMT encoder that created by various factory APIs like `rmt_new_bytes_encoder()`
 * @param[in] payload The raw data to be encoded into RMT symbols
 * @param[in] payload_bytes Size of the `payload` in bytes
 * @param[in] config Transmission specific configuration
 * @return
 *      - ESP_OK: Transmit data successfully
 *      - ESP_ERR_INVALID_ARG: Transmit data failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Transmit data failed because channel is not enabled
 *      - ESP_ERR_NOT_SUPPORTED: Transmit data failed because some feature is not supported by hardware, e.g. unsupported loop count
 *      - ESP_FAIL: Transmit data failed because of other error
 */
esp_err_t rmt_transmit(rmt_channel_handle_t tx_channel, rmt_encoder_handle_t encoder, const void *payload, size_t payload_bytes, const rmt_transmit_config_t *config);

/**
 * @brief Wait for all pending TX transactions done
 *
 * @note This function will block forever if the pending transaction can't be finished within a limited time (e.g. an infinite loop transaction).
 *       See also `rmt_disable()` for how to terminate a working channel.
 *
 * @param[in] tx_channel RMT TX channel that created by `rmt_new_tx_channel()`
 * @param[in] timeout_ms Wait timeout, in ms. Specially, -1 means to wait forever.
 * @return
 *      - ESP_OK: Flush transactions successfully
 *      - ESP_ERR_INVALID_ARG: Flush transactions failed because of invalid argument
 *      - ESP_ERR_TIMEOUT: Flush transactions failed because of timeout
 *      - ESP_FAIL: Flush transactions failed because of other error
 */
esp_err_t rmt_tx_wait_all_done(rmt_channel_handle_t tx_channel, int timeout_ms);

/**
 * @brief Set event callbacks for RMT TX channel
 *
 * @note User can deregister a previously registered callback by calling this function and setting the callback member in the `cbs` structure to NULL.
 * @note When CONFIG_RMT_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       The variables used in the function should be in the SRAM as well. The `user_data` should also reside in SRAM.
 *
 * @param[in] tx_channel RMT generic channel that created by `rmt_new_tx_channel()`
 * @param[in] cbs Group of callback functions
 * @param[in] user_data User data, which will be passed to callback functions directly
 * @return
 *      - ESP_OK: Set event callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set event callbacks failed because of invalid argument
 *      - ESP_FAIL: Set event callbacks failed because of other error
 */
esp_err_t rmt_tx_register_event_callbacks(rmt_channel_handle_t tx_channel, const rmt_tx_event_callbacks_t *cbs, void *user_data);

/**
 * @brief Create a synchronization manager for multiple TX channels, so that the managed channel can start transmitting at the same time
 *
 * @note All the channels to be managed should be enabled by `rmt_enable()` before put them into sync manager.
 *
 * @param[in] config Synchronization manager configuration
 * @param[out] ret_synchro Returned synchronization manager handle
 * @return
 *      - ESP_OK: Create sync manager successfully
 *      - ESP_ERR_INVALID_ARG: Create sync manager failed because of invalid argument
 *      - ESP_ERR_NOT_SUPPORTED: Create sync manager failed because it is not supported by hardware
 *      - ESP_ERR_INVALID_STATE: Create sync manager failed because not all channels are enabled
 *      - ESP_ERR_NO_MEM: Create sync manager failed because out of memory
 *      - ESP_ERR_NOT_FOUND: Create sync manager failed because all sync controllers are used up and no more free one
 *      - ESP_FAIL: Create sync manager failed because of other error
 */
esp_err_t rmt_new_sync_manager(const rmt_sync_manager_config_t *config, rmt_sync_manager_handle_t *ret_synchro);

/**
 * @brief Delete synchronization manager
 *
 * @param[in] synchro Synchronization manager handle returned from `rmt_new_sync_manager()`
 * @return
 *      - ESP_OK: Delete the synchronization manager successfully
 *      - ESP_ERR_INVALID_ARG: Delete the synchronization manager failed because of invalid argument
 *      - ESP_FAIL: Delete the synchronization manager failed because of other error
 */
esp_err_t rmt_del_sync_manager(rmt_sync_manager_handle_t synchro);

/**
 * @brief Reset synchronization manager
 *
 * @param[in] synchro Synchronization manager handle returned from `rmt_new_sync_manager()`
 * @return
 *      - ESP_OK: Reset the synchronization manager successfully
 *      - ESP_ERR_INVALID_ARG: Reset the synchronization manager failed because of invalid argument
 *      - ESP_FAIL: Reset the synchronization manager failed because of other error
 */
esp_err_t rmt_sync_reset(rmt_sync_manager_handle_t synchro);

#ifdef __cplusplus
}
#endif
