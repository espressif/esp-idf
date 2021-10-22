/*
 * SPDX-FileCopyrightText: 2016-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

//  mbc_tcp_slave.h Modbus controller TCP slave implementation header file

#ifndef _MODBUS_TCP_CONTROLLER_SLAVE
#define _MODBUS_TCP_CONTROLLER_SLAVE

#include <stdint.h>                 // for standard int types definition
#include <stddef.h>                 // for NULL and std defines
#include "esp_modbus_common.h"      // for common defines

/* ----------------------- Defines ------------------------------------------*/

#define MB_CONTROLLER_NOTIFY_QUEUE_SIZE     (CONFIG_FMB_CONTROLLER_NOTIFY_QUEUE_SIZE) // Number of messages in parameter notification queue
#define MB_CONTROLLER_NOTIFY_TIMEOUT        (pdMS_TO_TICKS(CONFIG_FMB_CONTROLLER_NOTIFY_TIMEOUT)) // notification timeout

/**
 * @brief Initialize Modbus controller and stack for TCP slave
 *
 * @param[out] handler handler(pointer) to slave data structure
 * @return
 *     - ESP_OK   Success
 *     - ESP_ERR_NO_MEM Parameter error
 */
esp_err_t mbc_tcp_slave_create(void** handler);

#endif // _MODBUS_TCP_CONTROLLER_SLAVE
