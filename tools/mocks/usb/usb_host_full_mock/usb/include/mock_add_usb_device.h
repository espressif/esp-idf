/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "usb_host.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Add mocked USB device to device list
 *
 * @param[in] dev_address device address
 * @param[in] dev_desc device descriptor
 * @param[in] config_desc configuration descriptor
 *
 * @return
 *    - ESP_OK: Mocked device added successfully
 *    - ESP_ERR_INVALID_ARG: Invalid input argument(s)
 */
esp_err_t usb_host_mock_add_device(uint8_t dev_address, const usb_device_desc_t *dev_desc, const usb_config_desc_t *config_desc);

/**
 * @brief Get number of mocked USB devices in device list
 *
 * @return number of devices in devices list
 */
int usb_host_mock_get_devs_count(void);

/**
 * @brief Print mocked devices info
 * @note if 0xFF is passed as dev_address, function prints all the mocked devices
 *
 * - printf all info about mocked device(s)
 * - device handler, device address, opened status, device descriptor, configuration descriptor
 *
 * @param[in] dev_address device address (if 0xFF is passed as dev_address, function prints all the mocked devices)
 *
 * @return
 *    - ESP_OK: Mocked device found and info printed
 *    - ESP_ERR_INVALID_SIZE: Mocked devices list empty
 *    - ESP_ERR_INVALID_ARG: Mocked device does not exist at the provided address
 */
esp_err_t usb_host_mock_print_mocked_devices(uint8_t dev_address);

/**
 * @brief Initialize mocked devices list before use
 */
void usb_host_mock_dev_list_init(void);

/**
 * @brief Get Mocked device config descriptor by address
 *
 * @param[in] dev_address device address
 * @param[in] config_desc configuration descriptor
 *
 * @return
 *    - ESP_OK: Configuration descriptor successfully provided
 *    - ESP_ERR_INVALID_ARG: Invalid input argument(s), or Mocked device does not exist at the provided address
 *    - ESP_ERR_INVALID_SIZE: Mocked devices list empty
 */
esp_err_t usb_host_mock_get_config_descriptor_by_address(uint8_t dev_address, const usb_config_desc_t **config_desc);

/**
 * @brief Get Mocked device device descriptor by address
 *
 * @param[in] dev_address device address
 * @param[in] config_desc device descriptor
 *
 * @return
 *    - ESP_OK: Device descriptor successfully provided
 *    - ESP_ERR_INVALID_ARG: Invalid input argument(s), or Mocked device does not exist at the provided address
 *    - ESP_ERR_INVALID_SIZE: Mocked devices list empty
 */
esp_err_t usb_host_mock_get_device_descriptor_by_address(uint8_t dev_address, const usb_device_desc_t **device_desc);

// ------------------------------------------------- CMock callback functions ------------------------------------------

/**
 * @brief Allocate a transfer object
 * @note CMock callback function, registered to usb_host_transfer_alloc()
 *
 * @param[in] data_buffer_size Size of the transfer's data buffer
 * @param[in] num_isoc_packets Number of isochronous packets in transfer (set to 0 for non-isochronous transfers)
 * @param[out] transfer Transfer object
 * @param[out] call_count Call count of the callback function (CMock mandatory argument)
 *
 * @return
 *    - ESP_OK: Transfer object allocated successfully
 *    - ESP_ERR_NO_MEM: Insufficient memory
 */
esp_err_t usb_host_transfer_alloc_mock_callback(size_t data_buffer_size, int num_isoc_packets, usb_transfer_t **transfer, int call_count);

/**
 * @brief Free a transfer object
 * @note CMock callback function, registered to usb_host_transfer_free()
 *
 * @param[in] transfer Transfer object
 * @param[out] call_count Call count of the callback function (CMock mandatory argument)
 *
 * @return
 *    - ESP_OK: Transfer object freed successfully
 */
esp_err_t usb_host_transfer_free_mock_callback(usb_transfer_t *transfer, int call_count);

/**
 * @brief Register Mocked USB Host client
 * @note CMock callback function, registered to usb_host_client_register()
 *
 * @param[in] client_config USB Host client config
 * @param[out] client_hdl_ret USB Host client handle
 * @param[out] call_count Call count of the callback function (CMock mandatory argument)
 *
 * @return
 *    - ESP_OK: Client registered successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_NO_MEM: Insufficient memory
 */
esp_err_t usb_host_client_register_mock_callback(const usb_host_client_config_t *client_config, usb_host_client_handle_t *client_hdl_ret, int call_count);

