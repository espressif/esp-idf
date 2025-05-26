/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "driver/parlio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Parallel IO RX unit configuration
 */
typedef struct {
    size_t                  trans_queue_depth;      /*!< Depth of internal transaction queue */
    size_t                  max_recv_size;          /*!< Maximum receive size in one transaction, in bytes. This decides the number of DMA nodes will be used for each transaction */
    size_t                  data_width;             /*!< Parallel IO data width, can set to 1/2/4/8/..., but can't be greater than PARLIO_RX_UNIT_MAX_DATA_WIDTH */
    parlio_clock_source_t   clk_src;                /*!< Parallel IO clock source */
    uint32_t                ext_clk_freq_hz;        /*!< The external source clock frequency. Only be valid when select PARLIO_CLK_SRC_EXTERNAL as clock source */
    uint32_t                exp_clk_freq_hz;        /*!< The expected sample/bit clock frequency, which is divided from the internal or external clock regarding the clock source */
    gpio_num_t              clk_in_gpio_num;        /*!< The the external clock input pin. Only be valid when select PARLIO_CLK_SRC_EXTERNAL as clock source. Set to -1 if not needed */
    gpio_num_t              clk_out_gpio_num;       /*!< The sample/bit clock output pin. Set to -1 if not needed */
    gpio_num_t              valid_gpio_num;         /*!< GPIO number of the valid signal. The signal on this pin is used to indicate whether the data on the data lines are valid.
                                                         Only takes effect when using level or pulse delimiter, set to `-1` if only use the soft delimiter */
    gpio_num_t              data_gpio_nums[PARLIO_RX_UNIT_MAX_DATA_WIDTH]; /*!< Parallel IO data GPIO numbers, set to `-1` if it's not used,
                                                                                The driver will take [0 .. (data_width - 1)] as the data pins */
    struct {
        uint32_t            free_clk : 1;           /*!< Whether the input external clock is a free-running clock. A free-running clock will always keep running (e.g. I2S bclk),
                                                         a non-free-running clock will start when there are data transporting and stop when the bus idle (e.g. SPI).
                                                         This flag only takes effect when select PARLIO_CLK_SRC_EXTERNAL as clock source */
        uint32_t            clk_gate_en : 1;        /*!< Enable RX clock gating, only available when the clock direction is output(not supported on ESP32-C6)
                                                         the output clock will be controlled by the valid gpio,
                                                         i.e. high level of valid gpio to enable the clock output, low to disable */
        uint32_t            allow_pd: 1;             /*!< Set to allow power down. When this flag set, the driver will backup/restore the PARLIO registers before/after entering/exist sleep mode.
                                                         By this approach, the system can power off PARLIO's power domain.
                                                         This can save power, but at the expense of more RAM being consumed. */
    } flags;                                        /*!< RX driver flags */
} parlio_rx_unit_config_t;

/**
 * @brief Create a Parallel IO RX unit
 *
 * @param[in]  config   Parallel IO RX unit configuration
 * @param[out] ret_unit Returned Parallel IO RX unit handle
 * @return
 *      - ESP_ERR_INVALID_ARG       Invalid arguments in the parameter list or the rx unit configuration
 *      - ESP_ERR_NOT_FOUND         No available rx unit found
 *      - ESP_ERR_NO_MEM            No enough memory for the rx unit resources
 *      - ESP_OK                    Success to allocate the rx unit
 */
esp_err_t parlio_new_rx_unit(const parlio_rx_unit_config_t *config, parlio_rx_unit_handle_t *ret_unit);

/**
 * @brief Delete a Parallel IO RX unit
 *
 * @param[in] rx_unit   Parallel IO RX unit handle that created by `parlio_new_rx_unit`
 * @return
 *      - ESP_ERR_INVALID_ARG       rx_unit is NULL
 *      - ESP_ERR_INVALID_STATE     The rx unit is enabled, can't delete an enabled rx unit
 *      - ESP_OK                    Success to delete the rx unit
 */
esp_err_t parlio_del_rx_unit(parlio_rx_unit_handle_t rx_unit);

/**
 * @brief Configuration of level delimiter
 */
