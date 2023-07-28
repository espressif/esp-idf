/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "esp_etm.h"
#include "driver/ana_cmpr_types.h"

#if SOC_ANA_CMPR_SUPPORT_ETM

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ANA_CMPR_EVENT_POS_CROSS,
    ANA_CMPR_EVENT_NEG_CROSS,
} ana_cmpr_event_type_t;

typedef struct {
    ana_cmpr_unit_t unit;
    ana_cmpr_event_type_t event_type; /*!< Which kind of cross type can trigger the ETM event module */
} ana_cmpr_etm_event_config_t;

esp_err_t ana_cmpr_new_etm_event(const ana_cmpr_etm_event_config_t *config, esp_etm_event_handle_t *ret_event);

#ifdef __cplusplus
}
#endif

#endif  // SOC_ANA_CMPR_SUPPORT_ETM
