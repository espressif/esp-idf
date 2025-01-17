/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/i2c_types.h"
#include "hal/gpio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief I2C master bus specific configurations
 */
typedef struct {
    i2c_port_num_t i2c_port;              /*!< I2C port number, `-1` for auto selecting, (not include LP I2C instance) */
    gpio_num_t sda_io_num;                /*!< GPIO number of I2C SDA signal, pulled-up internally */
    gpio_num_t scl_io_num;                /*!< GPIO number of I2C SCL signal, pulled-up internally */
    union {
        i2c_clock_source_t clk_source;        /*!< Clock source of I2C master bus */
#if SOC_LP_I2C_SUPPORTED
        lp_i2c_clock_source_t lp_source_clk;       /*!< LP_UART source clock selection */
#endif
    };
    uint8_t glitch_ignore_cnt;            /*!< If the glitch period on the line is less than this value, it can be filtered out, typically value is 7 (unit: I2C module clock cycle)*/
    int intr_priority;                    /*!< I2C interrupt priority, if set to 0, driver will select the default priority (1,2,3). */
    size_t trans_queue_depth;             /*!< Depth of internal transfer queue, increase this value can support more transfers pending in the background, only valid in asynchronous transaction. (Typically max_device_num * per_transaction)*/
    struct {
        uint32_t enable_internal_pullup: 1;  /*!< Enable internal pullups. Note: This is not strong enough to pullup buses under high-speed frequency. Recommend proper external pull-up if possible */
        uint32_t allow_pd:               1;  /*!< If set, the driver will backup/restore the I2C registers before/after entering/exist sleep mode.
                                              By this approach, the system can power off I2C's power domain.
                                              This can save power, but at the expense of more RAM being consumed */
    } flags;                              /*!< I2C master config flags */
} i2c_master_bus_config_t;

#define I2C_DEVICE_ADDRESS_NOT_USED    (0xffff) /*!< Skip carry address bit in driver transmit and receive */

/**
 * @brief I2C device configuration
 */
typedef struct {
    i2c_addr_bit_len_t dev_addr_length;         /*!< Select the address length of the slave device. */
    uint16_t device_address;                    /*!< I2C device raw address. (The 7/10 bit address without read/write bit). Macro I2C_DEVICE_ADDRESS_NOT_USED (0xFFFF) stands for skip the address config inside driver. */
    uint32_t scl_speed_hz;                      /*!< I2C SCL line frequency. */
    uint32_t scl_wait_us;                      /*!< Timeout value. (unit: us). Please note this value should not be so small that it can handle stretch/disturbance properly. If 0 is set, that means use the default reg value*/
    struct {
        uint32_t disable_ack_check:      1;     /*!< Disable ACK check. If this is set false, that means ack check is enabled, the transaction will be stopped and API returns error when nack is detected. */
    } flags;                                    /*!< I2C device config flags */
} i2c_device_config_t;

/**
 * @brief Structure representing an I2C operation job
 *
 * This structure is used to define individual I2C operations (write or read)
 * within a sequence of I2C master transactions.
 */
typedef struct {
    i2c_master_command_t command; /**< I2C command indicating the type of operation (START, WRITE, READ, or STOP) */
    union {
        /**
         * @brief Structure for WRITE command
         *
         * Used when the `command` is set to `I2C_MASTER_CMD_WRITE`.
         */
        struct {
            bool ack_check;        /**< Whether to enable ACK check during WRITE operation */
            uint8_t *data;         /**< Pointer to the data to be written */
            size_t total_bytes;    /**< Total number of bytes to write */
        } write;
        /**
         * @brief Structure for READ command
         *
         * Used when the `command` is set to `I2C_MASTER_CMD_READ`.
         */
        struct {
            i2c_ack_value_t ack_value; /**< ACK value to send after the read (ACK or NACK) */
            uint8_t *data;                    /**< Pointer to the buffer for storing the data read from the bus */
            size_t total_bytes;               /**< Total number of bytes to read */
        } read;
    };
} i2c_operation_job_t;

/**
 * @brief I2C master transmit buffer information structure
 */
typedef struct {
    uint8_t *write_buffer;               /*!< Pointer to buffer to be written. */
    size_t buffer_size;                  /*!< Size of data to be written. */
} i2c_master_transmit_multi_buffer_info_t;

/**
 * @brief Group of I2C master callbacks, can be used to get status during transaction or doing other small things. But take care potential concurrency issues.
 * @note The callbacks are all running under ISR context
 * @note When CONFIG_I2C_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       The variables used in the function should be in the SRAM as well.
 */
typedef struct {
    i2c_master_callback_t on_trans_done;  /*!< I2C master transaction finish callback */
} i2c_master_event_callbacks_t;

