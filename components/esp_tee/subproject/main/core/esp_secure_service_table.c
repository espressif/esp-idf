/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include "esp_attr.h"
#include "secure_service_num.h"
#include "secure_service_dec.h"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverride-init"
#endif

const DRAM_ATTR secure_service_entry_t tee_secure_service_table[] = {
#define __SECURE_SERVICE(NR, SYM, ARGC) { .id = NR, .func = _ss_##SYM, .nargs = ARGC },
#include "secure_service.h"
};
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
