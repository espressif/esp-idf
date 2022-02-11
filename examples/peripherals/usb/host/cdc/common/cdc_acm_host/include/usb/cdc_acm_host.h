/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "usb_types_cdc.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cdc_dev_s *cdc_acm_dev_hdl_t;

/**
 * @brief Line Coding structure
 * @see Table 17, USB CDC-PSTN specification rev. 1.2
 */
typedef struct {
    uint32_t dwDTERate;  // in bits per second
    uint8_t bCharFormat; // 0: 1 stopbit, 1: 1.5 stopbits, 2: 2 stopbits
    uint8_t bParityType; // 0: None, 1: Odd, 2: Even, 3: Mark, 4: Space
    uint8_t bDataBits;   // 5, 6, 7, 8 or 16
} __attribute__((packed)) cdc_acm_line_coding_t;

/**
 * @brief UART State Bitmap
 * @see Table 31, USB CDC-PSTN specification rev. 1.2
 */
typedef union {
    struct {
        uint16_t bRxCarrier : 1;  // State of receiver carrier detection mechanism of device. This signal corresponds to V.24 signal 109 and RS-232 signal DCD.
        uint16_t bTxCarrier : 1;  // State of transmission carrier. This signal corresponds to V.24 signal 106 and RS-232 signal DSR.
        uint16_t bBreak : 1;      // State of break detection mechanism of the device.
        uint16_t bRingSignal : 1; // State of ring signal detection of the device.
        uint16_t bFraming : 1;    // A framing error has occurred.
        uint16_t bParity : 1;     // A parity error has occurred.
        uint16_t bOverRun : 1;    // Received data has been discarded due to overrun in the device.
        uint16_t reserved : 9;
    };
    uint16_t val;
} cdc_acm_uart_state_t;

/**
 * @brief CDC-ACM Device Event types to upper layer
 *
 */
typedef enum {
    CDC_ACM_HOST_ERROR,
    CDC_ACM_HOST_SERIAL_STATE,
    CDC_ACM_HOST_NETWORK_CONNECTION,
    CDC_ACM_HOST_DEVICE_DISCONNECTED
} cdc_acm_host_dev_event_t;

/**
 * @brief CDC-ACM Device Event data structure
 *
 */
typedef struct {
    cdc_acm_host_dev_event_t type;
    union {
        int error;                         // Error code from USB Host
        cdc_acm_uart_state_t serial_state; // Serial (UART) state
        bool network_connected;            // Network connection event
    } data;
} cdc_acm_host_dev_event_data_t;

/**
 * @brief Data receive callback type
 */
typedef void (*cdc_acm_data_callback_t)(uint8_t* data, size_t data_len, void *user_arg);

/**
 * @brief Device event callback type
 * @see cdc_acm_host_dev_event_t
 */
typedef void (*cdc_acm_host_dev_callback_t)(cdc_acm_dev_hdl_t cdc_hdl, const cdc_acm_host_dev_event_data_t *event, void *user_ctx);

/**
 * @brief Configuration structure of USB Host CDC-ACM driver
 *
 */
typedef struct {
    size_t driver_task_stack_size;         /**< Stack size of the driver's task */
    unsigned driver_task_priority;         /**< Priority of the driver's task */
    int  xCoreID;                          /**< Core affinity of the driver's task */
} cdc_acm_host_driver_config_t;

/**
 * @brief Configuration structure of CDC-ACM device
 *
 */
typedef struct {
    uint32_t connection_timeout_ms;       /**< Timeout for USB device connection in [ms] */
    size_t out_buffer_size;               /**< Maximum size of USB bulk out transfer, set to 0 for read-only devices */
    cdc_acm_host_dev_callback_t event_cb; /**< Device's event callback function. Can be NULL */
    cdc_acm_data_callback_t data_cb;      /**< Device's data RX callback function. Can be NULL for write-only devices */
    void *user_arg;                       /**< User's argument that will be passed to the callbacks */
} cdc_acm_host_device_config_t;

/**
 * @brief Install CDC-ACM driver
 *
 * - USB Host Library must already be installed before calling this function (via usb_host_install())
 * - This function should be called before calling any other CDC driver functions
 *
 * @param[in] driver_config Driver configuration structure. If set to NULL, a default configuration will be used.
 * @return esp_err_t
 */
