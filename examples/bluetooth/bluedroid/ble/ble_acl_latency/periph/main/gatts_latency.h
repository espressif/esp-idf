/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef H_GATTS_LATENCY_
#define H_GATTS_LATENCY_

#include "esp_gatts_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Service UUID */
#define LATENCY_SERVICE_UUID    0x1234
#define LATENCY_CHAR_UUID       0x5678

/* Profile ID */
#define PROFILE_APP_ID          0

/* Handle table */
enum {
    LATENCY_IDX_SVC,
    LATENCY_IDX_CHAR,
    LATENCY_IDX_CHAR_VAL,
    LATENCY_IDX_CHAR_CFG,

    LATENCY_IDX_NB,
};

#ifdef __cplusplus
}
#endif

#endif /* H_GATTS_LATENCY_ */
