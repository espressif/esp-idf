
/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/usb_serial_jtag.h"

typedef enum {
    USJ_SELECT_READ_NOTIF,
    USJ_SELECT_WRITE_NOTIF,
    USJ_SELECT_ERROR_NOTIF,
} usj_select_notif_t;

typedef void (*usj_select_notif_callback_t)(usj_select_notif_t usb_serial_jtag_select_notif, BaseType_t *task_woken);

/**
 * @brief Set notification callback function for select() events
 * @param usb_serial_jtag_select_notif_callback callback function
 */
void usb_serial_jtag_set_select_notif_callback(usj_select_notif_callback_t usb_serial_jtag_select_notif_callback);

/**
 * @brief Return the readiness status of the driver for read operation
 *
 * @return true if driver read ready, false if not
 */
bool usb_serial_jtag_read_ready(void);

/**
 * @brief Return the readiness status of the driver for write operation
 *
 * @return true if driver is write ready, false if not
 */
bool usb_serial_jtag_write_ready(void);

#ifdef __cplusplus
}
#endif
