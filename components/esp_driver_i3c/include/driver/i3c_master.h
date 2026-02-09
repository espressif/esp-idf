/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/i3c_master_types.h"
#include "hal/i3c_master_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configuration structure for I3C master bus
 *
 * This structure defines the configuration parameters for the I3C master bus,
 * including GPIO pins, clock source, queue depth, interrupt priority, and other options.
 */
typedef struct {
    gpio_num_t sda_io_num;                                                   /*!< GPIO number of I3C SDA signal*/
    gpio_num_t scl_io_num;                                                   /*!< GPIO number of I3C SCL signal*/
    i3c_master_clock_source_t clock_source;                                  /*!< Clock source of I3C master bus */
    size_t trans_queue_depth;                                                /*!< Depth of internal transfer queue, increase this value can support more transfers pending in the background, only valid in asynchronous transaction. (Typically max_device_num * per_transaction)*/
    int intr_priority;                                                       /*!< I3C interrupt priority, if set to 0, driver will select the default priority (1,2,3). */
    uint32_t i3c_scl_freq_hz_od;                                             /*!< I3C SCL frequency for Open-Drain mode */
    uint32_t i3c_scl_freq_hz_pp;                                             /*!< I3C SCL frequency for Push-Pull mode */
    float i3c_scl_pp_duty_cycle;                                             /*!< Duty cycle of I3C Push-Pull SCL signal, range from (0, 1). When set 0, then duty cycle is 0.5 */
    float i3c_scl_od_duty_cycle;                                             /*!< Duty cycle of I3C Open-Drain SCL signal, range from (0, 1). When set 0, then duty cycle is 0.5 */
    uint32_t i3c_sda_od_hold_time_ns;                                        /*!< I3C Open-Drain sda drive point after scl neg, in nanoseconds, default 25 */
    uint32_t i3c_sda_pp_hold_time_ns;                                        /*!< I3C Push-Pull sda drive point after scl neg, in nanoseconds, default 0 */
    uint8_t entdaa_device_num;                                               /*!< Maximum number of devices can be discovered by ENTDAA, range from [0x0, 0x7F], 0x0 means no entdaa is used. */
    i3c_master_internal_pullup_resistor_val_t internal_pullup_resistor_val;  /*!< I3C internal pull-up resistor value (Please check program guide for the supported pins)*/
    struct {
        uint32_t enable_async_trans : 1;                                     /*!< Enable asynchronous transactions, allowing the master to perform other tasks while a transaction is in progress. Only works when DMA is enabled via i3c_master_bus_decorate_dma(). */
        uint32_t ibi_rstart_trans_en : 1;                                    /*!< Enable restart transaction on IBI, the I3C controller continues to execute the command that was previously interrupted by the IBI after the IBI is completed */
        uint32_t ibi_silent_sir_rejected : 1;                                /*!< If set, do not notify when a Slave Interrupt Request (SIR) is rejected */
        uint32_t ibi_no_auto_disable : 1;                                    /*!< If set, do not auto disable IBI, keep In-Band Interrupt enabled after the controller nack In-Band Interrupt */
    } flags;                                                                 /*!< I3C master config flags */
} i3c_master_bus_config_t;

/**
 * @brief Configuration structure for I3C master bus DMA decorator
 *
 * This structure defines the DMA configuration parameters for the I3C master bus.
 */
typedef struct {
    size_t max_transfer_size;                            /*!< Maximum transfer size in one transaction, in bytes. This decides the number of DMA nodes */
    size_t dma_burst_size;                               /*!< DMA burst size, in bytes. If 0, driver will use default value (16 bytes) */
} i3c_master_dma_config_t;

/**
 * @brief I3C device configuration used when adding a device via static address
 *
 * This structure contains addressing and characteristic information of an I3C device
 * as defined by the MIPI I3C specification. It is typically used to describe a device
 * to the bus before or during dynamic address assignment.
 */
typedef struct {
    uint8_t dynamic_addr;                               /*!< 7-bit dynamic address to assign for the device by SETDASA */
    uint8_t static_addr;                                /*!< 7-bit static address of the device, used for initial contact or assignment */
} i3c_device_i3c_config_t;

/**
 * @brief Parameters for a Common Command Code (CCC) transfer
 *
 * Describes a single CCC transaction, either broadcast or direct, with optional data phase.
 */