/**
 * @brief Mocked USB Host Library client processing function
 * @note CMock callback function, registered to usb_host_client_handle_events()
 *
 * - This function just blocks indefinitely, so the cdc_acm_client_task() does not loop
 * - If we only had usb_host_client_handle_events_ExpectAnyArgsAndReturn(ESP_OK) instead of this callback, the cdc_acm_client_task() would loop infinitely
 * - The only event implemented (for now) is client unblock, while uninstalling CDC-ACM driver
 *
 * @note This function can block
 * @param[in] client_hdl Client handle
 * @param[in] timeout_ticks Timeout in ticks to wait for an event to occur
 * @param[out] call_count Call count of the callback function (CMock mandatory argument)
 *
 * @return
 *    - ESP_OK: No event to be handled
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_TIMEOUT: Semaphore waiting for events has timed out
 */
esp_err_t usb_host_client_handle_events_mock_callback(usb_host_client_handle_t client_hdl, TickType_t timeout_ticks, int call_count);

/**
 * @brief Unblock a Mocked client
 * @note CMock callback function, registered to usb_host_client_unblock()
 *
 * @param[in] client_hdl Client handle
 * @param[out] call_count Call count of the callback function (CMock mandatory argument)
 *
 * @return
 *    - ESP_OK: Client unblocked successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usb_host_client_unblock_mock_callback(usb_host_client_handle_t client_hdl, int call_count);

/**
 * @brief Deregister a Mocked client
 * @note CMock callback function, registered to usb_host_client_deregister()
 *
 * @param[in] client_hdl Client handle
 * @param[out] call_count Call count of the callback function (CMock mandatory argument)
 *
 * @return
 *    - ESP_OK: Client deregistered successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usb_host_client_deregister_mock_callback(usb_host_client_handle_t client_hdl, int call_count);

/**
 * @brief Open a device
 * @note CMock callback function, registered to usb_host_device_open()
 *
 * @param[in] client_hdl Client handle
 * @param[in] dev_address Device's address
 * @param[out] dev_hdl_ret Device's handle
 * @param[out] call_count Call count of the callback function (CMock mandatory argument)
 *
 * @return
 *    - ESP_OK: Device opened successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_NOT_FOUND: Device with specified dev_address not found
 */
esp_err_t usb_host_device_open_mock_callback(usb_host_client_handle_t client_hdl, uint8_t dev_address, usb_device_handle_t *dev_hdl_ret, int call_count);

/**
 * @brief Close a device
 * @note CMock callback function, registered to usb_host_device_close()
 *
 * @param[in] client_hdl Client handle
 * @param[in] dev_hdl Device's handle
 * @param[out] call_count Call count of the callback function (CMock mandatory argument)
 *
 * @return
 *    - ESP_OK: Device closed successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_INVALID_STATE: Device was never opened
 */
esp_err_t usb_host_device_close_mock_callback(usb_host_client_handle_t client_hdl, usb_device_handle_t dev_hdl, int call_count);

/**
 * @brief Fill a list of device address
 * @note CMock callback function, registered to usb_host_device_addr_list_fill()
 *
 * @param[in] list_len Length of the empty list
 * @param[inout] dev_addr_list Empty list to be filled
 * @param[out] num_dev_ret Number of devices
 * @param[out] call_count Call count of the callback function (CMock mandatory argument)
 *
 * @return
 *    - ESP_OK: Device list filled successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usb_host_device_addr_list_fill_mock_callback(int list_len, uint8_t *dev_addr_list, int *num_dev_ret, int call_count);

/**
 * @brief Get device's device descriptor
 * @note CMock callback function, registered to usb_host_get_device_descriptor()
 *
 * @param[in] dev_hdl Device handle
 * @param[out] device_desc Device descriptor
 * @param[out] call_count Call count of the callback function (CMock mandatory argument)
 *
 * @return
 *    - ESP_OK: Device descriptor obtained successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usb_host_get_device_descriptor_mock_callback(usb_device_handle_t dev_hdl, const usb_device_desc_t **device_desc, int call_count);

/**
 * @brief Get device's active configuration descriptor
 * @note CMock callback function, registered to usb_host_get_active_config_descriptor()
 *
 * @param[in] dev_hdl Device handle
 * @param[out] config_desc Configuration descriptor
 * @param[out] call_count Call count of the callback function (CMock mandatory argument)
 *
 * @return
 *    - ESP_OK: Active configuration descriptor obtained successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usb_host_get_active_config_descriptor_mock_callback(usb_device_handle_t dev_hdl, const usb_config_desc_t **config_desc, int call_count);

/**
 * @brief Submit data transfer successfully
 * @note CMock callback function, registered to usb_host_transfer_submit()
 *
 * Transfer callback is called, transfer status marked as completed, correct number of transferred bytes
 *
 * @param[in] transfer Pointer to USB transfer
 * @param[out] call_count Call count of the callback function (CMock mandatory argument)
 *
 * @return
 *    - ESP_OK: Transfer submitted
 *    - ESP_ERR_INVALID_ARG: Invalid input argument
 */