/**
 * @brief Allocate an I2C master bus
 *
 * @param[in] bus_config I2C master bus configuration.
 * @param[out] ret_bus_handle I2C bus handle
 * @return
 *      - ESP_OK: I2C master bus initialized successfully.
 *      - ESP_ERR_INVALID_ARG: I2C bus initialization failed because of invalid argument.
 *      - ESP_ERR_NO_MEM: Create I2C bus failed because of out of memory.
 *      - ESP_ERR_NOT_FOUND: No more free bus.
 */
esp_err_t i2c_new_master_bus(const i2c_master_bus_config_t *bus_config, i2c_master_bus_handle_t *ret_bus_handle);

/**
 * @brief Add I2C master BUS device.
 *
 * @param[in] bus_handle I2C bus handle.
 * @param[in] dev_config device config.
 * @param[out] ret_handle device handle.
 * @return
 *      - ESP_OK: Create I2C master device successfully.
 *      - ESP_ERR_INVALID_ARG: I2C bus initialization failed because of invalid argument.
 *      - ESP_ERR_NO_MEM: Create I2C bus failed because of out of memory.
 */
esp_err_t i2c_master_bus_add_device(i2c_master_bus_handle_t bus_handle, const i2c_device_config_t *dev_config, i2c_master_dev_handle_t *ret_handle);

/**
 * @brief Deinitialize the I2C master bus and delete the handle.
 *
 * @param[in] bus_handle I2C bus handle.
 * @return
 *      - ESP_OK: Delete I2C bus success, otherwise, failed.
 *      - Otherwise: Some module delete failed.
 */
esp_err_t i2c_del_master_bus(i2c_master_bus_handle_t bus_handle);

/**
 * @brief I2C master bus delete device
 *
 * @param handle i2c device handle
 * @return
 *      - ESP_OK: If device is successfully deleted.
 */
esp_err_t i2c_master_bus_rm_device(i2c_master_dev_handle_t handle);

/**
 * @brief Perform a write transaction on the I2C bus.
 *        The transaction will be undergoing until it finishes or it reaches
 *        the timeout provided.
 *
 * @note If a callback was registered with `i2c_master_register_event_callbacks`, the transaction will be asynchronous, and thus, this function will return directly, without blocking.
 *       You will get finish information from callback. Besides, data buffer should always be completely prepared when callback is registered, otherwise, the data will get corrupt.
 *
 * @param[in] i2c_dev I2C master device handle that created by `i2c_master_bus_add_device`.
 * @param[in] write_buffer Data bytes to send on the I2C bus.
 * @param[in] write_size Size, in bytes, of the write buffer.
 * @param[in] xfer_timeout_ms Wait timeout, in ms. Note: -1 means wait forever.
 * @return
 *      - ESP_OK: I2C master transmit success
 *      - ESP_ERR_INVALID_ARG: I2C master transmit parameter invalid.
 *      - ESP_ERR_TIMEOUT: Operation timeout(larger than xfer_timeout_ms) because the bus is busy or hardware crash.
 */
esp_err_t i2c_master_transmit(i2c_master_dev_handle_t i2c_dev, const uint8_t *write_buffer, size_t write_size, int xfer_timeout_ms);

/**
 * @brief Transmit multiple buffers of data over an I2C bus.
 *
 * This function transmits multiple buffers of data over an I2C bus using the specified I2C master device handle.
 * It takes in an array of buffer information structures along with the size of the array and a transfer timeout value in milliseconds.
 *
 * @param i2c_dev I2C master device handle that created by `i2c_master_bus_add_device`.
 * @param buffer_info_array Pointer to buffer information array.
 * @param array_size size of buffer information array.
 * @param xfer_timeout_ms Wait timeout, in ms. Note: -1 means wait forever.
 *
 * @return
 *      - ESP_OK: I2C master transmit success
 *      - ESP_ERR_INVALID_ARG: I2C master transmit parameter invalid.
 *      - ESP_ERR_TIMEOUT: Operation timeout(larger than xfer_timeout_ms) because the bus is busy or hardware crash.
 */
esp_err_t i2c_master_multi_buffer_transmit(i2c_master_dev_handle_t i2c_dev, i2c_master_transmit_multi_buffer_info_t *buffer_info_array, size_t array_size, int xfer_timeout_ms);

