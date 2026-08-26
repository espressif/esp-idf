/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
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
 * @brief Analog Comparator ETM event configuration
 *
 */
typedef struct {
    ana_cmpr_event_type_t event_type;   /*!< Which kind of cross type can trigger the ETM event module */
    int                   src_chan_id;  /*!< The index of the source channel that triggers the event */
    uint32_t              event_delay;  /*!< The delay time after the cross event to trigger the ETM event.
                                             The delay is shared across all channels, last-writer-wins on the global delay timer */
} ana_cmpr_etm_event_config_t;

/**
 * @brief Allocate a new Analog Comparator ETM event
 * @note The returned ETM event handle must be deleted by `esp_etm_del_event()` before calling
 *       `ana_cmpr_del_unit()` on the parent comparator handle.
 *
 * @param[in]  cmpr         Analog Comparator handle that allocated by `ana_cmpr_new_unit`
 * @param[in]  config       Analog Comparator ETM event configuration
 * @param[out] ret_event    The returned generic handle of ETM event, which is used to connect to a task in the ETM driver
 * @return
 *      - ESP_OK                Success to create the new ETM event handle
 *      - ESP_ERR_NO_MEM        No memory for the ETM event
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the input parameters
 */
esp_err_t ana_cmpr_new_etm_event(ana_cmpr_handle_t cmpr, const ana_cmpr_etm_event_config_t *config, esp_etm_event_handle_t *ret_event);

/**
 * @brief Analog Comparator ETM task configuration
 */
typedef struct {
    ana_cmpr_task_type_t task_type;    /*!< The type of the task */
} ana_cmpr_etm_task_config_t;

/**
 * @brief Allocate a new Analog Comparator ETM task
 * @note The returned ETM task handle must be deleted by `esp_etm_del_task()` before calling
 *       `ana_cmpr_del_unit()` on the parent comparator handle.
 *
 * @param[in]  cmpr     Analog Comparator handle that allocated by `ana_cmpr_new_unit`
 * @param[in]  config   Analog Comparator ETM task configuration
 * @param[out] ret_task The returned generic handle of ETM task, which is used to connect to a event in the ETM driver
 * @return
 *      - ESP_OK                Success to create the new ETM task handle
 *      - ESP_ERR_NO_MEM        No memory for the ETM task
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the input parameters
 */
esp_err_t ana_cmpr_new_etm_task(ana_cmpr_handle_t cmpr, const ana_cmpr_etm_task_config_t *config, esp_etm_task_handle_t *ret_task);

#ifdef __cplusplus
}
#endif

#endif  // SOC_ANA_CMPR_SUPPORT_ETM