esp_err_t cdc_acm_host_install(const cdc_acm_host_driver_config_t *driver_config);

/**
 * @brief Uninstall CDC-ACM driver
 *
 * - Users must ensure that all CDC devices must be closed via cdc_acm_host_close() before calling this function
 *
 * @return esp_err_t
 */
esp_err_t cdc_acm_host_uninstall(void);

/**
 * @brief Open CDC-ACM compliant device
 *
 * CDC-ACM compliant device must contain either an Interface Association Descriptor or CDC-Union descriptor,
 * which are used for the driver's configuration.
 *
 * @param[in] vid           Device's Vendor ID
 * @param[in] pid           Device's Product ID
 * @param[in] interface_idx Index of device's interface used for CDC-ACM communication
 * @param[in] dev_config    Configuration structure of the device
 * @param[out] cdc_hdl_ret  CDC device handle
 * @return esp_err_t
 */
esp_err_t cdc_acm_host_open(uint16_t vid, uint16_t pid, uint8_t interface_idx, const cdc_acm_host_device_config_t *dev_config, cdc_acm_dev_hdl_t *cdc_hdl_ret);

/**
 * @brief Open CDC-ACM non-compliant device
 *
 * CDC-ACM non-compliant device acts as CDC-ACM device but doesn't support all its features.
 * User must provide the interface index that will be used (zero for non-composite devices).
 *
 * @param[in] vid           Device's Vendor ID
 * @param[in] pid           Device's Product ID
 * @param[in] interface_idx Index of device's interface used for CDC-ACM like communication
 * @param[in] dev_config    Configuration structure of the device
 * @param[out] cdc_hdl_ret  CDC device handle
 * @return esp_err_t
 */
esp_err_t cdc_acm_host_open_vendor_specific(uint16_t vid, uint16_t pid, uint8_t interface_num, const cdc_acm_host_device_config_t *dev_config, cdc_acm_dev_hdl_t *cdc_hdl_ret);

/**
 * @brief Close CDC device and release its resources
 *
 * @note All in-flight transfers will be prematurely canceled.
 * @param cdc_hdl CDC handle obtained from cdc_acm_host_open()
 * @return esp_err_t
 */
esp_err_t cdc_acm_host_close(cdc_acm_dev_hdl_t cdc_hdl);

/**
 * @brief Transmit data - blocking mode
 *
 * @param cdc_hdl CDC handle obtained from cdc_acm_host_open()
 * @param[in] data       Data to be sent
 * @param[in] data_len   Data length
 * @param[in] timeout_ms Timeout in [ms]
 * @return esp_err_t
 */
esp_err_t cdc_acm_host_data_tx_blocking(cdc_acm_dev_hdl_t cdc_hdl, const uint8_t *data, size_t data_len, uint32_t timeout_ms);

/**
 * @brief SetLineCoding function
 *
 * @see Chapter 6.3.10, USB CDC-PSTN specification rev. 1.2
 *
 * @param     cdc_hdl     CDC handle obtained from cdc_acm_host_open()
 * @param[in] line_coding Line Coding structure
 * @return esp_err_t
 */
esp_err_t cdc_acm_host_line_coding_set(cdc_acm_dev_hdl_t cdc_hdl, const cdc_acm_line_coding_t *line_coding);

/**
 * @brief GetLineCoding function
 *
 * @see Chapter 6.3.11, USB CDC-PSTN specification rev. 1.2
 *
 * @param      cdc_hdl     CDC handle obtained from cdc_acm_host_open()
 * @param[out] line_coding Line Coding structure to be filled
 * @return esp_err_t
 */
esp_err_t cdc_acm_host_line_coding_get(cdc_acm_dev_hdl_t cdc_hdl, cdc_acm_line_coding_t *line_coding);

