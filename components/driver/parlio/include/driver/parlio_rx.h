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
    uint32_t                clk_freq_hz;            /*!< The source clock frequency for external when the clock source is selected as PARLIO_CLK_SRC_EXTERNAL;
                                                         The expected output clock frequency when the clock source is from internal */
    gpio_num_t              clk_gpio_num;           /*!< The input or output clock pin. If the clock source is input from external,
                                                         the clock gpio must be set, otherwise set `-1` can disable the clock output if not needed */
    gpio_num_t              valid_gpio_num;         /*!< GPIO number of the valid signal. The signal on this pin is used to indicate whether the data on the data lines are valid.
                                                         Only takes effect when using level or pulse delimiter, set to `-1` if only use the soft delimiter */
    gpio_num_t              data_gpio_nums[PARLIO_RX_UNIT_MAX_DATA_WIDTH]; /*!< Parallel IO data GPIO numbers, set to `-1` if it's not used,
                                                                                The driver will take [0 .. (data_width - 1)] as the data pins */
    struct {
        uint32_t            free_clk : 1;           /*!< Whether the input external clock is a free-running clock. A free-running clock will always keep running (e.g. I2S bclk),
                                                         a non-free-running clock will start when there are data transporting and stop when the bus idle (e.g. SPI).
                                                         This flag only takes effect when the clock source is selected as PARLIO_CLK_SRC_EXTERNAL */
        uint32_t            clk_gate_en : 1;        /*!< Enable RX clock gating, only available when the clock direction is output(not supported on ESP32-C6)
                                                         the output clock will be controlled by the valid gpio,
                                                         i.e. high level of valid gpio to enable the clock output, low to disable */
        uint32_t            io_loop_back: 1;        /*!< For debug/test, the signal output from the GPIO will be fed to the input path as well */
        uint32_t            io_no_init: 1;          /*!< Set to skip initializing the GPIO, but only attach the pralio rx signals to those GPIOs via IO Matrix.
                                                         So that the signals that have attached to those GPIO won't be overwritten. Mainly used for self communication or self monitoring */
    } flags;
} parlio_rx_unit_config_t;

/**
 * @brief Create a Parallel IO RX unit
 *
 * @param[in]  config   Parallel IO RX unit configuration
 * @param[out] ret_unit Returned Parallel IO RX unit handle
 * @return esp_err_t
 */
esp_err_t parlio_new_rx_unit(const parlio_rx_unit_config_t *config, parlio_rx_unit_handle_t *ret_unit);

/**
 * @brief Delete a Parallel IO RX unit
 *
 * @param[in] rx_unit   Parallel IO RX unit handle that created by `parlio_new_rx_unit`
 * @return esp_err_t
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
        uint32_t            active_level: 1;        /*!< Which level indicates the validation of the transmitting data */
    } flags;                                        /*!< Extra flags */
} parlio_rx_level_delimiter_config_t;

/**
 * @brief Create a level delimiter
 *
 * @note The enable signal must be aligned with the valid data.
 * @note There're at most `SOC_PARLIO_RX_UNIT_MAX_DATA_WIDTH - 1` IO pins left for RXD
 *
 * @param[in]  config        Level delimiter configuration
 * @param[out] ret_delimiter Returned delimiter handle
 * @return esp_err_t
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
                                                         if no, the transaction will be terminated by user configured transcation length */
        uint32_t            pulse_invert: 1;        /*!< Whether to invert the pulse */
    } flags;                                        /*!< Extra flags */
} parlio_rx_pulse_delimiter_config_t;

/**
 * @brief Create a pulse delimiter
 *
 * @note There're at most `SOC_PARLIO_RX_UNIT_MAX_DATA_WIDTH - 1` IO pins left for RXD
 *
 * @param[in]  config        Pulse delimiter configuration
 * @param[out] ret_delimiter Returned delimiter handle
 * @return esp_err_t
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
                                                         `has_end_pulse` flag is set */
    uint32_t                timeout_ticks;          /*!< The number of APB clock ticks to trigger timeout interrupt. Set 0 to disable the receive timeout interrupt */
} parlio_rx_soft_delimiter_config_t;

/**
 * @brief Create a pulse delimiter
 *
 * @param[in]  config        Soft delimiter configuration
 * @param[out] ret_delimiter Returned delimiter handle
 * @return esp_err_t
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
 * @return esp_err_t
 */
esp_err_t parlio_rx_soft_delimiter_start_stop(parlio_rx_unit_handle_t rx_unit, parlio_rx_delimiter_handle_t delimiter, bool start_stop);

