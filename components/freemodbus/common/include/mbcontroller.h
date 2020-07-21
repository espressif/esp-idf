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
//  mbcontroller.h
//  mbcontroller - common Modbus controller header file

#ifndef _MODBUS_CONTROLLER_COMMON
#define _MODBUS_CONTROLLER_COMMON

#include <stdint.h>                 // for standard int types definition
#include <stddef.h>                 // for NULL and std defines
#include "string.h"                 // for strerror()
#include "errno.h"                  // for errno
#include "esp_err.h"                // for error handling
#include "driver/uart.h"            // for uart port number defines
#include "sdkconfig.h"              // for KConfig options

#include "esp_modbus_master.h"
#include "esp_modbus_slave.h"

#endif