/**
 * @brief SetControlLineState function
 *
 * @see Chapter 6.3.12, USB CDC-PSTN specification rev. 1.2
 *
 * @param     cdc_hdl CDC handle obtained from cdc_acm_host_open()
 * @param[in] dtr     Indicates to DCE if DTE is present or not. This signal corresponds to V.24 signal 108/2 and RS-232 signal Data Terminal Ready.
 * @param[in] rts     Carrier control for half duplex modems. This signal corresponds to V.24 signal 105 and RS-232 signal Request To Send.
 * @return esp_err_t
 */
esp_err_t cdc_acm_host_set_control_line_state(cdc_acm_dev_hdl_t cdc_hdl, bool dtr, bool rts);

/**
 * @brief SendBreak function
 *
 * This function will block until the duration_ms has passed.
 *
 * @see Chapter 6.3.13, USB CDC-PSTN specification rev. 1.2
 *
 * @param     cdc_hdl     CDC handle obtained from cdc_acm_host_open()
 * @param[in] duration_ms Duration of the Break signal in [ms]
 * @return esp_err_t
 */
esp_err_t cdc_acm_host_send_break(cdc_acm_dev_hdl_t cdc_hdl, uint16_t duration_ms);

/**
 * @brief Print device's descriptors
 *
 * Device and full Configuration descriptors are printed in human readable format to stdout.
 *
 * @param cdc_hdl CDC handle obtained from cdc_acm_host_open()
 */
void cdc_acm_host_desc_print(cdc_acm_dev_hdl_t cdc_hdl);

/**
 * @brief Get protocols defined in USB-CDC interface descriptors
 *
 * @param cdc_hdl   CDC handle obtained from cdc_acm_host_open()
 * @param[out] comm Communication protocol
 * @param[out] data Data protocol
 * @return esp_err_t
 */
esp_err_t cdc_acm_host_protocols_get(cdc_acm_dev_hdl_t cdc_hdl, cdc_comm_protocol_t *comm, cdc_data_protocol_t *data);

#ifdef __cplusplus
}
class CdcAcmDevice
{
public:
    // Operators
    CdcAcmDevice() : cdc_hdl(NULL){};
    ~CdcAcmDevice()
    {
        // Close CDC-ACM device, if it wasn't explicitly closed
        if (this->cdc_hdl != NULL) {
            this->close();
        }
    }

    inline esp_err_t tx_blocking(uint8_t *data, size_t len, uint32_t timeout_ms = 100)
    {
        return cdc_acm_host_data_tx_blocking(this->cdc_hdl, data, len, timeout_ms);
    }

    inline esp_err_t open(uint16_t vid, uint16_t pid, uint8_t interface_idx, const cdc_acm_host_device_config_t* dev_config)
    {
        return cdc_acm_host_open(vid, pid, interface_idx, dev_config, &this->cdc_hdl);
    }

    inline esp_err_t open_vendor_specific(uint16_t vid, uint16_t pid, uint8_t interface_idx, const cdc_acm_host_device_config_t* dev_config)
    {
        return cdc_acm_host_open_vendor_specific(vid, pid, interface_idx, dev_config, &this->cdc_hdl);
    }

    inline void close()
    {
        cdc_acm_host_close(this->cdc_hdl);
        this->cdc_hdl = NULL;
    }

    inline esp_err_t line_coding_get(cdc_acm_line_coding_t *line_coding)
    {
        return cdc_acm_host_line_coding_get(this->cdc_hdl, line_coding);
    }

    inline esp_err_t line_coding_set(cdc_acm_line_coding_t *line_coding)
    {
        return cdc_acm_host_line_coding_set(this->cdc_hdl, line_coding);
    }

    inline esp_err_t set_control_line_state(bool dtr, bool rts)
    {
        return cdc_acm_host_set_control_line_state(this->cdc_hdl, dtr, rts);
    }

    inline esp_err_t send_break(uint16_t duration_ms)
    {
        return cdc_acm_host_send_break(this->cdc_hdl, duration_ms);
    }

private:
    CdcAcmDevice(const CdcAcmDevice &Copy);
    CdcAcmDevice &operator= (const CdcAcmDevice &Copy);
    bool operator== (const CdcAcmDevice &param) const;
    bool operator!= (const CdcAcmDevice &param) const;
    cdc_acm_dev_hdl_t cdc_hdl;
};
#endif
