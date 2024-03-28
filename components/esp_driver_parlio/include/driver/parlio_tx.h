/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "driver/parlio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Parallel IO TX unit configuration
 */
typedef struct {
    parlio_clock_source_t clk_src;  /*!< Parallel IO internal clock source */
    gpio_num_t clk_in_gpio_num;     /*!< If the clock source is input from external, set the corresponding GPIO number.
                                         Otherwise, set to `-1` and the driver will use the internal `clk_src` as clock source.
                                         This option has higher priority than `clk_src` */
    uint32_t input_clk_src_freq_hz; /*!< Frequency of the input clock source, valid only if `clk_in_gpio_num` is not `-1` */
    uint32_t output_clk_freq_hz;    /*!< Frequency of the output clock. It's divided from either internal `clk_src` or external clock source */
    size_t data_width;              /*!< Parallel IO data width, can set to 1/2/4/8/..., but can't bigger than PARLIO_TX_UNIT_MAX_DATA_WIDTH */
    gpio_num_t data_gpio_nums[PARLIO_TX_UNIT_MAX_DATA_WIDTH]; /*!< Parallel IO data GPIO numbers, if any GPIO is not used, you can set it to `-1` */
    gpio_num_t clk_out_gpio_num; /*!< GPIO number of the output clock signal, the clock is synced with TX data */
    gpio_num_t valid_gpio_num;   /*!< GPIO number of the valid signal, which stays high when transferring data.
                                      Note that, the valid signal will always occupy the MSB data bit */
    size_t trans_queue_depth; /*!< Depth of internal transaction queue */
    size_t max_transfer_size; /*!< Maximum transfer size in one transaction, in bytes. This decides the number of DMA nodes will be used for each transaction */
    parlio_sample_edge_t sample_edge;       /*!< Parallel IO sample edge */
    parlio_bit_pack_order_t bit_pack_order; /*!< Set the order of packing the bits into bytes (only works when `data_width` < 8) */
    struct {
        uint32_t clk_gate_en: 1;  /*!< Enable TX clock gating,
                                       the output clock will be controlled by the MSB bit of the data bus,
                                       i.e. by data_gpio_nums[PARLIO_TX_UNIT_MAX_DATA_WIDTH-1]. High level to enable the clock output, low to disable */
        uint32_t io_loop_back: 1; /*!< For debug/test, the signal output from the GPIO will be fed to the input path as well */
    } flags;                      /*!< Extra configuration flags */
} parlio_tx_unit_config_t;

/**
 * @brief Create a Parallel IO TX unit
 *
 * @param[in] config Parallel IO TX unit configuration
 * @param[out] ret_unit Returned Parallel IO TX unit handle
 * @return
 *      - ESP_OK: Create Parallel IO TX unit successfully
 *      - ESP_ERR_INVALID_ARG: Create Parallel IO TX unit failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create Parallel IO TX unit failed because of out of memory
 *      - ESP_ERR_NOT_FOUND: Create Parallel IO TX unit failed because all TX units are used up and no more free one
 *      - ESP_ERR_NOT_SUPPORTED: Create Parallel IO TX unit failed because some feature is not supported by hardware, e.g. clock gating
 *      - ESP_FAIL: Create Parallel IO TX unit failed because of other error
 */
esp_err_t parlio_new_tx_unit(const parlio_tx_unit_config_t *config, parlio_tx_unit_handle_t *ret_unit);

/**
 * @brief Delete a Parallel IO TX unit
 *
 * @param[in] unit Parallel IO TX unit that created by `parlio_new_tx_unit`
 * @return
 *      - ESP_OK: Delete Parallel IO TX unit successfully
 *      - ESP_ERR_INVALID_ARG: Delete Parallel IO TX unit failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Delete Parallel IO TX unit failed because it is still in working
 *      - ESP_FAIL: Delete Parallel IO TX unit failed because of other error
 */
esp_err_t parlio_del_tx_unit(parlio_tx_unit_handle_t unit);

/**
 * @brief Enable the Parallel IO TX unit
 *
 * @note This function will transit the driver state from init to enable
 * @note This function will acquire a PM lock that might be installed during channel allocation
 * @note If there're transaction pending in the queue, this function will pick up the first one and start the transfer
 *
 * @param[in] unit Parallel IO TX unit that created by `parlio_new_tx_unit`
 * @return
 *      - ESP_OK: Enable Parallel IO TX unit successfully
 *      - ESP_ERR_INVALID_ARG: Enable Parallel IO TX unit failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Enable Parallel IO TX unit failed because it is already enabled
 *      - ESP_FAIL: Enable Parallel IO TX unit failed because of other error
 */
esp_err_t parlio_tx_unit_enable(parlio_tx_unit_handle_t unit);