typedef struct {
    i3c_ccc_code_t ccc_command;                         /*!< CCC opcode */
    i3c_master_transfer_direction_t direction;          /*!< Direction of data phase: I3C_MASTER_TRANSFER_DIRECTION_READ for read, I3C_MASTER_TRANSFER_DIRECTION_WRITE for write */
    uint8_t device_address;                             /*!< 7-bit target address for direct CCC; ignored for broadcast CCC with specific CCC command */
    uint8_t *data;                                      /*!< Data buffer: receives data for read, provides data for write */
    size_t data_size;                                   /*!< Size of the data buffer in bytes */
} i3c_master_ccc_transfer_config_t;

/**
 * @brief Basic I3C device information discovered on the bus
 */
typedef struct {
    uint8_t dynamic_addr;                               /*!< Assigned dynamic address */
    uint8_t bcr;                                        /*!< Bus Characteristics Register (BCR) reported by the device */
    uint8_t dcr;                                        /*!< Device Characteristics Register (DCR) reported by the device */
    uint64_t pid;                                       /*!< Provisional ID (PID). Lower 48 bits are valid as per spec. */
} i3c_device_information_t;

/**
 * @brief In-Band Interrupt (IBI) configuration for an I3C device
 */
typedef struct {
    bool enable_ibi;                                    /*!< Enable reception/handling of IBI from this device */
    bool enable_ibi_payload;                            /*!< Expect/allow one-byte IBI payload from the device */
} i3c_ibi_config_t;

/**
 * @brief Group of I3C master callbacks, can be used to get status during transaction or doing other small things. But take care potential concurrency issues.
 * @note The callbacks are all running under ISR context
 * @note When CONFIG_I3C_MASTER_ISR_CACHE_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       The variables used in the function should be in the SRAM as well.
 */
typedef struct {
    i3c_master_i3c_callback_t on_trans_done;             /*!< I3C master transaction finish callback */
    i3c_master_ibi_callback_t on_ibi;                    /*!< I3C master IBI callback */
} i3c_master_i3c_event_callbacks_t;

/**
 * @brief Create a new I3C master bus.
 *
 * This function initializes a new I3C master bus with the provided configuration.
 *
 * @param[in] bus_config      Pointer to the I3C master bus configuration structure.
 * @param[out] ret_bus_handle Pointer to the location where the handle of the newly created bus will be stored.
 *
 * @return
 *      - ESP_OK: Bus created successfully.
 *      - ESP_ERR_INVALID_ARG: Invalid configuration or parameters.
 *      - ESP_ERR_NO_MEM: Memory allocation failed.
 */
esp_err_t i3c_new_master_bus(const i3c_master_bus_config_t *bus_config, i3c_master_bus_handle_t *ret_bus_handle);

/**
 * @brief Deletes an I3C master bus and releases associated resources.
 *
 * This function deinitializes and deletes the specified I3C master bus instance. It ensures
 * that all resources allocated for the bus are properly released. The caller must ensure
 * that no active operations are ongoing on the bus before invoking this function.
 *
 * @param bus_handle Handle to the I3C master bus to be deleted. This handle must have been
 *                   previously obtained from a successful bus initialization.
 *
 * @return
 *  - `ESP_OK`: The bus was successfully deleted.
 *  - `ESP_ERR_INVALID_ARG`: The provided `bus_handle` is invalid (e.g., null or uninitialized).
 */
esp_err_t i3c_del_master_bus(i3c_master_bus_handle_t bus_handle);

/**
 * @brief Enable DMA for I3C master bus (decorator pattern)
 *
 * This function enables DMA functionality for an existing I3C master bus. It follows the decorator
 * pattern to avoid linking DMA code when not needed, thus reducing binary size.
 *
 * @param[in] bus_handle  Handle to the I3C master bus to be decorated with DMA capability.
 * @param[in] dma_config  Pointer to the DMA configuration structure. If NULL, DMA will be disabled and fifo will be used.
 *
 * @return
 *      - ESP_OK: DMA decorator applied successfully.
 *      - ESP_ERR_INVALID_ARG: Invalid bus handle or configuration.
 *      - ESP_ERR_INVALID_STATE: Bus is already decorated with DMA or bus is in use.
 *      - ESP_ERR_NO_MEM: Memory allocation failed for DMA resources.
 */
esp_err_t i3c_master_bus_decorate_dma(i3c_master_bus_handle_t bus_handle, const i3c_master_dma_config_t *dma_config);

