/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "driver/ana_cmpr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

ana_cmpr_unit_t ana_cmpr_priv_get_unit_by_handle(ana_cmpr_handle_t cmpr);

#ifdef __cplusplus
}
#endif