/**
 * @brief Perform a write-read transaction on the I2C bus.
 *        The transaction will be undergoing until it finishes or it reaches
 *        the timeout provided.
 *
 * @note If a callback was registered with `i2c_master_register_event_callbacks`, the transaction will be asynchronous, and thus, this function will return directly, without blocking.
 *       You will get finish information from callback. Besides, data buffer should always be completely prepared when callback is registered, otherwise, the data will get corrupt.
 *
 * @param[in] i2c_dev I2C master device handle that created by `i2c_master_bus_add_device`.
 * @param[in] write_buffer Data bytes to send on the I2C bus.
 * @param[in] write_size Size, in bytes, of the write buffer.
 * @param[out] read_buffer Data bytes received from i2c bus.
 * @param[in] read_size Size, in bytes, of the read buffer.
 * @param[in] xfer_timeout_ms Wait timeout, in ms. Note: -1 means wait forever.
 * @return
 *      - ESP_OK: I2C master transmit-receive success
 *      - ESP_ERR_INVALID_ARG: I2C master transmit parameter invalid.
 *      - ESP_ERR_TIMEOUT: Operation timeout(larger than xfer_timeout_ms) because the bus is busy or hardware crash.
 */
esp_err_t i2c_master_transmit_receive(i2c_master_dev_handle_t i2c_dev, const uint8_t *write_buffer, size_t write_size, uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms);

/**
 * @brief Perform a read transaction on the I2C bus.
 *        The transaction will be undergoing until it finishes or it reaches
 *        the timeout provided.
 *
 * @note If a callback was registered with `i2c_master_register_event_callbacks`, the transaction will be asynchronous, and thus, this function will return directly, without blocking.
 *       You will get finish information from callback. Besides, data buffer should always be completely prepared when callback is registered, otherwise, the data will get corrupt.
 *
 * @param[in] i2c_dev I2C master device handle that created by `i2c_master_bus_add_device`.
 * @param[out] read_buffer Data bytes received from i2c bus.
 * @param[in] read_size Size, in bytes, of the read buffer.
 * @param[in] xfer_timeout_ms Wait timeout, in ms. Note: -1 means wait forever.
 * @return
 *      - ESP_OK: I2C master receive success
 *      - ESP_ERR_INVALID_ARG: I2C master receive parameter invalid.
 *      - ESP_ERR_TIMEOUT: Operation timeout(larger than xfer_timeout_ms) because the bus is busy or hardware crash.
 */
esp_err_t i2c_master_receive(i2c_master_dev_handle_t i2c_dev, uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms);

/**
 * @brief Probe I2C address, if address is correct and ACK is received, this function will return ESP_OK.
 *
 * @param[in] bus_handle I2C master device handle that created by `i2c_master_bus_add_device`.
 * @param[in] address I2C device address that you want to probe.
 * @param[in] xfer_timeout_ms Wait timeout, in ms. Note: -1 means wait forever (Not recommended in this function).
 *
 * @attention Pull-ups must be connected to the SCL and SDA pins when this function is called. If you get `ESP_ERR_TIMEOUT
 * while `xfer_timeout_ms` was parsed correctly, you should check the pull-up resistors. If you do not have proper resistors nearby.
 * `flags.enable_internal_pullup` is also acceptable.
 *
 * @note The principle of this function is to sent device address with a write command. If the device on your I2C bus, there would be an ACK signal and function
 * returns `ESP_OK`. If the device is not on your I2C bus, there would be a NACK signal and function returns `ESP_ERR_NOT_FOUND`. `ESP_ERR_TIMEOUT` is not an expected
 * failure, which indicated that the i2c probe not works properly, usually caused by pull-up resistors not be connected properly. Suggestion check data on SDA/SCL line
 * to see whether there is ACK/NACK signal is on line when i2c probe function fails.
 *
 * @note There are lots of I2C devices all over the world, we assume that not all I2C device support the behavior like `device_address+nack/ack`.
 * So, if the on line data is strange and no ack/nack got respond. Please check the device datasheet.
 *
 * @return
 *      - ESP_OK: I2C device probe successfully
 *      - ESP_ERR_NOT_FOUND: I2C probe failed, doesn't find the device with specific address you gave.
 *      - ESP_ERR_TIMEOUT: Operation timeout(larger than xfer_timeout_ms) because the bus is busy or hardware crash.
 */
esp_err_t i2c_master_probe(i2c_master_bus_handle_t bus_handle, uint16_t address, int xfer_timeout_ms);

/**
 * @brief Execute a series of pre-defined I2C operations.
 *
 * This function processes a list of I2C operations, such as start, write, read, and stop,
 * according to the user-defined `i2c_operation_job_t` array. It performs these operations
 * sequentially on the specified I2C master device.
 *
 * @param[in] i2c_dev           Handle to the I2C master device.
 * @param[in] i2c_operation     Pointer to an array of user-defined I2C operation jobs.
 *                              Each job specifies a command and associated parameters.
 * @param[in] operation_list_num The number of operations in the `i2c_operation` array.
 * @param[in] xfer_timeout_ms   Timeout for the transaction, in milliseconds.
 *
 * @return
 *  - ESP_OK: Transaction completed successfully.
 *  - ESP_ERR_INVALID_ARG: One or more arguments are invalid.
 *  - ESP_ERR_TIMEOUT: Transaction timed out.
 *  - ESP_FAIL: Other error during transaction.
 *
 * @note The `ack_value` field in the READ operation must be set to `I2C_NACK_VAL` if the next
 *       operation is a STOP command.
 */
