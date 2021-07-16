/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include "usb/cdc_acm_host.h"
#include "esp_log.h"

#define BG96_VID            (0x2C7C)
#define BG96_PID            (0x0296)
#define BG96_AT_INTERFACE   (2)
#define BG96_NMEA_INTERFACE (1)

class Bg96Usb {
public:

    explicit  Bg96Usb() : at_opened(false) {
    };

    esp_err_t at_start(cdc_acm_data_callback_t data_cb, void *user_arg)
    {
        // This driver doesn't support CDC notifications. This can lead to silent failures
        const cdc_acm_host_device_config_t dev_config = {
            .connection_timeout_ms = 10000,
            .out_buffer_size = 64,
            .event_cb = NULL,
            .data_cb = data_cb,
            .user_arg = user_arg,
        };
        ESP_ERROR_CHECK(this->at_port.open_vendor_specific(BG96_VID, BG96_PID, BG96_AT_INTERFACE, &dev_config));
        this->at_opened = true;

        // Some FW versions have Echo enabled by default. Disable it with ATE0 command
        ESP_LOGD("BG96_USB", "Turning off echo with ATE0");
        ESP_ERROR_CHECK(this->at_port.tx_blocking((uint8_t *)"ATE0\r", 5, 1000));
        vTaskDelay(100);
        return ESP_OK;
    }

    void at_stop()
    {
        this->at_port.close();
        this->at_opened = false;
    }

    esp_err_t at_write(uint8_t *data, size_t len)
    {
        ESP_LOG_BUFFER_HEXDUMP("BG96_USB", data, len, ESP_LOG_DEBUG);
        return this->at_port.tx_blocking(data, len, 1000);
    }

    esp_err_t gnss_start(cdc_acm_data_callback_t data_cb)
    {
        if (!this->at_opened) {
            return  ESP_ERR_INVALID_STATE;
        }

        const cdc_acm_host_device_config_t dev_config = {
            .connection_timeout_ms = 1000,
            .out_buffer_size = 0, // Read-only
            .event_cb = NULL,
            .data_cb = data_cb,
            .user_arg = this,
        };
        ESP_ERROR_CHECK(this->nmea_port.open_vendor_specific(BG96_VID, BG96_PID, BG96_NMEA_INTERFACE, &dev_config));
        return this->at_port.tx_blocking((uint8_t*)"AT+QGPS=1\r", 10, 1000);
    }

    esp_err_t gnss_stop()
    {
        esp_err_t ret = this->at_port.tx_blocking((uint8_t*)"AT+QGPSEND\r", 11, 1000);
        this->nmea_port.close();
        return ret;
    }

protected:
    CdcAcmDevice at_port;   // Main control port for AT commands
    CdcAcmDevice nmea_port; // Read only port for NMEA messages
private:
    bool at_opened;
};
