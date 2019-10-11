/* Copyright 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//  mbc_serial_slave.h Modbus controller serial slave implementation header file

#ifndef _MODBUS_SERIAL_CONTROLLER_SLAVE
#define _MODBUS_SERIAL_CONTROLLER_SLAVE

#include <stdint.h>                 // for standard int types definition
#include <stddef.h>                 // for NULL and std defines
#include "esp_modbus_common.h"      // for common defines

/* ----------------------- Defines ------------------------------------------*/
#define MB_CONTROLLER_NOTIFY_QUEUE_SIZE     (CONFIG_FMB_CONTROLLER_NOTIFY_QUEUE_SIZE) // Number of messages in parameter notification queue
#define MB_CONTROLLER_NOTIFY_TIMEOUT        (pdMS_TO_TICKS(CONFIG_FMB_CONTROLLER_NOTIFY_TIMEOUT)) // notification timeout

/*
 * @brief Initialize Modbus controller and stack
 *
 * @param[out] handler handler(pointer) to master data structure
 * @return
 *     - ESP_OK   Success
 *     - ESP_ERR_NO_MEM Parameter error
 */
esp_err_t mbc_serial_slave_create(mb_port_type_t port_type, void** handler);

#endif // _MODBUS_SERIAL_CONTROLLER_SLAVE

