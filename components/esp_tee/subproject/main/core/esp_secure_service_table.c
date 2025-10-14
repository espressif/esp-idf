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

const DRAM_ATTR secure_service_entry_t tee_sec_srv_tbl_int_mem[] = {
    [0 ... SECURE_SERVICES_SPLIT_ID - 1] = { .func = NULL, .nargs = 0 },
#define __SECURE_SERVICE(NR, SYM, ARGC)  [NR] = { .func = _ss_##SYM, .nargs = ARGC },
#include "secure_service_int.h"
#undef __SECURE_SERVICE
};

const secure_service_entry_t tee_sec_srv_tbl_ext_mem[] = {
    [0 ... MAX_SECURE_SERVICES_ID] = { .func = NULL, .nargs = 0 },
#define __SECURE_SERVICE(NR, SYM, ARGC)  [NR] = { .func = _ss_##SYM, .nargs = ARGC },
#include "secure_service_ext.h"
#undef __SECURE_SERVICE
};

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
