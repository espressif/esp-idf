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

/**
 * @brief Analog Comparator ETM Events for each unit
 *
 */
typedef enum {
    ANA_CMPR_EVENT_POS_CROSS,           /*!< Positive cross event when the source signal becomes higher than the reference signal */
    ANA_CMPR_EVENT_NEG_CROSS,           /*!< Negative cross event when the source signal becomes lower than the reference signal */
} ana_cmpr_event_type_t;

/**
 * @brief Analog Comparator ETM event configuration
 *
 */
typedef struct {
    ana_cmpr_event_type_t event_type;   /*!< Which kind of cross type can trigger the ETM event module */
} ana_cmpr_etm_event_config_t;

/**
 * @brief Allocate a new Analog Comparator ETM event
 *
 * @param[in]  cmpr         Analog Comparator handle that allocated by `ana_cmpr_new_unit`
 * @param[in]  config       Analog Comparator ETM event configuration
 * @param[out] ret_event    The returned generic handle of ETM event, which is used to connect to a task in the ETM driver
 * @return
 *      - ESP_OK                Success to create the new ETM event handle
 *      - ESP_ERR_NO_MEM        No memory for the ETM event
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the input parameters
 *      - ESP_ERR_INVALID_STATE The event on the unit has been registered
 */
esp_err_t ana_cmpr_new_etm_event(ana_cmpr_handle_t cmpr, const ana_cmpr_etm_event_config_t *config, esp_etm_event_handle_t *ret_event);

#ifdef __cplusplus
}
#endif

#endif  // SOC_ANA_CMPR_SUPPORT_ETM
