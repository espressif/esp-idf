/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#warning "This header is deprecated. Please use esp_intr_types.h instead"

#include "esp_intr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief legacy type compatibility
typedef esp_intr_cpu_affinity_t intr_cpu_id_t;
#define INTR_CPU_CONVERT_ID ESP_INTR_CPU_AFFINITY_TO_CORE_ID
#define INTR_CPU_ID_AUTO ESP_INTR_CPU_AFFINITY_AUTO
#define INTR_CPU_ID_0 ESP_INTR_CPU_AFFINITY_0
#define INTR_CPU_ID_1 ESP_INTR_CPU_AFFINITY_1

#ifdef __cplusplus
}
#endif
