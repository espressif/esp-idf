/*
 * SPDX-FileCopyrightText: 2016-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
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
