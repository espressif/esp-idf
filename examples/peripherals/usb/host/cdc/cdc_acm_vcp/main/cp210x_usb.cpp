/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "cp210x_usb.hpp"
#include "usb/usb_types_ch9.h"
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SILICON_LABS_VID (0x10C4)
#define CP210X_READ_REQ  (USB_BM_REQUEST_TYPE_TYPE_VENDOR | USB_BM_REQUEST_TYPE_RECIP_INTERFACE | USB_BM_REQUEST_TYPE_DIR_IN)
#define CP210X_WRITE_REQ (USB_BM_REQUEST_TYPE_TYPE_VENDOR | USB_BM_REQUEST_TYPE_RECIP_INTERFACE | USB_BM_REQUEST_TYPE_DIR_OUT)

namespace esp_usb {
CP210x *CP210x::open_cp210x(uint16_t pid, const cdc_acm_host_device_config_t *dev_config, uint8_t interface_idx)
{
    return new CP210x(pid, dev_config, interface_idx);
}

CP210x::CP210x(uint16_t pid, const cdc_acm_host_device_config_t *dev_config, uint8_t interface_idx)
    : intf(interface_idx)
{
    esp_err_t err;
    err = this->open_vendor_specific(SILICON_LABS_VID, pid, this->intf, dev_config);
    if (err != ESP_OK) {
        throw(err);
    }

    // CP210X interfaces must be explicitly enabled
    err = this->send_custom_request(CP210X_WRITE_REQ, CP210X_CMD_IFC_ENABLE, 1, this->intf, 0, NULL);
    if (err != ESP_OK) {
        throw(err);
    }
};

esp_err_t CP210x::line_coding_get(cdc_acm_line_coding_t *line_coding)
{
    assert(line_coding);

    ESP_RETURN_ON_ERROR(this->send_custom_request(CP210X_READ_REQ, CP210X_CMD_GET_BAUDRATE, 0, this->intf, sizeof(line_coding->dwDTERate), (uint8_t *)&line_coding->dwDTERate), "CP210X",);

    uint8_t temp_data[2];
    ESP_RETURN_ON_ERROR(this->send_custom_request(CP210X_READ_REQ, CP210X_CMD_GET_LINE_CTL, 0, this->intf, 2, temp_data), "CP210X",);
    line_coding->bCharFormat = temp_data[0] & 0x0F;
    line_coding->bParityType = (temp_data[0] & 0xF0) >> 4;
    line_coding->bDataBits   = temp_data[1];

    return ESP_OK;
}

esp_err_t CP210x::line_coding_set(cdc_acm_line_coding_t *line_coding)
{
    assert(line_coding);

    if (line_coding->dwDTERate != 0) {
        ESP_RETURN_ON_ERROR(this->send_custom_request(CP210X_WRITE_REQ, CP210X_CMD_SET_BAUDRATE, 0, this->intf, sizeof(line_coding->dwDTERate), (uint8_t *)&line_coding->dwDTERate), "CP210X",);
    }

    if (line_coding->bDataBits != 0) {
        const uint16_t wValue = line_coding->bCharFormat | (line_coding->bParityType << 4) | (line_coding->bDataBits << 8);
        return this->send_custom_request(CP210X_WRITE_REQ, CP210X_CMD_SET_LINE_CTL, wValue, this->intf, 0, NULL);
    }
    return ESP_OK;
}

esp_err_t CP210x::set_control_line_state(bool dtr, bool rts)
{
    const uint16_t wValue = (uint16_t)dtr | ((uint16_t)rts << 1) | 0x0300;
    return this->send_custom_request(CP210X_WRITE_REQ, CP210X_CMD_SET_MHS, wValue, this->intf, 0, NULL);
}

esp_err_t CP210x::send_break(uint16_t duration_ms)
{
    ESP_RETURN_ON_ERROR(this->send_custom_request(CP210X_WRITE_REQ, CP210X_CMD_SET_BREAK, 1, this->intf, 0, NULL), "CP210x",);
    vTaskDelay(pdMS_TO_TICKS(duration_ms));
    return this->send_custom_request(CP210X_WRITE_REQ, CP210X_CMD_SET_BREAK, 0, this->intf, 0, NULL);
}
}
