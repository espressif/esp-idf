/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <string.h>
#include "ftdi_usb.hpp"
#include "usb/usb_types_ch9.h"
#include "esp_log.h"
#include "esp_check.h"

#define FTDI_VID       (0x0403)
#define FTDI_READ_REQ  (USB_BM_REQUEST_TYPE_TYPE_VENDOR | USB_BM_REQUEST_TYPE_DIR_IN)
#define FTDI_WRITE_REQ (USB_BM_REQUEST_TYPE_TYPE_VENDOR | USB_BM_REQUEST_TYPE_DIR_OUT)

namespace esp_usb {
FT23x *FT23x::open_ftdi(uint16_t pid, const cdc_acm_host_device_config_t *dev_config, uint8_t interface_idx)
{
    return new FT23x(pid, dev_config, interface_idx);
}

FT23x::FT23x(uint16_t pid, const cdc_acm_host_device_config_t *dev_config, uint8_t interface_idx)
    : intf(interface_idx), user_data_cb(dev_config->data_cb), user_event_cb(dev_config->event_cb),
    user_arg(dev_config->user_arg), uart_state(0)
{
    cdc_acm_host_device_config_t ftdi_config;
    memcpy(&ftdi_config, dev_config, sizeof(cdc_acm_host_device_config_t));
    // FT23x reports modem status in first two bytes of RX data
    // so here we override the RX handler with our own

    if (dev_config->data_cb) {
        ftdi_config.data_cb = ftdi_rx;
        ftdi_config.user_arg = this;
    }

    if (dev_config->event_cb) {
        ftdi_config.event_cb = ftdi_event;
        ftdi_config.user_arg = this;
    }

    esp_err_t err;
    err = this->open_vendor_specific(FTDI_VID, pid, this->intf, &ftdi_config);
    if (err != ESP_OK) {
        throw(err);
    }

    // FT23x interface must be first reset and configured (115200 8N1)
    err = this->send_custom_request(FTDI_WRITE_REQ, FTDI_CMD_RESET, 0, this->intf + 1, 0, NULL);
    if (err != ESP_OK) {
        throw(err);
    }

    cdc_acm_line_coding_t line_coding = {
        .dwDTERate = 115200,
        .bCharFormat = 0,
        .bParityType = 0,
        .bDataBits = 8,
    };
    err = this->line_coding_set(&line_coding);
    if (err != ESP_OK) {
        throw(err);
    }
};

esp_err_t FT23x::line_coding_set(cdc_acm_line_coding_t *line_coding)
{
    assert(line_coding);

    if (line_coding->dwDTERate != 0) {
        uint16_t wIndex, wValue;
        calculate_baudrate(line_coding->dwDTERate, &wValue, &wIndex);
        ESP_RETURN_ON_ERROR(this->send_custom_request(FTDI_WRITE_REQ, FTDI_CMD_SET_BAUDRATE, wValue, wIndex, 0, NULL), "FT23x",);
    }

    if (line_coding->bDataBits != 0) {
        const uint16_t wValue = (line_coding->bDataBits) | (line_coding->bParityType << 8) | (line_coding->bCharFormat << 11);
        return this->send_custom_request(FTDI_WRITE_REQ, FTDI_CMD_SET_LINE_CTL, wValue, this->intf, 0, NULL);
    }
    return ESP_OK;
}

esp_err_t FT23x::set_control_line_state(bool dtr, bool rts)
{
    ESP_RETURN_ON_ERROR(this->send_custom_request(FTDI_WRITE_REQ, FTDI_CMD_SET_MHS, dtr ? 0x11 : 0x10, this->intf, 0, NULL), "FT23x",); // DTR
    return this->send_custom_request(FTDI_WRITE_REQ, FTDI_CMD_SET_MHS, rts ? 0x21 : 0x20, this->intf, 0, NULL); // RTS
}

void FT23x::ftdi_rx(uint8_t* data, size_t data_len, void *user_arg)
{
    FT23x *this_ftdi = (FT23x *)user_arg;

    // Dispatch serial state if it has changed
    if (this_ftdi->user_event_cb) {
        cdc_acm_uart_state_t new_state;
        new_state.val = 0;
        new_state.bRxCarrier =  data[0] & 0x80; // DCD
        new_state.bTxCarrier =  data[0] & 0x20; // DSR
        new_state.bBreak =      data[1] & 0x10;
        new_state.bRingSignal = data[0] & 0x40;
        new_state.bFraming =    data[1] & 0x08;
        new_state.bParity =     data[1] & 0x04;
        new_state.bOverRun =    data[1] & 0x02;

        if (this_ftdi->uart_state != new_state.val) {
            cdc_acm_host_dev_event_data_t serial_event;
            serial_event.type = CDC_ACM_HOST_SERIAL_STATE;
            serial_event.data.serial_state = new_state;
            this_ftdi->user_event_cb(&serial_event, this_ftdi->user_arg);
            this_ftdi->uart_state = new_state.val;
        }
    }

    // Dispatch data if any
    if (data_len > 2) {
        this_ftdi->user_data_cb(&data[2], data_len - 2, this_ftdi->user_arg);
    }
}

void FT23x::ftdi_event(const cdc_acm_host_dev_event_data_t *event, void *user_ctx)
{
    FT23x *this_ftdi = (FT23x *)user_ctx;
    this_ftdi->user_event_cb(event, this_ftdi->user_arg);
}

int FT23x::calculate_baudrate(uint32_t baudrate, uint16_t *wValue, uint16_t *wIndex)
{
    #define FTDI_BASE_CLK (3000000)

    int baudrate_real;
    if (baudrate > 2000000) {
        // set to 3000000
        *wValue = 0;
        *wIndex = 0;
        baudrate_real = 3000000;
    } else if (baudrate >= 1000000) {
        // set to 1000000
        *wValue = 1;
        *wIndex = 0;
        baudrate_real = 1000000;
    } else {
        const float ftdi_fractal[] = {0, 0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 0.875, 1};
        const uint8_t ftdi_fractal_bits[] = {0, 0x03, 0x02, 0x04, 0x01, 0x05, 0x06, 0x07};
        uint16_t divider_n = FTDI_BASE_CLK / baudrate; // integer value
        int ftdi_fractal_idx = 0;
        float divider = FTDI_BASE_CLK / (float)baudrate; // float value
        float divider_fractal = divider - (float)divider_n;

        // Find closest bigger FT23x fractal divider
        for (ftdi_fractal_idx = 0; ftdi_fractal[ftdi_fractal_idx] <= divider_fractal; ftdi_fractal_idx++) {};

        // Calculate baudrate errors for two closest fractal divisors
        int diff1 = baudrate - (int)(FTDI_BASE_CLK / (divider_n + ftdi_fractal[ftdi_fractal_idx]));     // Greater than required baudrate
        int diff2 = (int)(FTDI_BASE_CLK / (divider_n + ftdi_fractal[ftdi_fractal_idx - 1])) - baudrate; // Lesser than required baudrate

        // Chose divider and fractal divider with smallest error
        if (diff2 < diff1) {
            ftdi_fractal_idx--;
        } else {
            if (ftdi_fractal_idx == 8) {
                ftdi_fractal_idx = 0;
                divider_n++;
            }
        }

        baudrate_real = FTDI_BASE_CLK / (float)((float)divider_n + ftdi_fractal[ftdi_fractal_idx]);
        *wValue = ((0x3FFFF) & divider_n) | (ftdi_fractal_bits[ftdi_fractal_idx] << 14);
        *wIndex = ftdi_fractal_bits[ftdi_fractal_idx] >> 2;
    }
    ESP_LOGD("FT23x", "wValue: 0x%04X wIndex: 0x%04X", *wValue, *wIndex);
    ESP_LOGI("FT23x", "Baudrate required: %d, set: %d", baudrate, baudrate_real);

    return baudrate_real;
}
} // esp_usb
