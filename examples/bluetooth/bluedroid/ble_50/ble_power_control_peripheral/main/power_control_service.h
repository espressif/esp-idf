/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef POWER_CONTROL_SERVICE_H
#define POWER_CONTROL_SERVICE_H

#include <stdint.h>

/* Power Control Service UUIDs */
#define BLE_UUID_POWER_CONTROL_SERVICE_VAL   0xFF01

/* Attributes State Machine */
enum {
    IDX_POWER_CONTROL_SVC,

    /* Power Level Characteristic */
    IDX_POWER_LEVEL_CHAR,
    IDX_POWER_LEVEL_VAL,

    POWER_CONTROL_IDX_NB,
};

#endif /* POWER_CONTROL_SERVICE_H */
