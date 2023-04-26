/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const periph_module_t module;  // peripheral module
    const int irq_id;              // interrupt source ID
} ieee802154_conn_t;

extern const ieee802154_conn_t ieee802154_periph;

#ifdef __cplusplus
}
#endif
