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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Group of RMT RX callbacks
 * @note The callbacks are all running under ISR environment
 * @note When CONFIG_RMT_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       The variables used in the function should be in the SRAM as well.
 */
typedef struct {
    rmt_rx_done_callback_t on_recv_done; /*!< Event callback, invoked when one RMT channel receiving transaction completes */
} rmt_rx_event_callbacks_t;

/**
 * @brief RMT RX channel specific configuration
 */
typedef struct {
    gpio_num_t gpio_num;        /*!< GPIO number used by RMT RX channel. Set to -1 if unused */
    rmt_clock_source_t clk_src; /*!< Clock source of RMT RX channel, channels in the same group must use the same clock source */
    uint32_t resolution_hz;     /*!< Channel clock resolution, in Hz */
    size_t mem_block_symbols;   /*!< Size of memory block, in number of `rmt_symbol_word_t`, must be an even.
                                     In the DMA mode, this field controls the DMA buffer size, it can be set to a large value (e.g. 1024);
                                     In the normal mode, this field controls the number of RMT memory block that will be used by the channel. */
    struct {
        uint32_t invert_in: 1;    /*!< Whether to invert the incoming RMT channel signal */
        uint32_t with_dma: 1;     /*!< If set, the driver will allocate an RMT channel with DMA capability */
        uint32_t io_loop_back: 1; /*!< For debug/test, the signal output from the GPIO will be fed to the input path as well */
    } flags;                      /*!< RX channel config flags */
    int intr_priority;            /*!< RMT interrupt priority,
                                       if set to 0, the driver will try to allocate an interrupt with a relative low priority (1,2,3) */
} rmt_rx_channel_config_t;

/**
 * @brief RMT receive specific configuration
 */
typedef struct {
    uint32_t signal_range_min_ns; /*!< A pulse whose width is smaller than this threshold will be treated as glitch and ignored */
    uint32_t signal_range_max_ns; /*!< RMT will stop receiving if one symbol level has kept more than `signal_range_max_ns` */
} rmt_receive_config_t;

/**
 * @brief Create a RMT RX channel
 *
 * @param[in] config RX channel configurations
 * @param[out] ret_chan Returned generic RMT channel handle
 * @return
 *      - ESP_OK: Create RMT RX channel successfully
 *      - ESP_ERR_INVALID_ARG: Create RMT RX channel failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create RMT RX channel failed because out of memory
 *      - ESP_ERR_NOT_FOUND: Create RMT RX channel failed because all RMT channels are used up and no more free one
 *      - ESP_ERR_NOT_SUPPORTED: Create RMT RX channel failed because some feature is not supported by hardware, e.g. DMA feature is not supported by hardware
 *      - ESP_FAIL: Create RMT RX channel failed because of other error
 */
esp_err_t rmt_new_rx_channel(const rmt_rx_channel_config_t *config, rmt_channel_handle_t *ret_chan);

/**
 * @brief Initiate a receive job for RMT RX channel
 *
 * @note This function is non-blocking, it initiates a new receive job and then returns.
 *       User should check the received data from the `on_recv_done` callback that registered by `rmt_rx_register_event_callbacks()`.
 * @note This function can also be called in ISR context.
 * @note If you want this function to work even when the flash cache is disabled, please enable the `CONFIG_RMT_RECV_FUNC_IN_IRAM` option.
 *
 * @param[in] rx_channel RMT RX channel that created by `rmt_new_rx_channel()`
 * @param[in] buffer The buffer to store the received RMT symbols
 * @param[in] buffer_size size of the `buffer`, in bytes
 * @param[in] config Receive specific configurations
 * @return
 *      - ESP_OK: Initiate receive job successfully
 *      - ESP_ERR_INVALID_ARG: Initiate receive job failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Initiate receive job failed because channel is not enabled
 *      - ESP_FAIL: Initiate receive job failed because of other error
 */
esp_err_t rmt_receive(rmt_channel_handle_t rx_channel, void *buffer, size_t buffer_size, const rmt_receive_config_t *config);

/**
 * @brief Set callbacks for RMT RX channel
 *
 * @note User can deregister a previously registered callback by calling this function and setting the callback member in the `cbs` structure to NULL.
 * @note When CONFIG_RMT_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       The variables used in the function should be in the SRAM as well. The `user_data` should also reside in SRAM.
 *
 * @param[in] rx_channel RMT generic channel that created by `rmt_new_rx_channel()`
 * @param[in] cbs Group of callback functions
 * @param[in] user_data User data, which will be passed to callback functions directly
 * @return
 *      - ESP_OK: Set event callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set event callbacks failed because of invalid argument
 *      - ESP_FAIL: Set event callbacks failed because of other error
 */
esp_err_t rmt_rx_register_event_callbacks(rmt_channel_handle_t rx_channel, const rmt_rx_event_callbacks_t *cbs, void *user_data);

#ifdef __cplusplus
}
#endif