typedef struct {
    uint32_t                valid_sig_line_id;      /*!< The data line id of valid/enable signal.
                                                         The selected data line will be used as the valid/enable signal (i.e. level signal) in this delimiter.
                                                         As the line of valid/enable signal is shared with the data line, this line_id will be conflict
                                                         with the data line if set the id within 'data_width',
                                                         therefore the range is (data_width, PARLIO_RX_UNIT_MAX_DATA_WIDTH]. */
    parlio_sample_edge_t    sample_edge;            /*!< Parallel IO sample edge */
    parlio_bit_pack_order_t bit_pack_order;         /*!< Set how we pack the bits into one bytes */
    uint32_t                eof_data_len;           /*!< Set the data length to trigger the End Of Frame (EOF, i.e. transaction done)
                                                         interrupt, if the data length is set to `0`, that mean the EOF will only triggers
                                                         when the enable signal inactivated */
    uint32_t                timeout_ticks;          /*!< The number of source clock ticks to trigger timeout interrupt. Set 0 to disable the receive timeout interrupt
                                                         The timeout counter starts when the valid/enable signal is invalid/disabled. */
    struct {
        uint32_t            active_low_en: 1;       /*!< Set true to set the valid signal active when the level is low,
                                                         otherwise, the valid signal becomes active when its level is high */
    } flags;                                        /*!< Extra flags */
} parlio_rx_level_delimiter_config_t;

/**
 * @brief Create a level delimiter
 * @note This function only allocate the software resources, the hardware configurations
 *       will lazy installed while the transaction that using this delimiter start processing
 * @note The enable signal must be aligned with the valid data.
 * @note There're at most `SOC_PARLIO_RX_UNIT_MAX_DATA_WIDTH - 1` IO pins left for RXD
 *
 * @param[in]  config        Level delimiter configuration
 * @param[out] ret_delimiter Returned delimiter handle
 * @return
 *      - ESP_ERR_INVALID_ARG       Invalid arguments in the parameter list or the level delimiter configuration
 *      - ESP_ERR_NO_MEM            No enough memory for the level delimiter resources
 *      - ESP_OK                    Success to allocate the level delimiter
 */
esp_err_t parlio_new_rx_level_delimiter(const parlio_rx_level_delimiter_config_t *config,
                                        parlio_rx_delimiter_handle_t *ret_delimiter);

/**
 * @brief Configuration of pulse delimiter
 */
typedef struct {
    uint32_t                valid_sig_line_id;      /*!< The data line id of valid/enable signal.
                                                         The selected data line will be used as the valid/enable signal (i.e. pulse signal) in this delimiter.
                                                         As the line of valid/enable signal is shared with the data line, this line_id will be conflict
                                                         with the data line if set the id within 'data_width',
                                                         therefore the range is (data_width, PARLIO_RX_UNIT_MAX_DATA_WIDTH]. */
    parlio_sample_edge_t    sample_edge;            /*!< Parallel IO sample edge */
    parlio_bit_pack_order_t bit_pack_order;         /*!< Set how we pack the bits into one bytes */
    uint32_t                eof_data_len;           /*!< Set the data length to trigger the End Of Frame (EOF, i.e. transaction done)
                                                         interrupt, if the data length is set to `0`, that mean the EOF will only triggers
                                                         when the end pulse detected, please ensure there is an end pulse for a frame and
                                                         `has_end_pulse` flag is set */
    uint32_t                timeout_ticks;          /*!< The number of source clock ticks to trigger timeout interrupt. Set 0 to disable the receive timeout interrupt
                                                         The timeout counter starts when the valid/enable signal is invalid/disabled. */
    struct {
        uint32_t            start_bit_included: 1;  /*!< Whether data bit is included in the start pulse */
        uint32_t            end_bit_included: 1;    /*!< Whether data bit is included in the end pulse, only valid when `has_end_pulse` is true */
        uint32_t            has_end_pulse: 1;       /*!< Whether there's an end pulse to terminate the transaction,
                                                         if no, the transaction will be terminated by user configured transaction length */
        uint32_t            pulse_invert: 1;        /*!< Whether to invert the pulse */
    } flags;                                        /*!< Extra flags */
} parlio_rx_pulse_delimiter_config_t;

/**
 * @brief Create a pulse delimiter
 * @note This function only allocate the software resources, the hardware configurations
 *       will lazy installed while the transaction that using this delimiter start processing
 * @note There're at most `SOC_PARLIO_RX_UNIT_MAX_DATA_WIDTH - 1` IO pins left for RXD
 *
 * @param[in]  config        Pulse delimiter configuration
 * @param[out] ret_delimiter Returned delimiter handle
 * @return
 *      - ESP_ERR_INVALID_ARG       Invalid arguments in the parameter list or the pulse delimiter configuration
 *      - ESP_ERR_NO_MEM            No enough memory for the pulse delimiter resources
 *      - ESP_OK                    Success to allocate the pulse delimiter
 */
esp_err_t parlio_new_rx_pulse_delimiter(const parlio_rx_pulse_delimiter_config_t *config,
                                        parlio_rx_delimiter_handle_t *ret_delimiter);

/**
 * @brief Configuration of soft delimiter
 */
