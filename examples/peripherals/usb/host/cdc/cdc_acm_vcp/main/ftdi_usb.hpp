/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include "usb/cdc_acm_host.h"

#define FTDI_FT232_PID       (0x6001)
#define FTDI_FT231_PID       (0x6015)

#define FTDI_CMD_RESET        (0x00)
#define FTDI_CMD_SET_FLOW     (0x01)
#define FTDI_CMD_SET_MHS      (0x02) // Modem hanshaking
#define FTDI_CMD_SET_BAUDRATE (0x03)
#define FTDI_CMD_SET_LINE_CTL (0x04)
#define FTDI_CMD_GET_MDMSTS   (0x05) // Modem status

namespace esp_usb {
class FT23x : public CdcAcmDevice {
public:
    /**
     * @brief Factory method for this FTDI driver
     *
     * @note USB Host library and CDC-ACM driver must be already installed
     *
     * @param[in] pid            PID eg. FTDI_FT232_PID
     * @param[in] dev_config     CDC device configuration
     * @param[in] interface_idx  Interface number
     * @return FT23x             Pointer to created and opened FTDI device
     */
    static FT23x *open_ftdi(uint16_t pid, const cdc_acm_host_device_config_t *dev_config, uint8_t interface_idx = 0);

    /**
     * @brief Set Line Coding method
     *
     * @note Overrides default implementation in CDC-ACM driver
     * @param[in] line_coding Line Coding structure
     * @return esp_err_t
     */
    esp_err_t line_coding_set(cdc_acm_line_coding_t *line_coding);

    /**
     * @brief Set Control Line State method
     *
     * @note Overrides default implementation in CDC-ACM driver
     * @note Both signals are active low
     * @param[in] dtr Indicates to DCE if DTE is present or not. This signal corresponds to V.24 signal 108/2 and RS-232 signal Data Terminal Ready.
     * @param[in] rts Carrier control for half duplex modems. This signal corresponds to V.24 signal 105 and RS-232 signal Request To Send.
     * @return esp_err_t
     */
    esp_err_t set_control_line_state(bool dtr, bool rts);

private:
    /**
     * @brief FT23x's RX data handler
     *
     * First two bytes are status bytes, the RX data start at data[2].
     * Coding of status bytes:
     * Byte 0:
     *      Bit 0: Full Speed packet
     *      Bit 1: High Speed packet
     *      Bit 4: CTS
     *      Bit 5: DSR
     *      Bit 6: RI
     *      Bit 7: DCD
     * Byte 1:
     *      Bit 1: RX overflow
     *      Bit 2: Parity error
     *      Bit 3: Framing error
     *      Bit 4: Break received
     *      Bit 5: Transmitter holding register empty
     *      Bit 6: Transmitter empty
     *
     * @todo When CTS is asserted, this driver should stop sending data.
     *
     * @param[in] data     Received data
     * @param[in] data_len Received data length
     * @param[in] user_arg Pointer to FT23x class
     */
    static void ftdi_rx(uint8_t* data, size_t data_len, void *user_arg);

    // Just a wrapper to recover user's argument
    static void ftdi_event(const cdc_acm_host_dev_event_data_t *event, void *user_ctx);

    /**
     * @brief Construct a new calculate baudrate object
     *
     * A Baud rate for the FT232R, FT2232 (UART mode) or FT232B is generated using the chips
     * internal 48MHz clock. This is input to Baud rate generator circuitry where it is then divided by 16
     * and fed into a prescaler as a 3MHz reference clock. This 3MHz reference clock is then divided
     * down to provide the required Baud rate for the device's on chip UART. The value of the Baud rate
     * divisor is an integer plus a sub-integer prescaler.
     * Allowed values for the Baud rate divisor are:
     * Divisor = n + 0, 0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 0.875; where n is an integer between 2 and
     * 16384 (214).
     *
     * Note: Divisor = 1 and Divisor = 0 are special cases. A divisor of 0 will give 3 MBaud, and a divisor
     * of 1 will give 2 MBaud. Sub-integer divisors between 0 and 2 are not allowed.
     * Therefore the value of the divisor needed for a given Baud rate is found by dividing 3000000 by the
     * required Baud rate.
     *
     * @see FTDI AN232B-05 Configuring FT232R, FT2232 and FT232B Baud Rates
     * @param[in]  baudrate
     * @param[out] wValue
     * @param[out] wIndex
     */
    static int calculate_baudrate(uint32_t baudrate, uint16_t *wValue, uint16_t *wIndex);

    // Constructors are private, use factory method open_ftdi() to create this object
    FT23x();
    FT23x(uint16_t pid, const cdc_acm_host_device_config_t *dev_config, uint8_t interface_idx = 0);

    // Make open functions from CdcAcmDevice class private
    using CdcAcmDevice::open;
    using CdcAcmDevice::open_vendor_specific;

    const uint8_t intf;
    const cdc_acm_data_callback_t user_data_cb;
    const cdc_acm_host_dev_callback_t user_event_cb;
    void *user_arg;
    uint16_t uart_state;
};
} // namespace esp_usb