/**
 * @brief Disable the Parallel IO TX unit
 *
 * @note This function will transit the driver state from enable to init
 * @note This function will release the PM lock that might be installed during channel allocation
 * @note If one transaction is undergoing, this function will terminate it immediately
 *
 * @param[in] unit Parallel IO TX unit that created by `parlio_new_tx_unit`
 * @return
 *      - ESP_OK: Disable Parallel IO TX unit successfully
 *      - ESP_ERR_INVALID_ARG: Disable Parallel IO TX unit failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Disable Parallel IO TX unit failed because it's not enabled yet
 *      - ESP_FAIL: Disable Parallel IO TX unit failed because of other error
 */
esp_err_t parlio_tx_unit_disable(parlio_tx_unit_handle_t unit);

/**
 * @brief Type of Parallel IO TX done event data
 */
typedef struct {
} parlio_tx_done_event_data_t;

/**
 * @brief Prototype of parlio tx event callback
 * @param[in] tx_unit Parallel IO TX unit that created by `parlio_new_tx_unit`
 * @param[in] edata Point to Parallel IO TX event data. The lifecycle of this pointer memory is inside this function,
 *                  user should copy it into static memory if used outside this function.
 * @param[in] user_ctx User registered context, passed from `parlio_tx_unit_register_event_callbacks`
 *
 * @return Whether a high priority task has been waken up by this callback function
 */
typedef bool (*parlio_tx_done_callback_t)(parlio_tx_unit_handle_t tx_unit, const parlio_tx_done_event_data_t *edata, void *user_ctx);

/**
 * @brief Group of Parallel IO TX callbacks
 * @note The callbacks are all running under ISR environment
 * @note When CONFIG_PARLIO_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       The variables used in the function should be in the SRAM as well.
 */
typedef struct {
    parlio_tx_done_callback_t on_trans_done; /*!< Event callback, invoked when one transmission is finished */
} parlio_tx_event_callbacks_t;

/**
 * @brief Set event callbacks for Parallel IO TX unit
 *
 * @note User can deregister a previously registered callback by calling this function and setting the callback member in the `cbs` structure to NULL.
 * @note When CONFIG_PARLIO_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       The variables used in the function should be in the SRAM as well. The `user_data` should also reside in SRAM.
 *
 * @param[in] tx_unit Parallel IO TX unit that created by `parlio_new_tx_unit`
 * @param[in] cbs Group of callback functions
 * @param[in] user_data User data, which will be passed to callback functions directly
 * @return
 *      - ESP_OK: Set event callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set event callbacks failed because of invalid argument
 *      - ESP_FAIL: Set event callbacks failed because of other error
 */
esp_err_t parlio_tx_unit_register_event_callbacks(parlio_tx_unit_handle_t tx_unit, const parlio_tx_event_callbacks_t *cbs, void *user_data);

/**
 * @brief Parallel IO transmit configuration
 */
typedef struct {
    uint32_t idle_value; /*!< The value on the data line when the parallel IO is in idle state */
    struct {
        uint32_t queue_nonblocking : 1; /*!< If set, when the transaction queue is full, driver will not block the thread but return directly */
    } flags;                            /*!< Transmit specific config flags */
} parlio_transmit_config_t;

/**
 * @brief Transmit data on by Parallel IO TX unit
 *
 * @note After the function returns, it doesn't mean the transaction is finished. This function only constructs a transaction structure and push into a queue.
 *
 * @param[in] tx_unit Parallel IO TX unit that created by `parlio_new_tx_unit`
 * @param[in] payload Pointer to the data to be transmitted
 * @param[in] payload_bits Length of the data to be transmitted, in bits
 * @param[in] config Transmit configuration
 * @return
 *      - ESP_OK: Transmit data successfully
 *      - ESP_ERR_INVALID_ARG: Transmit data failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Transmit data failed because the Parallel IO TX unit is not enabled
 *      - ESP_FAIL: Transmit data failed because of other error
 */
esp_err_t parlio_tx_unit_transmit(parlio_tx_unit_handle_t tx_unit, const void *payload, size_t payload_bits, const parlio_transmit_config_t *config);

/**
 * @brief Wait for all pending TX transactions done
 *
 * @param[in] tx_unit Parallel IO TX unit that created by `parlio_new_tx_unit`
 * @param[in] timeout_ms Timeout in milliseconds, `-1` means to wait forever
 * @return
 *      - ESP_OK: All pending TX transactions is finished and recycled
 *      - ESP_ERR_INVALID_ARG: Wait for all pending TX transactions done failed because of invalid argument
 *      - ESP_ERR_TIMEOUT: Wait for all pending TX transactions done timeout
 *      - ESP_FAIL: Wait for all pending TX transactions done failed because of other error
 */
esp_err_t parlio_tx_unit_wait_all_done(parlio_tx_unit_handle_t tx_unit, int timeout_ms);

#ifdef __cplusplus
}
#endif
