/*
 * SPDX-FileCopyrightText: 2016-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_err.h"                // for esp_err_t
#include "mbc_master.h"             // for master interface define
#include "esp_modbus_master.h"      // for public slave defines
#include "mbc_serial_master.h"      // for public interface defines

/**
 * Initialization of Modbus master serial
 */
esp_err_t mbc_master_init(mb_port_type_t port_type, void** handler)
{
    void* port_handler = NULL;
    esp_err_t error = ESP_ERR_NOT_SUPPORTED;
    switch(port_type)
    {
    case MB_PORT_SERIAL_MASTER:
        error = mbc_serial_master_create(&port_handler);
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }
    if ((port_handler != NULL) && (error == ESP_OK)) {
        mbc_master_init_iface(port_handler);
        *handler = port_handler;
    }
    return error;
}
