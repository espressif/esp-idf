/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/gpio_filter.h"
#include "esp_heap_caps.h"

#define FILTER_MEM_ALLOC_CAPS   MALLOC_CAP_DEFAULT

#define GLITCH_FILTER_PM_LOCK_NAME_LEN_MAX 16

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gpio_glitch_filter_t gpio_glitch_filter_t;

typedef enum {
    GLITCH_FILTER_FSM_INIT,
    GLITCH_FILTER_FSM_ENABLE,
} glitch_filter_fsm_t;

/**
 * @brief Glitch Filter base class
 */
struct gpio_glitch_filter_t {
    glitch_filter_fsm_t fsm;
    gpio_num_t gpio_num;
    esp_err_t (*enable)(gpio_glitch_filter_t *filter);
    esp_err_t (*disable)(gpio_glitch_filter_t *filter);
    esp_err_t (*del)(gpio_glitch_filter_t *filter);
};

#ifdef __cplusplus
}
#endif
