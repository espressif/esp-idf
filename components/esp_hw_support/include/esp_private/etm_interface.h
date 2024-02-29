/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esp_etm_event_t esp_etm_event_t;
typedef struct esp_etm_task_t esp_etm_task_t;

/**
 * @brief List the peripherals that can trigger ETM task/event
 */
typedef enum {
    ETM_TRIG_PERIPH_GPIO,     /*!< ETM trigger source: GPIO */
    ETM_TRIG_PERIPH_GDMA,     /*!< ETM trigger source: GDMA */
    ETM_TRIG_PERIPH_GPTIMER,  /*!< ETM trigger source: GPTimer */
    ETM_TRIG_PERIPH_SYSTIMER, /*!< ETM trigger source: Systimer */
    ETM_TRIG_PERIPH_MCPWM,    /*!< ETM trigger source: MCPWM */
    ETM_TRIG_PERIPH_ANA_CMPR, /*!< ETM trigger source: Analog Comparator */
    ETM_TRIG_PERIPH_TSENS,    /*!< ETM trigger source: Temperature Sensor */
} etm_trigger_peripheral_t;

/**
 * @brief ETM event interface definition
 */
struct esp_etm_event_t {
    /**
     * @brief Unique event ID
     */
    uint32_t event_id;

    /**
     * @brief ETM trigger peripheral
     */
    etm_trigger_peripheral_t trig_periph;

    /**
     * @brief Resource destroy
     */
    esp_err_t (*del)(esp_etm_event_t *event);
};

/**
 * @brief ETM task interface definition
 */
struct esp_etm_task_t {
    /**
     * @brief Unique task ID
     */
    uint32_t task_id;

    /**
     * @brief ETM trigger peripheral
     */
    etm_trigger_peripheral_t trig_periph;

    /**
     * @brief Resource destroy
     */
    esp_err_t (*del)(esp_etm_task_t *task);
};

#ifdef __cplusplus
}
#endif
