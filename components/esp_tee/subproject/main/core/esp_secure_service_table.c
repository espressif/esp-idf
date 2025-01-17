/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include "esp_attr.h"
#include "secure_service_num.h"
#include "secure_service_dec.h"

typedef void (*secure_service_t)(void);

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverride-init"
#endif

const DRAM_ATTR secure_service_t tee_secure_service_table[] = {
    [0 ... MAX_SECURE_SERVICES - 1] = (secure_service_t)NULL,

#define __SECURE_SERVICE(nr, symbol, nargs)  [nr] = (secure_service_t)_ss_##symbol,
#include "secure_service.h"
};
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
