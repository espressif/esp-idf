/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "hal/cordic_hal.h"
#include "esp_intr_types.h"
#include "sdkconfig.h"
#include "driver/cordic.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_CORDIC_ONESHOT_CTRL_FUNC_IN_IRAM
#define CORDIC_MEM_ALLOC_CAPS   (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define CORDIC_MEM_ALLOC_CAPS   MALLOC_CAP_DEFAULT
#endif

#define CORDIC_ONESHOT_CALCULATE_TIMEOUT_MS (2) // 2ms is enough for cordic one shot calculation api
#define CORDIC_Q15_SCALE_FACTOR    (32768.0f)      // 2^15, scale factor for Q15 format conversion
#define CORDIC_Q31_SCALE_FACTOR    (2147483648.0f) // 2^31, scale factor for Q31 format conversion

typedef struct cordic_engine_t cordic_engine_t;

// Function pointer type for setting CORDIC arguments
typedef void (*cordic_set_argument_func_t)(cordic_hal_context_t *hal, const cordic_input_buffer_desc_t *input_buffer, size_t index);

// Function pointer type for getting CORDIC results
typedef void (*cordic_get_result_func_t)(cordic_hal_context_t *hal, cordic_output_buffer_desc_t *output_buffer, size_t index);

struct cordic_engine_t {
    cordic_hal_context_t hal;                      // HAL context
};

#ifdef __cplusplus
}
#endif