typedef struct {
    parlio_sample_edge_t    sample_edge;            /*!< Parallel IO sample edge */
    parlio_bit_pack_order_t bit_pack_order;         /*!< Set how we pack the bits into one bytes, set 1 to pack the bits into a byte from LSB,
                                                         otherwise from MSB */
    uint32_t                eof_data_len;           /*!< Set the data length to trigger the End Of Frame (EOF, i.e. transaction done)
                                                         interrupt, if the data length is set to `0`, that mean the EOF will only triggers
                                                         when the end pulse detected, please ensure there is an end pulse for a frame and
                                                         `parlio_rx_pulse_delimiter_config_t::has_end_pulse` flag is set */
    uint32_t                timeout_ticks;          /*!< The number of APB clock ticks to trigger timeout interrupt. Set 0 to disable the receive timeout interrupt */
} parlio_rx_soft_delimiter_config_t;

/**
 * @brief Create a pulse delimiter
 * @note This function only allocate the software resources, the hardware configurations
 *       will lazy installed while the transaction that using this delimiter start processing
 * @param[in]  config        Soft delimiter configuration
 * @param[out] ret_delimiter Returned delimiter handle
 * @return
 *      - ESP_ERR_INVALID_ARG       Invalid arguments in the parameter list or the soft delimiter configuration
 *      - ESP_ERR_NO_MEM            No enough memory for the soft delimiter resources
 *      - ESP_OK                    Success to allocate the soft delimiter
 */
esp_err_t parlio_new_rx_soft_delimiter(const parlio_rx_soft_delimiter_config_t *config,
                                       parlio_rx_delimiter_handle_t *ret_delimiter);

/**
 * @brief Start/stop the soft delimiter
 * @note  Soft delimiter need to start or stop manually because it has no validating/enabling signal to indicate the data has started or stopped
 *
 * @param[in]  rx_unit      Parallel IO RX unit handle that created by `parlio_new_rx_unit`
 * @param[in]  delimiter    Delimiter handle
 * @param[in]  start_stop   Set true to start, set false to stop
 * @return
 *      - ESP_ERR_INVALID_ARG       Invalid arguments in the parameter list or not soft delimiter
 *      - ESP_ERR_INVALID_STATE     The rx unit not enabled
 *      - ESP_OK                    Success to start or stop the soft delimiter
 */
esp_err_t parlio_rx_soft_delimiter_start_stop(parlio_rx_unit_handle_t rx_unit, parlio_rx_delimiter_handle_t delimiter, bool start_stop);

/**
 * @brief Delete the delimiter
 * @note  To delete the delimiter safely, please delete it after disable all the RX units
 *
 * @param[in]  delimiter     Delimiter handle
 * @return
 *      - ESP_ERR_INVALID_ARG       The input delimiter is NULL
 *      - ESP_ERR_INVALID_STATE     The delimiter is on receiving
 *      - ESP_OK                    Success to delete the delimiter
 */
esp_err_t parlio_del_rx_delimiter(parlio_rx_delimiter_handle_t delimiter);

/**
 * @brief Enable the Parallel IO RX unit
 *
 * @param[in]  rx_unit       Parallel IO RX unit handle that created by `parlio_new_rx_unit`
 * @param[in]  reset_queue   Whether to reset the receiving queue.
 *                           If set to false, the legacy receive transactions in the queue are still available,
 *                           If set to true, the legacy receive transactions in the queue are dropped.
 * @return
 *      - ESP_ERR_INVALID_ARG       The input rx_unit is NULL
 *      - ESP_ERR_INVALID_STATE     The rx unit has been enabled
 *      - ESP_OK                    Success to enable the rx unit
 */
esp_err_t parlio_rx_unit_enable(parlio_rx_unit_handle_t rx_unit, bool reset_queue);

/**
 * @brief Disable the Parallel IO RX unit
 *
 * @param[in]  rx_unit       Parallel IO RX unit handle that created by `parlio_new_rx_unit`
 * @return
 *      - ESP_ERR_INVALID_ARG       The input rx_unit is NULL
 *      - ESP_ERR_INVALID_STATE     The rx unit has been disabled
 *      - ESP_OK                    Success to disable the rx unit
 */
esp_err_t parlio_rx_unit_disable(parlio_rx_unit_handle_t rx_unit);

/**
 * @brief Configuration of a receive transaction
 */
typedef struct {
    parlio_rx_delimiter_handle_t delimiter;         /*!< The delimiter of this receiving transaction */
    struct {
        uint32_t                 partial_rx_en: 1;  /*!< Whether this is an infinite transaction that supposed to receive continuously and partially */
        uint32_t                 indirect_mount: 1; /*!< This flag only take effect when `partial_rx_en` is enabled.
                                                     *   Enable this flag, an INTERNAL DMA buffer will be mounted to the DMA descriptor instead,
                                                     *   The data will be copy to the payload in every interrupt. So that to guarantee the payload buffer
                                                     *   is valid during the `on_receive_done` callback.
                                                     *   Either `partial_rx_en` or `indirect_mount` is disabled,
                                                     *   the user given finite payload will be mounted to the DMA descriptor directly.
                                                     *   By default, the user given receive payload will be mounted to the DMA descriptor directly.
                                                     */
    } flags;                                        /*!< Extra flags */
} parlio_receive_config_t;

