/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "common/bt_target.h"

/* GOEP Client or Server(not supported yet) API return code */
#define GOEP_SUCCESS                0x00        /* Operation successful */
#define GOEP_FAILURE                0x01        /* Operation failed */
#define GOEP_NO_RESOURCES           0x02        /* Not enough resources */
#define GOEP_BAD_HANDLE             0x04        /* Bad handle */
#define GOEP_INVALID_PARAM          0x08        /* Invalid parameter */
#define GOEP_INVALID_STATE          0x10        /* Operation not allow in current state */
#define GOEP_CONGEST                0x20        /* Congest */
#define GOEP_TL_ERROR               0x40        /* Lower transport layer error */