/**
 * @brief Generic Selection of the delimiter create functions
 * @note  This is a helper function for creating the delimiter generically,
 *        input different type of configuration to create a corresponding delimiter
 *
 * @param[in]  config        The configuration of the delimiter, whose type decides the delimiter type that created,
 *                           Can be `parlio_rx_level_delimiter_config_t`, `parlio_rx_pulse_delimiter_config_t` or `parlio_rx_soft_delimiter_config_t`
 * @param[out] ret_delimiter Returned delimiter handle
 */
#define parlio_new_rx_delimiter(config, ret_delimiter)  _Generic((config),      \
        parlio_rx_level_delimiter_config_t* : parlio_new_rx_level_delimiter,    \
        parlio_rx_pulse_delimiter_config_t* : parlio_new_rx_pulse_delimiter,    \
        parlio_rx_soft_delimiter_config_t*  : parlio_new_rx_soft_delimiter,     \
        default: parlio_new_rx_soft_delimiter) (config, ret_delimiter)

/**
 * @brief Delete the delimiter
 * @note  To delete the delimiter safely, please delete it after disable all the RX units
 *
 * @param[in]  delimiter     Delimiter handle
 * @return esp_err_t
 */
esp_err_t parlio_del_rx_delimiter(parlio_rx_delimiter_handle_t delimiter);

/**
 * @brief Enable the Parallel IO RX unit
 *
 * @param[in]  rx_unit       Parallel IO RX unit handle that created by `parlio_new_rx_unit`
 * @param[in]  reset_queue   Whether to reset the receiving queue.
 *                           If set to false, the legacy receive transactions in the queue are still available,
 *                           If set to true, the legacy receive transactions in the queue are dropped.
 * @return esp_err_t
 */
esp_err_t parlio_rx_unit_enable(parlio_rx_unit_handle_t rx_unit, bool reset_queue);

/**
 * @brief Disable the Parallel IO TX unit
 *
 * @param[in]  rx_unit       Parallel IO RX unit handle that created by `parlio_new_rx_unit`
 * @return esp_err_t
 */
esp_err_t parlio_rx_unit_disable(parlio_rx_unit_handle_t rx_unit);

/**
 * @brief Configuration of a receive transaction
 */
typedef struct {
    parlio_rx_delimiter_handle_t    delimiter;      /*!< The delimiter of this receiving transaction */
    struct {
        uint32_t                    is_infinite: 1; /*!< Whether this is an infinite transaction that supposed to receive continuously */
        uint32_t                    indirect_mount: 1;/*!< This flag only take effect when `is_infinite` is enabled.
                                                     *   Enable this flag, the DMA descriptor will mount to an internal DMA buffer instead,
                                                     *   The data will be copy to the payload in every interrupt. So that to guarantee the payload buffer
                                                     *   is valid during the `on_receive_done` callback.
                                                     *   Either `is_infinite` or `indirect_mount` is disabled,
                                                     *   the finite payload will be mounted to the DMA descriptor directly.
                                                     *   By default, the receive payload will be mounted to the DMA descriptor directly,
                                                     */
    } flags;                                        /*!< Extra flags */
} parlio_receive_config_t;

/**
 * @brief Receive data by Parallel IO RX unit
 * @note  The receive transaction will start immediately when there is not other transaction on receiving,
 *        Otherwise it will be sent to the transaction queue to wait for the bus.
 *
 * @param[in]  rx_unit       Parallel IO RX unit handle that created by `parlio_new_rx_unit`
 * @param[in]  payload       The payload buffer pointer
 * @param[in]  payload_size  The size of the payload buffer, in bytes.
 * @param[in]  recv_cfg      The configuration of this receive transaction
 * @return esp_err_t
 */
esp_err_t parlio_rx_unit_receive(parlio_rx_unit_handle_t rx_unit,
                                 void *payload,
                                 size_t payload_size,
                                 const parlio_receive_config_t* recv_cfg);

/**
 * @brief Wait for all pending RX transactions done
 *
 * @param[in]  rx_unit       Parallel IO RX unit handle that created by `parlio_new_rx_unit`
 * @param[in]  timeout_ms    Timeout in milliseconds, `-1` means to wait forever (software timeout)
 * @return esp_err_t
 */
esp_err_t parlio_rx_unit_wait_all_done(parlio_rx_unit_handle_t rx_unit, int timeout_ms);

/**
 * @brief Event callback data
 */
typedef struct {
    parlio_rx_delimiter_handle_t    delimiter;      /*!< The current delimiter of this receiving event */
    void                            *data;          /*!< The data buffer address that just finished receiving */
    size_t                          size;           /*!< The total size of the data buffer , in byte */
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
 * @param[in]  cbs           Callbacks
 * @param[in]  user_data     User specified data that will be transported to the callbacks
 * @return esp_err_t
 */
esp_err_t parlio_rx_unit_register_event_callbacks(parlio_rx_unit_handle_t rx_unit, const parlio_rx_event_callbacks_t *cbs, void *user_data);

#ifdef __cplusplus
}
#endif
