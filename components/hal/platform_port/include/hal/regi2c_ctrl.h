/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#if __has_include("esp_private/regi2c_ctrl.h")
    #include "esp_private/regi2c_ctrl.h"
#else
    #include "esp_rom_regi2c.h"
    #define REGI2C_WRITE_MASK(block, reg_add, indata) esp_rom_regi2c_write_mask(block, block##_HOSTID,  reg_add,  reg_add##_MSB,  reg_add##_LSB,  indata)
#endif
