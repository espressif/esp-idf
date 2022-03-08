/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _xtensa_perfmon_apis_H_
#define _xtensa_perfmon_apis_H_
#include "xtensa_perfmon_access.h"
#include "xtensa_perfmon_masks.h"

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @brief Performance monitor configuration structure
 *
 * Structure to configure performance counter to measure dedicated function
 */
typedef struct xtensa_perfmon_config {
    int repeat_count;       /*!< how much times function will be called before the calback will be repeated */
    float max_deviation;    /*!<  Difference between min and max counter number 0..1, 0 - no difference, 1 - not used */
    void *call_params;      /*!<  This pointer will be passed to the call_function as a parameter */
    void (*call_function)(void *params); /*!< pointer to the function that have to be called */
    void (*callback)(void *params, uint32_t select, uint32_t mask, uint32_t value); /*!< pointer to the function that will be called with result parameters */
    void *callback_params;  /*!<  parameter that will be passed to the callback */
    int tracelevel;         /*!<  trace level for all counters.
                                  In case of negative value, the filter will be ignored.
                                  If it's >=0, then the perfmon will count only when interrupt level > tracelevel. It's useful to monitor interrupts. */
    uint32_t  counters_size;/*!<  amount of counter in the list */
    const uint32_t *select_mask;  /*!<  list of the select/mask parameters */
} xtensa_perfmon_config_t;


/**
 * @brief      Execute PM
 *
 * Execute performance counter for dedicated function with defined parameters
 *
 * @param[in] config: pointer to the configuration structure
 *
 * @return
 *      - ESP_OK if no errors
 *      - ESP_ERR_INVALID_ARG if one of the required parameters not defined
 *      - ESP_FAIL - counter overflow
 */
esp_err_t xtensa_perfmon_exec(const xtensa_perfmon_config_t *config);

/**
 * @brief      Dump PM results
 *
 * Callback to dump perfmon result to a FILE* stream specified in
 * perfmon_config_t::callback_params. If callback_params is set to NULL, will print to stdout
 *
 * @param[in] params:   used parameters passed from configuration (callback_params).
 *                      This parameter expected as FILE* hanle, where data will be stored.
 *                      If this parameter NULL, then data will be stored to the stdout.
 * @param[in] select:   select value for current counter
 * @param[in] mask:     mask value for current counter
 * @param[in] value:    counter value for current counter
 *
 */
void xtensa_perfmon_view_cb(void *params, uint32_t select, uint32_t mask, uint32_t value);

#ifdef __cplusplus
}
#endif

#endif // _xtensa_perfmon_apis_H_
