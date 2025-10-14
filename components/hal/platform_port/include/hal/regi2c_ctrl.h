/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

/**
 * When compiling G0-layer only, we can't depend on `esp_hw_support` component.
 * However, `esp_private/regi2c_ctrl.h` is part of that component.
 * Thus, if we don't have this header file in our compilation unit, we should use
 * ROM functions.
 * The main difference is that `regi2c_ctrl.h` implementation protects the functions
 * with mutex. ROM functions must be protected explicitly by the user.
 */
#if __has_include("esp_private/regi2c_ctrl.h")
    #include "esp_private/regi2c_ctrl.h"
#else
    #include "esp_rom_regi2c.h"

    #define REGI2C_WRITE_MASK(block, reg_add, indata) \
        esp_rom_regi2c_write_mask(block, block##_HOSTID,  reg_add,  reg_add##_MSB,  reg_add##_LSB,  indata)

    #define REGI2C_WRITE(block, reg_add, indata) \
        esp_rom_regi2c_write(block, block##_HOSTID,  reg_add, indata)

    #define REGI2C_READ_MASK(block, reg_add) \
        esp_rom_regi2c_read_mask(block, block##_HOSTID,  reg_add,  reg_add##_MSB,  reg_add##_LSB)

    #define REGI2C_READ(block, reg_add) \
        esp_rom_regi2c_read(block, block##_HOSTID,  reg_add)

    #define REGI2C_ENTER_CRITICAL()
    #define REGI2C_EXIT_CRITICAL()
#endif