esp_err_t usb_host_transfer_submit_success_mock_callback(usb_transfer_t *transfer, int call_count);

/**
 * @brief Submit data transfer, but fail to deliver
 * @note CMock callback function, registered to usb_host_transfer_submit()
 *
 * Transfer callback is called, transfer status marked as error, incorrect number of transferred bytes
 *
 * @param[in] transfer Pointer to USB transfer
 * @param[out] call_count Call count of the callback function (CMock mandatory argument)
 *
 * @return
 *    - ESP_OK: Transfer submitted
 *    - ESP_ERR_INVALID_ARG: Invalid input argument
 */
esp_err_t usb_host_transfer_submit_invalid_response_mock_callback(usb_transfer_t *transfer, int call_count);

/**
 * @brief Submit data transfer, time the transfer out
 * @note CMock callback function, registered to usb_host_transfer_submit()
 *
 * Transfer callback is not called, because transfer timeout is not implemented in the USB Host Library
 *
 * @param[in] transfer Pointer to USB transfer
 * @param[out] call_count Call count of the callback function (CMock mandatory argument)
 *
 * @return
 *    - ESP_OK: Transfer submitted
 *    - ESP_ERR_INVALID_ARG: Invalid input argument
 */
esp_err_t usb_host_transfer_submit_timeout_mock_callback(usb_transfer_t *transfer, int call_count);

/**
 * @brief Submit CTRL transfer, success
 * @note CMock callback function, registered to usb_host_transfer_submit_control()
 *
 * Transfer callback is called, transfer status marked as completed, correct number of transferred bytes
 *
 * @param[in] client_hdl Client handle
 * @param[in] transfer Pointer to USB transfer
 * @param[out] call_count Call count of the callback function (CMock mandatory argument)
 *
 * @return
 *    - ESP_OK: Transfer submitted
 *    - ESP_ERR_INVALID_ARG: Invalid input argument
 */
esp_err_t usb_host_transfer_submit_control_success_mock_callback(usb_host_client_handle_t client_hdl, usb_transfer_t *transfer, int call_count);

/**
 * @brief Submit CTRL transfer, error
 * @note CMock callback function, registered to usb_host_transfer_submit_control()
 *
 * Transfer callback is called, transfer status marked as error, incorrect number of transferred bytes
 *
 * @param[in] client_hdl Client handle
 * @param[in] transfer Pointer to USB transfer
 * @param[out] call_count Call count of the callback function (CMock mandatory argument)
 *
 * @return
 *    - ESP_OK: Transfer submitted
 *    - ESP_ERR_INVALID_ARG: Invalid input argument
 */
esp_err_t usb_host_transfer_submit_control_invalid_response_mock_callback(usb_host_client_handle_t client_hdl, usb_transfer_t *transfer, int call_count);

/**
 * @brief Submit CTRL transfer, time the transfer out
 * @note CMock callback function, registered to usb_host_transfer_submit_control()
 *
 * Transfer callback is not called, because transfer timeout is not implemented in the USB Host Library
 *
 * @param[in] client_hdl Client handle
 * @param[in] transfer Pointer to USB transfer
 * @param[out] call_count Call count of the callback function (CMock mandatory argument)
 *
 * @return
 *    - ESP_OK: Transfer submitted
 *    - ESP_ERR_INVALID_ARG: Invalid input argument
 */
esp_err_t usb_host_transfer_submit_control_timeout_mock_callback(usb_host_client_handle_t client_hdl, usb_transfer_t *transfer, int call_count);

/**
 * @brief USB Host device info mock callback
 * @note CMock callback function registered to usb_host_device_info()
 *
 * @param[in]  dev_hdl    Device handle
 * @param[out] dev_info   Device information
 * @param[out] call_count Call count of the callback function (CMock mandatory argument)
 *
 * @return
 *    - ESP_OK: Device information obtained successfully
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t usb_host_device_info_mock_callback(usb_device_handle_t dev_hdl, usb_device_info_t *dev_info, int call_count);

#ifdef __cplusplus
}
#endif
