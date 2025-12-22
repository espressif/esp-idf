/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef SUBRATING_SERVICE_H
#define SUBRATING_SERVICE_H

#include <stdint.h>

/* Subrating Service UUIDs */
#define BLE_UUID_SUBRATING_SERVICE_VAL       0xFF00

/* Attributes State Machine */
enum {
    IDX_SUBRATING_SVC,

    /* Subrating Info Characteristic */
    IDX_SUBRATING_INFO_CHAR,
    IDX_SUBRATING_INFO_VAL,

    SUBRATING_IDX_NB,
};

#endif /* SUBRATING_SERVICE_H */