/**
 * @brief Wait for all pending I3C transactions done
 *
 * @param[in] bus_handle I3C bus handle
 * @param[in] timeout_ms Wait timeout, in ms. Specially, -1 means to wait forever.
 * @return
 *      - ESP_OK: Flush transactions successfully
 *      - ESP_ERR_INVALID_ARG: Flush transactions failed because of invalid argument
 *      - ESP_ERR_TIMEOUT: Flush transactions failed because of timeout
 */
esp_err_t i3c_master_bus_wait_all_done(i3c_master_bus_handle_t bus_handle, int timeout_ms);

/**
 * @brief Add an I3C device to the bus via its static address
 *
 * Creates and attaches a device object to the given I3C master bus using the device's
 * static address and characteristics. On success, a device handle is returned for
 * subsequent transactions.
 *
 * @param[in]  bus_handle  I3C master bus handle
 * @param[in]  dev_config  Pointer to the device configuration (static/dynamic address, BCR/DCR)
 * @param[out] ret_handle  Returned device handle on success
 * @return ESP_OK on success, otherwise an esp_err_t code indicating the failure
 */
esp_err_t i3c_master_bus_add_i3c_static_device(i3c_master_bus_handle_t bus_handle, const i3c_device_i3c_config_t *dev_config, i3c_master_i3c_device_handle_t *ret_handle);

/**
 * @brief Perform a Common Command Code (CCC) transfer
 *
 * Executes a CCC transaction (broadcast or direct) described by the provided
 * transfer descriptor. The call is synchronous and returns when the transfer
 * completes or an error/timeout occurs.
 *
 * @param[in] bus_handle I3C master bus handle
 * @param[in,out] ccc_trans Transfer descriptor; for read operations, the data buffer receives data
 * @return ESP_OK on success, otherwise an esp_err_t code indicating the failure
 */
esp_err_t i3c_master_transfer_ccc(i3c_master_bus_handle_t bus_handle, const i3c_master_ccc_transfer_config_t *ccc_trans);

/**
 * @brief Transmit data to an I3C device
 *
 * Performs a write transfer to the target I3C device.
 *
 * @param[in] dev_handle        I3C device handle
 * @param[in] write_buffer      Pointer to the data to send
 * @param[in] write_size        Number of bytes to send
 * @param[in] xfer_timeout_ms   Timeout for the transfer in milliseconds (-1 for wait forever)
 * @return ESP_OK on success, otherwise an esp_err_t code indicating the failure
 */
esp_err_t i3c_master_i3c_device_transmit(i3c_master_i3c_device_handle_t dev_handle, const uint8_t *write_buffer, size_t write_size, int xfer_timeout_ms);

/**
 * @brief Receive data from an I3C device
 *
 * Performs a read transfer from the target I3C device.
 *
 * @param[in]  dev_handle       I3C device handle
 * @param[out] read_buffer      Buffer to store the received data
 * @param[in]  read_size        Number of bytes to read
 * @param[in]  xfer_timeout_ms  Timeout for the transfer in milliseconds (-1 for wait forever)
 * @return ESP_OK on success, otherwise an esp_err_t code indicating the failure
 */
esp_err_t i3c_master_i3c_device_receive(i3c_master_i3c_device_handle_t dev_handle, uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms);

/**
 * @brief Combined transmit-then-receive transaction with a repeated START
 *
 * Writes a small command or register address to the device, then reads data back
 * without releasing the bus (no STOP between phases).
 *
 * @param[in]  dev_handle       I3C device handle
 * @param[in]  write_buffer     Data to transmit first
 * @param[in]  write_size       Number of bytes to transmit
 * @param[out] read_buffer      Buffer to receive data
 * @param[in]  read_size        Number of bytes to read
 * @param[in]  xfer_timeout_ms  Timeout for the transaction in milliseconds (-1 for wait forever)
 * @return ESP_OK on success, otherwise an esp_err_t code indicating the failure
 */
esp_err_t i3c_master_i3c_device_transmit_receive(i3c_master_i3c_device_handle_t dev_handle, const uint8_t *write_buffer, size_t write_size, uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms);

/**
 * @brief Get basic information of an I3C device
 *
 * Queries the driver for device attributes such as dynamic address, BCR/DCR and PID.
 *
 * @param[in]  dev_handle I3C device handle
 * @param[out] info       Pointer to a structure to receive the device information
 * @return ESP_OK on success, otherwise an esp_err_t code indicating the failure
 */
