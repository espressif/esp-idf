/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include "usb/cdc_acm_host.h"

#define CP210X_PID       (0xEA60) // Single i.e. CP2101 - CP2104
#define CP2105_PID       (0xEA70) // Dual
#define CP2108_PID       (0xEA71) // Quad

// @see AN571: CP210x Virtual COM Port Interface, chapter 5
#define CP210X_CMD_IFC_ENABLE      (0x00) // Enable or disable the interface
#define CP210X_CMD_SET_BAUDDIV     (0x01) // Set the baud rate divisor
#define CP210X_CMD_GET_BAUDDIV     (0x02) // Get the baud rate divisor
#define CP210X_CMD_SET_LINE_CTL    (0x03) // Set the line control
#define CP210X_CMD_GET_LINE_CTL    (0x04) // Get the line control
#define CP210X_CMD_SET_BREAK       (0x05) // Set a BREAK
#define CP210X_CMD_IMM_CHAR        (0x06) // Send character out of order
#define CP210X_CMD_SET_MHS         (0x07) // Set modem handshaking
#define CP210X_CMD_GET_MDMSTS      (0x08) // Get modem status
#define CP210X_CMD_SET_XON         (0x09) // Emulate XON
#define CP210X_CMD_SET_XOFF        (0x0A) // Emulate XOFF
#define CP210X_CMD_SET_EVENTMASK   (0x0B) // Set the event mask
#define CP210X_CMD_GET_EVENTMASK   (0x0C) // Get the event mask
#define CP210X_CMD_GET_EVENTSTATE  (0x16) // Get the event state
#define CP210X_CMD_SET_RECEIVE     (0x17) // Set receiver max timeout
#define CP210X_CMD_GET_RECEIVE     (0x18) // Get receiver max timeout
#define CP210X_CMD_SET_CHAR        (0x0D) // Set special character individually
#define CP210X_CMD_GET_CHARS       (0x0E) // Get special characters
#define CP210X_CMD_GET_PROPS       (0x0F) // Get properties
#define CP210X_CMD_GET_COMM_STATUS (0x10) // Get the serial status
#define CP210X_CMD_RESET           (0x11) // Reset
#define CP210X_CMD_PURGE           (0x12) // Purge
#define CP210X_CMD_SET_FLOW        (0x13) // Set flow control
#define CP210X_CMD_GET_FLOW        (0x14) // Get flow control
#define CP210X_CMD_EMBED_EVENTS    (0x15) // Control embedding of events in the data stream
#define CP210X_CMD_GET_BAUDRATE    (0x1D) // Get the baud rate
#define CP210X_CMD_SET_BAUDRATE    (0x1E) // Set the baud rate
#define CP210X_CMD_SET_CHARS       (0x19) // Set special characters
#define CP210X_CMD_VENDOR_SPECIFIC (0xFF) // Read/write latch values

namespace esp_usb {
class CP210x : public CdcAcmDevice {
public:
    /**
     * @brief Factory method for this CP210x driver
     *
     * @note USB Host library and CDC-ACM driver must be already installed
     *
     * @param[in] pid            PID eg. CP210X_PID
     * @param[in] dev_config     CDC device configuration
     * @param[in] interface_idx  Interface number
     * @return CP210x            Pointer to created and opened CP210x device
     */
    static CP210x *open_cp210x(uint16_t pid, const cdc_acm_host_device_config_t *dev_config, uint8_t interface_idx = 0);

    /**
     * @brief Get Line Coding method
     *
     * @see AN571: CP210x Virtual COM Port Interface chapters 5.6 and 5.8
     * @note Overrides default implementation in CDC-ACM driver
     * @param[out] line_coding Line Coding structure
     * @return esp_err_t
     */
    esp_err_t line_coding_get(cdc_acm_line_coding_t *line_coding);

    /**
     * @brief Set Line Coding method
     *
     * @see AN571: CP210x Virtual COM Port Interface chapters 5.5 and 5.7
     * @note Overrides default implementation in CDC-ACM driver
     * @param[in] line_coding Line Coding structure
     * @return esp_err_t
     */
    esp_err_t line_coding_set(cdc_acm_line_coding_t *line_coding);

    /**
     * @brief Set Control Line State method
     *
     * @see AN571: CP210x Virtual COM Port Interface chapter 5.9
     * @note Overrides default implementation in CDC-ACM driver
     * @note Both signals are active low
     * @param[in] dtr Indicates to DCE if DTE is present or not. This signal corresponds to V.24 signal 108/2 and RS-232 signal Data Terminal Ready.
     * @param[in] rts Carrier control for half duplex modems. This signal corresponds to V.24 signal 105 and RS-232 signal Request To Send.
     * @return esp_err_t
     */
    esp_err_t set_control_line_state(bool dtr, bool rts);

    /**
     * @brief Send Break method
     *
     * @see AN571: CP210x Virtual COM Port Interface chapter 5.20
     * @note Overrides default implementation in CDC-ACM driver
     * @param[in] duration_ms Duration of the break condition in [ms]
     * @return esp_err_t
     */
    esp_err_t send_break(uint16_t duration_ms);

private:
    const uint8_t intf;

    // Constructors are private, use factory method to create this object
    CP210x();
    CP210x(uint16_t pid, const cdc_acm_host_device_config_t *dev_config, uint8_t interface_idx = 0);

    // Make open functions from CdcAcmDevice class private
    using CdcAcmDevice::open;
    using CdcAcmDevice::open_vendor_specific;
};
} // namespace esp_usb
