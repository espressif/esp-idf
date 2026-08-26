/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
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
#include "hal/regi2c_impl.h"

#define REGI2C_WRITE_MASK(block, reg_add, indata) do { \
        int __DECLARE_REGI2C_ATOMIC_ENV __attribute__((unused)); \
        regi2c_impl_write_mask(block, block##_HOSTID,  reg_add,  reg_add##_MSB,  reg_add##_LSB,  indata); \
    } while(0)

#define REGI2C_WRITE(block, reg_add, indata) do { \
        int __DECLARE_REGI2C_ATOMIC_ENV __attribute__((unused)); \
        regi2c_impl_write(block, block##_HOSTID,  reg_add, indata); \
    } while(0)

#define REGI2C_READ_MASK(block, reg_add) ({ \
        int __DECLARE_REGI2C_ATOMIC_ENV __attribute__((unused)); \
        regi2c_impl_read_mask(block, block##_HOSTID,  reg_add,  reg_add##_MSB,  reg_add##_LSB); \
    })

#define REGI2C_READ(block, reg_add) ({ \
        int __DECLARE_REGI2C_ATOMIC_ENV __attribute__((unused)); \
        regi2c_impl_read(block, block##_HOSTID,  reg_add); \
    })

#define REGI2C_ENTER_CRITICAL()     int __DECLARE_REGI2C_ATOMIC_ENV __attribute__((unused));
#define REGI2C_EXIT_CRITICAL()

#define REGI2C_CLOCK_ENABLE()
#define REGI2C_CLOCK_DISABLE()
#endif
