/*
 * SPDX-FileCopyrightText: 2016-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

//  mbc_tcp_master.h Modbus controller TCP master implementation header file

#ifndef _MODBUS_TCP_CONTROLLER_MASTER
#define _MODBUS_TCP_CONTROLLER_MASTER

#include <stdint.h>                 // for standard int types definition
#include <stddef.h>                 // for NULL and std defines
#include "esp_modbus_common.h"      // for common defines

/* ----------------------- Defines ------------------------------------------*/
#define MB_INST_MIN_SIZE                    (2) // The minimal size of Modbus registers area in bytes
#define MB_INST_MAX_SIZE                    (CONFIG_MB_INST_MAX_SIZE) // The maximum size of Modbus area in bytes

#define MB_CONTROLLER_NOTIFY_QUEUE_SIZE     (CONFIG_MB_CONTROLLER_NOTIFY_QUEUE_SIZE) // Number of messages in parameter notification queue
#define MB_CONTROLLER_NOTIFY_TIMEOUT        (pdMS_TO_TICKS(CONFIG_MB_CONTROLLER_NOTIFY_TIMEOUT)) // notification timeout

/**
 * @brief Create Modbus Master controller and stack for TCP port
 *
 * @param[out] handler handler(pointer) to master data structure
 * @return
 *     - ESP_OK   Success
 *     - ESP_ERR_NO_MEM Parameter error
 */
esp_err_t mbc_tcp_master_create(void** handler);

#endif // _MODBUS_TCP_CONTROLLER_SLAVE
