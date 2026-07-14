/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef BLE50_DUAL_IDENTITY_SERVER_H
#define BLE50_DUAL_IDENTITY_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Attributes State Machine */
enum {
    IDX_SVC,
    IDX_CHAR_A,
    IDX_CHAR_VAL_A,
    IDX_CHAR_CFG_A,

    HRS_IDX_NB,
};

#endif // BLE50_DUAL_IDENTITY_SERVER_H