esp_err_t i3c_master_i3c_device_get_info(i3c_master_i3c_device_handle_t dev_handle, i3c_device_information_t *info);

/**
 * @brief Configure In-Band Interrupt (IBI) behavior for a device
 *
 * Enables/disables IBI reception and optional payload handling for the specified device.
 *
 * @param[in] dev_handle I3C device handle
 * @param[in] config     Pointer to the IBI configuration
 * @return ESP_OK on success, otherwise an esp_err_t code indicating the failure
 */
esp_err_t i3c_master_i3c_device_ibi_config(i3c_master_i3c_device_handle_t dev_handle, const i3c_ibi_config_t *config);

/**
 * @brief Register I3C device event callbacks
 *
 * Installs callbacks for transaction completion and IBI events.
 * Callbacks are invoked in ISR context; ensure the code is IRAM-safe if required by config.
 *
 * @param[in] dev_handle I3C device handle
 * @param[in] cbs        Pointer to the callback structure
 * @param[in] user_data  User-provided context pointer passed back on callbacks
 * @return ESP_OK on success, otherwise an esp_err_t code indicating the failure
 */
esp_err_t i3c_master_i3c_device_register_event_callbacks(i3c_master_i3c_device_handle_t dev_handle, const i3c_master_i3c_event_callbacks_t *cbs, void *user_data);

/**
 * @brief Remove an I3C device from the bus
 *
 * Detaches the specified device from the master bus and releases associated resources.
 *
 * @param[in] dev_handle I3C device handle to remove
 * @return ESP_OK on success, otherwise an esp_err_t code indicating the failure
 */
esp_err_t i3c_master_bus_rm_i3c_device(i3c_master_i3c_device_handle_t dev_handle);

/**
 * @brief Discover I3C devices via ENTDAA and return a table of device handles
 *
 * Runs the ENTDAA (Enter Dynamic Address Assignment) procedure to discover I3C devices
 * on the bus and returns a newly allocated device table handle.
 *
 * @param[in]  bus_handle      I3C master bus handle
 * @param[out] ret_table_handle Pointer to receive the device table handle (allocated on success)
 * @return ESP_OK on success, otherwise an esp_err_t code indicating the failure
 */
esp_err_t i3c_master_scan_devices_by_entdaa(i3c_master_bus_handle_t bus_handle, i3c_master_i3c_device_table_handle_t *ret_table_handle);

/**
 * @brief Free device handle table allocated by discovery
 *
 * Frees the device table handle previously returned by i3c_master_scan_devices_by_entdaa().
 *
 * @param[in] table_handle Device table handle to free
 * @return ESP_OK on success, otherwise an esp_err_t code indicating the failure
 */
esp_err_t i3c_master_free_device_handle_table(i3c_master_i3c_device_table_handle_t table_handle);

/**
 * @brief Get the number of devices in the device table
 *
 * Returns the count of I3C devices stored in the device table handle.
 *
 * @param[in]  table_handle Device table handle
 * @param[out] device_count Pointer to store the device count
 * @return ESP_OK on success, otherwise an esp_err_t code indicating the failure
 */
esp_err_t i3c_master_get_device_count(i3c_master_i3c_device_table_handle_t table_handle, size_t *device_count);

/**
 * @brief Get device handle by index from the device table
 *
 * Retrieves the I3C device handle at the specified index in the device table.
 *
 * @param[in]  table_handle Device table handle
 * @param[in]  index         Index of the device in the table (0-based)
 * @param[out] ret_handle    Pointer to store the device handle
 * @return ESP_OK on success, otherwise an esp_err_t code indicating the failure
 */
esp_err_t i3c_master_get_device_handle(i3c_master_i3c_device_table_handle_t table_handle, size_t index, i3c_master_i3c_device_handle_t *ret_handle);

/**
 * @brief Get a valid address slot for dynamic address assignment
 *
 * @param[in] bus_handle I3C master bus handle
 * @param[in] start_addr  Start address to search from
 * @param[out] ret_addr  Returned valid address slot
 * @return ESP_OK on success, otherwise an esp_err_t code indicating the failure
 */
esp_err_t i3c_master_get_valid_address_slot(i3c_master_bus_handle_t bus_handle, uint8_t start_addr, uint8_t *ret_addr);

#ifdef __cplusplus
}
#endif
