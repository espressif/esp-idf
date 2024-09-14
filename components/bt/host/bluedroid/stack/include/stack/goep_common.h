/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "common/bt_target.h"

#define GOEP_SUCCESS                0       /* Operation successful */
#define GOEP_FAILURE                1       /* Operation failed */
#define GOEP_NO_RESOURCES           3       /* Not enough resources */
#define GOEP_BAD_HANDLE             4       /* Bad handle */
#define GOEP_INVALID_PARAM          5       /* Invalid parameter */
#define GOEP_INVALID_STATE          6       /* Operation not allow in current state */
#define GOEP_CONGEST                7       /* Congest */
#define GOEP_TL_ERROR               8       /* Lower transport layer error */
