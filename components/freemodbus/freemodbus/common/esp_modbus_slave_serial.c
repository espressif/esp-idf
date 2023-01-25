/*
 * SPDX-FileCopyrightText: 2016-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_err.h"                    // for esp_err_t
#include "sdkconfig.h"                  // for KConfig defines
#include "mbc_slave.h"                  // for slave interface define
#include "esp_modbus_slave.h"           // for public slave defines
#include "mbc_serial_slave.h"           // for public interface defines

/**
 * Initialization of Modbus Serial slave controller
 */
esp_err_t mbc_slave_init(mb_port_type_t port_type, void** handler)
{
    void* port_handler = NULL;
    esp_err_t error = ESP_ERR_NOT_SUPPORTED;
    switch(port_type)
    {
        case MB_PORT_SERIAL_SLAVE:
            // Call constructor function of actual port implementation
            error = mbc_serial_slave_create(&port_handler);
            break;
        default:
            return ESP_ERR_NOT_SUPPORTED;
    }
    if ((port_handler != NULL) && (error == ESP_OK)) {
        mbc_slave_init_iface(port_handler);
        *handler = port_handler;
    }
    return error;
}
