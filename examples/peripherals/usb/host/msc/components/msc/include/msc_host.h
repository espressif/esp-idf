/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <wchar.h>
#include <stdint.h>
#include "esp_err.h"
#include <freertos/FreeRTOS.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_ERR_MSC_HOST_BASE        0x1700                      /*!< MSC host error code base */
#define ESP_ERR_MSC_MOUNT_FAILED    (ESP_ERR_MSC_HOST_BASE + 1)  /*!< Failed to mount storage */
#define ESP_ERR_MSC_FORMAT_FAILED   (ESP_ERR_MSC_HOST_BASE + 2)  /*!< Failed to format storage */
#define ESP_ERR_MSC_INTERNAL        (ESP_ERR_MSC_HOST_BASE + 3)  /*!< MSC host internal error */

#define MSC_STR_DESC_SIZE 32

typedef struct msc_host_device *msc_host_device_handle_t;     /**< Handle to a Mass Storage Device */

/**
 * @brief USB Mass Storage event containing event type and associated device handle.
*/
typedef struct {
    enum {
        MSC_DEVICE_CONNECTED,       /**< MSC device has been connected to the system.*/
        MSC_DEVICE_DISCONNECTED,    /**< MSC device has been disconnected from the system.*/
    } event;
    union {
        uint8_t address;                /**< Address of connected MSC device.*/
        msc_host_device_handle_t handle; /**< MSC device handle to disconnected device.*/
    } device;
} msc_host_event_t;

/**
 * @brief USB Mass Storage event callback.
 *
 * @param[in] event mass storage event
*/
typedef void (*msc_host_event_cb_t)(const msc_host_event_t *event, void *arg);

/**
 * @brief MSC configuration structure.
*/
typedef struct {
    bool create_backround_task;     /**< When set to true, background task handling usb events is created.
                                         Otherwise user has to periodically call msc_host_handle_events function */
    size_t task_priority;           /**< Task priority of crated background task */
    size_t stack_size;              /**< Stack size of crated background task */
    BaseType_t core_id;             /**< Select core on which background task will run or tskNO_AFFINITY  */
    msc_host_event_cb_t callback;   /**< Callback invoked when MSC event occurs. Must not be NULL. */
    void *callback_arg;             /**< User provided argument passed to callback */
} msc_host_driver_config_t;

/**
 * @brief MSC device info.
*/
typedef struct {
    uint32_t sector_count;
    uint32_t sector_size;
    uint16_t idProduct;
    uint16_t idVendor;
    wchar_t iManufacturer[MSC_STR_DESC_SIZE];
    wchar_t iProduct[MSC_STR_DESC_SIZE];
    wchar_t iSerialNumber[MSC_STR_DESC_SIZE];
} msc_host_device_info_t;

/**
 * @brief Install USB Host Mass Storage Class driver
 *
 * @param[in] config configuration structure MSC to create
 * @return esp_err_r
 */
esp_err_t msc_host_install(const msc_host_driver_config_t *config);

/**
 * @brief Uninstall Mass Storage Class driver
 * @return esp_err_t
 */
esp_err_t msc_host_uninstall(void);

/**
 * @brief Initialization of MSC device.
 *
 * @param[in]  device_address  Device address obtained from MSC callback provided upon connection and enumeration
 * @param[out] device          Mass storage device handle to be used for subsequent calls.
 * @return esp_err_t
 */
esp_err_t msc_host_install_device(uint8_t device_address, msc_host_device_handle_t *device);

/**
 * @brief Deinitialization of MSC device.
 *
 * @param[in]  device  Device handle obtained from msc_host_install_device function
 * @return esp_err_t
 */
esp_err_t msc_host_uninstall_device(msc_host_device_handle_t device);

/**
 * @brief Helper function for reading sector from mass storage device.
 *
 * @warning This call is not thread safe and should not be combined
 *          with accesses to storage through file system.
 *
 * @note  Provided sector and size cannot exceed
 *        sector_count and sector_size obtained from msc_host_device_info_t
 *
 * @param[in]  device Device handle
 * @param[in]  sector Number of sector to be read
 * @param[out] data   Buffer into which data will be written
 * @param[in]  size   Number of bytes to be read
 * @return esp_err_t
 */
esp_err_t msc_host_read_sector(msc_host_device_handle_t device, size_t sector, void *data, size_t size);

/**
 * @brief Helper function for writing sector to mass storage device.
 *
 * @warning This call is not thread safe and should not be combined
 *          with accesses to storare through file system.
 *
 * @note  Provided sector and size cannot exceed
 *        sector_count and sector_size obtained from msc_host_device_info_t
 *
 * @param[in]  device Device handle
 * @param[in]  sector Number of sector to be read
 * @param[in]  data   Data to be written to the sector
 * @param[in]  size   Number of bytes to be written
 * @return esp_err_t
 */
esp_err_t msc_host_write_sector(msc_host_device_handle_t device, size_t sector, const void *data, size_t size);

/**
 * @brief Handle MSC HOST events.
 *
 * @param[in]  timeout_ms  Timeout in miliseconds
 * @return esp_err_t
 */
esp_err_t msc_host_handle_events(uint32_t timeout_ms);

/**
 * @brief Gets devices information.
 *
 * @warning This call is not thread safe and should not be combined
 *          with accesses to storare through file system.
 *
 * @param[in]  device  Handle to device
 * @param[out] info  Structure to be populated with device info
 * @return esp_err_t
 */
esp_err_t msc_host_get_device_info(msc_host_device_handle_t device, msc_host_device_info_t *info);

/**
 * @brief Print configuration descriptor.
 *
 * @param[in]  device  Handle of MSC device
 * @return esp_err_t
 */
esp_err_t msc_host_print_descriptors(msc_host_device_handle_t device);

#ifdef __cplusplus
}
#endif //__cplusplus
