/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdlib.h>
#include "soc/temperature_sensor_periph.h"
#include "hal/temperature_sensor_types.h"
#include "driver/temperature_sensor.h"
#include "esp_intr_alloc.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TEMP_SENSOR_FSM_INIT,
    TEMP_SENSOR_FSM_ENABLE,
} temp_sensor_fsm_t;

#if CONFIG_TEMP_SENSOR_ISR_IRAM_SAFE
#define TEMPERATURE_SENSOR_INTR_ALLOC_FLAGS    (ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_LOWMED)
#define TEMPERATURE_SENSOR_MEM_ALLOC_CAPS      (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define TEMPERATURE_SENSOR_INTR_ALLOC_FLAGS    (ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_LOWMED)
#define TEMPERATURE_SENSOR_MEM_ALLOC_CAPS      (MALLOC_CAP_DEFAULT)
#endif

typedef struct temperature_sensor_obj_t temperature_sensor_obj_t;

struct temperature_sensor_obj_t {
    const temperature_sensor_attribute_t *tsens_attribute;
    temp_sensor_fsm_t  fsm;
    temperature_sensor_clk_src_t clk_src;
#if SOC_TEMPERATURE_SENSOR_INTR_SUPPORT
    intr_handle_t temp_sensor_isr_handle;
    temperature_thres_cb_t threshold_cbs;
    void *cb_user_arg;
#endif // SOC_TEMPERATURE_SENSOR_INTR_SUPPORT
};

#ifdef __cplusplus
}
#endif