esp_err_t i2c_master_execute_defined_operations(i2c_master_dev_handle_t i2c_dev, i2c_operation_job_t *i2c_operation, size_t operation_list_num, int xfer_timeout_ms);

/**
 * @brief Register I2C transaction callbacks for a master device
 *
 * @note User can deregister a previously registered callback by calling this function and setting the callback member in the `cbs` structure to NULL.
 * @note When CONFIG_I2C_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       The variables used in the function should be in the SRAM as well. The `user_data` should also reside in SRAM.
 * @note If the callback is used for helping asynchronous transaction. On the same bus, only one device can be used for performing asynchronous operation.
 *
 * @param[in] i2c_dev I2C master device handle that created by `i2c_master_bus_add_device`.
 * @param[in] cbs Group of callback functions
 * @param[in] user_data User data, which will be passed to callback functions directly
 * @return
 *      - ESP_OK: Set I2C transaction callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set I2C transaction callbacks failed because of invalid argument
 *      - ESP_FAIL: Set I2C transaction callbacks failed because of other error
 */
esp_err_t i2c_master_register_event_callbacks(i2c_master_dev_handle_t i2c_dev, const i2c_master_event_callbacks_t *cbs, void *user_data);

/**
 * @brief Reset the I2C master bus.
 *
 * @param bus_handle I2C bus handle.
 * @return
 *      - ESP_OK: Reset succeed.
 *      - ESP_ERR_INVALID_ARG: I2C master bus handle is not initialized.
 *      - Otherwise: Reset failed.
 */
esp_err_t i2c_master_bus_reset(i2c_master_bus_handle_t bus_handle);

/**
 * @brief Change the I2C device address at runtime.
 *
 * This function updates the device address of an existing I2C device handle.
 * It is useful for devices that support dynamic address assignment or when
 * switching communication to a device with a different address on the same bus.
 *
 * @param[in] i2c_dev           I2C device handle.
 * @param[in] new_device_address The new device address.
 * @param[in] timeout_ms        Timeout for the address change operation, in milliseconds.
 *
 * @return
 *      - ESP_OK: Address successfully changed.
 *      - ESP_ERR_INVALID_ARG: Invalid arguments (e.g., NULL handle or invalid address).
 *      - ESP_ERR_TIMEOUT: Operation timed out.
 *
 * @note
 *      - This function does not send commands to the I2C device. It only updates
 *        the address used in subsequent transactions through the I2C handle.
 *      - Ensure that the new address is valid and does not conflict with other devices on the bus.
 */
esp_err_t i2c_master_device_change_address(i2c_master_dev_handle_t i2c_dev, uint16_t new_device_address, int timeout_ms);

/**
 * @brief Wait for all pending I2C transactions done
 *
 * @param[in] bus_handle I2C bus handle
 * @param[in] timeout_ms Wait timeout, in ms. Specially, -1 means to wait forever.
 * @return
 *      - ESP_OK: Flush transactions successfully
 *      - ESP_ERR_INVALID_ARG: Flush transactions failed because of invalid argument
 *      - ESP_ERR_TIMEOUT: Flush transactions failed because of timeout
 *      - ESP_FAIL: Flush transactions failed because of other error
 */
esp_err_t i2c_master_bus_wait_all_done(i2c_master_bus_handle_t bus_handle, int timeout_ms);

/**
 * @brief Retrieves the I2C master bus handle for a specified I2C port number.
 *
 * This function retrieves the I2C master bus handle for the
 * given I2C port number. Please make sure the handle has already been initialized, and this
 * function would simply returns the existing handle. Note that the returned handle still can't be used concurrently
 *
 * @param port_num I2C port number for which the handle is to be retrieved.
 * @param ret_handle Pointer to a variable where the retrieved handle will be stored.
 * @return
 *     - ESP_OK: Success. The handle is retrieved successfully.
 *     - ESP_ERR_INVALID_ARG: Invalid argument, such as invalid port number
 *     - ESP_ERR_INVALID_STATE: Invalid state, such as the I2C port is not initialized.
 */
esp_err_t i2c_master_get_bus_handle(i2c_port_num_t port_num, i2c_master_bus_handle_t *ret_handle);

#ifdef __cplusplus
}
#endif