/**
 * @brief Receive data by Parallel IO RX unit
 * @note  This is a non-blocking and asynchronous function. To block or realize synchronous receive,
 *        please call `parlio_rx_unit_wait_all_done` after this function
 * @note  The receive transaction will start immediately when there is not other transaction on receiving,
 *        Otherwise it will be sent to the transaction queue to wait for the bus.
 *
 * @param[in]  rx_unit       Parallel IO RX unit handle that created by `parlio_new_rx_unit`
 * @param[in]  payload       The payload buffer pointer
 * @param[in]  payload_size  The size of the payload buffer, in bytes.
 * @param[in]  recv_cfg      The configuration of this receive transaction
 * @return
 *      - ESP_ERR_INVALID_ARG       Invalid arguments in the parameter list or the receive configuration
 *      - ESP_ERR_NO_MEM            No memory for the internal DMA buffer (only when parlio_receive_config_t::indirect_mount enabled)
 *      - ESP_ERR_INVALID_STATE     Transaction queue is full, failed to queue the current transaction.
 *                                  Or the internal buffer is under using by an infinite transaction, can't allocate a new one
 *      - ESP_OK                    Success to queue the current receiving transaction
 */
esp_err_t parlio_rx_unit_receive(parlio_rx_unit_handle_t rx_unit,
                                 void *payload,
                                 size_t payload_size,
                                 const parlio_receive_config_t* recv_cfg);

/**
 * @brief Wait for all pending RX transactions done
 * @note This function will block until all receiving transactions done or timeout.
 *       When timeout occurs, either the timeout limitation too short for all transactions done,
 *       or the peripheral got stuck and no more interrupts trigger (e.g., external clock stopped).
 *
 * @param[in]  rx_unit       Parallel IO RX unit handle that created by `parlio_new_rx_unit`
 * @param[in]  timeout_ms    Timeout in milliseconds, `-1` means to wait forever (software timeout)
 * @return
 *      - ESP_ERR_INVALID_ARG       The input rx_unit is NULL
 *      - ESP_ERR_TIMEOUT           Wait for all transactions done timeout
 *      - ESP_OK                    All transaction done
 */
esp_err_t parlio_rx_unit_wait_all_done(parlio_rx_unit_handle_t rx_unit, int timeout_ms);

/**
 * @brief Event callback data
 */
typedef struct {
    parlio_rx_delimiter_handle_t    delimiter;      /*!< The current delimiter of this receiving event */
    void                            *data;          /*!< The data buffer address that just finished receiving */
    size_t                          recv_bytes;     /*!< The number of received bytes in the data buffer */
} parlio_rx_event_data_t;

/**
 * @brief The template of the Parallel IO RX callback function
 *
 * @param[in]  rx_unit       Parallel IO RX unit handle that given from ISR
 * @param[in]  edata         The event data that given from ISR
 * @param[in]  user_data     The user specified data that given while registering the callbacks
 *
 * @return
 *      - True: to awoke high priority tasks
 *      - False: not to awoke high priority tasks
 */
typedef bool (*parlio_rx_callback_t)(parlio_rx_unit_handle_t rx_unit, const parlio_rx_event_data_t *edata, void *user_data);

/**
 * @brief Parallel IO RX event callbacks
 */
typedef struct {
    parlio_rx_callback_t    on_partial_receive;     /*!< Callback of received partial data */
    parlio_rx_callback_t    on_receive_done;        /*!< Callback of receiving transaction done */
    parlio_rx_callback_t    on_timeout;             /*!< Callback of hardware receiving timeout */
} parlio_rx_event_callbacks_t;

/**
 * @brief Register event callbacks for Parallel IO RX unit
 *
 * @param[in]  rx_unit       Parallel IO RX unit handle that created by `parlio_new_rx_unit`
 * @param[in]  cbs           Callback group, set callback to NULL to deregister the corresponding callback (callback group pointer shouldn't be NULL)
 * @param[in]  user_data     User specified data that will be transported to the callbacks
 * @return
 *      - ESP_ERR_INVALID_ARG       The input rx_unit is NULL
 *      - ESP_ERR_INVALID_STATE     The rx unit has been enabled, callback should be registered before enabling the unit
 *      - ESP_OK                    Success to register the callbacks
 */
esp_err_t parlio_rx_unit_register_event_callbacks(parlio_rx_unit_handle_t rx_unit, const parlio_rx_event_callbacks_t *cbs, void *user_data);

#ifdef __cplusplus
}
#endif
