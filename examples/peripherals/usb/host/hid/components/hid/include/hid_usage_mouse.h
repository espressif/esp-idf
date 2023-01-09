/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief HID Mouse Input Report for Boot Interfaces
 *
 * @see B.1, p.60 of Device Class Definition for Human Interface Devices (HID) Version 1.11
 */
typedef struct {
    union {
        struct {
            uint8_t button1:    1;
            uint8_t button2:    1;
            uint8_t button3:    1;
            uint8_t reserved:   5;
        };
        uint8_t val;
    } buttons;
    int8_t x_displacement;
    int8_t y_displacement;
} __attribute__((packed)) hid_mouse_input_report_boot_t;

#ifdef __cplusplus
}
#endif //__cplusplus
